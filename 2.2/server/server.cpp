/*
  Major Assignment #1 Part 1
  Restaurant Finder
  CMPUT 275 Winter 2020

  Names: Gurbani Baweja, Weichen Qiu
  ID: 1590254, 1578205
*/

#include <unordered_map>
#include "wdigraph.h" // file for graph
#include "dijkstra.h" // file for dijkstra algorithm
#include <iostream>
#include <fstream> //for reading text file
#include <list> //for calculated path
#include <cassert>
#include <string.h>
#include "serialport.h"

using namespace std;

SerialPort Serial("/dev/ttyACM0"); //intiialize serial communication

struct Point {
	long long lat; // latitude of the point
	long long lon; // longitude of the point
};

long long manhattan(const Point& pt1, const Point& pt2) {
/*
	 Return the Manhattan distance between the two given points
	 Arguments: the two points as Point struct to get manhattan distance of
	 Returns: manhattan distance as long long
*/
	// |x1 - x2| + |y1 - y2|
	return abs(pt1.lat - pt2.lat) + abs(pt1.lon - pt2.lon);
}

void readGraph(string filename, WDigraph& graph, unordered_map<int, Point>& points) {
/*
	Read the Edmonton map data from the provided file
	and load it into the given WDigraph object.
	Store vertex coordinates in Point struct and map
	each vertex to its corresponding Point struct.
	Arguments:
	filename: name of the file describing a road network
	graph: an instance of the weighted directed graph (WDigraph) class
	points: a mapping between vertex identifiers and their coordinates
*/
	// declare variables
	double coord;
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
  				// if decimal, use static_cast to get right format
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
  			graph.addEdge(start,end,manhattan(points[start],points[end]) );
  			inFile >> charInput; // (6) read in third comma
  			getline(inFile,address); // (7) read in rest of line as address
  			// go to next line
  		}
  	}
  	inFile.close(); //close file
}

void request(WDigraph graph, unordered_map<int, PIL> &tree,
			 unordered_map<int, Point> points){
/*
	Processes route request
	Arguments: graph with all vertices and edgies, unordered map to
			store search tree, points to store lat and lon of each vertex
	Returns: tree through pass by reference
*/
	// declare variables
	enum {INITIAL_REQUEST, CALCULATE_ROUTE, TIMEOUT} curr_mode = INITIAL_REQUEST;

	Point start; 
	Point end;
	string newLine;
	string A;
	string line;

	// read in user request
	if (curr_mode == INITIAL_REQUEST){
		// (1) get lat and lon of start and end points
		start.lat = stol(Serial.readline());
		start.lon = stol(Serial.readline());
		end.lat = stol(Serial.readline());
		end.lon = stol(Serial.readline());

		cout<<"R "<<start.lat<<" "<<start.lon<<" "<<
		end.lat<<" "<<end.lon<<endl;

		curr_mode = CALCULATE_ROUTE;
	}

	if (curr_mode == CALCULATE_ROUTE){
		// find the closest vertices using Points
		long long startMin = 500; // arbitrary large number
		long long endMin = 500;
		long long startVertex, endVertex;
		for (auto p: points){
			if (manhattan(p.second, start) < startMin){
				// if closer than start minimum
				startMin = manhattan(p.second, start);
				startVertex = p.first; // p.first is the vertex number
			}
			if (manhattan(p.second, end) < endMin){
				// same for end coordinates
				endMin = manhattan(p.second, end);
				endVertex = p.first;
			}
		}

		// generate search tree of startVertex using dijkstra 
		dijkstra(graph,startVertex,tree);

	    if (tree.find(endVertex) == tree.end()) {
	    	// no path found
	      	Serial.writeline("N 0\n");
	    }else{
			// calculate path using searchtree
			list<int> path;
			int stepping = endVertex;
			while (stepping != startVertex) {
			    path.push_front(stepping); // push to path
			    // crawl up the search tree one step
			    stepping = tree[stepping].first;
			}
			path.push_front(startVertex); // push startVertex to path

			// send size
			Serial.writeline("N "+to_string(path.size())+"\n");
			Serial.readline(1); // wait for acknowledgement
			cout<< "N "<<path.size();

			for (auto p: path) {
				// loop through all waypoints in path
				line = "W "+to_string(points[p].lat)+" "+to_string(points[p].lon)+"\n";
				Serial.writeline(line);
			    Serial.readline(1); //wait for acknowledgement
			}
			Serial.writeline("E\n"); //End
		}
	}
	
}

int main(){

    WDigraph graph;
    unordered_map<int, PIL> tree;
	unordered_map<int, Point> points;

	// read graph from txt file
	readGraph("edmonton-roads-2.0.1.txt",graph,points);

	// read request
	string clientRequest;
	
	while (true){
		cout<<"waiting for request"<<endl;
		clientRequest = Serial.readline();
		cout<<"request code: "<<clientRequest<<endl;
		
		if (strcmp(clientRequest.c_str(),"R")){
			// if begins with R
			cout<<"going to request"<<endl;
			request(graph, tree, points);
		}
		cout<<"after request"<<endl;
	}
	
}