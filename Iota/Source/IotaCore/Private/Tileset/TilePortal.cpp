// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "Tileset/TilePortal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TilePortal)

FTilePortal::FTilePortal()
	: Location(0, 0, 0)
	, Direction(1, 0, 0)
	, PlaneSize(3, 3)
{
	// Default constructor.
}

FTilePortal::FTilePortal(const FVector& InLocation, const FVector& InDirection, const FIntPoint& InPlaneSize)
	: Location(InLocation)
	, Direction(InDirection)
	, PlaneSize(InPlaneSize)
{
	// Complete constructor.
}

FTilePortal::FTilePortal(const FTilePortal& TilePortal, const FTransform& Transform)
	: Location(Transform.TransformPosition(TilePortal.Location))
	, Direction(Transform.TransformVector(TilePortal.Direction))
	, PlaneSize(TilePortal.PlaneSize)
{
	// Copy constructor.
}

FTransform FTilePortal::TileToWorldTransform(const FTilePortal& TilePortal, const FTilePortal& WorldPortal)
{
	// Make a tile-to-portal transform by inverting the tile portal's transform.
	// We also need to flip the direction so that the portal faces the other way.
	FTransform TileToPortal = FTransform(
		FRotationMatrix::MakeFromX(-TilePortal.Direction).Rotator(),
		TilePortal.Location
	).Inverse();

	// The portal-to-world transform is just the world portal's transform.
	FTransform PortalToWorld = FTransform(
		FRotationMatrix::MakeFromX(WorldPortal.Direction).Rotator(),
		WorldPortal.Location
	);

	// Final transform is the product.
	return TileToPortal * PortalToWorld;
}
