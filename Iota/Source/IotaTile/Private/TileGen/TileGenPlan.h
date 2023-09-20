// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileData/TileBound.h"
#include "TileData/TilePortal.h"

struct FTileGenData;

/** FTilePortal extension that includes a tile connection index. */
struct FPlanPortal : public FTilePortal
{
	/** Index of the tile connected to this portal within the level plan. */
	int32 ConnectionIndex = -1;

	/**
	 * Copies values from the given portal to create a planned portal.
	 *
	 * @param BasePortal Base portal data for the new planned portal.
	 * @param Transform World transform to apply to the new portal.
	 */
	FPlanPortal(const FTilePortal& BasePortal, const FTransform& Transform = FTransform::Identity);

	/**
	 * Duplicates the given planned portal and optionally transforms it.
	 *
	 * @param PlanPortal Planned portal to duplicate.
	 * @param Transform Transform to apply to the new portal.
	 */
	FPlanPortal(const FPlanPortal& PlanPortal, const FTransform& Transform = FTransform::Identity);
};

/** Represents a planned tile within the generated level plan. */
struct FTileGenPlan
{
	/** Path to the tile level instance. */
	FSoftObjectPath Level;

	/** List of tile portals. */
	TArray<FPlanPortal> Portals;

	/** List of tile collision bounds. */
	TArray<FTileBound> Bounds;

	/** Tile world location. */
	FVector Location;

	/** Tile world rotation. */
	FRotator Rotation;

	/** Defines an empty tile plan. */
	FTileGenPlan();

	/**
	 * Copies values from the given tile data struct to create a tile plan.
	 *
	 * @param DataStruct Base tile data struct from which to build the plan.
	 * @param Transform World transform to apply to the base tile data.
	 */
	FTileGenPlan(const FTileGenData& DataStruct, const FTransform& Transform = FTransform::Identity);

	/**
	 * Duplicates the given tile plan and optionally transforms it.
	 *
	 * @param TilePlan Tile plan to duplicate.
	 * @param Transform Transform to apply to the new tile plan.
	 */
	FTileGenPlan(const FTileGenPlan& TilePlan, const FTransform& Transform = FTransform::Identity);

	/**
	 * Sets the connection value at the given portal index to the provided tile index. If the new
	 * connection is marked as a parent, it will be swapped into the index at 0 once it is set.
	 *
	 * @param Index Portal index for which to set the connection.
	 * @param GraphIndex Tile index to set as the connection.
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
