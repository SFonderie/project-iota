// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TileSubsystem.generated.h"

class FTileGenAction;
class UTilePlanStream;

struct FTileGenParams;
struct FTilePlan;

/** Blueprint-accessible delegate used for generator events. */
DECLARE_DYNAMIC_DELEGATE(FGeneratorDelegate);

/** Public interface for tile map generation features. */
UCLASS()
class IOTATILE_API UTileSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	/**
	 * Determines if the subsystem has the authority needed to generate tile maps. Authority is
	 * conferred via the subsystem world - if the world is running on a server, then it has the
	 * authority to generate.
	 *
	 * @return True if the subsystem is running on a server.
	 */
	bool CanGenerate() const;

	/**
	 * Attempts to start a new tile map generation action and stores the result on the subsystem.
	 * In order to generate, the subsystem must call this method from a server.
	 *
	 * @param Parameters Generation parameters used to create the new tile map.
	 * @param OnComplete Delegate invoked when the generator action completes.
	 * @return True if an action was created successfully.
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile|Subsystem")
	bool GenerateNewMap(const FTileGenParams& Parameters, const FGeneratorDelegate& OnComplete);

	/**
	 * Attempts to access the tile map last generated via the subsystem. If there is no active
	 * generator, or the generator has yet to complete, then this method will return false.
	 *
	 * @param OutTileMap Current generated tile map stored on the subsystem.
	 * @param OutMapIndex Server map index for the generated tile map.
	 * @return True if there was a tile map to export.
	 */
	UFUNCTION(BlueprintPure = false, Category = "Tile|Subsystem")
	bool GetTileMap(TArray<FTilePlan>& OutTileMap, uint8& OutMapIndex) const;

	/**
	 * Dynamically streams a tile map into the subsystem world, using array indices to assign each
	 * tile a unique identifier. Calling this method while there is another tile map active in the
	 * world will cause the active map to be discarded in favor of the new one.
	 *
	 * When streaming on a server, each client must stream the map using the same map index
	 * independently to ensure replication. Standalone users can ignore this value.
	 *
	 * @param TileMap Tile plan array to stream into the world.
	 * @param MapIndex Server map index used to ensure safe replication.
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile|Subsystem")
	void SetActiveTileMap(const TArray<FTilePlan>& NewTileMap, uint8 MapIndex);

private:

	/** Invoked when the generator action completes. */
	void NotifyGeneratorComplete();

private:

	/** Tracks the active generator action. */
	TSharedPtr<FTileGenAction> GeneratorAction;

	/** Stores the delegate invoked on generator completion. */
	FGeneratorDelegate OnGeneratorComplete;

	/** Tracks the number of generated tile maps produced by the subsystem. */
	int32 MapCount = 0;

	/** Tracks all tile level streams active in the current world. */
	UPROPERTY()
	TArray<TObjectPtr<UTilePlanStream>> ActiveStreams;
};
