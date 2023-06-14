// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenAsyncAction.h"
#include "TileGen/TileGenWorker.h"
#include "TileData/TileDataAsset.h"
#include "Engine/AssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileGenAsyncAction)

UTileGenAsyncAction* UTileGenAsyncAction::GenerateLevel(const UObject* WorldContext, const FTileGenParams& Params)
{
	UTileGenAsyncAction* NewAction = NewObject<UTileGenAsyncAction>();
	NewAction->RegisterWithGameInstance(WorldContext);
	NewAction->Params = Params;
	return NewAction;
}

void UTileGenAsyncAction::Activate()
{
	UAssetManager& AssetManager = UAssetManager::Get();
	TileAssetList.Empty();

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

			if (AssetDataTag == Params.Tileset)
			{
				TileAssetList.Emplace(AssetData.GetPrimaryAssetId());
			}
		}
	}

	// First asynchronous phase: executing is passed to the streamable manager to load in requested
	// tile data assets. If no loading is necessary, the delegate will fire immediately.
	FStreamableDelegate Callback = FStreamableDelegate::CreateUObject(this, &UTileGenAsyncAction::StartWorkerThread);
	AssetLoadingHandle = AssetManager.LoadPrimaryAssets(TileAssetList, TArray<FName>(), Callback);
}

void UTileGenAsyncAction::Cancel()
{
	Super::Cancel();

	if (AssetLoadingHandle.IsValid())
	{
		AssetLoadingHandle->CancelHandle();
		AssetLoadingHandle.Reset();
	}

	if (GenerationWorker.IsValid())
	{
		GenerationWorker.Reset();
	}
}

void UTileGenAsyncAction::GetPlan(TArray<FTilePlan>& OutPlan) const
{
	if (GenerationWorker.IsValid())
	{
		GenerationWorker->GetPlan(OutPlan);
	}
}

void UTileGenAsyncAction::GetBounds(TArray<FTileBound>& OutBounds) const
{
	if (GenerationWorker.IsValid())
	{
		GenerationWorker->GetPlan(OutBounds);
	}
}

void UTileGenAsyncAction::GetPortals(TArray<FTilePortal>& OutPortals) const
{
	if (GenerationWorker.IsValid())
	{
		GenerationWorker->GetPlan(OutPortals);
	}
}

void UTileGenAsyncAction::StartWorkerThread()
{
	// Create the worker, which actually handles most of the generation. 
	// See the Tile Gen Worker class for the actual generation code.
	GenerationWorker = MakeShared<FTileGenWorker>(Params, TileAssetList);

	// Bind a callback to the generation worker and actually start it up.
	if (GenerationWorker.IsValid())
	{
		GenerationWorker->OnExit.BindUObject(this, &UTileGenAsyncAction::NotifyComplete);
		GenerationWorker->Start();
	}

	// Creating the worker thread copies all loaded assets into a thread-local cache.
	// The assets can therefore be unloaded since they have served their purpose.
	if (AssetLoadingHandle.IsValid())
	{
		AssetLoadingHandle.Reset();
	}

	// Unloading assets only works if the assets aren't referenced anywhere else.
	// Since the handle was released, they should now be isolated.
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.UnloadPrimaryAssets(TileAssetList);
}

void UTileGenAsyncAction::NotifyComplete()
{
	if (GenerationWorker.IsValid() && GenerationWorker->IsComplete())
	{
		OnGenerationComplete.Broadcast();
		return;
	}

	OnGenerationFailure.Broadcast();
}