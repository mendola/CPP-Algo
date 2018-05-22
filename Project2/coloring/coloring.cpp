#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <tuple>

#include <boost/range.hpp>
#include <boost/graph/adjacency_list.hpp>


struct VertexProperties { int color; };
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexProperties> Graph;


template<typename G>
auto vertices_range(G &g) { 
    return boost::make_iterator_range(boost::vertices(g));
}


template<typename G>
auto edges_range(G &g) { 
    return boost::make_iterator_range(boost::edges(g));
}


void initializeGraph(Graph &g, std::ifstream &fin) {
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
    for(auto v : vertices_range(g)) {
        g[v].color = w;	    
    }
}


int countConflicts(Graph &g) {
    int counter = 0;
    for (auto e : edges_range(g)) {
        if (g[source(e, g)].color == g[target(e, g)].color)
	    counter++;
    }
    return counter;
}


void printSolution(std::ostream& os, Graph &g){
    os << "Number of Conflicts: " << countConflicts(g) << std::endl;
    
    for (auto v: vertices_range(g)) {
        os << "Node " << v << " color: "<<g[v].color << std::endl;
    }
}


int availColor(Graph &g, Graph::vertex_descriptor v) {
    std::vector<bool> unavailable;

    Graph::adjacency_iterator start, end;
    std::tie(start, end) = adjacent_vertices(v, g);
    std::for_each(start, end, [&g, &unavailable](auto u) {		
        if (g[u].color >= 0) {
	    if (g[u].color >= unavailable.size()) {
                unavailable.resize(g[u].color + 1);
	    }
            unavailable[g[u].color] = true;
        }
    });
       
    auto avail = std::find(unavailable.begin(), unavailable.end(), false);
    return std::distance(unavailable.begin(), avail);
}	


void greedyColoring(Graph &g, int numColors) {
    typedef Graph::vertex_descriptor VD;
    auto sort = [&g](VD a, VD b) { return degree(a, g) < degree(b, g); };
    std::priority_queue<VD, std::vector<VD>, decltype(sort)> p(sort);    
    
    for (auto v : vertices_range(g)) {
        p.push(v);
    }
    
    while (!p.empty()) {
        auto v = p.top(); p.pop();
	auto color = availColor(g, v);
	g[v].color = color < numColors ? color : 0;
    }
}


int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " instance" << std::endl;
        exit(1);
    }

    std::ifstream fin(argv[1]);
    if (!fin)
    {
        std::cerr << "Cannot open " << argv[1] << std::endl;
        exit(1);
    }
    
    srand(time(NULL));
    
    try {
       	int numColors;
        fin >> numColors;
        
	Graph g;
        initializeGraph(g, fin);

        greedyColoring(g, numColors);
        printSolution(std::cout, g);
        
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
	exit(1);
    }
}
