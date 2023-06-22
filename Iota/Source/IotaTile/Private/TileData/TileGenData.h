// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileData/TileBound.h"
#include "TileData/TilePortal.h"

class UTileDataAsset;
struct FTilePlan;

/** UTileDataAsset proxy structure. */
struct FTileGenData
{
	/** Path to the actual tile level instance. */
	FSoftObjectPath Level;

	/** A list of the tile's exit portals. */
	TArray<FTilePortal> Portals;

	/** A list of the tile's collision bounds. */
	TArray<FTileBound> Bounds;

	/** Defines an empty data object. */
	FTileGenData();

	/** Copies values from the given tile data asset. */
	FTileGenData(const UTileDataAsset* DataAsset);

	/** Copies values from the given tile data object. */
	FTileGenData(const FTileGenData& TileData);
};

/** Extension of FTileGenData that includes transform and generation data. */
struct FTileGenPlan : public FTileGenData
{
	/** Tile world position. */
	FVector Position;

	/** Tile world rotation. */
	FRotator Rotation;

	/** Mask tracking portal index state. 0 values are open, 1 values are closed. */
	uint32 PortalsMask = 0;

	/** Index of the parent tile plan array element. Used to traverse the tile tree. */
	int32 ParentIndex = -1;

	/**
	 * Copies values from the given tile data object and transform to create a world tile plan.
	 *
	 * @param TileData Base tile data object with which to build the plan.
	 * @param Transform World transform to apply to the base tile data.
	 */
	FTileGenPlan(const FTileGenData& TileData, const FTransform& Transform);

	/** @return True if the portal at the given index is marked open. */
	bool IsOpenPortal(int32 Index) const;

	/** Marks the portal at the given index open. */
	void OpenPortal(int32 Index);

	/** Marks the portal at the given index closed. */
	void ClosePortal(int32 Index);

	/** @return Exports the generation plan to a tile plan. */
	FTilePlan GetTilePlan() const;
};
