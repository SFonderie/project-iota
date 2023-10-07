// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileMap/TileMapGraph.h"
#include "Engine/World.h"

FTileDoor::~FTileDoor()
{
	if (IsValid(DoorActor))
	{
		DoorActor->Destroy();
	}
}

FTileMapGraph::FTileMapGraph(UWorld* InWorld) : World(InWorld)
{
	// Default constructor.
}

UWorld* FTileMapGraph::GetWorld() const
{
	return World;
}

bool FTileMapGraph::IsLive() const
{
	return bLiveGraph;
}

void FTileMapGraph::SetLive(bool bLive)
{
	bLiveGraph = true;

	// Graph is live; handle all door requests and spawn them into the world.
	for (const FTileDoorRequest& Request : DoorRequests)
	{
		if (UClass* DoorClass = *Request.DoorClass)
		{
			// Spawn the new door into the world but do not invoke its construction script.
			ATileDoorBase* NewDoor = World->SpawnActorDeferred<ATileDoorBase>(DoorClass, Request.DoorTransform);

			// Add the new door to the owner edge if one was provided.
			if (Request.OwnerEdge)
			{
				Request.OwnerEdge->DoorActor = NewDoor;
			}

			// Otherwise, seal the door since it has no connection.
			else
			{
				NewDoor->bIsSealed = true;
			}

			// Now that the door has been sealed, finish it.
			NewDoor->FinishSpawning(Request.DoorTransform);
		}
	}
}

void FTileMapGraph::Empty()
{
	// Smart pointers ensure that emptying the map graph will destroy all true doors via the
	// destructor method above.
	TGraphBase::Empty();

	// Door seals must be manually destroyed since they are not stored in the graph.
	for (ATileDoorBase* DoorActor : DoorSeals)
	{
		if (IsValid(DoorActor))
		{
			DoorActor->Destroy();
		}
	}

	// Drop the references to the old seals.
	DoorSeals.Empty();
}

void FTileMapGraph::GetPlans(TArray<FTilePlan>& OutTilePlans) const
{
	OutTilePlans.Empty(GetSize());

	for (int32 Index = 0; Index < GetSize(); Index++)
	{
		OutTilePlans.Emplace(GetNodeData(Index));
	}
}

void FTileMapGraph::RequestDoor(const TSubclassOf<ATileDoorBase>& DoorClass, const FTransform& DoorTransform, FTileDoor* OwnerEdge)
{
	// Package and submit a new door request.
	FTileDoorRequest NewRequest;
	NewRequest.DoorClass = DoorClass;
	NewRequest.DoorTransform = DoorTransform;
	NewRequest.OwnerEdge = OwnerEdge;
	DoorRequests.Add(NewRequest);

	// If the graph is already live, handle the new request.
	if (IsLive())
	{
		SetLive();
	}
}
