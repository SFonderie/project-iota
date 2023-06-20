// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileScheme.generated.h"

/** Defines different tile functions within a tileset. */
UENUM(BlueprintType, meta = (Bitflags))
enum class ETileScheme : uint8
{
	/**
	 * Indicates that a tile should be used to determine a level spawn point.
	 * Start tiles should have at least one portal.
	 */
	Start,

	/**
	 * Indicates that a tile should be used to bridge between other tiles.
	 * Connector tiles should have at least two portals.
	 */
	Connector,

	/**
	 * Indicates that a tile should be used to branch the level or as an arena.
	 * Intermediate tiles should have at least three portals.
	 */
	Intermediate,

	/**
	 * Indicates that a tile should be used as a level objective.
	 * Objective tiles should have at least two portals.
	 */
	Objective,

	/**
	 * Indicates that a tile should be used to seal-off empty portals.
	 * Terminal tiles should have at most one portal.
	 */
	Terminal,

	/**
	 * Indicates that a tile should be used to determine a level end point.
	 * Exit tiles should have at least one portal.
	 */
	Exit,

	/**
	 * Internal enum value used to create static arrays.
	 */
	Count UMETA(Hidden),
};

/** @return Scheme as its underlying byte value. */
uint8 IOTATILE_API operator*(ETileScheme Scheme);

/** @return Integer shifted by the given tile scheme. */
int32 IOTATILE_API operator<<(int32 Integer, ETileScheme Scheme);
