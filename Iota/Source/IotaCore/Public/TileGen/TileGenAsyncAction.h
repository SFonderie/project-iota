// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "TileGen/TileGenDataStructs.h"
#include "TileGenAsyncAction.generated.h"

struct FStreamableHandle;
class FTileGenWorker;

/** Asynchronous action that wraps level generation. */
UCLASS()
class IOTACORE_API UTileGenAsyncAction : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:

	/**
	 * Starts level generation using the provided parameters.
	 *
	 * @param WorldContext World context object.
	 * @param Params Tile generation parameters.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = true, WorldContext = "WorldContext"))
	static UTileGenAsyncAction* GenerateLevel(const UObject* WorldContext, const FTileGenParams& Params);

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

	/** Ends or closes generation. */
	virtual void Cancel() override;

	/**
	 * Attempts to return the generated level plan as an array of Tile Plans. If the async action
	 * has not completed or an error was thrown, this method will return an empty array.
	 *
	 * @return Level plan array.
	 */
	UFUNCTION(BlueprintPure, Category = "Async Action")
	const TArray<FTilePlan>& GetPlan() const
	{
		return CompletePlan;
	}

	/**
	 * Attempts to return the generated level plan bounding boxes. If the async action has not
	 * completed or an error was thrown, this method will return an empty array.
	 *
	 * @return Level plan bounds array.
	 */
	UFUNCTION(BlueprintPure, Category = "Async Action")
	const TArray<FTileBound>& GetBounds() const
	{
		return PlanBounds;
	}

	/**
	 * Attempts to return the generated level plan portals. If the async action has not completed
	 * or an error was thrown, this method will return an empty array.
	 *
	 * @return Level plan portals array.
	 */
	UFUNCTION(BlueprintPure, Category = "Async Action")
	const TArray<FTilePortal>& GetPortals() const
	{
		return PlanPortals;
	}

private:

	/** Starts the Tile Gen Worker once tile loading is complete. */
	void StartWorkerThread();

	/** Completes the action once the Tile Gen Worker exits. */
	void NotifyComplete();

	/** Generation parameters. */
	FTileGenParams Params;

	/** Unloaded generator tile list. */
	TArray<FPrimaryAssetId> TileAssetList;

	/** Handle used to track tile asset loading. */
	TSharedPtr<FStreamableHandle> AssetLoadingHandle;

	/** Handle used to track the worker thread. */
	TSharedPtr<FTileGenWorker> GenerationWorker;

	/** Holds the completed level plan. */
	TArray<FTilePlan> CompletePlan;

	/** Holds the level plan bounds. */
	TArray<FTileBound> PlanBounds;

	/** Holds the level plan portals. */
	TArray<FTilePortal> PlanPortals;
};
