// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileSubsystem.h"
#include "TileData/TilePlan.h"
#include "TileGen/TileGenAction.h"
#include "TileGen/TileGraphPlan.h"
#include "TileMap/TileDoorBase.h"
#include "TileMap/TileMapGraph.h"
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

		// Create a new tile map graph and pass in the subsystem world context. Doing so also 
		// automatically destroys the previous map graph and destroys all actors stored on it.
		MapGraph = MakeShared<FTileMapGraph>(GetWorld());
	}
}

void UTileSubsystem::NotifyGeneratorComplete()
{
	if (GeneratorAction.IsValid())
	{
		// If the generated tile map is valid and contains a complete core tile sequence then it
		// can be loaded into the subsystem map graph.
		if (GeneratorAction->IsMapValid())
		{
			// Increment the map counter.
			MapCount++;

			// Collect all door subtypes that belong to the tileset and store them in a table.
			// For each door added, use its door size as its key for the table.
			TActorTable<FIntPoint, ATileDoorBase> DoorTable(GeneratorAction->Params.Tileset, [](ATileDoorBase* AssetObject)
			{
				return AssetObject->DoorSize;
			});

			// Populate the map graph. Each graph plan can be converted into a graph node using the
			// base tile plan to fill the node data. One edge can also be added for each graph plan
			// parent connection.
			for (const FTileGraphPlan& GraphPlan : *GeneratorAction->GetTileMap())
			{
				int32 NewNode = MapGraph->MakeNode(GraphPlan);

				if (0 <= GraphPlan.GetConnection())
				{
					FTileDoor& NewDoor = MapGraph->MakeEdge(NewNode, GraphPlan.GetConnection());

					// If the current graph plan index exceeds the parameter length, then the plan
					// must represent a terminal tile.
					NewDoor.bTerminal = GeneratorAction->Params.Length <= NewNode;

					// Isolate the first portal on the plan for easy access.
					const FTileGraphPortal& Portal = *GraphPlan.Portals.GetData();

					// Calculate the door transform from the portal values.
					FRotator Rotation = FRotationMatrix::MakeFromX(Portal.Direction).Rotator();
					FTransform DoorTransform(Rotation, Portal.Location);

					// Spawn a new door, using the table to select a size-appropriate door class
					// Also pass in the calculated door transform and edge data address.
					MapGraph->RequestDoor(DoorTable.GetRandomSubtype(Portal.PlaneSize), DoorTransform, &NewDoor);
				}

				// Vacant portals represent holes in level geometry, so they need to be filled.
				for (int32 Index = 1; Index < GraphPlan.Portals.Num(); Index++)
				{
					if (GraphPlan.IsOpenPortal(Index))
					{
						// Isolate the current portal using the index value.
						const FTileGraphPortal& Portal = GraphPlan.Portals[Index];

						// Calculate the door transform from the portal values.
						FRotator Rotation = FRotationMatrix::MakeFromX(Portal.Direction).Rotator();
						FTransform DoorTransform(Rotation, Portal.Location);

						// Spawn a new door, using the table to select a size-appropriate door class
						// Also pass in the calculated door transform.
						MapGraph->RequestDoor(DoorTable.GetRandomSubtype(Portal.PlaneSize), DoorTransform);
					}
				}
			}

			// Trigger the callback delegate once the map is stored.
			if (OnGeneratorComplete.IsBound())
			{
				OnGeneratorComplete.Execute();
			}
		}

		// If the generated tile map is not valid, regenerate it and wait for the next completion.
		// Keep regenerating until a valid map is generated.
		else
		{
			GeneratorAction->Regenerate();
		}
	}
}

void UTileSubsystem::GetGraphTileMap(TArray<FTilePlan>& OutTileMap, int32& OutMapIndex) const
{
	OutTileMap.Empty();
	OutMapIndex = 0;

	if (MapGraph.IsValid() && !MapGraph->IsEmpty())
	{
		MapGraph->GetPlans(OutTileMap);
		OutMapIndex = MapCount;
	}
}

void UTileSubsystem::SetLiveTileMap(const TArray<FTilePlan>& NewTileMap, int32 MapIndex)
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

	// If the subsystem is running on a server and has a valid map graph stored within itself, then
	// mark the map graph as live to spawn in additional actors (such as doors).
	if (CanGenerate() && MapGraph.IsValid() && !MapGraph->IsEmpty())
	{
		MapGraph->SetLive();
	}
}
