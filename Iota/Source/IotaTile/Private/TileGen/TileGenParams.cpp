// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#include "TileGen/TileGenParams.h"
#include "TileData/TileScheme.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileGenParams)

FTileGenParams::FTileGenParams()
	: Location(0, 0, 0)
	, Rotation(0, 0, 0)
{
	// Default constructor.
}

FTileGenParams::FTileGenParams(const FTileGenParams& Params)
	: Tileset(Params.Tileset)
	, MainObjective(Params.MainObjective)
	, SideObjectives(Params.SideObjectives)
	, Location(Params.Location)
	, Rotation(Params.Rotation)
	, ObjectiveCount(Params.ObjectiveCount)
	, Length(Params.Length)
	, Branch(Params.Branch)
	, Seed(Params.Seed)
	, AssetActors(Params.AssetActors)
{
	// Copy constructor.
}

void FTileGenParams::GetSchemeSequence(TArray<ETileScheme>& OutSequence) const
{
	OutSequence.Empty(Length);
	OutSequence.Add(ETileScheme::Start);

	// All other tiles in the sequence will be (main) objectives, connectors, or intermediates.
	// Objectives are the most important, so they should be divided evenly through the sequence.
	int32 BracketLength = FMath::Floor((Length - 2) / (ObjectiveCount + 1));

	// Brackets represent a sequence of tiles starting with an objective.
	// The first bracket is an exception, as it starts with a connector instead.
	// Iterate through the brackets, and then through the tiles in each bracket.
	for (int32 Bracket = 0; Bracket <= ObjectiveCount; Bracket++)
	{
		for (int32 Tile = 0; Tile < BracketLength; Tile++)
		{
			if (Tile == 0)
			{
				// First tile is an objective, except for the first bracket connector.
				OutSequence.Add(Bracket > 0 ? ETileScheme::Objective : ETileScheme::Connector);
			}
			else
			{
				// All other tiles should alternate between connector and intermediate.
				OutSequence.Add(Tile % 2 == 0 ? ETileScheme::Connector : ETileScheme::Intermediate);
			}
		}
	}

	// Special case. If the level has requested an objective tile, one exists, but none have been
	// added (due to a zero bracket length) then an objective tile should be included.
	if (ObjectiveCount > 0 && !OutSequence.Contains(ETileScheme::Objective))
	{
		OutSequence.Add(ETileScheme::Objective);
	}

	// Special case. The floor method is used to handle bracket division, so there will sometimes 
	// be a leftover tile. If that occurs, it should be filled with an extra connector.
	if (OutSequence.Num() < Length - 1)
	{
		OutSequence.Add(ETileScheme::Connector);
	}

	// If there is room for a last tile, add an exit.
	if (OutSequence.Num() < Length)
	{
		OutSequence.Add(ETileScheme::Exit);
	}
}
