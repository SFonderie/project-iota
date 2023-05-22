// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TilePortal.generated.h"

/** Simple bounded plane implementation for use during level generation tile alignment. */
USTRUCT(BlueprintType)
struct IOTACORE_API FTilePortal
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
	bool operator==(const FTilePortal& Other) const
	{
		return PlaneSize == Other.PlaneSize;
	}

	/**
	 * Calculates the tile-to-world transform needed to position the owning level tile into the 
	 * world space of the given connection portal.
	 * 
	 * @param Connection The world-space tile portal with which to connect.
	 * @return The tile-to-world transform needed to connect with the given portal.
	 */
	FTransform GetTransformThrough(const FTilePortal& Connection) const
	{
		return TileToWorldTransform(*this, Connection);
	}

	/**
	 * Calculates the transform needed to position a level tile into a generated world through the
	 * connection between the given tile portals.
	 * 
	 * @param TilePortal Connection portal from the level tile.
	 * @param WorldPortal Connection portal from the world.
	 * @return The level-tile-to-world transform.
	 */
	static FTransform TileToWorldTransform(const FTilePortal& TilePortal, const FTilePortal& WorldPortal);
};
