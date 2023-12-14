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

	// Load all asset data matching the tile data asset type into a list.
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
				ActionAssetList.Emplace(AssetData.GetPrimaryAssetId());
			}
		}
	}

	// Add each asset type associated with the parameter actor types into the asset list as well.
	// FTileGenAction handles loading these types because doing so elsewhere would be cumbersome.
	for (const FPrimaryAssetType& AssetActor : Params.AssetActors)
	{
		TArray<FAssetData> AssetActorData;
		AssetManager.GetPrimaryAssetDataList(AssetActor, AssetActorData);

		for (const FAssetData& AssetData : AssetActorData)
		{
			ActionAssetList.Emplace(AssetData.GetPrimaryAssetId());
		}
	}

	// Load the compiled list of assets and invoke the next phase when complete. Note that the 
	// Asset Manager will keep the assets loaded until they are manually released, so the action
	// will need to unload them prior to destruction.
	FStreamableDelegate Callback = FStreamableDelegate::CreateRaw(this, &FTileGenAction::NotifyAssetsLoaded);
	ActionAssetHandle = AssetManager.LoadPrimaryAssets(ActionAssetList, TArray<FName>(), Callback);
}

FTileGenAction::~FTileGenAction()
{
	// NotifyAssetsLoaded handles releasing Asset Manager resources and clears the handle when it
	// completes. If the handle is still valid, therefore, NotifyAssetsLoaded has not run yet and
	// will be called from a dangling pointer.
	if (ActionAssetHandle.IsValid())
	{
		// Cancel the data asset handle so that it does not try to execute NotifyAssetsLoaded on
		// the soon-to-be-deleted action, as doing so would cause an access violation.
		ActionAssetHandle->CancelHandle();
	}

	// Attempt to release all requested assets so that their memory can be reclaimed.
	// Do this regardless of load state since assets might be partially loaded.
	UAssetManager::Get().UnloadPrimaryAssets(ActionAssetList);
}

void FTileGenAction::NotifyAssetsLoaded()
{
	UAssetManager& AssetManager = UAssetManager::Get();

	// Create an array of loaded tile data assets.
	TArray<UTileDataAsset*> TileDataAssets;

	// Filter and extract tile data assets from the assets loaded by the action and place them in
	// an array that can then be fed to the generation worker.
	for (const FPrimaryAssetId& AssetID : ActionAssetList)
	{
		if (AssetID.PrimaryAssetType == UTileDataAsset::StaticClass()->GetFName())
		{
			TileDataAssets.Emplace(AssetManager.GetPrimaryAssetObject<UTileDataAsset>(AssetID));
		}
	}

	// Create the generation worker, which handles the rest of the process.
	// Doing so also starts the worker, so the action is done running for now.
	AsyncWorker = MakeShared<FTileGenWorker>(Params, TileDataAssets, OnComplete);

	// Clear the handle so that the destructor knows the action is safe.
	ActionAssetHandle.Reset();
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
