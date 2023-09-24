// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "TileData/TileScheme.h"
#include "TileGen/TileGenParams.h"
#include "Misc/SingleThreadRunnable.h"

class FRunnableThread;
class UTileDataAsset;

struct FTileData;
struct FTileGraphPlan;

/** Asynchronous worker that actually handles tile map generation. */
class FTileGenWorker : public FRunnable, public FSingleThreadRunnable
{
	friend class FTileGenAction;

public:

	/**
	 * Creates a new tile map generation worker using the given parameters and tile list, and
	 * starts a thread to run it. When the thread exits, it will invoke the provided delegate.
	 *
	 * @param InParams Tile map generation parameters.
	 * @param InTileList Loaded tiles to use in the generated tile map.
	 * @param InDelegate Delegate invoked when the generation thread exits.
	 */
	FTileGenWorker(const FTileGenParams& InParams, const TArray<UTileDataAsset*>& InTileList, const FSimpleDelegate& InDelegate);

	/**
	 * Safely discards the worker and its thread. If the thread has not finished running when this
	 * method is invoked, it will stop the thread and wait for it to return.
	 */
	virtual ~FTileGenWorker();

private:

	/** Handles pre-loop worker initialization. */
	virtual bool Init() override;

	/** Generates a new tile map as a single asynchronous process. */
	virtual uint32 Run() override;

	/** Generates a new tile map one tile at a time. */
	virtual void Tick() override;

	/** Stops the generation worker. */
	virtual void Stop() override;

	/** Returns execution to the game thread via the exit delegate. */
	virtual void Exit() override;

	/** Returns the worker as a runnable for fake threads. */
	virtual FSingleThreadRunnable* GetSingleThreadInterface() override;

	/**
	 * Attempts to add a new tile of the given scheme to the tile map.
	 *
	 * @param Scheme Tile scheme to add to the tile map.
	 * @return True if a tile was added successfully.
	 */
	bool PlaceNewTile(ETileScheme Scheme);

	/**
	 * Attempts to find a viable attachment point for the given tile, and adds the tile to the
	 * tile map if a point is found.
	 *
	 * @param NewTile Tile to attempt to add to the tile map.
	 * @return True if the tile was attached successfully.
	 */
	bool TryPlaceTile(const FTileData& NewTile);

	/**
	 * Attempts to attach terminal tiles to any vacant portals left on the given tile.
	 *
	 * @param PlanIndex Tile map index on which to attach terminals.
	 */
	void PlaceTerminals(int32 PlanIndex);

	/**
	 * Attempts to attach a terminal tile to the given plan portal.
	 *
	 * @param PlanIndex Tile map index on which to attach a terminal.
	 * @param Portal Portal index on which to attach a terminal.
	 */
	void TryPlaceTerminal(int32 PlanIndex, int32 Portal);

	/**
	 * Checks to see if the given tile can be placed at the given transform by checking for any
	 * collisions with the existing tile map.
	 *
	 * @param NewTile Tile to attempt to add to the tile map.
	 * @param Transform World transform to apply to the tile.
	 * @return True if the tile would not collide with the tile map.
	 */
	bool CanPlaceTile(const FTileData& NewTile, const FTransform& Transform) const;

	/**
	 * Shuffles the given array in place using the worker's random number stream.
	 *
	 * @param Array Array to shuffle in place.
	 */
	template <class ElementType>
	void ShuffleArray(TArray<ElementType>& Array);

	/** Actual worker thread. */
	FRunnableThread* Thread = nullptr;

	/** Thread exit callback. */
	FSimpleDelegate OnExit;

	/** Generation parameters. */
	FTileGenParams Params;

	/** Random number stream. */
	FRandomStream RandomStream;

	/** Tile data palettes sorted by tile scheme. */
	TArray<FTileData> TilePalettes[*ETileScheme::Count];

	/** Generated scheme sequence. */
	TArray<ETileScheme> Sequence;

	/** Current generated tile map. */
	TArray<FTileGraphPlan> TileMap;

	/** Allows the thread to stop. */
	FThreadSafeBool bStopThread;

	/** Marks the thread as safe. */
	FThreadSafeBool bCanAccess;

	/** Tracks worker progress. */
	FThreadSafeCounter Progress;
};
