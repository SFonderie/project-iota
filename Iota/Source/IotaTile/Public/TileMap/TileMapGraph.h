// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileData/TilePlan.h"
#include "TileMap/TileGraphBase.h"

class ATileDoorBase;

/** Tracks an active doorway in the tile map graph. */
struct IOTATILE_API FTileDoor
{
	/** Object pointer to the actual door actor. */
	TObjectPtr<ATileDoorBase> DoorActor;

	/** True if the door connects to a terminal. */
	bool bTerminal = false;

	/** Cleans up the door actor. */
	~FTileDoor();
};

/** Represents tile maps as undirected graphs where tiles are nodes and doors are edges. */
class IOTATILE_API FTileMapGraph : public TTileGraphBase<FTilePlan, FTileDoor>
{

public:

	/**
	 * Copies the tile plans stored within each graph node into the provided array.
	 *
	 * @param OutTilePlans Array in which to place the graph tile plans.
	 */
	void GetTilePlans(TArray<FTilePlan>& OutTilePlans) const;
};
