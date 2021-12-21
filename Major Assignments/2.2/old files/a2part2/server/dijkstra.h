#ifndef _DIJKSTRA_H_
#define _DIJKSTRA_H_

#include "wdigraph.h"
#include "heap.h"
#include <utility>
#include <unordered_map>
#include <iostream>

using namespace std;

// used to store a vertex v and its predecessor pair (u,d) on the search
// where u is the node prior to v on a path to v of cost d
// eg. PIPIL x;
// x.first is "v", x.second.first is "u" and x.second.second is "d" from this
// v = vertex  ;  u = node before v  ;  d = cost
// (v,(u,d)) => (x.first,(x.second.first, x.second.second))
typedef pair<int, long long> PIL;

// predeclaration of dijkstra algorithmn to return a search tree 
void dijkstra(const WDigraph& graph, int startVertex, unordered_map<int, PIL>& tree);


#endif
