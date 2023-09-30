// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TileSubsystem.generated.h"

class UTilePlanStream;

struct FTilePlan;

/** Public interface for tile map generation features. */
UCLASS()
class IOTATILE_API UTileSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	/** Releases subsystem resources. */
	virtual void Deinitialize() override;

	/**
	 * Dynamically streams a tile map into the subsystem world, using array indices to assign each
	 * tile a unique identifier. Calling this method while there is another tile map active in the
	 * world will cause the active map to be discarded in favor of the new one.
	 *
	 * Note that each client must invoke this method to stream a tile map over a network.
	 *
	 * @param TileMap Tile plan array to stream into the world.
	 */
	UFUNCTION(BlueprintCallable, Category = "Tile|Subsystem")
	void SetActiveTileMap(const TArray<FTilePlan>& NewTileMap);

private:

	/** Tracks all tile level streams active in the current world. */
	UPROPERTY()
	TArray<TObjectPtr<UTilePlanStream>> ActiveStreams;

	/** Tracks how many tile maps have been streamed into the world. */
	int32 NewMapCount = 0;
};
