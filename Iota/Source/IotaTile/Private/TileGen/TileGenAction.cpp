// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenAction.h"
#include "TileGen/TileGenWorker.h"
#include "TileGen/TileGraphPlan.h"
#include "TileData/TileDataAsset.h"
#include "TileData/TilePlan.h"
#include "Engine/AssetManager.h"

FTileGenAction::FTileGenAction(const FTileGenParams& InParams, const FSimpleDelegate& InDelegate)
	: Params(InParams)
	, OnComplete(InDelegate)
{
	UAssetManager& AssetManager = UAssetManager::Get();

	// Load all asset registry data about tile data assets into a list so that it can be filtered.
	TArray<FAssetData> TileAssetData;
	AssetManager.GetPrimaryAssetDataList(UTileDataAsset::StaticClass()->GetFName(), TileAssetData);

	// Search the asset tags on each asset for a tileset tag matching the parameter value.
	// Add any matching assets to the action list via their Asset ID value.
	for (const FAssetData& AssetData : TileAssetData)
	{
		FGameplayTag AssetDataTag;
		FString AssetDataRawString;

		if (AssetData.GetTagValue(GET_MEMBER_NAME_CHECKED(UTileDataAsset, Tileset), AssetDataRawString))
		{
			AssetDataTag.FromExportString(AssetDataRawString);

			if (AssetDataTag == Params.Tileset)
			{
				TileAssetList.Emplace(AssetData.GetPrimaryAssetId());
			}
		}
	}

	// Load the compiled list of tile data assets and invoke the next phase when complete. Note
	// that the Asset Manager will keep the assets loaded until they are manually released, so the
	// action will need to unload them prior to destruction.
	FStreamableDelegate Callback = FStreamableDelegate::CreateRaw(this, &FTileGenAction::NotifyAssetsLoaded);
	DataAssetHandle = AssetManager.LoadPrimaryAssets(TileAssetList, TArray<FName>(), Callback);
}

FTileGenAction::~FTileGenAction()
{
	// NotifyAssetsLoaded handles releasing Asset Manager resources and clears the handle when it
	// completes. If the handle is still valid, therefore, NotifyAssetsLoaded has not run yet and
	// resources need to be cleared manually.
	if (DataAssetHandle.IsValid())
	{
		// Cancel the data asset handle so that it does not try to execute NotifyAssetsLoaded on
		// the soon-to-be-deleted action, as doing so would cause an access violation.
		DataAssetHandle->CancelHandle();

		// If the tile asset list is not manually unloaded it will persist in memory forever.
		UAssetManager::Get().UnloadPrimaryAssets(TileAssetList);
	}
}

void FTileGenAction::NotifyAssetsLoaded()
{
	UAssetManager& AssetManager = UAssetManager::Get();

	// Create an array of loaded assets.
	TArray<UTileDataAsset*> TileDataAssets;

	// Access each loaded tile data asset and store them in an array.
	for (const FPrimaryAssetId& AssetID : TileAssetList)
	{
		TileDataAssets.Emplace(AssetManager.GetPrimaryAssetObject<UTileDataAsset>(AssetID));
	}

	// Create the generation worker, which handles the rest of the process.
	// Doing so also starts the worker, so the action is done running for now.
	AsyncWorker = MakeShared<FTileGenWorker>(Params, TileDataAssets, OnComplete);

	// Now that the generation worker exists the tile data assets can be safely unloaded.
	// We need to do this because the otherwise the assets will persist in memory forever.
	AssetManager.UnloadPrimaryAssets(TileAssetList);

	// Empty the handle to indicate to other methods that resources have been dumped.
	DataAssetHandle.Reset();
}

void FTileGenAction::Regenerate()
{
	if (CanAccess())
	{
		AsyncWorker->Start();
	}
}

bool FTileGenAction::CanAccess() const
{
	return AsyncWorker.IsValid() && AsyncWorker->bCanAccess;
}

bool FTileGenAction::IsMapValid() const
{
	return CanAccess() && Params.Length <= AsyncWorker->TileMap.Num();
}

const TArray<FTileGraphPlan>* FTileGenAction::GetTileMap() const
{
	return CanAccess() ? &AsyncWorker->TileMap : nullptr;
}
