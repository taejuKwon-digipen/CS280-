#include "ALGraph.h"
#include <algorithm>
#include <iostream>
#include <queue>
#include <functional>

ALGraph::ALGraph(unsigned size)
{
	for (unsigned i = 0; i < size; i++)
	{
		adjinfovec.push_back(AdjacencyInfo());
		alist.push_back(std::vector<AdjacencyInfo>());
	}
}

ALGraph::~ALGraph()
{
	alist.clear();
	adjinfovec.clear();
}

void ALGraph::AddDEdge(unsigned source, unsigned destination, unsigned weight)
{
	unsigned i = source - 1;
	adjinfovec[i].id = destination;
	adjinfovec[i].weight = weight;
	alist[i].push_back(adjinfovec[i]);
	std::sort(alist[i].begin(), alist[i].end(), [](AdjacencyInfo& a, AdjacencyInfo& b) -> bool
		{
			if (a.weight == b.weight)
			{
				return a.id < b.id;
			}
			else {
				return a.weight < b.weight;
			}
		}
	);
}

void ALGraph::AddUEdge(unsigned node1, unsigned node2, unsigned weight)
{
	AddDEdge(node1, node2, weight);
	AddDEdge(node2, node1, weight);
}

std::vector<unsigned> ALGraph::SearchFrom([[maybe_unused]] unsigned start_node, [[maybe_unused]] TRAVERSAL_METHOD method) const
{
	return std::vector<unsigned>();
}

std::vector<DijkstraInfo> ALGraph::Dijkstra([[maybe_unused]] unsigned start_node) const
{
	std::vector<DijkstraInfo> dijvec;

	unsigned i = 0;

	for (i = 0; i < alist.size(); i++)
	{
		if (adjinfovec[i].id != start_node)
		{
			dinfo->cost += adjinfovec[i].weight;
			dinfo->path.push_back(i);
		}
	}
	dijvec[i].cost = dinfo->cost;
	dijvec[i].path = dinfo->path;

	return dijvec;

}

ALIST ALGraph::GetAList() const
{
	return alist;
}

bool ALGraph::ImplementedSearches()
{
	return false;
}


