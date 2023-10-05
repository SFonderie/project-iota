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

void FTileMapGraph::GetTilePlans(TArray<FTilePlan>& OutTilePlans) const
{
	OutTilePlans.Empty(GetSize());

	for (int32 Index = 0; Index < GetSize(); Index++)
	{
		OutTilePlans.Emplace(GetNodeData(Index));
	}
}
