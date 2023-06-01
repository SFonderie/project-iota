// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/GenerationSubsystem.h"
#include "TileData/TileDataAsset.h"
#include "Engine/AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GenerationSubsystem)

void UGenerationSubsystem::BeginGeneration(const FGameplayTag& Tileset, int32 Seed)
{
	UE_LOG(LogTemp, Log, TEXT("Started pre-generation at %s"), *FDateTime::UtcNow().ToString());

	GenerationSeed.Initialize(Seed);
	FilterTileAssetPrimaryIDs(Tileset);

	if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		FStreamableDelegate Callback = FStreamableDelegate::CreateUObject(this, &UGenerationSubsystem::AssetLoadCompleted);
		AssetLoadHandle = AssetManager->LoadPrimaryAssets(TileAssetPrimaryIDs, TArray<FName>(), Callback);
	}
}

void UGenerationSubsystem::FilterTileAssetPrimaryIDs(const FGameplayTag& Tileset)
{
	if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
	{
		TileAssetPrimaryIDs.Empty();

		// Get the list of all tile data asset asset data and store them locally.
		TArray<FAssetData> AssetDataList;
		AssetManager->GetPrimaryAssetDataList(UTileDataAsset::StaticClass()->GetFName(), AssetDataList);

		// Filter the asset data tags for the target tileset.
		// If found, add the asset data to the tile ID list.
		for (const FAssetData& AssetData : AssetDataList)
		{
			FGameplayTag AssetDataTag;
			FString AssetDataRawString;

			if (AssetData.GetTagValue(GET_MEMBER_NAME_CHECKED(UTileDataAsset, Tileset), AssetDataRawString))
			{
				AssetDataTag.FromExportString(AssetDataRawString);

				if (AssetDataTag == Tileset)
				{
					TileAssetPrimaryIDs.Emplace(AssetData.GetPrimaryAssetId());
				}
			}
		}
	}
}

void UGenerationSubsystem::AssetLoadCompleted()
{
	check(AssetLoadHandle.IsValid());
	check(AssetLoadHandle->HasLoadCompleted());

	InitializeTileSelectors();
	// GenerateGoldenPath();

	UE_LOG(LogTemp, Log, TEXT("Finished pre-generation at %s"), *FDateTime::UtcNow().ToString());
}

void UGenerationSubsystem::InitializeTileSelectors()
{
	for (FTileSelector& Selector : TileSelectors)
	{
		Selector.TileDataAssets.Empty();
		Selector.FilterLength = 0;
		Selector.IgnoreLength = 0;
	}

	TArray<UObject*> RawAssetBuffer;
	AssetLoadHandle->GetLoadedAssets(RawAssetBuffer);

	for (UObject* RawAsset : RawAssetBuffer)
	{
		if (UTileDataAsset* TileDataAsset = CastChecked<UTileDataAsset>(RawAsset))
		{
			Selector(TileDataAsset->Scheme).TileDataAssets.Emplace(TileDataAsset);
		}
	}
}

void UGenerationSubsystem::ReleaseGeneration()
{
	for (FTileSelector& Selector : TileSelectors)
	{
		// Release data asset pointers.
		Selector.TileDataAssets.Empty();
	}

	if (AssetLoadHandle.IsValid())
	{
		// Release tile data assets.
		AssetLoadHandle->ReleaseHandle();
		AssetLoadHandle.Reset();
	}

	// Clear local asset list.
	TileAssetPrimaryIDs.Empty();
}

bool UGenerationSubsystem::FTileSelector::GetRandomTile(const FRandomStream& InRandom, UTileData& OutTileData, int32& OutIndex) const
{
	if (TileDataAssets.Num() - FilterLength <= IgnoreLength)
	{
		return false;
	}

	OutIndex = InRandom.RandRange(IgnoreLength, TileDataAssets.Num() - FilterLength - 1);
	OutTileData = TileDataAssets[OutIndex];
	return true;
}

void UGenerationSubsystem::FTileSelector::AcceptTile(int32 Index, int32 MaxIgnoreLength)
{
	check(IgnoreLength <= Index && Index <= TileDataAssets.Num() - FilterLength - 1);

	// The ignore filter is at the front of the array, so we can move an element there by swapping
	// it and the first non-filtered element and incrementing the filter length.
	TileDataAssets.Swap(IgnoreLength, Index);
	++IgnoreLength;

	while (IgnoreLength > MaxIgnoreLength)
	{
		// If the ignore would overflow, the oldest ignored element needs to be released. Ignored
		// elements are ordered chronologically, so we need to swap out the first element.
		TileDataAssets.Swap(0, IgnoreLength);
		--IgnoreLength;

		// Now the newest element is in the wrong place, so it must be shifted up the array.
		for (int i = 0; i < IgnoreLength - 1; i++)
		{
			TileDataAssets.Swap(i, i + 1);
		}
	}

	// Reset filter.
	FilterLength = 0;
}

void UGenerationSubsystem::FTileSelector::RejectTile(int32 Index)
{
	check(IgnoreLength <= Index && Index <= TileDataAssets.Num() - FilterLength - 1);

	// The selection filter is at the back of the array, so we can move an element there by
	// swapping it and the last non-filtered element and incrementing the filter length.
	TileDataAssets.Swap(Index, TileDataAssets.Num() - FilterLength - 1);
	++FilterLength;
}
