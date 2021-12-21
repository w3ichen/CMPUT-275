#include <iostream>
#include <list>
#include <utility> // for pair()
// pair is a simple container of two heterogeneous objects
// which can be accessed by dot operator followed by 
// first or second keyword

#include <unordered_map>
#include "wdigraph.h"



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
// for brevity
// typedef introduces a synonym (alias) 
// for the type specified
typedef pair<int, long long> PIL;

// again, for brevity
// used to store a vertex v and its predecessor pair (u,d) on the search
// where u is the node prior to v on a path to v of cost d
// eg. PIPIL x;
// x.first is "v", x.second.first is "u" and x.second.second is "d" from this
typedef pair<int, PIL> PIPIL;

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
            //cout<<"adding vertex ID: "<<ID<<endl;
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
            graph.addEdge(end,start,
                    manhattan(points[start],points[end]) );
            //cout<<"adding edge: "<<start<<" to "<<end<<" and cost: "<<manhattan(points[start],points[end])<<endl;
            inFile >> charInput; // (6) read in third comma
            getline(inFile,address); // (7) read in rest of line as address
            // go to next line
        }
    }
    inFile.close(); //close file

}

void dijkstra(const WDigraph& graph, int startVertex, 
    unordered_map<int, PIL>& searchTree) {

    // All active fires stored as follows:
    // say an entry is (v, (u, d)), then there is a fire that started at u
    // and will burn the u->v edge, reaching v at time d
    list<PIPIL> fires;

    // at time 0, the startVertex burns, we use -1 to indicate there is
    // no "predecessor" of the startVertex
    fires.push_back(PIPIL(startVertex, PIL(-1, 0)));

    // while there is an active fire
    while (!fires.empty()) {
        // find the fire that reaches its endpoint "v" earliest,
        // represented as an iterator into the list
        auto earliestFire = fires.begin();
        for (auto iter = fires.begin(); iter != fires.end(); ++iter) {
            if (iter->second.second < earliestFire->second.second) {
                earliestFire = iter;
            }
        }

        // to reduce notation in the code below, this u,v,d agrees with
        // the intuition presented in the comment when PIPIL is typedef'ed
        int v = earliestFire->first, u = earliestFire->second.first, d = earliestFire->second.second;

        // remove this fire
        fires.erase(earliestFire);

        // if u is already "burned", there nothing to do
        if (searchTree.find(v) != searchTree.end()) {
            continue;
        }

        // declare that v is "burned" at time d with a fire that spawned from u
        searchTree[v] = PIL(u, d);

        // now start fires from all edges exiting vertex v
        for (auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {
            int nbr = *iter;

            // the fire starts at v at time d and will reach nbr
            // at time d + (length of v->nbr edge)
            int burn = d + graph.getCost(v, nbr);
            fires.push_back(PIPIL(nbr, PIL(v, burn)));
        }
    }
}

int main() {
    WDigraph graph;

    int startVertex;
    int endVertex;


    unordered_map<int, Point> points;
    readGraph("edmonton-roads-2.0.1.txt",graph,points);

    startVertex = 277483195;
    endVertex = 561041122;


    // run the search
    // searchTree[v] will be the pair (u,d) of the node u prior to v
    // in a shortest path to v and d will be the shortest path distance to v
    unordered_map<int, PIL> searchTree;
    dijkstra(graph, startVertex, searchTree);

    // read off a path
    list<int> path;
    if (searchTree.find(endVertex) == searchTree.end()) {
      cout << "Vertex " << endVertex << " not reachable from " << startVertex << endl;
    }
    else {
      int stepping = endVertex;
      while (stepping != startVertex) {
        path.push_front(stepping);

        // crawl up the search tree one step
        stepping = searchTree[stepping].first;
      }
      path.push_front(startVertex);

      cout << "Cost of cheapest path from " << startVertex << " to "
           << endVertex << " is: " << searchTree[endVertex].second << endl;
      cout << "Path:";
      for (auto it : path) {
        cout << ' ' << it;
      }
      cout << endl;
          cout << "SIZE:  " << path.size() <<endl;
    }

    return 0;
}