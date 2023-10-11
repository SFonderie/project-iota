// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileData/TileDataAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileDataAsset)

FTileData UTileDataAsset::GetTileData() const
{
	return FTileData(Level, Portals, Bounds);
}
