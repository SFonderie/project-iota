// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "TileData/TileGenParams.h"
#include "TileData/TileScheme.h"

struct FPrimaryAssetId;
struct FTileGenData;
struct FTileGenPlan;
struct FTilePortal;

/** Runnable that actually executes level generation. */
class FTileGenWorker : public FRunnable
{

public:

	/**
	 * Creates a new generation worker with the given parameters and tileset.
	 * Called on the source thread prior to the runnable thread's creation.
	 *
	 * @param InParams Parameters used for tile generation.
	 * @param TileList Tile data asset primary IDs used to get loaded data.
	 */
	FTileGenWorker(const FTileGenParams& InParams, const TArray<FPrimaryAssetId>& TileList);

	/**
	 * Safely discards the worker and its thread. If the thread is still running when the worker is
	 * destroyed, the thread will first stop before being deleted.
	 */
	virtual ~FTileGenWorker();

	/**
	 * Starts the tile generation worker by creating its runnable thread.
	 * Called on the source thread.
	 */
	void Start();

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

	/** Callback delegate invoked when the worker exits. */
	FSimpleDelegate OnExit;

	/** @return Normalized thread progress. */
	float GetProgress() const;

	/** @return Current thread error code. */
	int32 GetErrorCode() const;

	/** @return True if the worker completed successfully. */
	bool IsComplete() const;

	/**
	 * Outputs the generated level plan, or an array of plan members. Can return Tile Plans, Tile
	 * Bounds, and Tile Portals. If the worker is still active, was stopped early, or exited with
	 * an error code, then this method will return an empty array.
	 *
	 * @param InMemberType Tile plan member type to return.
	 */
	template <typename InMemberType>
	void GetPlan(TArray<InMemberType>& OutPlan) const;

private:

	/**
	 * Attempts to add a new tile of the given scheme to the level plan.
	 *
	 * @param Scheme Tile scheme for the new tile to add.
	 * @return False if no tile could be placed.
	 */
	bool PlaceNewTile(ETileScheme Scheme);

	/**
	 * Attempts to fit a terminal tile to the given attachment portal. Keeps trying new terminals
	 * until one can be attached or no terminals remain.
	 *
	 * @param AttachPortal Tile portal upon which to attach a terminal.
	 * @return False if no terminal tile could be attached.
	 */
	bool PlaceTerminal(const FTilePortal& AttachPortal);

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

private:

	/** Generation parameters. */
	const FTileGenParams Parameters;

	/** Pointer to the actual thread. */
	FRunnableThread* Thread = nullptr;

	/** Generation random stream. */
	FRandomStream RandomStream;

	/** Stores tile data sorted per tile scheme. */
	TArray<FTileGenData> TilePalettes[*ETileScheme::Count];

	/** Current list of generated tile plans. */
	TArray<FTileGenPlan> TilePlans;

	/** Thread-safe counter tracking approximate progress. */
	FThreadSafeCounter Progress = 0;

	/** Thread-safe value tracking the current error code. */
	FThreadSafeCounter ErrorCode = 0;

	/** Thread-safe flag used to stop the thread. */
	FThreadSafeBool bStopThread = false;

	/** Thread-safe flag used to track completion. */
	FThreadSafeBool bWorkComplete = false;
};