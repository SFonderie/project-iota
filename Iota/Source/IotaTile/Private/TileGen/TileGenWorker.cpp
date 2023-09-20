// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenWorker.h"
#include "TileData/TileDataAsset.h"
#include "HAL/RunnableThread.h"
#include "Async/Async.h"

FTileGenWorker::FTileGenWorker(const FTileGenParams& InParams, const TArray<UTileDataAsset*>& InTileList, const FSimpleDelegate& InDelegate)
	: OnExit(InDelegate)
	, Params(InParams)
	, RandomStream(InParams.Seed)
{
	for (const UTileDataAsset* TileDataAsset : InTileList)
	{
		bool bMainObjective = TileDataAsset->Objectives.HasTagExact(Params.MainObjective);
		bool bSideObjective = TileDataAsset->Objectives.HasAnyExact(Params.SideObjectives);
		bool bZeroObjective = TileDataAsset->Objectives.IsEmpty();

		for (ETileScheme Scheme : TEnumRange<ETileScheme>())
		{
			bool bSchemeMatch = 1 << Scheme & TileDataAsset->Schemes;
			bool bIsObjective = IsObjective(Scheme);

			// If the tile is an objective tile, it can only be added to the objective palette if
			// it also matches the main objective.
			bool bObjectiveMatch = bIsObjective && bMainObjective;

			// If the tile is not an objective tile, it can only be added to a given palette if it
			// also matches the main objective, matches any side objective, or has no objective.
			bool bBasicMatch = !bIsObjective && (bMainObjective || bSideObjective || bZeroObjective);

			// In order to add a tile to a given palette, the tile must match the palette scheme
			// and meet one of the two conditions above.
			if (bSchemeMatch && (bObjectiveMatch || bBasicMatch))
			{
				// Copy each asset into a thread-safe proxy.
				TilePalettes[*Scheme].Emplace(TileDataAsset);
			}
		}
	}

	Thread = FRunnableThread::Create(this, TEXT("IotaTileGenThread"));
}

FTileGenWorker::~FTileGenWorker()
{
	if (Thread)
	{
		Thread->Kill();
		delete Thread;
	}
}

uint32 FTileGenWorker::Run()
{
	Params.GetTileSequence(TileSequence);

	// Core loop. Builds out the main level path using the tile sequence.
	for (int32 Tile = 0; Tile < Params.Length && !bStopThread; Tile++)
	{
		if (!PlaceNewTile(TileSequence[Tile]))
		{
			return 1;
		}
	}

	// Second loop. Goes through each main tile and adds terminal seals.
	for (int32 Tile = 0; Tile < Params.Length && !bStopThread; Tile++)
	{
		PlaceTerminals(Tile);
	}

	return 0;
}

bool FTileGenWorker::PlaceNewTile(ETileScheme Scheme)
{
	TArray<FTileGenData>& Palette = TilePalettes[*Scheme];
	ShuffleArray(Palette);

	for (int32 Index = 0; Index < Palette.Num() && !bStopThread; Index++)
	{
		if (TryPlaceTile(Palette[Index]))
		{
			if (IsObjective(Scheme))
			{
				Palette.RemoveAtSwap(Index);
			}

			return true;
		}
	}

	return false;
}

bool FTileGenWorker::TryPlaceTile(const FTileGenData& NewTile)
{
	if (TilePlans.IsEmpty())
	{
		TilePlans.Emplace(NewTile, FTransform(Params.Rotation, Params.Location));
		return true;
	}

	// Gather viable portal connections for the new tile.
	// Existing portals are stored as Plan, Portal.
	TArray<TPair<int32, int32>> OpenPortals;
	int32 PlanIndex = TilePlans.Num() - 1;
	int32 Depth = 0;

	// Traverse the tile plan array as if it was a tree and add any open portals to the list.
	// Traversal will continue until an empty parent is found or the branch length is reached.
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

		// If the tile is an objective and there is at least one open portal, then stop searching
		// for additional portals. Doing so turns objectives with more than one portal into a sort
		// of "one-way valve" that divides the level plan into "before" and "after" sections.
		if (IsObjective(TileSequence[PlanIndex]) && !OpenPortals.IsEmpty())
		{
			break;
		}

		// Traverse to the plan's parent and continue.
		PlanIndex = PlanValue.GetConnection();
		++Depth;
	}

	// Gather portals on the new tile.
	TArray<int32> TilePortals;

	for (int32 Index = 0; Index < NewTile.Portals.Num(); Index++)
	{
		TilePortals.Add(Index);
	}

	// Shuffle both arrays.
	ShuffleArray(OpenPortals);
	ShuffleArray(TilePortals);

	// Attempt to position the new tile at each open-new portal combination.
	for (const TPair<int32, int32>& OpenIndex : OpenPortals)
	{
		for (const int32& NewIndex : TilePortals)
		{
			const FTilePortal& PlanPortal = TilePlans[OpenIndex.Key].Portals[OpenIndex.Value];
			const FTilePortal& NewPortal = NewTile.Portals[NewIndex];

			if (NewPortal.CanConnect(PlanPortal))
			{
				FTransform NewTransform = FTilePortal::ConnectionTransform(NewPortal, PlanPortal);

				if (CanPlaceTile(NewTile, NewTransform))
				{
					// Placement check successful; create the tile plan now.
					FTileGenPlan NewPlan = FTileGenPlan(NewTile, NewTransform);

					// Make the open tile into the parent of the new tile.
					NewPlan.SetConnection(NewIndex, OpenIndex.Key, true);

					// Make the new tile (index = length) into a child of the open tile.
					TilePlans[OpenIndex.Key].SetConnection(OpenIndex.Value, TilePlans.Num());

					// Append the plan and exit.
					TilePlans.Emplace(NewPlan);
					return true;
				}
			}
		}
	}

	return false;
}

bool FTileGenWorker::CanPlaceTile(const FTileGenData& NewTile, const FTransform& Transform) const
{
	for (const FTileBound& NewBound : NewTile.Bounds)
	{
		// Transform the new bound into world space.
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

void FTileGenWorker::PlaceTerminals(int32 PlanIndex)
{
	for (int32 Portal = 0; Portal < TilePlans[PlanIndex].Portals.Num() && !bStopThread; Portal++)
	{
		if (TilePlans[PlanIndex].IsOpenPortal(Portal))
		{
			TryPlaceTerminal(PlanIndex, Portal);
		}
	}
}

void FTileGenWorker::TryPlaceTerminal(int32 PlanIndex, int32 Portal)
{
	TArray<FTileGenData>& Palette = TilePalettes[*ETileScheme::Terminal];
	ShuffleArray(Palette);

	for (const FTileGenData& NewTile : Palette)
	{
		for (int32 NewIndex = 0; NewIndex < NewTile.Portals.Num(); NewIndex++)
		{
			const FTilePortal& PlanPortal = TilePlans[PlanIndex].Portals[Portal];
			const FTilePortal& NewPortal = NewTile.Portals[NewIndex];

			if (NewPortal.CanConnect(PlanPortal))
			{
				FTransform NewTransform = FTilePortal::ConnectionTransform(NewPortal, PlanPortal);

				if (CanPlaceTile(NewTile, NewTransform))
				{
					// Placement check successful; create the tile plan now.
					FTileGenPlan NewPlan = FTileGenPlan(NewTile, NewTransform);

					// Make the plan tile into the parent of the new tile.
					NewPlan.SetConnection(NewIndex, PlanIndex, true);

					// Make the new tile (index = length) into a child of the plan tile.
					TilePlans[PlanIndex].SetConnection(Portal, TilePlans.Num());

					// Append the plan and exit.
					TilePlans.Emplace(NewPlan);
					return;
				}
			}
		}
	}
}

template <typename ElementType>
void FTileGenWorker::ShuffleArray(TArray<ElementType>& Array)
{
	for (int32 i = 0; i < Array.Num() - 1; i++)
	{
		Array.Swap(i, RandomStream.RandRange(i + 1, Array.Num() - 1));
	}
}

void FTileGenWorker::Stop()
{
	bStopThread = true;
}

void FTileGenWorker::Exit()
{
	// Move back to the game thread by invoking the exit delegate asynchronously. The lambda uses
	// a value capture to access the delegate because the worker might get destroyed.
	AsyncTask(ENamedThreads::GameThread, [OnExitCapture = OnExit]()
	{
		OnExitCapture.ExecuteIfBound();
	});
}

void FTileGenWorker::GetTilePlans(TArray<FTileGenPlan>& OutTilePlans) const
{
	Thread->WaitForCompletion();
	OutTilePlans.Empty(TilePlans.Num());

	for (const FTileGenPlan& TilePlan : TilePlans)
	{
		OutTilePlans.Emplace(TilePlan);
	}
}
