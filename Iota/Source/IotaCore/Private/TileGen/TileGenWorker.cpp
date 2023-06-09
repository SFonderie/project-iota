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
	// Generate a scheme sequence here.

	// Core level generation loop. Worst-case scenario is O(N^6).
	for (int32 Tile = 0; Tile < Params.Size && !bStopThread; Tile++)
	{
		if (!PlaceNewTile(ETileScheme::Connection))
		{
			return 1;
		}
	}

	// Iterate through the tile plan and try to install stoppers on remaining open portals.
	for (const FTileGenPlan& Plan : TilePlans)
	{
		for (int32 Index = 0; Index < Plan.Portals.Num() && !bStopThread; Index++)
		{
			if (Plan.IsOpenPortal(Index))
			{
				// Try to place a stopper.

				// When the TileGenSubsystem actually loads the levels, it can treat open portals
				// as places to put seals and closed portals as places to put doors.
			}
		}
	}

	return 0;
}

void FTileGenWorker::Stop()
{
	bStopThread = true;
}

void FTileGenWorker::Exit()
{

}

bool FTileGenWorker::PlaceNewTile(ETileScheme Scheme)
{
	ShuffleArray(TilePalettes[uint8(Scheme)]);

	for (const FTileGenData& NewTile : TilePalettes[uint8(Scheme)])
	{
		if (bStopThread || TryPlaceTile(NewTile, Scheme))
		{
			Progress.Increment();
			return true;
		}
	}

	return false;
}

bool FTileGenWorker::TryPlaceTile(const FTileGenData& NewTile, ETileScheme Scheme)
{
	if (TilePlans.IsEmpty())
	{
		// No plans, so add the tile as-is as the first.
		TilePlans.Emplace(NewTile, FTransform::Identity);
		return true;
	}

	// Portals are stored as Plan, Portal
	TArray<TPair<int32, int32>> OpenPortals;
	int32 PlanIndex = TilePlans.Num() - 1;
	int32 Depth = 0;

	// Traverse the tile plan array as if it was a tree and add any open portals to the list.
	// Traversal will continue until a null parent is found or the branch length is reached.
	while (0 <= PlanIndex && PlanIndex < TilePlans.Num() && Depth < Params.Branch)
	{
		const FTileGenPlan& PlanValue = TilePlans[PlanIndex];

		for (int32 Index = 0; Index < PlanValue.Portals.Num(); Index++)
		{
			if (PlanValue.IsOpenPortal(Index))
			{
				OpenPortals.Emplace(PlanIndex, Index);
			}
		}

		// Traverse to the plan's parent.
		PlanIndex = PlanValue.ParentIndex;
		++Depth;
	}

	// New tile portal indices.
	TArray<int32> TilePortals;

	for (int32 Index = 0; Index < NewTile.Portals.Num(); Index++)
	{
		TilePortals.Add(Index);
	}

	// Shuffle both arrays.
	ShuffleArray(OpenPortals);
	ShuffleArray(TilePortals);

	// These loops are O(N^4) and are thus the whole reason level generation gets its own thread.
	// Each orientation of the new tile is attempted for an open portal before moving to the next.
	for (const TPair<int32, int32>& OpenPair : OpenPortals)
	{
		for (const int32& TileIndex : TilePortals)
		{
			if (bStopThread)
			{
				return true;
			}

			const FTilePortal& PlanPortal = TilePlans[OpenPair.Key].Portals[OpenPair.Value];
			const FTilePortal& NewPortal = NewTile.Portals[TileIndex];

			FTransform NewTransform = FTilePortal::ConnectionTransform(NewPortal, PlanPortal);

			if (CanPlaceTile(NewTile, NewTransform))
			{
				// Placement check successful; create the tile plan now.
				FTileGenPlan NewPlan = FTileGenPlan(NewTile, NewTransform);

				// Determine the parenting rules here.
				NewPlan.ParentIndex = OpenPair.Key;

				// Close the portals used in the connection and then
				// append the plan to the list.
				TilePlans[OpenPair.Key].ClosePortal(OpenPair.Value);
				NewPlan.ClosePortal(TileIndex);
				TilePlans.Emplace(NewPlan);

				// Exit.
				return true;
			}
		}
	}

	return false;
}

bool FTileGenWorker::CanPlaceTile(const FTileGenData& NewTile, const FTransform& Transform) const
{
	for (const FTileBound& NewBound : NewTile.Bounds)
	{
		// Transform the new bound into the world test space.
		FTileBound TestBound = FTileBound(NewBound, Transform);

		// Compare the new bound with each existing bound.
		for (const FTileGenPlan& Tile : TilePlans)
		{
			for (const FTileBound& Bound : Tile.Bounds)
			{
				if (TestBound.IsCollidingWith(Bound))
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename InElementType>
void FTileGenWorker::ShuffleArray(TArray<InElementType>& Array)
{
	for (int32 i = 0; i < Array.Num() - 1; i++)
	{
		Array.Swap(i, Random.RandRange(i + 1, Array.Num() - 1));
	}
}
