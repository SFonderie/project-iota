// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileSubsystem.h"
#include "TileData/TilePlan.h"
#include "TileGen/TileGenAction.h"
#include "TileGen/TileGraphPlan.h"
#include "TileMap/TilePlanStream.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileSubsystem)

bool UTileSubsystem::CanGenerate() const
{
	return GetWorld()->GetNetMode() < NM_Client;
}

bool UTileSubsystem::GenerateNewMap(const FTileGenParams& Parameters, const FGeneratorDelegate& OnComplete)
{
	if (CanGenerate())
	{
		FSimpleDelegate Callback = FSimpleDelegate::CreateUObject(this, &UTileSubsystem::NotifyGeneratorComplete);

		// Create a new generation action and store the provided delegate as a member. Doing so
		// automatically destroys the previous action instance and dumps its resources.
		GeneratorAction = MakeShared<FTileGenAction>(Parameters, Callback);
		OnGeneratorComplete = OnComplete;
		return true;
	}

	return false;
}

void UTileSubsystem::NotifyGeneratorComplete()
{
	if (CanGenerate() && GeneratorAction.IsValid())
	{
		if (GeneratorAction->IsMapValid())
		{
			// SOMETHING SOMETHING TILE GRAPH HERE

			MapCount++;
			OnGeneratorComplete.ExecuteIfBound();
		}
		else
		{
			GeneratorAction->Regenerate();
		}
	}
}

bool UTileSubsystem::GetTileMap(TArray<FTilePlan>& OutTileMap, uint8& OutMapIndex) const
{
	OutTileMap.Empty();
	OutMapIndex = 0;

	if (GeneratorAction.IsValid() && GeneratorAction->IsMapValid())
	{
		if (const TArray<FTileGraphPlan>* TileMap = GeneratorAction->GetTileMap())
		{
			for (const FTileGraphPlan& GraphPlan : *TileMap)
			{
				OutTileMap.Emplace(GraphPlan);
			}
		}

		OutMapIndex = uint8(MapCount);
		return true;
	}

	return false;
}

void UTileSubsystem::SetActiveTileMap(const TArray<FTilePlan>& NewTileMap, uint8 MapIndex)
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
		FString PlanName = FString::Printf(TEXT("Tile_%i_%i"), MapIndex, PlanIndex);

		// Attempt to stream in the new tile level and track it if successful.
		if (UTilePlanStream* NewStream = UTilePlanStream::StreamInstance(GetWorld(), NewTileMap[PlanIndex], PlanName))
		{
			ActiveStreams.Emplace(NewStream);
		}
	}
}
