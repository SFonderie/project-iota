// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "TileGen/TileGenData.h"
#include "TileGen/TileGenPlan.h"
#include "TileGen/TileGenParams.h"
#include "TileData/TileScheme.h"

class UTileDataAsset;

class FTileGenWorker : public FRunnable
{

public:

	/**
	 * Creates a new level generation runnable using the given parameters and tile list, and starts
	 * its worker thread. When the thread exits, it will invoke the provided delegate and stop.
	 *
	 * @param InParams Level generation parameters.
	 * @param InTileList Loaded tiles to use in the generated level.
	 * @param InDelegate Delegate invoked when the generation thread exits.
	 */
	FTileGenWorker(const FTileGenParams& InParams, const TArray<UTileDataAsset*>& InTileList, const FSimpleDelegate& InDelegate);

	/**
	 * Safely discards the runnable and its thread. If the worker thread has not finished running
	 * when this method is called, it stop the thread and wait for it to return.
	 */
	virtual ~FTileGenWorker() override;

	/**
	 * Actually runs the level generation process. Invoked automatically by the worker thread once
	 * the runnable is initialized, and should not be invoked outside of that context.
	 *
	 * @return Generation exit code.
	 */
	virtual uint32 Run() override;

	/**
	 * Safely stops the runnable early via an escape condition. Calling this method ensures that
	 * the runnable still invokes its exit delegate.
	 */
	virtual void Stop() override;

	/**
	 * Returns control to the game thread via the exit delegate. Invoked automatically by the
	 * worker thread when the runnable exits, and should not be invoked outside of that context.
	 *
	 * @return Generation exit code.
	 */
	virtual void Exit() override;

private:

	/**
	 * Attempts to add a new tile of the given scheme to the level plan.
	 *
	 * @param Scheme Tile scheme to add to the level plan.
	 * @return True if a tile was placed successfully.
	 */
	bool PlaceNewTile(ETileScheme Scheme);

	/**
	 * Attempts to find a viable attachment point for the given tile, and adds the tile to the
	 * level plan if a point is found.
	 *
	 * @param NewTile Tile to attempt to add to the level.
	 * @return True if the tile was attached successfully.
	 */
	bool TryPlaceTile(const FTileGenData& NewTile);

	/**
	 * Checks to see if the given tile can be placed at the given transform by checking for any
	 * collisions with the existing level plan.
	 *
	 * @param NewTile Tile to attempt to place in the world.
	 * @param Transform World transform to apply to the tile.
	 * @return True if the tile would not collide with the level plan.
	 */
	bool CanPlaceTile(const FTileGenData& NewTile, const FTransform& Transform) const;

	/**
	 * Shuffles the given array in place using the worker's random number stream.
	 *
	 * @param Array Array to shuffle in place.
	 */
	template <class ElementType>
	void ShuffleArray(TArray<ElementType>& Array);

private:

	/** Actual worker thread. */
	FRunnableThread* Thread = nullptr;

	/** Thread exit callback. */
	FSimpleDelegate OnExit;

	/** Generation parameters. */
	FTileGenParams Params;

	/** Random number stream. */
	FRandomStream RandomStream;

	/** Raw generation tile data objects sorted by scheme. */
	TArray<FTileGenData> TilePalettes[*ETileScheme::Count];

	/** Generated level plan sequence. */
	TArray<ETileScheme> TileSequence;

	/** Current generated level plan. */
	TArray<FTileGenPlan> TilePlans;

	/** Allows the thread to stop. */
	FThreadSafeBool bStopThread;
};
