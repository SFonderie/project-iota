// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileData/TilePlan.h"
#include "IotaCore/GraphBase.h"
#include "TileMap/TileDoorBase.h"

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
class IOTATILE_API FTileMapGraph : public TGraphBase<FTilePlan, FTileDoor>
{

public:

	/** Constructs a new map graph within the provided world context. */
	FTileMapGraph(UWorld* InWorld);

	/** Empties the tile map graph and removes all tracked door actors. */
	virtual void Empty() override;

	/** @return Tile map graph world context pointer. */
	UWorld* GetWorld() const;

	/** @return True if the map graph is marked live. */
	bool IsLive() const;

	/**
	 * Marks or unmarks the tile map graph as live. Doing so automatically handles any door
	 * requests stored on the map graph and spawns them into the world.
	 *
	 * @param bLive New live value.
	 */
	void SetLive(bool bNewLive = true);

	/**
	 * Copies the tile plans stored within each graph node into the provided array.
	 *
	 * @param OutTilePlans Array in which to place the graph tile plans.
	 */
	void GetPlans(TArray<FTilePlan>& OutTilePlans) const;

	/**
	 * Requests a new door spawn. Door spawn requests will wait until the map graph is marked live,
	 * at which point they will automatically spawn into the graph world context.
	 *
	 * @param DoorClass Door subtype to spawn.
	 * @param DoorTransform Transform to apply to the new door.
	 * @param OwnerEdge Tile map graph edge with which to track the new door. Leave null for seals.
	 */
	void RequestDoor(const TSubclassOf<ATileDoorBase>& DoorClass, const FTransform& DoorTransform, FTileDoor* OwnerEdge = nullptr);

private:

	/** Graph world context. */
	TObjectPtr<UWorld> World;

	/**
	 * Door seals are tile door actors used to seal off any portals left vacant after generation
	 * completes. Since they do not connect to another tile, they cannot be stored in the graph
	 * and must instead be tracked alongside it.
	 */
	TArray<TObjectPtr<ATileDoorBase>> DoorSeals;

	/** Stores SpawnActor parameters for later use. */
	struct FTileDoorRequest
	{
		TSubclassOf<ATileDoorBase> DoorClass;
		FTransform DoorTransform;
		FTileDoor* OwnerEdge = nullptr;
	};

	/** Stores door spawn data so that doors can be spawned into the rest of the level. */
	TArray<FTileDoorRequest> DoorRequests;

	/** True once the graph loads. Door requests will auto-spawn while this is true. */
	bool bLiveGraph = false;
};
