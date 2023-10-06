// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileMap/TileMapGraph.h"
#include "TileMap/TileDoorBase.h"

FTileDoor::~FTileDoor()
{
	if (IsValid(DoorActor))
	{
		DoorActor->Destroy();
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

void FTileMapGraph::GetTilePlans(TArray<FTilePlan>& OutTilePlans) const
{
	OutTilePlans.Empty(GetSize());

	for (int32 Index = 0; Index < GetSize(); Index++)
	{
		OutTilePlans.Emplace(GetNodeData(Index));
	}
}
