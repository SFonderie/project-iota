// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileGen/TileGenParams.h"

class FTileGenWorker;

struct FPrimaryAssetId;
struct FStreamableHandle;
struct FTileGraphPlan;

/** Asynchronous generation action. */
class IOTATILE_API FTileGenAction
{

public:

	/**
	 * Asynchronously generates a new tile map from the provided parameters, invoking the given
	 * delegate once generation is complete.
	 *
	 * @param InParams Tile map generation parameters.
	 * @param InDelegate
	 */
	FTileGenAction(const FTileGenParams& InParams, const FSimpleDelegate& InDelegate);

	/** Ensures that the Asset Manager releases requested assets. */
	~FTileGenAction();

	/**
	 * Checks to see if the asynchronous worker is finished generating the tile map. This method
	 * will always return true if the action's completion delegate has fired.
	 *
	 * @return True if it is safe to access the tile map.
	 */
	bool CanAccess() const;

	/**
	 * Exports the generated tile map into the provided tile graph and array of tile plans. If the
	 * map has not finished generating yet, the containers will be returned empty.
	 *
	 * @param OutTileGraph Tile graph in which to load the generated tile map.
	 * @param OutTilePlans Tile plan array in which to place the generated tile plans.
	 * @return True if the method found a valid and complete tile map.
	 */
	bool GetTileMap(TArray<FTileGraphPlan>& OutTileMap) const;

private:

	/** Invoked by the engine when it has loaded the tile data assets that the action requested. */
	void NotifyAssetsLoaded();

private:

	/** Action generation parameters. */
	FTileGenParams Params;

	/** Delegate invoked when the generation action completes. */
	FSimpleDelegate OnComplete;

	/** Tile data assets requested by the action. */
	TArray<FPrimaryAssetId> TileAssetList;

	/** Handle used to track data asset loading. */
	TSharedPtr<FStreamableHandle> DataAssetHandle;

	/** Asynchronous worker dispatched by the action. */
	TSharedPtr<FTileGenWorker> AsyncWorker;
};
