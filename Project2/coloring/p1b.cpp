#include <algorithm>
#include <iostream>
#include <fstream>
#include <queue>
#include <tuple>
#include <limits.h>
#include <time.h>
#include <boost/graph/adjacency_list.hpp>


using namespace std;
using namespace boost;


struct VertexProperties { int color; };
typedef adjacency_list<vecS, vecS, bidirectionalS, VertexProperties> Graph;


template<typename G, typename F>
void for_vertices(G &g, F f) { 
    typename G::vertex_iterator start, end;
    tie(start, end) = vertices(g);

    for_each(start, end, f);
}


template<typename G, typename F>
void for_edges(G &g, F f) { 
    typename G::edge_iterator start, end;
    tie(start, end) = edges(g);

    for_each(start, end, f);
}


void initializeGraph(Graph &g, ifstream &fin) {
    int nVertices, nEdges;
    fin >> nVertices >> nEdges;

    for (int i = 0; i < nVertices; i++) {
        g[add_vertex(g)].color = -1;
    }
    
    for (int i = 0; i < nEdges; i++) {
        Graph::vertex_descriptor from, to;
        fin >> from >> to;
        add_edge(from, to, g);
    }
}



void setNodeWeights(Graph &g, int w) {
    for_vertices(g, [&g, w](auto v) { g[v].color = w; });
}


int countConflicts(Graph &g) {
    int counter = 0;
    for_edges(g, [&g, &counter](auto e) {
        if (g[source(e, g)].color == g[target(e, g)].color)
	    counter++;
    });
    return counter;
}


void printSolution(Graph &g){
    cout << "Number of Conflicts: " << countConflicts(g) << endl;
    
    for_vertices(g, [&g](auto v) {
        cout<<"Node "<< v << " color: "<<g[v].color << endl;
    });
}


void randWeightInitialization(Graph &g, int numColors){
    for_vertices(g, [numColors, &g](auto v) {
        g[v].color = rand() % numColors;
    });
}


void greedyColoring(Graph &g, int numColors) {
    typedef Graph::vertex_descriptor VD;
    auto sort = [&g](VD a, VD b) { return degree(a, g) < degree(b, g); };
    priority_queue<VD, vector<VD>, decltype(sort)> p(sort);    
    
    for_vertices(g, [&p](auto v) {
        p.push(v);
    });
    while (!p.empty()) {
        auto v = p.top(); p.pop();
	
    }
}


int main(int argc, char* argv[])
{
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " instance" << std::endl;
        exit(1);
    }

    ifstream fin(argv[1]);
    if (!fin)
    {
        cerr << "Cannot open " << argv[1] << endl;
        exit(1);
    }
    
    srand(time(NULL));
    
    try {
       	int numColors;
        fin >> numColors;
        
	Graph g;
        initializeGraph(g, fin);

        greedyColoring(g, numColors);
        printSolution(g);
        
    }
    catch (std::exception &ex)
    {
        cout << ex.what() << endl; exit(1);
    }
}
