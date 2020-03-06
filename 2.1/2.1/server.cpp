#include <unordered_map>
#include "wdigraph.h"
#include <iostream>
#include <fstream> //for reading text file

using namespace std;

struct Point {
	long long lat; // latitude of the point
	long long lon; // longitude of the point
};

long long manhattan(const Point& pt1, const Point& pt2) {
	// Return the Manhattan distance between the two given points
	// |x1 - x2| + |y1 - y2|
	return abs(pt1.lat - pt2.lat) + abs(pt1.lon - pt2.lon);
}

void readGraph(string filename, WDigraph& graph, unordered_map<int, Point>& points) {
	/*
	Read the Edmonton map data from the provided file
	and load it into the given WDigraph object.
	Store vertex coordinates in Point struct and map
	each vertex to its corresponding Point struct.
	PARAMETERS:
	filename: name of the file describing a road network
	graph: an instance of the weighted directed graph (WDigraph) class
	points: a mapping between vertex identifiers and their coordinates
	*/
	double coord;
	static_cast <long long> (coord*100000);

	char charInput;
  	int ID, start, end;
  	string address;
  	Point coord_xy;

  	ifstream inFile; // read file object
  	inFile.open(filename); // open the file
  	// keep reading file if not end of file
  	while (!inFile.eof()){
  		inFile >> charInput; // (1) read V or E
  		if (charInput == 'V'){
  			// is a vertex
  			// V, ID, LAT, LON
  			inFile >> charInput; // (2) first comma
  			inFile >> ID; // (3) read ID
  			graph.addVertex(ID); // add vertex to graph
  			inFile >> charInput; // (4) second comma
  			inFile >> coord; // (5) lat
  			if (coord < 100){
  				coord_xy.lat = static_cast <long long> (coord*100000);
  			}else{
  				coord_xy.lat = static_cast <long long> (coord);
  			}
  			inFile >> charInput; // (6) third comma
  			inFile >> coord; // (7) lon
  			if (coord < 100){
  				coord_xy.lon = static_cast <long long> (coord*100000);
  			}else{
  				coord_xy.lon = static_cast <long long> (coord);
  			}
  			points[ID] = coord_xy; // add point to unordered map points
  			// go to next line
  		}else {
  			// is an edge
  			// E, start, end, name
  			inFile >> charInput; // (2) read in first comma
  			inFile >> start; // (3) read in start
  			inFile >> charInput; // (4) read in second comma
  			inFile >> end; // (5) read in end
  			// directed edge where cost is calculated using manhattan function
  			graph.addEdge(start,end,
  					manhattan(points[start],points[end]) );
  			inFile >> charInput; // (6) read in third comma
  			getline(inFile,address); // (7) read in rest of line as address
  			// go to next line
  		}
  	}
  	inFile.close(); //close file

}

int main(){
	  	
    WDigraph graph;

	unordered_map<int, Point> points;
	readGraph("edmonton-roads-2.0.1.txt",graph,points);

	cout<<graph.size()<<endl;
	#include<vector>
	vector<int> v = graph.vertices();
	for (auto i:v){
		cout<<i<<"  -> "<<points[i].lat<<" "<<points[i].lon<<endl;
	}cout<<endl<<endl<<endl;


}