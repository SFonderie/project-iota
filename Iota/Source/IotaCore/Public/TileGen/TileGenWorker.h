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
	 * Starts the tile generation worker by creating its runnable thread.
	 * Called on the source thread to create the runnable thread.
	 */
	virtual void Start();

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

	/**
	 * @return Current normalized thread progress.
	 */
	virtual float Status() const;

	/**
	 * Outputs the generated level plan. If the worker was stopped, exited with an error code, or
	 * is still active, the given array will be returned empty.
	 */
	virtual void Output(TArray<FTilePlan>& OutTilePlan) const;

	/** 
	 * Callback invoked when the worker exits.
	 */
	FSimpleDelegate OnExit;

protected:

	/**
	 * Invoked at the start of level generation to produce a sequence of tile schemes. By default,
	 * this method generates a sequence composed of a start tile at the first position, an exit
	 * tile at the last position, and fills the rest with connectors and intermediates.
	 *
	 * Objectives and stoppers can be included where necessary.
	 *
	 * @param OutSequence Container for the completed sequence.
	 */
	virtual void GenerateSequence(TArray<ETileScheme>& OutSequence);

	/**
	 * Invoked for each tile added to the level plan to determine valid parentage. By default,
	 * this method will return the desired value unless the tile is an objective tile, in which
	 * case the parent will be invalid (-1) to split the level over objectives.
	 *
	 * @param ParentIndex Desired parent index.
	 * @param Scheme New tile scheme.
	 * @return Validated parent index.
	 */
	virtual int32 DetermineParent(int32 ParentIndex, ETileScheme Scheme);

private:

	/**
	 * Attempts to add a new tile of the given scheme to the level plan.
	 *
	 * @param Scheme Tile scheme for the new tile to add.
	 * @return False if no tile could be placed.
	 */
	bool PlaceNewTile(ETileScheme Scheme);

	/**
	 * Attempts to fit a stopper tile to the given attachment portal. Keeps trying new stoppers
	 * until one can be attached or no stoppers remain.
	 *
	 * @param AttachPortal Tile portal to which to attach a stopper.
	 * @return False if no stopper tile could be attached.
	 */
	bool PlaceStopper(const FTilePortal& AttachPortal);

	/**
	 * Attempts to find a viable attachment point for the given tile data. If one is found, the
	 * tile data will be converted into a plan and appended to the level plan. Attachment points
	 * are determined by traversing the last-placed tile's parents to the branch parameter depth
	 * and considering all open portals on those tiles.
	 *
	 * @param NewTile Tile to attach.
	 * @param Scheme Scheme of the new tile.
	 * @return False if there was no viable attachment point.
	 */
	bool TryPlaceTile(const FTileGenData& NewTile, ETileScheme Scheme);

	/**
	 * Checks to see if the given tile can be placed at the given transform via collision checks.
	 * Each bound on the new tile is compared to each bound on the tile plans, and the method will
	 * return false if even a single collision check returns true. Collision checks automatically
	 * pass if the two bounds are too distant to collide.
	 *
	 * @param NewTile Tile data to test.
	 * @param Transform World transform to apply to the tile.
	 * @return False if the tile would collide at its new transform.
	 */
	bool CanPlaceTile(const FTileGenData& NewTile, const FTransform& Transform) const;

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

	/** Thread-safe flag used to check completion. */
	FThreadSafeBool bWorkComplete = false;

	/** Thread-safe flag providing error insights. */
	FThreadSafeBool bWithError = false;
};
