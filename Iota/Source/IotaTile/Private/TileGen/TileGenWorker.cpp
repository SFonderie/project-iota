// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenWorker.h"
#include "TileGen/TileGraphPlan.h"
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
				TilePalettes[*Scheme].Emplace(TileDataAsset->GetTileData());
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

bool FTileGenWorker::Init()
{
	Params.GetSchemeSequence(Sequence);
	Progress.Reset();

	return true;
}

uint32 FTileGenWorker::Run()
{
	// Core loop. Builds out the main level path using the tile sequence.
	for (int32 Tile = 0; Tile < Params.Length && !bStopThread; Tile++)
	{
		if (!PlaceNewTile(Sequence[Tile]))
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

void FTileGenWorker::Tick()
{
	int32 Tile = Progress.GetValue();

	// Core branch. Builds out the main level path using the tile sequence.
	if (Tile < Params.Length && !bStopThread)
	{
		if (!PlaceNewTile(Sequence[Tile]))
		{
			Thread->Kill();
		}
	}

	// Second branch. Goes through each main tile and adds terminal seals.
	else if (Tile < Params.Length * 2 && !bStopThread)
	{
		PlaceTerminals(Tile - Params.Length);
	}

	// Exit condition.
	else
	{
		Thread->Kill();
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

FSingleThreadRunnable* FTileGenWorker::GetSingleThreadInterface()
{
	return this;
}

bool FTileGenWorker::PlaceNewTile(ETileScheme Scheme)
{
	TArray<FTileData>& Palette = TilePalettes[*Scheme];
	ShuffleArray(Palette);

	for (int32 Index = 0; Index < Palette.Num() && !bStopThread; Index++)
	{
		if (TryPlaceTile(Palette[Index]))
		{
			if (IsObjective(Scheme))
			{
				Palette.RemoveAtSwap(Index);
			}

			Progress.Increment();
			return true;
		}
	}

	return false;
}

bool FTileGenWorker::TryPlaceTile(const FTileData& NewTile)
{
	if (TileMap.IsEmpty())
	{
		TileMap.Emplace(NewTile, FTransform(Params.Rotation, Params.Location));
		return true;
	}

	// Gather viable portal connections for the new tile.
	// Existing portals are stored as Plan, Portal.
	TArray<TPair<int32, int32>> OpenPortals;
	int32 PlanIndex = TileMap.Num() - 1;
	int32 Depth = 0;

	// Traverse the tile map from the top-down like a tree and add any open portals to the list.
	// Traversal continues until an invalid parent is found or the branch length is reached.
	while (0 <= PlanIndex && PlanIndex < TileMap.Num() && Depth < Params.Branch)
	{
		const FTileGraphPlan& PlanValue = TileMap[PlanIndex];

		for (int32 Index = 0; Index < PlanValue.Portals.Num(); Index++)
		{
			if (PlanValue.IsOpenPortal(Index))
			{
				OpenPortals.Emplace(PlanIndex, Index);
			}
		}

		// If the plan is an objective and there is at least one open portal, then stop searching
		// for additional portals. Doing so turns objectives with more than one portal into a sort
		// of "one-way valve" that divides the tile map into "before" and "after" sections.
		if (IsObjective(Sequence[PlanIndex]) && !OpenPortals.IsEmpty())
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
			const FTilePortal& MapPortal = TileMap[OpenIndex.Key].Portals[OpenIndex.Value];
			const FTilePortal& NewPortal = NewTile.Portals[NewIndex];

			if (NewPortal.CanConnect(MapPortal))
			{
				FTransform NewTransform = FTilePortal::ConnectionTransform(NewPortal, MapPortal);

				if (CanPlaceTile(NewTile, NewTransform))
				{
					// Placement check successful; create the tile plan now.
					FTileGraphPlan NewPlan = FTileGraphPlan(NewTile, NewTransform);

					// Make the map tile into the parent of the new tile.
					NewPlan.SetConnection(NewIndex, OpenIndex.Key, true);

					// Make the new tile (index = length) into a child of the map tile.
					TileMap[OpenIndex.Key].SetConnection(OpenIndex.Value, TileMap.Num());

					// Append the plan and exit.
					TileMap.Emplace(NewPlan);
					return true;
				}
			}
		}
	}

	return false;
}

void FTileGenWorker::PlaceTerminals(int32 PlanIndex)
{
	for (int32 Portal = 0; Portal < TileMap[PlanIndex].Portals.Num() && !bStopThread; Portal++)
	{
		if (TileMap[PlanIndex].IsOpenPortal(Portal))
		{
			TryPlaceTerminal(PlanIndex, Portal);
		}
	}

	Progress.Increment();
}

void FTileGenWorker::TryPlaceTerminal(int32 PlanIndex, int32 Portal)
{
	TArray<FTileData>& Palette = TilePalettes[*ETileScheme::Terminal];
	ShuffleArray(Palette);

	for (const FTileData& NewTile : Palette)
	{
		for (int32 NewIndex = 0; NewIndex < NewTile.Portals.Num(); NewIndex++)
		{
			const FTilePortal& MapPortal = TileMap[PlanIndex].Portals[Portal];
			const FTilePortal& NewPortal = NewTile.Portals[NewIndex];

			if (NewPortal.CanConnect(MapPortal))
			{
				FTransform NewTransform = FTilePortal::ConnectionTransform(NewPortal, MapPortal);

				if (CanPlaceTile(NewTile, NewTransform))
				{
					// Placement check successful; create the tile plan now.
					FTileGraphPlan NewPlan = FTileGraphPlan(NewTile, NewTransform);

					// Make the map tile into the parent of the new tile.
					NewPlan.SetConnection(NewIndex, PlanIndex, true);

					// Make the new tile (index = length) into a child of the map tile.
					TileMap[PlanIndex].SetConnection(Portal, TileMap.Num());

					// Append the plan and exit.
					TileMap.Emplace(NewPlan);
					return;
				}
			}
		}
	}
}

bool FTileGenWorker::CanPlaceTile(const FTileData& NewTile, const FTransform& Transform) const
{
	for (const FTileBound& NewBound : NewTile.Bounds)
	{
		// Transform the new bound into world space.
		FTileBound TestBound = FTileBound(NewBound, Transform);

		// Compare the new bound with each existing bound.
		for (const FTileGraphPlan& Tile : TileMap)
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

template <typename ElementType>
void FTileGenWorker::ShuffleArray(TArray<ElementType>& Array)
{
	for (int32 i = 0; i < Array.Num() - 1; i++)
	{
		Array.Swap(i, RandomStream.RandRange(i + 1, Array.Num() - 1));
	}
}

void FTileGenWorker::GetRawTileMap(TArray<FTileGraphPlan>& OutTileMap) const
{
	Thread->WaitForCompletion();
	OutTileMap = TileMap;
}
