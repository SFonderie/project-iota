// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "TileGen/TileGenDataStructs.h"
#include "TileGenAsyncAction.generated.h"

struct FStreamableHandle;
class FTileGenWorker;

/** Asynchronous action prepares and tracks level generation. */
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

	/** Executes when the generation process has completed. */
	UPROPERTY(BlueprintAssignable, DisplayName = "Generation Complete")
	FTileGenAsyncEvent OnGenerationComplete;

	/** Initiates generation. */
	virtual void Activate() override;

	/** Stops generation. */
	virtual void Cancel() override;

private:

	/** Starts the Tile Gen Worker once tile loading is complete. */
	void StartWorkerThread();

	/** Generation parameters. */
	FTileGenParams Params;

	/** Unloaded generator tile list. */
	TArray<FPrimaryAssetId> TileAssetList;

	/** Handle used to track tile asset loading. */
	TSharedPtr<FStreamableHandle> AssetLoadingHandle;

	/** Handle used to track the worker thread. */
	TSharedPtr<FTileGenWorker> GenerationWorker;
};
