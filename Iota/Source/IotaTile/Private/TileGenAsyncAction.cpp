// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGenAsyncAction.h"
#include "Engine/AssetManager.h"
#include "TileData/TileDataAsset.h"
#include "TileGenWorker.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileGenAsyncAction)

UTileGenAsyncAction* UTileGenAsyncAction::StartGeneration(const UObject* WorldContextObject, const FTileGenParams& Parameters)
{
	UTileGenAsyncAction* NewAction = NewObject<UTileGenAsyncAction>();
	NewAction->RegisterWithGameInstance(WorldContextObject);
	NewAction->WorldContextObject = WorldContextObject;
	NewAction->Parameters = Parameters;
	return NewAction;
}

void UTileGenAsyncAction::Activate()
{
	UAssetManager& AssetManager = UAssetManager::Get();

	// Load all tile data asset information into a list so it can be filtered.
	TArray<FAssetData> TileAssetData;
	AssetManager.GetPrimaryAssetDataList(UTileDataAsset::StaticClass()->GetFName(), TileAssetData);

	// Filter the asset data tags for the target tileset.
	// If found, add the asset data to the tile ID list.
	for (const FAssetData& AssetData : TileAssetData)
	{
		FGameplayTag AssetDataTag;
		FString AssetDataRawString;

		if (AssetData.GetTagValue(GET_MEMBER_NAME_CHECKED(UTileDataAsset, Tileset), AssetDataRawString))
		{
			AssetDataTag.FromExportString(AssetDataRawString);

			if (AssetDataTag == Parameters.Tileset)
			{
				TileAssetList.Emplace(AssetData.GetPrimaryAssetId());
			}
		}
	}

	// First asynchronous phase: executing is passed to the streamable manager to load in requested
	// tile data assets. If no loading is necessary, the delegate will fire immediately.
	FStreamableDelegate Callback = FStreamableDelegate::CreateUObject(this, &UTileGenAsyncAction::NotifyAssetsLoaded);
	DataAssetHandle = AssetManager.LoadPrimaryAssets(TileAssetList, TArray<FName>(), Callback);
}

void UTileGenAsyncAction::Cancel()
{
	Super::Cancel();

	if (DataAssetHandle.IsValid())
	{
		DataAssetHandle->CancelHandle();
		DataAssetHandle.Reset();
	}

	if (GenerationWorker.IsValid())
	{
		GenerationWorker.Reset();
	}
}

void UTileGenAsyncAction::NotifyAssetsLoaded()
{
	// Create the worker, which actually handles most of the generation. 
	// See the Tile Gen Worker class for the actual generation code.
	GenerationWorker = MakeShared<FTileGenWorker>(Parameters, TileAssetList);

	// Bind a callback to the generation worker and actually start it up.
	if (GenerationWorker.IsValid())
	{
		GenerationWorker->OnExit.BindUObject(this, &UTileGenAsyncAction::NotifyWorkerComplete);
		GenerationWorker->Start();
	}

	// Creating the worker thread copies all loaded assets into a thread-local cache.
	// The assets can therefore be unloaded since they have served their purpose.
	if (DataAssetHandle.IsValid())
	{
		DataAssetHandle.Reset();
	}

	// Loaded primary assets must also be manually unloaded.
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.UnloadPrimaryAssets(TileAssetList);
}

void UTileGenAsyncAction::NotifyWorkerComplete()
{
	if (GenerationWorker.IsValid() && GenerationWorker->IsComplete())
	{
		OnGenerationComplete.Broadcast();
		return;
	}

	OnGenerationFailure.Broadcast();
}
