// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TileDataLibrary.h"
#include "TileData/TileBound.h"
#include "TileData/TileData.h"
#include "TileData/TilePortal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileDataLibrary)

bool UTileDataLibrary::TileBoundCollision(const FTileBound& A, const FTileBound& B)
{
	return FTileBound::CheckCollision(A, B);
}

void UTileDataLibrary::TransformTileBound(const FTileBound& TileBound, const FTransform& Transform, FTileBound& NewBound)
{
	NewBound = FTileBound(TileBound, Transform);
}

bool UTileDataLibrary::TileCollision(const FTileData& A, const FTileData& B)
{
	for (const FTileBound& BoundA : A.Bounds)
	{
		for (const FTileBound& BoundB : B.Bounds)
		{
			if (FTileBound::CheckCollision(BoundA, BoundB))
			{
				return true;
			}
		}
	}

	return false;
}

void UTileDataLibrary::TransformTile(const FTileData& TileData, const FTransform& Transform, FTileData& NewTile)
{
	NewTile = FTileData(TileData, Transform);
}

FTransform UTileDataLibrary::ConnectionTransform(const FTilePortal& TilePortal, const FTilePortal& WorldPortal)
{
	return FTilePortal::ConnectionTransform(TilePortal, WorldPortal);
}

void UTileDataLibrary::TransformTilePortal(const FTilePortal& TilePortal, const FTransform& Transform, FTilePortal& NewPortal)
{
	NewPortal = FTilePortal(TilePortal, Transform);
}
