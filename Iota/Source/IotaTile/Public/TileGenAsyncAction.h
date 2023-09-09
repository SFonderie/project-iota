// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "TileData/TileGenParams.h"
#include "TileGenAsyncAction.generated.h"

struct FStreamableHandle;
class FTileGenWorker;

struct FTilePlan;
struct FTileBound;
struct FTilePortal;

/** Asynchronous action that manages level generation. */
UCLASS()
class IOTATILE_API UTileGenAsyncAction : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:

	/**
	 * Starts level generation using the provided parameters.
	 *
	 * @param WorldContext World context object.
	 * @param Parameters Level generation parameters.
	 * @return Level generation handle.
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelGeneration", meta = (BlueprintInternalUseOnly = true, WorldContext = "WorldContextObject"))
	static UTileGenAsyncAction* StartGeneration(UObject* WorldContextObject, const FTileGenParams& Parameters);

	/** Blueprint-accessible generation callback event. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTileGenAsyncEvent);

	/** Executes when the generation process successfully completes. */
	UPROPERTY(BlueprintAssignable, DisplayName = "Generation Complete")
	FTileGenAsyncEvent OnGenerationComplete;

	/** Executes when the generation process fails or stops early. */
	UPROPERTY(BlueprintAssignable, DisplayName = "Generation Failed")
	FTileGenAsyncEvent OnGenerationFailure;

	/** Initiates generation. */
	virtual void Activate() override;

	/** Closes generation. */
	virtual void Cancel() override;

	/**
	 * Attempts to output the generated list of level plans, bounds, and portals. If generation
	 * failed or has yet to complete, this method will return empty arrays.
	 * 
	 * @param TilePlans Array of generated level plans.
	 * @param TileBounds Array of generated level bounding boxes.
	 * @param TilePortals Array of generated level plan portals.
	 */
	UFUNCTION(BlueprintPure = false, Category = "LevelGeneration")
	void GetCompletePlan(TArray<FTilePlan>& TilePlans, TArray<FTileBound>& TileBounds, TArray<FTilePortal>& TilePortals) const;

private:

	/** Invoked by the asset manager to begin the generation worker thread. */
	void NotifyAssetsLoaded();

	/** Invoked by the generation worker to begin the level loading process. */
	void NotifyWorkerComplete();

private:

	/** World context object passed to level streams. */
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject;

	/** Generation parameters. */
	FTileGenParams Parameters;

	/** Unloaded tile generator list. */
	TArray<FPrimaryAssetId> TileAssetList;

	/** Handle used to track data asset loading. */
	TSharedPtr<FStreamableHandle> DataAssetHandle;

	/** Handle used to track the generation worker. */
	TSharedPtr<FTileGenWorker> GenerationWorker;
};
