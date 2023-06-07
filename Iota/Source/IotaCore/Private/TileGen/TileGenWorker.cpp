// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenWorker.h"
#include "Engine/AssetManager.h"

FTileGenWorker::FTileGenWorker(const FTileGenParams& InParams, const TArray<FPrimaryAssetId>& InTileList)
	: Params(InParams)
	, RandomStream(InParams.Seed)
{
	UAssetManager& AssetManager = UAssetManager::Get();

	for (const FPrimaryAssetId& TileID : InTileList)
	{
		const UTileDataAsset* TileDataAsset = AssetManager.GetPrimaryAssetObject<UTileDataAsset>(TileID);

		if (TileDataAsset)
		{
			TileData[uint8(TileDataAsset->Scheme)].Emplace(TileDataAsset);
		}
	}
}

bool FTileGenWorker::Init()
{
	return true;
}

uint32 FTileGenWorker::Run()
{
	return 0;
}

void FTileGenWorker::Stop()
{

}

void FTileGenWorker::Exit()
{

}
