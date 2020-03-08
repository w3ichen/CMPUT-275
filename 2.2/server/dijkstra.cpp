/*
  Major Assignment #1 Part 1
  Restaurant Finder
  CMPUT 275 Winter 2020

  Names: Gurbani Baweja, Weichen Qiu
  ID: 1590254, 1578205
*/

#include "dijkstra.h"
using namespace std;

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

	BinaryHeap <PIL, int> events; // new heap structure
	HeapItem<PIL, int> fromHeap; // to store values returned from heap
	int v, u, time; // v is vertex, u is vertex before v 

	// PIL(previous vertex u, vertex v), time
	// insert startvertex to heap
	events.insert(PIL(startVertex,startVertex),0); 
	
	while (events.size() > 0){
		events.popMin(); // pop the min value
		fromHeap = events.min(); // gets the value from heap
		time = fromHeap.key; 
		u = fromHeap.item.first; // vertex preceding v
		v = fromHeap.item.second; // vertex

		if (tree.find(v) == tree.end()){ // not already in tree
			tree[v] = PIL(u, time); //record start of edge in tree
			// going through each of the neighbours
			for (auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {
		      	// *iter is neighbours, w
		      	// insert into heap
		        events.insert(PIL(v,*iter),(time + graph.getCost(v,*iter)));
		    }
		}
	}
};
