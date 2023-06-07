// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "TileGenAsyncAction.generated.h"

struct FTileGenParams;

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

	/** Executes if the generation process fails. */
	UPROPERTY(BlueprintAssignable, DisplayName = "Generation Failure")
	FTileGenAsyncEvent OnGenerationFailure;

	/** Initiates generation. */
	virtual void Activate() override;

	/** Closes generation. */
	virtual void Cancel() override;
};
