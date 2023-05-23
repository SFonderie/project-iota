// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "Tileset/TileDataAsset.h"
#include "Tileset/TileBound.h"
#include "Tileset/TilePortal.h"
#include "Engine/World.h"

#include "UObject/ObjectSaveContext.h"

DEFINE_LOG_CATEGORY_CLASS(UTileDataAsset, LogTileData);

void UTileDataAsset::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);
}
