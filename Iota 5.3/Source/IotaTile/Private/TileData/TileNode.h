// Copyright Sydney Fonderie. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TileNode.generated.h"

class AActor;

/** Area graph node structure for use in tile level data. */
USTRUCT()
struct FTileNode
{
	GENERATED_BODY()

public:

	/** Location of the tile node in world space. */
	UPROPERTY(EditAnywhere)
	FVector Location;

	/** Gameplay actors relevant to this tile node. */
	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<AActor>> References;

	/** Tile nodes to which this node is connected. */
	UPROPERTY(EditAnywhere)
	TArray<int32> EdgeIndices;

	/** Defines an empty tile node structure. */
	FTileNode() = default;

	/**
	 * Defines a tile node with the given location and references. Node connections can be built
	 * after construction via the static MakeConnection method.
	 *
	 * @param InLocation Location of the area node in world space.
	 * @param InReferences Gameplay actors relevant to this area node.
	 */
	FTileNode(const FVector& InLocation, const TArray<TSoftObjectPtr<AActor>>& InReferences);

	/**
	 * Transforms the tile node and returns the result.
	 *
	 * @param Transform Transform to be applied to the new node.
	 * @return Transformed tile node.
	 */
	FTileNode WithTransform(const FTransform& Transform) const;

	/**
	 * Internally handles the logic needed to connect the node at the second index to the node at
	 * the first index within the graph array provided.
	 *
	 * @param NodeGraph Graph array containing the nodes to connect.
	 * @param IndexA Graph index of the first node to connect.
	 * @param IndexB Graph index of the second node to connect.
	 */
	static void MakeConnection(TArray<FTileNode>& NodeGraph, int32 IndexA, int32 IndexB);
};
