// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenWorker.h"
#include "Engine/AssetManager.h"
#include "HAL/RunnableThread.h"

FTileGenWorker::FTileGenWorker(const FTileGenParams& InParams, const TArray<FPrimaryAssetId>& InTileList)
	: Params(InParams)
	, Random(InParams.Seed)
{
	const UAssetManager& AssetManager = UAssetManager::Get();

	// Asset loading must occur in the constructor since UObjects are not safely accessible from
	// threads other than the game thread. Once this completes, the objects can be unloaded.
	for (const FPrimaryAssetId& TileID : InTileList)
	{
		if (const UTileDataAsset* TileDataAsset = AssetManager.GetPrimaryAssetObject<UTileDataAsset>(TileID))
		{
			TilePalettes[uint8(TileDataAsset->Scheme)].Emplace(TileDataAsset);
		}
	}

	// Create the actual runnable thread here. Doing so will automatically call Init.
}

FTileGenWorker::~FTileGenWorker()
{
	if (Thread)
	{
		Thread->Kill();
		delete Thread;
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
	bStopThread = true;
}

void FTileGenWorker::Exit()
{

}

template <typename InElementType>
void FTileGenWorker::ShuffleArray(TArray<InElementType>& Array)
{
	for (int32 i = 0; i < Array.Num() - 1; i++)
	{
		Array.Swap(i, Random.RandRange(i + 1, Array.Num() - 1));
	}
}
