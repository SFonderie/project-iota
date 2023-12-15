// Copyright Sydney Fonderie. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TilePortal.generated.h"

/** Bounded plane structure through which tiles can connect to one another. */
USTRUCT(BlueprintType)
struct IOTATILE_API FTilePortal
{
	GENERATED_BODY()

public:

	/** Location of the tile portal in world space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location;

	/** Direction of the tile portal in world space. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Direction;

	/** Portal plane dimensions in meters. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIntPoint PlaneSize;

	/** Defines a default unoriented tile portal. */
	FTilePortal();

	/**
	 * Defines a bounded tile portal plane.
	 *
	 * @param InLocation Location of the tile portal in world space.
	 * @param InDirection Direction of the tile portal in world space.
	 * @param InPlaneSize Portal plane dimensions in meters.
	 */
	FTilePortal(const FVector& InLocation, const FVector& InDirection, const FIntPoint& InPlaneSize);

	/**
	 * Duplicates the given tile portal and optionally transforms it.
	 *
	 * @param TilePortal Tile portal to duplicate.
	 * @param Transform Transform to be applied to the new portal.
	 */
	FTilePortal(const FTilePortal& TilePortal, const FTransform& Transform = FTransform::Identity);

	/** @return True if the portals have the same plane size. */
	bool CanConnect(const FTilePortal& Connection) const
	{
		return PlaneSize == Connection.PlaneSize;
	}

	/**
	 * Calculates the transform needed to position a new tile within a tile map such that it
	 * connects via the given tile portals.
	 *
	 * @param TilePortal Connection portal from the tile level.
	 * @param WorldPortal Connection portal from the world.
	 * @return The level-tile-to-world transform.
	 */
	static FTransform ConnectionTransform(const FTilePortal& TilePortal, const FTilePortal& WorldPortal);
};
