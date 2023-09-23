// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileData/TileBound.h"
#include "TileData/TilePlan.h"
#include "TileData/TilePortal.h"

struct FTileData;

/** Generation system portal extension with a graph index. */
struct FTileGraphPortal : public FTilePortal
{
	/** Tile map index to which this portal connects. Negative values indicate a vacant portal. */
	int32 ConnectionIndex = -1;

	/**
	 * Defines a new graph portal by transforming the provided base portal.
	 *
	 * @param BasePortal Portal template to duplicate.
	 * @param Transform World transform to apply to the new portal.
	 */
	FTileGraphPortal(const FTilePortal& BasePortal, const FTransform& Transform);
};

/** Generation system plan extension with portal and collision values. */
struct FTileGraphPlan : public FTilePlan
{
	/** List of tile portals. */
	TArray<FTileGraphPortal> Portals;

	/** List of tile collision bounds. */
	TArray<FTileBound> Bounds;

	/**
	 * Defines a new generation plan by applying a world transform to a provided tile template.
	 *
	 * @param TemplateData Tile template data to duplicate.
	 * @param Transform World transform to apply to the tile template.
	 */
	FTileGraphPlan(const FTileData& TemplateData, const FTransform& Transform);

	/**
	 * Sets the connection value at the given portal index to the provided tile map index. If the
	 * new connection is a parent, it will be swapped into the index at 0 once it is set.
	 *
	 * @param Index Portal index for which to set the connection.
	 * @param GraphIndex Tile map index to set as the connection.
	 * @param bParent True if the connection is a parent.
	 */
	void SetConnection(int32 Index, int32 GraphIndex = -1, bool bParent = false);

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
	bool IsOpenPortal(int32 Index) const;
};
