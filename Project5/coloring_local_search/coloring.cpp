#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <tuple>
#include <vector>

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

int bestNeighbor1NodeColorChange(Graph &g, int numColors){
    // Iterate over all vertices
    int improvementFound = 0;
    auto bestNode = -1;
    int bestNewColor = -1;
    int bestImprovement = 0;

    for(auto v : vertices_range(g)){
  //      std::cout<<"Testing vertex "<<v<<std::endl;
        int currColor = g[v].color;
        std::vector<int> neighborColorHist(numColors,0);

        Graph::adjacency_iterator start, end;
        std::tie(start, end) = adjacent_vertices(v, g);

        
        for(Graph::adjacency_iterator v2 = start; v2 != end; v2++){
            neighborColorHist[g[*v2].color] += 1;
        }
        
  //      std::cout<<"CurrNode Color: "<<currColor<<"   neighborHist:"<<std::endl;
        for(int i = 0; i < numColors; i++){
  //          std::cout<<neighborColorHist[i]<<" ";
        }
  //      std::cout<<std::endl;
        int currConflicts = neighborColorHist[currColor];
        for(int i = 0; i<numColors; i++){
            if(currConflicts - neighborColorHist[i] > bestImprovement){
                bestNode = v;
                bestNewColor = i;
                bestImprovement = currConflicts - neighborColorHist[i];
                improvementFound = 1;
            }
        }
    }

    if(improvementFound){
  //      std::cout<<"IMPROVEMENT FOUND"<<std::endl;
        g[bestNode].color = bestNewColor;
    }

    return improvementFound;
}

int steepestDescent(Graph &g, int numColors, int t){
    clock_t startTime = clock();
    int done = 0;
    int iter = 0;
    while(done == 0 && ((double)(clock()-startTime) / (double)CLOCKS_PER_SEC) <= (float)t ){
        iter++;
        if(bestNeighbor1NodeColorChange(g,numColors) == 0){
            done = 1;
        }
    }
    float p1 = (float)(clock()-startTime);
    float p2 = (float)CLOCKS_PER_SEC;
    float tmp = (float)(clock()-startTime) / (float)CLOCKS_PER_SEC;
//    std::cout<<"Time: \n"<<p1<<"\n"<<p2<<"\n"<<tmp <<std::endl;
    std::cout<<"Steepest Descent completed in "<<iter<<" iterations"<<std::endl;
    return done;
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
        
        std::cout<<"\n\nInitial solution: "<<std::endl;
        printSolution(std::cout, g);
       
        int foundOptimal = -1;
        foundOptimal = steepestDescent(g, numColors,300);
        if(foundOptimal == 1){
            std::cout<<"Found an optimal solution"<<std::endl;
        }else if(foundOptimal == 0){
            std::cout<<"Steepest Descent timed out. Local Optimum not found." <<std::endl;
        }
        std::cout<<"\nLocal Search Solution: "<<std::endl;
        printSolution(std::cout, g);
        
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
	exit(1);
    }
}
