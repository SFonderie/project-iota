// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TileMap/TileMapGraph.h"
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
	 * Generates a new tile map from the given parameters and stores the results on the subsystem.
	 * Invokes the provided delegate once the tile map has been stored. In order to generate a new
	 * map, the subsystem must call this method from a server.
	 *
	 * @param Parameters Generation parameters used to create the new tile map.
	 * @param OnComplete Delegate invoked when the generator action completes.
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile|Subsystem")
	void MakeNewTileMap(const FTileGenParams& Parameters, const FGeneratorDelegate& OnComplete);

	/**
	 * Returns the generated tile map currently stored on the subsystem along with a server map
	 * index for replication. If the subsystem does not contain a complete tile map, the provided
	 * parameters will be returned empty.
	 *
	 * @param OutTileMap Generated tile plan array stored on the subsystem, if one exists.
	 * @param OutMapIndex Server map index used to ensure safe replication.
	 */
	UFUNCTION(BlueprintPure = false, Category = "Tile|Subsystem")
	void GetNewTileMap(TArray<FTilePlan>& OutTileMap, int32& OutMapIndex) const;

	/**
	 * Dynamically streams a tile map into the subsystem world, using array indices to assign each
	 * tile a unique identifier. Calling this method while there is another tile map active in the
	 * world will cause the active map to be discarded if the active map index is lower than the
	 * new map index.
	 *
	 * When streaming over a network, each client must stream independently using the same map
	 * index to ensure the map replicates properly to all players.
	 *
	 * @param NewTileMap Tile plan array to stream into the world.
	 * @param MapIndex Server map index used to ensure safe replication.
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile|Subsystem")
	void SetActiveTileMap(const TArray<FTilePlan>& NewTileMap, int32 MapIndex);

private:

	/** Invoked when a generator action completes. */
	void NotifyGeneratorComplete();

private:

	/** Tracks the active generator action. */
	TSharedPtr<FTileGenAction> GeneratorAction;

	/** Stores the delegate invoked on generator completion. */
	FGeneratorDelegate OnGeneratorComplete;

	/** Stores the active tile map as a graph structure. */
	FTileMapGraph MapGraph;

	/** Tracks the number of generated tile maps produced by the subsystem. */
	int32 MapCount = 0;

	/** Tracks all tile level streams active in the current world. */
	UPROPERTY()
	TArray<TObjectPtr<UTilePlanStream>> ActiveStreams;

	/** Tracks the server index of the active tile map. */
	int32 ActiveIndex = 0;
};
