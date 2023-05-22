// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "Tileset/TileLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileLibrary)

bool UTileLibrary::TileBoundCollision(const FTileBound& A, const FTileBound& B)
{
	return FTileBound::CheckCollision(A, B);
}

FTileBound UTileLibrary::TransformTileBound(const FTileBound& TileBound, const FTransform& Transform)
{
	return FTileBound(TileBound, Transform);
}

FTransform UTileLibrary::ConnectionTransform(const FTilePortal& TilePortal, const FTilePortal& WorldPortal)
{
	return FTilePortal::ConnectionTransform(TilePortal, WorldPortal);
}

FTilePortal UTileLibrary::TransformTilePortal(const FTilePortal& TilePortal, const FTransform& Transform)
{
	return FTilePortal(TilePortal, Transform);
}
