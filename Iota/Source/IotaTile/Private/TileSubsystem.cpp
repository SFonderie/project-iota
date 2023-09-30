// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileSubsystem.h"
#include "TileData/TilePlan.h"
#include "TileMap/TilePlanStream.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileSubsystem)

void UTileSubsystem::Deinitialize()
{

}

void UTileSubsystem::SetActiveTileMap(const TArray<FTilePlan>& NewTileMap)
{
	// Mark the active tile map as requesting an unload and removal. Doing so is asynchronous, so
	// the active map will remain in-world when the new map begins its load.
	for (UTilePlanStream* Stream : ActiveStreams)
	{
		Stream->SetIsRequestingUnloadAndRemoval(true);
	}

	// Empty and reserve the active array.
	ActiveStreams.Empty(NewTileMap.Num());

	for (int32 PlanIndex = 0; PlanIndex < NewTileMap.Num(); PlanIndex++)
	{
		// Generate a unique identifier using the subsystem counter and the plan index. The counter
		// ensures that there will be no conflicts between the active map and the new map.
		FString PlanName = FString::Printf(TEXT("Tile_%i_%i"), NewMapCount, PlanIndex);

		// Attempt to stream in the new tile level and track it if successful.
		if (UTilePlanStream* NewStream = UTilePlanStream::StreamInstance(GetWorld(), NewTileMap[PlanIndex], PlanName))
		{
			ActiveStreams.Emplace(NewStream);
		}
	}

	// Increment the tracker so that the next map switch can happen safely.
	NewMapCount++;
}
