// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TileDataLibrary.h"
#include "TileData/TileBound.h"
#include "TileData/TilePortal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileDataLibrary)

bool UTileDataLibrary::TileBoundCollision(const FTileBound& A, const FTileBound& B)
{
	return FTileBound::CheckCollision(A, B);
}

FTileBound UTileDataLibrary::TransformTileBound(const FTileBound& TileBound, const FTransform& Transform)
{
	return FTileBound(TileBound, Transform);
}

FTransform UTileDataLibrary::ConnectionTransform(const FTilePortal& TilePortal, const FTilePortal& WorldPortal)
{
	return FTilePortal::ConnectionTransform(TilePortal, WorldPortal);
}

FTilePortal UTileDataLibrary::TransformTilePortal(const FTilePortal& TilePortal, const FTransform& Transform)
{
	return FTilePortal(TilePortal, Transform);
}
