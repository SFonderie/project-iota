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
	 * Should be called on the source thread, well before the runnable thread exists.
	 *
	 * @param InParams Parameters used for tile generation.
	 * @param InTileList Tile data asset primary IDs used to get loaded data.
	 */
	FTileGenWorker(const FTileGenParams& InParams, const TArray<FPrimaryAssetId>& InTileList);

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
	 * Called automatically on the runnable thread when another process requests that it stop.
	 */
	virtual void Stop() override;

	/**
	 * Exits the tile generation worker. Used to clean up member variables.
	 * Called automatically on the runnable thread when the tile generation process is complete.
	 */
	virtual void Exit() override;

private:

	// bool PlaceTile();

	// bool TryPlaceTile();

	// bool CanPlaceTile();

	/** Generation parameters. */
	FTileGenParams Params;

	/** Generation random stream. */
	FRandomStream RandomStream;

	/** Stores available tile options per tile scheme. */
	TArray<FTileGenData> TileData[uint8(ETileScheme::Count)];
};
