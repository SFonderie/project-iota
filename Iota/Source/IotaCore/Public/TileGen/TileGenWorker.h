// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "TileData/TileDataAsset.h"
#include "TileGen/TileGenDataStructs.h"
#include "Math/RandomStream.h"

struct FPrimaryAssetId;

/** Runnable that actually executes level generation. */
class IOTACORE_API FTileGenWorker : public FRunnable
{

public:

	/**
	 * Creates a new generation worker with the given parameters and tileset.
	 * Called on the source thread prior to the runnable thread's creation.
	 *
	 * @param InParams Parameters used for tile generation.
	 * @param InTileList Tile data asset primary IDs used to get loaded data.
	 */
	FTileGenWorker(const FTileGenParams& InParams, const TArray<FPrimaryAssetId>& InTileList);

	/** 
	 * Safely discards the worker and its thread. If the thread is still running when the worker is
	 * destroyed, the thread will first stop before being deleted.
	 */
	virtual ~FTileGenWorker();

	/**
	 * Initializes the tile generation worker. Used to prepare member variables.
	 * Called automatically on the runnable thread when it starts up.
	 *
	 * @return False if the initialization fails.
	 */
	virtual bool Init() override;

	/**
	 * Runs the tile generation worker. Used to actually perform tile generation.
	 * Called automatically on the runnable thread when it begins executing.
	 *
	 * @return Exit code for the tile generation process.
	 */
	virtual uint32 Run() override;

	/**
	 * Stops the tile generation worker. Used to break out of tile generation.
	 * Can be called on both the source and runnable threads.
	 */
	virtual void Stop() override;

	/**
	 * Exits the tile generation worker. Used to clean up member variables.
	 * Called automatically on the runnable thread when the run method returns.
	 */
	virtual void Exit() override;

private:

	// bool PlaceNewTile();

	// bool TryPlaceTile();

	// bool CanPlaceTile();

	/**
	 * Shuffles the given array using the worker's random stream. Elements are swapped in order,
	 * such that the first element is guaranteed to never be the same as the last shuffle.
	 *
	 * @param Array Array to shuffle in place.
	 */
	template <typename InElementType>
	void ShuffleArray(TArray<InElementType>& Array);

	/** Generation parameters. */
	const FTileGenParams Params;

	/** Pointer to the actual runnable thread. */
	FRunnableThread* Thread = nullptr;

	/** Thread-safe flag that can stop the thread. */
	FThreadSafeBool bStopThread = false;

	/** Thread-safe counter approximating generation progress. */
	FThreadSafeCounter Progress = 0;

	/** Generation random stream. */
	FRandomStream Random;

	/** Stores available tile options per tile scheme. */
	TArray<FTileGenData> TilePalettes[uint8(ETileScheme::Count)];

	/** Current list of generated tile plans. */
	TArray<FTileGenPlan> TilePlans;
};
