// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileData/TileBound.h"
#include "TileData/TilePortal.h"

class UTileDataAsset;

/** UTileDataAsset thread-safe proxy. */
struct FTileGenData
{
	/** Path to the actual tile level instance. */
	FSoftObjectPath Level;

	/** List of tile exit portals. */
	TArray<FTilePortal> Portals;

	/** List of tile collision bounds. */
	TArray<FTileBound> Bounds;

	/** Defines an empty data struct. */
	FTileGenData();

	/** Copies values from the given tile data asset. */
	FTileGenData(const UTileDataAsset* DataAsset);

	/** Copies values from the given tile data struct. */
	FTileGenData(const FTileGenData& DataStruct);
};

/** Extension of FTileGenData that includes a transform and tile graph data. */
struct FTileGenPlan : public FTileGenData
{
	/** Tile world position. */
	FVector Position;

	/** Tile world rotation. */
	FRotator Rotation;

	/**
	 * Copies values from the given tile data struct to create a tile plan.
	 *
	 * @param DataStruct Base tile data struct from which to build the plan.
	 * @param Transform World transform to apply to the base tile data.
	 */
	FTileGenPlan(const FTileGenData& DataStruct, const FTransform& Transform);

	/**
	 * Sets the connection value at the given portal index to the provided tile index. If the new
	 * connection is marked as a parent, it will be swapped into the index at 0 once it is set.
	 *
	 * @param Index Portal index for which to set the connection.
	 * @param GraphIndex Tile index to set as the connection.
	 * @param bParent True if the connection is a parent.
	 */
	void SetConnection(int32 Index, int32 GraphIndex, bool bParent = false);

	/**
	 * Returns the value of the connection at the given index. Returns the parent connection if no
	 * index is provided.
	 *
	 * @param Index Connection index to return. Defaults to the parent connection.
	 * @return Graph index value for the given connection.
	 */
	int32 GetConnection(int32 Index = 0) const;

	/**
	 * Determines whether the portal at the given index is currently connected to a tile.
	 *
	 * @return True if the given portal index is vacant.
	 */
	bool IsPortalOpen(int32 Index) const;

private:

	/**
	 * Tracks adjacent tile graph indices. Values are stored at the index of the portal connecting
	 * them to the tile, with the index at 0 always indicating the tile parent. If a value is ever
	 * negative, the portal at its index is vacant.
	 */
	TArray<int32> Connections;
};
