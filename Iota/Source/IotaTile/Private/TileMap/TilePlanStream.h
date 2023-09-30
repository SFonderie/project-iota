// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelStreamingDynamic.h"
#include "TilePlanStream.generated.h"

struct FTilePlan;

/** Dynamic level stream extension specialized for tile map loads. */
UCLASS()
class IOTATILE_API UTilePlanStream : public ULevelStreamingDynamic
{
	GENERATED_BODY()

public:

	/**
	 * Streams a new tile level into the provided world using the given tile plan as a template.
	 * Dynamic level streams are client-side, so each client must individually invoke this method
	 * with the same name value to ensure each stream replicates the server correctly.
	 *
	 * @param World Stream the new tile level into this world.
	 * @param BasePlan Tile plan describing the tile to stream.
	 * @param PlanName Replication level name override.
	 * @return Streaming handle, or null if unsuccessful.
	 */
	static UTilePlanStream* StreamInstance(UWorld* World, const FTilePlan& BasePlan, const FString& PlanName);
};
