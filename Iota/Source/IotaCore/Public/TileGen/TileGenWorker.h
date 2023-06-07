// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

/** Runnable that actually executes level generation. */
class IOTACORE_API FTileGenWorker// : public FRunnable
{

public:

	/**
	 * Initializes the tile generation worker. Used to prepare member variables.
	 * Called automatically when the runnable thread starts up.
	 *
	 * @return False if the initialization fails.
	 */
	// virtual bool Init() override;

	/**
	 * Runs the tile generation worker. Used to actually perform tile generation.
	 * Called automatically when the runnable thread begins executing.
	 *
	 * @return Exit code for the tile generation process.
	 */
	// virtual uint32 Run() override;

	/**
	 * Stops the tile generation worker. Used to break out of tile generation.
	 * Called automatically when another process requests that this worker stop.
	 */
	// virtual void Stop() override;

	/**
	 * Exits the tile generation worker. Used to clean up member variables.
	 * Called automatically when the tile generation process is complete.
	 */
	// virtual void Exit() override;
};
