// Code to read graph instances from a file.  Uses the Boost Graph Library (BGL).

#include <iostream>
#include <limits.h>
#include "d_except.h"
#include <fstream>
#include <time.h>
#include <boost/graph/adjacency_list.hpp>

#define LargeValue 99999999

using namespace std;
using namespace boost;

int const NONE = -1;  // Used to represent a node that does not exist

struct VertexProperties;
struct EdgeProperties;

typedef adjacency_list<vecS, vecS, bidirectionalS, VertexProperties, EdgeProperties> Graph;

struct VertexProperties
{
    pair<int,int> cell; // maze cell (x,y) value
    Graph::vertex_descriptor pred;
    bool visited;
    bool marked;
    int weight; // USING AS COLOR
};

// Create a struct to hold properties for each edge
struct EdgeProperties
{
    int weight;
    bool visited;
    bool marked;
};

void initializeGraph(Graph &g, ifstream &fin)
// Initialize g using data from fin.
{
    int n, e;
    int j,k;
    
    fin >> n >> e;
    Graph::vertex_descriptor v;
    
    // Add nodes.
    for (int i = 0; i < n; i++)
        v = add_vertex(g);
    
    for (int i = 0; i < e; i++)
    {
        fin >> j >> k;
        add_edge(j,k,g);  // Assumes vertex list is type vecS
    }
}

void setNodeWeights(Graph &g, int w)
// Set all node weights to w.
{
    pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
    
    for (Graph::vertex_iterator vItr= vItrRange.first; vItr != vItrRange.second; ++vItr)
    {
        g[*vItr].weight = w;
    }
}

void printSolution(Graph &g, int numConflicts){
    cout<<"Number of Conflicts: "<<numConflicts<<endl;
    
    pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
    
    int i = 0;
    for (Graph::vertex_iterator vItr= vItrRange.first; vItr != vItrRange.second; ++vItr)
    {
        cout<<"Node "<< i<< " color: "<<g[*vItr].weight<<endl;
        i++;
    }
}

// Using a base-[numColors] counter to represent coloring, increment counter and adjust coloring. This was our first counter method
int changeColoringCounterMethod(Graph &g, int numColors, vector<int> &initalColors){
    pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
    Graph::vertex_iterator vItr= vItrRange.first;
    unsigned int carry = 1;
    int ret = 1;
    while (vItr != vItrRange.second && carry != 0) {
        int currWeight = g[*vItr].weight;
        if(currWeight == numColors - 1){
            g[*vItr].weight = 0;
        }else{
            g[*vItr].weight++;
            carry = 0;
        }
        vItr++;
    }

    // Return 1 if the last coloring has been tested
    int i = 0;
    for (Graph::vertex_iterator vItr= vItrRange.first; vItr != vItrRange.second; ++vItr){
        if(g[*vItr].weight != initalColors[i]){
            ret = 0;
            break;
        }
        i++;
    }
    return ret;
}

// Count the number of coloring conflicts in the given graph
int countConflicts(Graph &g){
    pair<Graph::edge_iterator, Graph::edge_iterator> eItrRange = edges(g);
    
    int conflictCount = 0;
    for(Graph::edge_iterator eItr= eItrRange.first; eItr != eItrRange.second; ++eItr){
        Graph::edge_descriptor currEdge = *eItr;
        Graph::vertex_descriptor sourceVertex = source(currEdge,g);
        Graph::vertex_descriptor targetVertex = target(currEdge,g);

        // Increment conflict counter if needed
        if(g[sourceVertex].weight == g[targetVertex].weight){
            conflictCount++;
        }
    }
    return conflictCount;
}

// Randomly assign colors to all nodes in the graph.
vector<int>  randWeightInitialization(Graph &g, int numColors){
    srand(time(NULL));
    vector<int> ret(num_vertices(g));
    pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
    int i = 0;
    for (Graph::vertex_iterator vItr= vItrRange.first; vItr != vItrRange.second; ++vItr) {
        int randInt = rand() % numColors;
        g[*vItr].weight = randInt;
        ret[i] = randInt;
        i++;
    }
    return ret;
}

// Tests all possible colorings  given graph g and given number of colors. Algorithm terminates after t seconds if all combinations haven't been tested.
int exhaustiveColoring(Graph &g, int numColors, int t){
    // Get start time
    clock_t startTime = clock();

    int numVertices = num_vertices(g);
    int bestNumConflicts = numVertices * numVertices; // Initialize such that even a fully-connected graph with all 1 color will be better
    vector<int> bestColoring(numVertices,0); // Will store the best coloring as it's updated

    // Initialize all weights (colors) to 0;
    //setNodeWeights(g, 0);

    // Randomly initialize weights (colors)
    vector<int> initialColoring = randWeightInitialization(g, numColors);


    // Search Loop. Break if timeout occurs or all colorings have been tried
    int iter = 0;
    while(1){
        iter++;
        // Break if time is out
        clock_t now = clock();
        if( (float)(now-startTime) / CLOCKS_PER_SEC > (float)t ){
            cout<<"Timed out. time = "<<now-startTime<<endl;
            break;
        }
        // Update Coloring 
        if(changeColoringCounterMethod(g,numColors, initialColoring) == 1){
            cout<<"Tested all combinations"<<endl;
            break;
        }

        // Check Number of Conflicts
        int numConflicts = countConflicts(g);
        if(numConflicts < bestNumConflicts){
            pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
            bestNumConflicts = numConflicts;
            int i = 0;
            // Update saved best coloring
            for (Graph::vertex_iterator vItr= vItrRange.first; vItr != vItrRange.second; ++vItr) {
                bestColoring[i] = g[*vItr].weight;
                i++;
            }

        }
        if(numConflicts == 0){
            cout<<"Search was stopped. 0-conflict solution found."<<endl;
            break;
        }
    }
    // Color with Best Coloring
    int i = 0;
    pair<Graph::vertex_iterator, Graph::vertex_iterator> vItrRange = vertices(g);
    for (Graph::vertex_iterator vItr= vItrRange.first; vItr != vItrRange.second; ++vItr) {
        g[*vItr].weight = bestColoring[i];
        i++;
    }
    cout<<"Best number of conflicts found: "<<bestNumConflicts<<endl;
    return bestNumConflicts;
}

int main()
{
    char x;
    ifstream fin;
    string fileName;
    
    // Read the name of the graph from the keyboard or
    // hard code it here for testing.
    
    //fileName = "data/color12-3.input";
    
       cout << "Enter filename" << endl;
       cin >> fileName;
    
    fin.open(fileName.c_str());
    if (!fin)
    {
        cerr << "Cannot open " << fileName << endl;
        exit(1);
    }
    
    try
    {
        cout << "Reading graph" << endl;
        Graph g;
        int numColors;
        int numConflicts = -1;
        fin >> numColors;
        cout<<numColors<<" Colors."<<endl;
        initializeGraph(g,fin);
        
        cout << "Num nodes: " << num_vertices(g) << endl;
        cout << "Num edges: " << num_edges(g) << endl;
        cout << endl;
        
        // cout << g;
        numConflicts = exhaustiveColoring(g, numColors, 600);
        printSolution(g, numConflicts);
        
    }
    catch (indexRangeError &ex)
    {
        cout << ex.what() << endl; exit(1);
    }
    catch (rangeError &ex)
    {
        cout << ex.what() << endl; exit(1);
    }
}