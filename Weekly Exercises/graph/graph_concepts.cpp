/*
	Name: Weichen Qiu
	ID: 1578205
	CMPUT 275 Winter 2020

	Weekly Exercise #5: Graph Concepts
*/
#include <iostream>
#include <fstream>
#include "digraph.cpp"
#include <cassert>
#include <string>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <list>

using namespace std;

unordered_map<int, int> breadthFirstSearch(const Digraph& graph, int startVertex) {
/*
	function copied from breadthfirstsearch file to search through the graph
	and return the searchtree
	Arguments: the graph to search and the start vertex to start searching from
	Returns: search tree as unordered map
*/

  unordered_map<int, int> searchTree; // map each vertex to its predecessor

  searchTree[startVertex] = -1;

  queue<int> q;
  q.push(startVertex);

  while (!q.empty()) {
    int v = q.front();
    q.pop();

    for (auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {
      if (searchTree.find(*iter) == searchTree.end()) {
        searchTree[*iter] = v;
        q.push(*iter);
      }
    }
  }
  return searchTree;
}

int count_components(Digraph* g) {
/* 
	Counts number of components in the graph
	Arguments: the graph to count components of as a pointer
	Returns: integer of the number of components in graph
*/
	// unordered map to store the returned searchtree
	unordered_map<int, int> searchTree;

	// get a vector of all the vertices in the graph
	vector<int> vertices = g->vertices();
	unordered_set<int> output;
	for (auto v : vertices){
		// copy vertices over to unordered set for faster reads than vector
		output.insert(v);
	}
	// counter to count number of components
	int counter = 0;
	// keep looping until set is empty
  	while (output.size() != 0){
  		// always look at the same index because old numbers are erased
  		int v = *output.begin();
  		// get the searchtree of the vertex v
  		searchTree = breadthFirstSearch(*g,v);
  		for (auto it : searchTree) {
  			// erase all the neighbours of that vertex so the same component is not repeated
  			// it.first is the neighbours
  			output.erase(v);
  			output.erase(it.first);
   		}
   		// this is one component so increment counter
   		counter ++;
  	}
    return counter;
}


Digraph* read_city_graph_undirected(char filename[]) {
/*
	Reads the text file and adds vertices and edges to the graph
	Arguments: the name of the file to read from
	Returns: the graph with all the edges and vertices added
*/
	// create new digraph to store vertices and edges
  	Digraph* outputGraph = new Digraph;
  	// file reads
  	char charInput;
  	int intInput;
  	int intInput2;
  	float floatInput;
  	string address;

  	ifstream inFile; // read file object
  	inFile.open(filename); // open the file
  	if (inFile.fail()){
  		// error checking
  		assert("Error opening file");
  	}

  	// keep reading file if not end of file
  	while (!inFile.eof()){
  		// is a vertex
  		// V, ID, LAT, LON
  		inFile >> charInput; // (1) read V or E
  		if (charInput == 'V'){
  			inFile >> charInput; // (2) first comma
  			inFile >> intInput; // (3) read ID
  			outputGraph->addVertex(intInput); // add vertex to graph
  			inFile >> charInput; // (4) second comma
  			inFile >> floatInput; // (5) lat
  			inFile >> charInput; // (6) third comma
  			inFile >> floatInput; // (7) lon
  			// go to next line
  		}else {
  			// is an edge
  			// E, start, end, name
  			inFile >> charInput; // (2) read in first comma
  			inFile >> intInput; // (3) read in start
  			inFile >> charInput; // (4) read in second comma
  			inFile >> intInput2; // (5) read in end
  			outputGraph->addEdge(intInput,intInput2);
  			outputGraph->addEdge(intInput2,intInput);
  			inFile >> charInput; // (6) read in third comma
  			getline(inFile,address); // (7) read in rest of line as address
  			// go to next line
  		}
  	}
  	inFile.close(); //close file
  	return outputGraph;
}


int main(int argc, char *argv[]) {

	//FILE:  "edmonton-roads-2.0.1.txt";
	// reads in graph from text file and counts the number of components
	
	cout << count_components(read_city_graph_undirected(argv[1]))<<endl;

    return 0;
}