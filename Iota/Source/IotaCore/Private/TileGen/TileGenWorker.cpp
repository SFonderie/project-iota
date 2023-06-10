// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenWorker.h"
#include "Engine/AssetManager.h"
#include "HAL/RunnableThread.h"
#include "Async/Async.h"

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
	Progress.Reset();

	bWorkComplete = false;
	bStopThread = false;
	bWithError = false;

	return true;
}

uint32 FTileGenWorker::Run()
{
	TArray<ETileScheme> Sequence;
	GenerateSequence(Sequence);

	// Core level generation loop. Worst-case scenario is O(n^4).
	for (int32 Tile = 0; Tile < Params.Size && !bStopThread; Tile++)
	{
		if (!PlaceNewTile(Sequence[Tile]))
		{
			bWithError = true;
			return 1;
		}
	}

	// Iterate through the tile plan and try to install stoppers on remaining open portals.
	// Numeric for-loop is needed here to prevent added stoppers from iterating.
	for (int32 Tile = 0; Tile < Params.Size && !bStopThread; Tile++)
	{
		for (int32 Index = 0; Index < TilePlans[Tile].Portals.Num() && !bStopThread; Index++)
		{
			if (TilePlans[Tile].IsOpenPortal(Index))
			{
				// No error code since any empty doors will just get sealed.
				PlaceStopper(TilePlans[Tile].Portals[Index]);
			}
		}

		// Regardless of whether any stoppers were placed, increment the progress per tile.
		Progress.Increment();
	}

	return 0;
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

float FTileGenWorker::Status() const
{
	// Divide by double the size because the loop runs twice.
	// Once to generate the plan, once to place stoppers.
	return Progress.GetValue() / (Params.Size * 2.0f);
}

void FTileGenWorker::Output(TArray<FTilePlan>& OutPlan) const
{
	OutPlan.Empty();

	// Only export the level plan if the thread completed naturally and without issue.
	// If the thread was stopped or had an error, the plan should be empty.
	if (bWorkComplete && !bStopThread && !bWithError)
	{
		for (const FTileGenPlan& Plan : TilePlans)
		{
			OutPlan.Emplace(Plan.GetTilePlan());
		}
	}
}

void FTileGenWorker::GenerateSequence(TArray<ETileScheme>& OutSequence)
{
	OutSequence.Add(ETileScheme::Start);
	OutSequence.Add(ETileScheme::Connection);

	for (int32 Index = 2; Index < Params.Size - 1; Index++)
	{
		ETileScheme Last1 = OutSequence[Index - 1];
		ETileScheme Last2 = OutSequence[Index - 2];

		switch (Last1)
		{
			case ETileScheme::Connection:
			{
				bool bIntermediate = Last2 == ETileScheme::Connection || Random.GetFraction() < 0.5f;
				OutSequence.Add(bIntermediate ? ETileScheme::Intermediate : ETileScheme::Connection);
				break;
			}

			default:
			{
				OutSequence.Add(ETileScheme::Connection);
				break;
			}
		}
	}

	OutSequence.Add(ETileScheme::Exit);
}

int32 FTileGenWorker::DetermineParent(int32 ParentIndex, ETileScheme Scheme)
{
	return Scheme != ETileScheme::Objective ? ParentIndex : -1;
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

bool FTileGenWorker::PlaceStopper(const FTilePortal& AttachPortal)
{
	ShuffleArray(TilePalettes[uint8(ETileScheme::Stopper)]);

	for (const FTileGenData& NewTile : TilePalettes[uint8(ETileScheme::Stopper)])
	{
		if (!NewTile.Portals.IsEmpty() && NewTile.Portals[0].CanConnect(AttachPortal))
		{
			FTransform NewTransform = FTilePortal::ConnectionTransform(NewTile.Portals[0], AttachPortal);

			if (CanPlaceTile(NewTile, NewTransform))
			{
				// Add the stopper without any of the normal registry settings.
				// Stoppers don't need to worry about portals or parents.
				TilePlans.Emplace(NewTile, NewTransform);
				return true;
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

	// Each orientation of the new tile is attempted for an open portal before moving to the next.
	for (const TPair<int32, int32>& OpenPair : OpenPortals)
	{
		for (const int32& TileIndex : TilePortals)
		{
			const FTilePortal& PlanPortal = TilePlans[OpenPair.Key].Portals[OpenPair.Value];
			const FTilePortal& NewPortal = NewTile.Portals[TileIndex];

			if (NewPortal.CanConnect(PlanPortal))
			{
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
