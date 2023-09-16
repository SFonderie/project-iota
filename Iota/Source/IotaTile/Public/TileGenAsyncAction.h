// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "TileData/TileGenParams.h"
#include "TileGenAsyncAction.generated.h"

struct FPrimaryAssetId;
struct FStreamableHandle;

class ATileGenDoor;
class FTileGenWorker;
class ULevelStreaming;
class UWorld;

/** Manages the generation and loading of a complete tile level. */
UCLASS()
class IOTATILE_API UTileGenAsyncAction : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:

	/**
	 * Generates a new tile level and loads it into the world using the given parameters.
	 *
	 * @param WorldContextObject World context object used to determine the level world.
	 * @param Parameters Generation parameters used to characterize the new level.
	 * @return Generator handle for managing the new level.
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelGeneration", meta = (BlueprintInternalUseOnly = true, WorldContext = "WorldContextObject"))
	static UTileGenAsyncAction* GenerateTileLevel(UObject* WorldContextObject, const FTileGenParams& Parameters);

	/** Blueprint-accessible generation callback event. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTileGenAsyncEvent);

	/** Executes when the generation process successfully completes. */
	UPROPERTY(BlueprintAssignable, DisplayName = "Generation Complete")
	FTileGenAsyncEvent OnGenerationComplete;

	/** Executes when the generation process fails or stops early. */
	UPROPERTY(BlueprintAssignable, DisplayName = "Generation Failed")
	FTileGenAsyncEvent OnGenerationFailure;

	/** Initiates the generation process. */
	virtual void Activate() override;

	/** Cancels the generation process and unloads any loaded tiles. */
	virtual void Cancel() override;

private:

	/** Invoked by the activate method to begin loading assets. */
	void NotifyProcessStart();

	/** Invoked by the asset manager to begin the generation worker thread. */
	void NotifyAssetsLoaded();

	/** Invoked by the generation worker to begin the level loading process. */
	void NotifyWorkerComplete();

	/** Adds doors to the level plan. */
	void GenerateDoors();

private:

	/** Level world context. */
	TObjectPtr<UWorld> World;

	/** Generation parameters. */
	FTileGenParams Params;

	/** World ID number. */
	int32 SubsystemID = 0;

	/** Indicates that the generator has activated. */
	bool bHasActivated = false;

	/** Unloaded tile data asset list. */
	TArray<FPrimaryAssetId> TileAssetList;

	/** Handle used to track data asset loading. */
	TSharedPtr<FStreamableHandle> DataAssetHandle;

	/** Handle used to track the generation worker. */
	TSharedPtr<FTileGenWorker> GenerationWorker;

	/** Loaded tile level list. */
	TArray<ULevelStreaming*> TileStreams;

	/** Door actors list. */
	TArray<ATileGenDoor*> DoorActors;
};
