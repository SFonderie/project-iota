// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileSubsystem.h"
#include "TileData/TilePlan.h"
#include "TileGen/TileGenAction.h"
#include "TileGen/TileGraphPlan.h"
#include "TileMap/TileDoorBase.h"
#include "TileMap/TilePlanStream.h"
#include "IotaCore/ActorTable.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileSubsystem)

bool UTileSubsystem::CanGenerate() const
{
	return GetWorld()->GetNetMode() < NM_Client;
}

void UTileSubsystem::MakeNewTileMap(const FTileGenParams& Parameters, const FGeneratorDelegate& OnComplete)
{
	if (CanGenerate())
	{
		FSimpleDelegate Callback = FSimpleDelegate::CreateUObject(this, &UTileSubsystem::NotifyGeneratorComplete);

		// Create a new generation action and store the provided delegate as a member. Doing so
		// automatically destroys the previous action instance and dumps its resources.
		GeneratorAction = MakeShared<FTileGenAction>(Parameters, Callback);
		OnGeneratorComplete = OnComplete;
	}
}

void UTileSubsystem::NotifyGeneratorComplete()
{
	if (GeneratorAction.IsValid())
	{
		if (GeneratorAction->IsMapValid())
		{
			MapGraph.Empty();
			MapCount++;

			TActorTable<FIntPoint, ATileDoorBase> DoorTable;

			// Collect all door subtypes that belong to the tileset and store them in the table.
			// Use door size as the key for each entry.
			DoorTable.CollectWithCategory(GeneratorAction->Params.Tileset, [](ATileDoorBase* AssetObject)
			{
				return AssetObject->DoorSize;
			});

			for (const FTileGraphPlan& GraphPlan : *GeneratorAction->GetTileMap())
			{
				int32 NewNode = MapGraph.MakeNode(GraphPlan);

				if (0 <= GraphPlan.GetConnection())
				{
					MapGraph.MakeEdge(NewNode, GraphPlan.GetConnection());

					// CREATE STANDARD DOOR SPAWN REQUEST
				}

				for (int32 Index = 1; Index < GraphPlan.Portals.Num(); Index++)
				{
					if (GraphPlan.IsOpenPortal(Index))
					{
						// CREATE SEALED DOOR SPAWN REQUEST
					}
				}
			}

			if (OnGeneratorComplete.IsBound())
			{
				OnGeneratorComplete.Execute();
			}
		}
		else
		{
			GeneratorAction->Regenerate();
		}
	}
}

void UTileSubsystem::GetNewTileMap(TArray<FTilePlan>& OutTileMap, int32& OutMapIndex) const
{
	OutTileMap.Empty();
	OutMapIndex = 0;

	if (GeneratorAction.IsValid() && GeneratorAction->IsMapValid())
	{
		MapGraph.GetTilePlans(OutTileMap);
		OutMapIndex = MapCount;
	}
}

void UTileSubsystem::SetActiveTileMap(const TArray<FTilePlan>& NewTileMap, int32 MapIndex)
{
	// Ensure that the map index exceeds the active index. Doing so guarantees that each new map
	// index is always unique and thus eliminates the risk of name overlaps.
	if (MapIndex <= ActiveIndex)
	{
		return;
	}

	// Update the active index.
	ActiveIndex = MapIndex;

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
