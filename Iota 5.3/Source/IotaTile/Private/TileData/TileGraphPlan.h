// Copyright Sydney Fonderie. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileData/TileBound.h"
#include "TileData/TileData.h"
#include "TileData/TileNode.h"
#include "TileData/TilePlan.h"
#include "TileData/TilePortal.h"

class UTileDataAsset;

/** Tile data extension that also manages the local area graph. */
struct FTileGraphData : public FTileData
{
	/** A list of the tile's area graph nodes. */
	TArray<FTileNode> AreaNodes;

	/** Area graph indices nearest to each portal. */
	TArray<int32> AreaExits;

	/**
	 * Defines a tile graph data structure from the provided data asset base.
	 *
	 * @param InDataAsset Tile data asset from which to build.
	 */
	FTileGraphData(const UTileDataAsset* InDataAsset);
};

/** Tile portal extension that tracks graph connections. */
struct FTileGraphPortal : public FTilePortal
{
	/** Tile map index to which this portal connects. Negative values indicate a vacant portal. */
	int32 TileConnectionIndex = -1;

	/** Local area graph index to which this portal should connect. */
	int32 AreaConnectionIndex = -1;

	/**
	 * Transforms the provided portal base to create a new graph portal in world space.
	 *
	 * @param TilePortal Portal template to duplicate.
	 * @param Transform World transform to apply to the new portal.
	 */
	FTileGraphPortal(const FTilePortal& TilePortal, const FTransform& Transform);
};

/** Tile plan extension with graph and collision values. */
struct FTileGraphPlan : public FTilePlan
{
	/** A list of the plan's exit portals. */
	TArray<FTileGraphPortal> Portals;

	/** A list of the plan's collision bounds. */
	TArray<FTileBound> Bounds;

	/** A list of the plan's area graph nodes. */
	TArray<FTileNode> AreaNodes;

	/**
	 * Copies and transforms the provided tile data to create a new planned map tile. If this tile
	 * is the root of the tile map it should be marked as such.
	 *
	 * @param TileData Template from which to build the new planned tile.
	 * @param Transform World transform to apply to the new tile.
	 * @param bRoot True if the new tile is the map root.
	 */
	FTileGraphPlan(const FTileGraphData& TileData, const FTransform& Transform, bool bRoot = false);

	/**
	 * Sets the tile connection value at the given portal index to the provided map index. If the
	 * new connection is a parent, it will be swapped into the index at 0 once set.
	 *
	 * @param Index Portal index for which to set the connection.
	 * @param MapIndex Tile map index to set as the connection.
	 * @param bParent True if the connection is a parent.
	 */
	void SetConnection(int32 Index, int32 MapIndex = -1, bool bParent = false);

	/**
	 * Returns the value of the tile connection at the given index. Returns the parent connection
	 * if no index is provided.
	 *
	 * @param Index Tile connection index to return. Defaults to the parent connection.
	 * @return Map index value for the given connection.
	 */
	int32 GetConnection(int32 Index = 0) const;

	/**
	 * Determines whether the portal at the given index is currently connected to a tile.
	 *
	 * @param Index Tile connection index to check for vacancy.
	 * @return True if the given connection index is vacant.
	 */
	bool IsConnectionOpen(int32 Index) const;
};
