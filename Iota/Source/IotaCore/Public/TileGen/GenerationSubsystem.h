// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TileData/TileDataAsset.h"
#include "GameplayTagContainer.h"
#include "GenerationSubsystem.generated.h"

struct FStreamableHandle;

/** Provides an interface for tile level generation. */
UCLASS()
class IOTACORE_API UGenerationSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	/**
	 * Begins generating a random tile-based level from the given tileset and seed.
	 *
	 * @param Tileset Gameplay tag identifying the tileset to use for generation.
	 * @param Seed Random seed used to generate the tile via a random stream.
	 */
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void BeginGeneration(const FGameplayTag& Tileset, int32 Seed);

private:

	/**
	 * Searches the asset registry for tile data assets tagged with the given tileset and loads the
	 * results into the subsystem's member array.
	 *
	 * @param Tileset Gameplay tag identifying the desired tileset.
	 */
	void FilterTileAssetPrimaryIDs(const FGameplayTag& Tileset);

	/**
	 * Callback function used at the end of the asset load to continue generation. Initializes the
	 * tile selectors and then actually begins generation.
	 */
	void AssetLoadCompleted();

	/**
	 * Clears the tile selectors and loads the contents of the asset load handle into them.
	 */
	void InitializeTileSelectors();

	/**
	 * Releases assets involved with level generation.
	 */
	void ReleaseGeneration();

	/** List of all tile asset primary IDs currently being used by the subsystem. */
	TArray<FPrimaryAssetId> TileAssetPrimaryIDs;

	/** Handle representing the tile data asset load during pre-generation. */
	TSharedPtr<FStreamableHandle> AssetLoadHandle;

	/** Provides the random number generation needed to select tiles. */
	FRandomStream GenerationSeed;

	/**
	 * Array wrapper providing random access utilities. During load, the generation subsystem uses
	 * tile selectors to hold references to loaded tile data assets.
	 */
	struct FTileSelector
	{
		/** Tile data within a selector should be read-only. */
		typedef const UTileDataAsset* UTileData;

		/** Stores the selector's tile data assets. */
		TArray<UTileData> TileDataAssets;

		/** Length of the filter exclusion at the end of the array. */
		int32 FilterLength = 0;

		/** Length of the ignore exclusion at the front of the array. */
		int32 IgnoreLength = 0;

		/**
		 * Attempts to randomly select a tile from the selector's active range.
		 *
		 * @param InRandom Random stream with which to select a tile.
		 * @param OutTileData Returns the selected tile data here, if applicable.
		 * @param OutIndex Returns the selected index here, if applicable.
		 * @return Whether there are any tiles left to select.
		 */
		bool GetRandomTile(const FRandomStream& Random, UTileData& OutTileData, int32& OutIndex) const;

		/**
		 * Accepts the tile at the given index. Doing so resets the selector and moves the tile to
		 * the ignore filter, preventing it from being selected again for a few cycles.
		 */
		void AcceptTile(int32 Index, int32 MaxIgnoreLength = 1);

		/**
		 * Rejects the tile at the given index. Doing so moves the tile into the selection filter,
		 * preventing it from being selected again until the selector is reset.
		 */
		void RejectTile(int32 Index);
	};

	/** Array of tile selectors keyed to tile schemes. */
	FTileSelector TileSelectors[(uint8)ETileScheme::Count];

	/** @return Tile selector keyed to the given scheme index. */
	FTileSelector& Selector(ETileScheme SchemeIndex)
	{
		return TileSelectors[(uint8)SchemeIndex];
	}
};
