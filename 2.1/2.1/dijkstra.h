#ifndef _DIJKSTRA_H_
#define _DIJKSTRA_H_

#include "wdigraph.h"
#include "heap.h"
#include <utility>
#include <unordered_map>
#include <iostream>

using namespace std;

// for brevity
// typedef introduces a synonym (alias)
// for the type specified
typedef pair<int, long long> PIL;

// again, for brevity
// used to store a vertex v and its predecessor pair (u,d) on the search
// where u is the node prior to v on a path to v of cost d
// eg. PIPIL x;
// x.first is "v", x.second.first is "u" and x.second.second is "d" from this
// v = vertex  ;  u = node before v  ;  d = cost
// (v,(u,d)) => (x.first,(x.second.first, x.second.second))
typedef pair<int, PIL> PIPIL;

// NOTE: you are not required to use PIPIL in your solution if you would prefer
// to implement Dijkstra's algorithm differently, this is here simply because
// it was used in the lecture for the slower version of Dijkstra's algorithm.

void dijkstra(const WDigraph& graph, int startVertex,
              unordered_map<int, PIL>& tree){
	/*
	Compute least cost paths that start from a given vertex
	Use a binary heap to efficiently retrieve an unexplored
	vertex that has the minimum distance from the start vertex
	at every iteration
	PIL is an alias for "pair<int, long long>" type as discussed in class
	PARAMETERS:
	WDigraph: an instance of the weighted directed graph (WDigraph) class
	startVertex: a vertex in this graph which serves as the root of the search tree
	tree: a search tree used to construct the least cost path to some vertex
	*/
	// PIL is (time,(u,v))
	BinaryHeap <PIL, int> events; // new heap structure
	HeapItem<PIL, int> fromHeap; // to store values returned from heap
	// PIL(previous vertex u, vertex v), time
	events.insert(PIL(startVertex,startVertex),0); // insert startvertex to heap
	int v, u, time; // v is vertex, u is vertex before v 
	while (events.size() > 0){
		events.popMin(); // pop the min value
		fromHeap = events.min(); // gets the value from heap
		time = fromHeap.key; 
		u = fromHeap.item.first; // vertex before v
		v = fromHeap.item.second; // vertex

		if (tree.find(v) == tree.end()){ // not already in tree
			tree[v] = PIL(u, time); //record start of edge in tree
			// going through each of the neighbours
			for (auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {
		      	// *iter is neighbours, w
		        events.insert(PIL(v,*iter),(time + graph.getCost(v,*iter)));
		      
		    }
		}
	}



};

#endif
