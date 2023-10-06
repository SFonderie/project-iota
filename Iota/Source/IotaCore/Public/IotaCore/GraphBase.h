// Copyright Sydney Fonderie, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Implements an undirected graph structure that stores both nodes and edges as objects with data.
 *
 * @param NodeData Data type to store within each graph node.
 * @param EdgeData Data type to store within each graph edge. Defaults to an integer weight value.
 */
template <typename NodeData, typename EdgeData = int32>
class TGraphBase
{

public:

	struct FGraphNode;
	struct FGraphEdge;

	/**
	 * Graph node container. Each node container initializes with a unique data object defined by
	 * the graph template parameters and keeps track of its neighbors via an array of edges.
	 */
	struct FGraphNode
	{
		/** Edges directed out from this node. */
		TArray<FGraphEdge> Edges;

		/** Data stored within this node. */
		TUniquePtr<NodeData> Data;

		/**
		 * Constructs a new node container and initializes its node data object using the provided
		 * parameters.
		 *
		 * @param Params Parameters to be forwarded to the node data object constructor.
		 */
		template <typename... NodeDataParams>
		FGraphNode(NodeDataParams&&... Params)
		{
			Data = MakeUnique<NodeData>(Params...);
		}

		/**
		 * Returns the node degree, measured as the number of adjacent edges.
		 *
		 * @return Number of edges connected to the node.
		 */
		int32 GetDegree() const
		{
			return Edges.Num();
		}

		/**
		 * Returns a constant reference to the edge container stored at the provided edge index.
		 *
		 * @param Index Edge index for which to return a reference.
		 * @return Edge container stored at the provided index.
		 */
		const FGraphEdge& GetEdge(int32 Index) const
		{
			return Edges[Index];
		}

		/**
		 * Returns a constant reference to the node container tied to the provided edge index.
		 *
		 * @param Index Edge index for which to return a reference.
		 * @return Node container tied to the provided index.
		 */
		const FGraphNode& GetConnection(int32 Index) const
		{
			return *GetEdge(Index).Connection.Pin();
		}

		/**
		 * Returns a mutable reference to the edge data object tied to the provided edge index.
		 *
		 * @param Index Edge index for which to return a reference.
		 * @return Edge data object tied to the provided index.
		 */
		EdgeData& GetEdgeData(int32 Index) const
		{
			return *GetEdge(Index).Data;
		}
	};

	/**
	 * Graph edge container. Each edge container represents a directed relationship between two
	 * nodes where only the destination node is known. Edge containers are therefore created in
	 * mirrored pairs to simulate undirected behavior.
	 *
	 * Each pair initializes with a shared data object defined by the graph template parameters,
	 * and each edge container tracks its connected node via a weak pointer.
	 */
	struct FGraphEdge
	{
		/** Node to which this edge is directed. */
		TWeakPtr<FGraphNode> Connection;

		/** Data shared within this edge. */
		TSharedPtr<EdgeData> Data;

		/**
		 * Constructs a new edge container directed at the provided node address but does not
		 * initialize its data. Edge data objects should be shared across edge container pairs,
		 * so they should be constructed externally and assigned into each edge individually.
		 *
		 * @param InConnection Node to which the new edge will be directed.
		 */
		FGraphEdge(const TSharedPtr<FGraphNode>& InConnection) : Connection(InConnection)
		{
			// Complete constructor.
		}
	};

	/** Constructs an empty graph. */
	TGraphBase()
	{
		// Default constructor.
	}

	/**
	 * Creates a new node container, adds it to the graph, and then initializes its node data
	 * object using the provided parameters.
	 *
	 * @param Params Parameters to be forwarded to the node data object constructor.
	 * @return Index of the new node.
	 */
	template <typename... NodeDataParams>
	int32 MakeNode(NodeDataParams&&... Params)
	{
		return Nodes.Emplace(MakeShared<FGraphNode>(Params...));
	}

	/**
	 * Creates a new pair of edge containers, adds them to the graph, and then initializes and
	 * returns their shared edge data object using the provided parameters.
	 *
	 * @param NodeIndexA Graph index of the first node to connect.
	 * @param NodeIndexB Graph index of the second node to connect.
	 * @param Params Parameters to be forwarded to the edge data object constructor.
	 * @return Mutable reference to the new edge data object.
	 */
	template <typename... EdgeDataParams>
	EdgeData& MakeEdge(int32 NodeIndexA, int32 NodeIndexB, EdgeDataParams&&... Params)
	{
		// Disallow loops.
		check(NodeIndexA != NodeIndexB);

		// Create an empty pair of mirrored edges on each of the indexed nodes.
		int32 EdgeIndexA = Nodes[NodeIndexA]->Edges.Emplace(Nodes[NodeIndexB]);
		int32 EdgeIndexB = Nodes[NodeIndexB]->Edges.Emplace(Nodes[NodeIndexA]);

		// Create the new edge data object and pass in the parameters.
		TSharedPtr<EdgeData> NewEdgeData = MakeShared<EdgeData>(Params...);

		// Load the shared edge data object into each new edge.
		Nodes[NodeIndexA]->Edges[EdgeIndexA].Data = NewEdgeData;
		Nodes[NodeIndexB]->Edges[EdgeIndexB].Data = NewEdgeData;

		// Return the data.
		return *NewEdgeData;
	}

	/**
	 * Returns the number of nodes currently stored within the graph.
	 *
	 * @return Number of nodes in the graph.
	 */
	int32 GetSize() const
	{
		return Nodes.Num();
	}

	/**
	 * Returns a constant reference to the node container stored at the provided node index.
	 *
	 * @param Index Node index for which to return a reference.
	 * @return Node container stored at the provided index.
	 */
	const FGraphNode& GetNode(int32 Index) const
	{
		return *Nodes[Index];
	}

	/**
	 * Returns a mutable reference to the node data object tied to the provided node index.
	 *
	 * @param Index Node index for which to return a reference.
	 * @return Node data object tied to the provided index.
	 */
	NodeData& GetNodeData(int32 Index) const
	{
		return *GetNode(Index).Data;
	}

	/** Empties the graph and discards all graph data. */
	void Empty()
	{
		Nodes.Empty();
	}

private:

	/** Nodes stored within the graph. */
	TArray<TSharedPtr<FGraphNode>> Nodes;
};
