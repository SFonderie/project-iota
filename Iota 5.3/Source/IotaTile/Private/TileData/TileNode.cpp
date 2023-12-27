// Copyright Sydney Fonderie. All Rights Reserved.

#include "TileData/TileNode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TileNode)

FTileNode::FTileNode(const FVector& InLocation, const TArray<TSoftObjectPtr<AActor>>& InReferences)
	: Location(InLocation)
	, References(InReferences)
{
	// Complete constructor.
}

FTileNode FTileNode::WithTransform(const FTransform& Transform) const
{
	FTileNode NewNode(Transform.TransformPosition(Location), References);
	NewNode.EdgeIndices = EdgeIndices;
	return NewNode;
}

void FTileNode::MakeConnection(TArray<FTileNode>& NodeGraph, int32 IndexA, int32 IndexB)
{
	const double CosAngle = 0.995;

	// Create some convenient references.
	FTileNode& NodeA = NodeGraph[IndexA];
	FTileNode& NodeB = NodeGraph[IndexB];

	// Add the connection to each of the two nodes.
	int32 EdgeIndexAB = NodeA.EdgeIndices.Add(IndexB);
	int32 EdgeIndexBA = NodeB.EdgeIndices.Add(IndexA);

	// Calculate the vector formed by the edge.
	FVector EdgeVectorAB = NodeB.Location - NodeA.Location;
	double EdgeLengthAB = EdgeVectorAB.Length();

	// Iterate through each previous connection on Node A to resolve collinear edges.
	for (int32 EdgeIndexAC = 0; EdgeIndexAC < EdgeIndexAB; EdgeIndexAC++)
	{
		// Determine the iterated index and reference.
		int32 IndexC = NodeA.EdgeIndices[EdgeIndexAC];
		FTileNode& NodeC = NodeGraph[IndexC];

		// Locate the final edge index with a value search.
		int32 EdgeIndexCA = NodeC.EdgeIndices.IndexOfByKey(IndexA);

		// Calculate the dimensions of the other edge vector.
		FVector EdgeVectorAC = NodeC.Location - NodeA.Location;
		double EdgeLengthAC = EdgeVectorAC.Length();

		// Compare the two edges with cosine similarity. If the test returns true, then the two
		// edges are collinear and will need to be rearranged. To rearrange a collinear edge, the
		// start of the longer edge is rerouted to the end of the shorter edge and mirrored. 
		if ((EdgeVectorAB | EdgeVectorAC) > CosAngle * EdgeLengthAB * EdgeLengthAC)
		{
			if (EdgeLengthAB < EdgeLengthAC)
			{
				NodeA.EdgeIndices.RemoveAtSwap(EdgeIndexAC);
				NodeB.EdgeIndices.Add(IndexC);
				NodeC.EdgeIndices[EdgeIndexCA] = IndexB;
			}
			else
			{
				NodeA.EdgeIndices.RemoveAtSwap(EdgeIndexAB);
				NodeB.EdgeIndices[EdgeIndexBA] = IndexC;
				NodeC.EdgeIndices.Add(IndexB);
			}
		}
	}
}
