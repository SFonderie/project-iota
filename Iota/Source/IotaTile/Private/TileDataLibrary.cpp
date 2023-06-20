// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileDataLibrary.h"
#include "TileData/TileBound.h"
#include "TileData/TilePortal.h"
#include "TileData/TileScheme.h"
#include "TileData/TileGenParams.h"
#include "TileData/TilePlan.h"

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

void UTileDataLibrary::MakeTileSequence(const FTileGenParams& Parameters, TArray<ETileScheme>& Sequence)
{
	Parameters.GetTileSequence(Sequence);
}

FTilePlan UTileDataLibrary::TransformTilePlan(const FTilePlan& TilePlan, const FTransform& Transform)
{
	return FTilePlan(TilePlan, Transform);
}
