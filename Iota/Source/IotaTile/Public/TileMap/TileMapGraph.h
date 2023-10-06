// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileData/TilePlan.h"
#include "IotaCore/GraphBase.h"

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

struct FTileDoorSpawner
{

};

/** Represents tile maps as undirected graphs where tiles are nodes and doors are edges. */
class IOTATILE_API FTileMapGraph : public TGraphBase<FTilePlan, FTileDoor>
{

public:

	/** Empties the tile map graph and removes all tracked door actors. */
	virtual void Empty() override;

	/**
	 * Copies the tile plans stored within each graph node into the provided array.
	 *
	 * @param OutTilePlans Array in which to place the graph tile plans.
	 */
	void GetTilePlans(TArray<FTilePlan>& OutTilePlans) const;

private:

	TArray<FTileDoorSpawner> DoorRequests;

	/**
	 * Door seals are tile door actors used to seal off any portals left vacant after generation
	 * completes. Since they do not connect to another tile, they cannot be stored in the graph
	 * and must instead be tracked alongside it.
	 */
	TArray<TObjectPtr<ATileDoorBase>> DoorSeals;
};
