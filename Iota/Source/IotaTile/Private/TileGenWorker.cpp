// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGenWorker.h"
#include "Engine/AssetManager.h"
#include "TileData/TileGenData.h"
#include "TileData/TileDataAsset.h"
#include "TileData/TilePlan.h"
#include "HAL/RunnableThread.h"
#include "Async/Async.h"

FTileGenWorker::FTileGenWorker(const FTileGenParams& InParams, const TArray<FPrimaryAssetId>& TileList)
	: Parameters(InParams)
	, RandomStream(InParams.Seed)
{
	UAssetManager& AssetManager = UAssetManager::Get();

	for (const FPrimaryAssetId& TileID : TileList)
	{
		if (const UTileDataAsset* TileDataAsset = AssetManager.GetPrimaryAssetObject<UTileDataAsset>(TileID))
		{
			bool bMainObjective = TileDataAsset->Objectives.HasTagExact(InParams.MainObjective);
			bool bSideObjective = TileDataAsset->Objectives.HasAnyExact(InParams.SideObjectives);
			bool bZeroObjective = TileDataAsset->Objectives.IsEmpty();

			for (ETileScheme Scheme : TEnumRange<ETileScheme>())
			{
				bool bSchemeMatch = 1 << Scheme & TileDataAsset->Schemes;
				bool bIsObjective = Scheme == ETileScheme::Objective;

				// If the tile is an objective tile, it can only be added to the objective palette
				// if the tile also matches the level main objective.
				bool bObjectiveMatch = bIsObjective && bMainObjective;

				// If the tile is not an objective tile, it can be added if the tile matches the
				// level main objective, any level side objective, or has no objective.
				bool bOtherMatch = !bIsObjective && (bMainObjective || bSideObjective || bZeroObjective);

				// In order to add a tile to a given palette, the tile must match the palette
				// scheme and meet one of the two conditions above.
				if (bSchemeMatch && (bObjectiveMatch || bOtherMatch))
				{
					TilePalettes[*Scheme].Emplace(TileDataAsset);
				}
			}
		}
	}
}

FTileGenWorker::~FTileGenWorker()
{
	if (Thread)
	{
		Thread->Kill();
		delete Thread;
	}
}

void FTileGenWorker::Start()
{
	Thread = FRunnableThread::Create(this, TEXT("TileGenerationThread"));
}

bool FTileGenWorker::Init()
{
	return true;
}

uint32 FTileGenWorker::Run()
{
	TArray<ETileScheme> Sequence;
	Parameters.GetTileSequence(Sequence);

	// Core loop. Builds out the main level path using the tile sequence.
	for (int32 Tile = 0; Tile < Parameters.Length && !bStopThread; Tile++)
	{
		if (!PlaceNewTile(Sequence[Tile]))
		{
			ErrorCode.Set(1);
			return 1;
		}
	}

	// Second loop. Goes through each portal on each planned tile and attempts to add a terminal
	// to each open portal. Doing so builds out the plan array, so a numeric for must be used.
	for (int32 Tile = 0; Tile < Parameters.Length && !bStopThread; Tile++)
	{
		for (int32 Index = 0; Index < TilePlans[Tile].Portals.Num() && !bStopThread; Index++)
		{
			if (TilePlans[Tile].IsOpenPortal(Index))
			{
				// No error code since empty doors can just be sealed.
				PlaceTerminal(TilePlans[Tile].Portals[Index]);
			}
		}

		// Increment regardless of success.
		Progress.Increment();
	}

	return 0;
}

bool FTileGenWorker::PlaceNewTile(ETileScheme Scheme)
{
	TArray<FTileGenData>& Palette = TilePalettes[*Scheme];
	ShuffleArray(Palette);

	for (int32 Index = 0; Index < Palette.Num() && !bStopThread; Index++)
	{
		if (TryPlaceTile(Palette[Index], Scheme))
		{
			if (Scheme == ETileScheme::Objective)
			{
				Palette.RemoveAtSwap(Index);
			}

			Progress.Increment();
			return true;
		}
	}

	return false;
}

bool FTileGenWorker::PlaceTerminal(const FTilePortal& AttachPortal)
{
	TArray<FTileGenData>& Palette = TilePalettes[*ETileScheme::Terminal];
	ShuffleArray(Palette);

	for (const FTileGenData& NewTile : Palette)
	{
		for (const FTilePortal& NewPortal : NewTile.Portals)
		{
			if (NewPortal.CanConnect(AttachPortal))
			{
				FTransform NewTransform = FTilePortal::ConnectionTransform(NewPortal, AttachPortal);

				if (CanPlaceTile(NewTile, NewTransform))
				{
					// Add the terminal without any of the normal registry settings.
					// Terminals don't need to worry about portals or parents.
					TilePlans.Emplace(NewTile, NewTransform);
					return true;
				}
			}
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

	// Gather viable portal connections for the new tile.
	// Existing portals are stored as Plan, Portal.
	TArray<TPair<int32, int32>> OpenPortals;
	int32 PlanIndex = TilePlans.Num() - 1;
	int32 Depth = 0;

	// Traverse the tile plan array as if it was a tree and add any open portals to the list.
	// Traversal will continue until a null parent is found or the branch length is reached.
	while (0 <= PlanIndex && PlanIndex < TilePlans.Num() && Depth < Parameters.Branch)
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
		if (Scheme == ETileScheme::Objective && !OpenPortals.IsEmpty())
		{
			break;
		}

		// Traverse to the plan's parent and continue.
		PlanIndex = PlanValue.ParentIndex;
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

	// Each orientation of the new tile is attempted for an open portal before moving to the next.
	for (const TPair<int32, int32>& OpenPair : OpenPortals)
	{
		for (const int32& NewOpenIndex : TilePortals)
		{
			const FTilePortal& PlanPortal = TilePlans[OpenPair.Key].Portals[OpenPair.Value];
			const FTilePortal& NewPortal = NewTile.Portals[NewOpenIndex];

			if (NewPortal.CanConnect(PlanPortal))
			{
				FTransform NewTransform = FTilePortal::ConnectionTransform(NewPortal, PlanPortal);

				if (CanPlaceTile(NewTile, NewTransform))
				{
					// Placement check successful; create the tile plan now.
					FTileGenPlan NewPlan = FTileGenPlan(NewTile, NewTransform);

					// New tile should be parented.
					NewPlan.ParentIndex = OpenPair.Key;

					// Close the portals used in the connection.
					TilePlans[OpenPair.Key].ClosePortal(OpenPair.Value);
					NewPlan.ClosePortal(NewOpenIndex);

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

template <typename InElementType>
void FTileGenWorker::ShuffleArray(TArray<InElementType>& Array)
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
	bWorkComplete = true;

	// Move back to the game thread by invoking the exit delegate asynchronously. The lambda uses
	// a value capture to access the delegate because the worker might get destroyed.
	AsyncTask(ENamedThreads::GameThread, [OnExitCapture = OnExit]()
	{
		OnExitCapture.ExecuteIfBound();
	});
}

float FTileGenWorker::GetProgress() const
{
	// Divide by double the size because the loop runs twice: once to generate the plan, once to
	// place stoppers. Ensure that the length won't cause a divide by zero error.
	return Progress.GetValue() / (FMath::Max(Parameters.Length, 1) * 2.0f);
}

int32 FTileGenWorker::GetErrorCode() const
{
	return ErrorCode.GetValue();
}

bool FTileGenWorker::IsComplete() const
{
	return bWorkComplete && !bStopThread && ErrorCode.GetValue() == 0;
}

template <> // Tile Plan specialization.
void FTileGenWorker::GetPlan(TArray<FTilePlan>& OutPlan) const
{
	OutPlan.Empty();

	if (IsComplete())
	{
		for (const FTileGenPlan& Plan : TilePlans)
		{
			OutPlan.Emplace(Plan.GetTilePlan());
		}
	}
}

template <> // Tile Bound specialization.
void FTileGenWorker::GetPlan(TArray<FTileBound>& OutPlan) const
{
	OutPlan.Empty();

	if (IsComplete())
	{
		for (const FTileGenPlan& Plan : TilePlans)
		{
			OutPlan.Append(Plan.Bounds);
		}
	}
}

template <> // Tile Portal specialization.
void FTileGenWorker::GetPlan(TArray<FTilePortal>& OutPlan) const
{
	OutPlan.Empty();

	if (IsComplete())
	{
		for (const FTileGenPlan& Plan : TilePlans)
		{
			OutPlan.Append(Plan.Portals);
		}
	}
}
