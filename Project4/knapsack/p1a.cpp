// Project 1a: Solving knapsack using exhaustive search
//

#include <stdlib.h>
#include <iostream>
#include <limits.h>
#include <list>
#include <fstream>
#include <queue>
#include <vector>
#include <stack>
#include <time.h>

using namespace std;

#include "knapsack.h"

bool operator>(const knapsack& lhs, const knapsack& rhs){
    return lhs.bound < rhs.bound;
}

void printPartialKnapsack(knapsack &k){
    for(int i = 0; i< k.depth; i++){
        cout<<k.isSelected(i)<<" ";
    }
    cout<<endl;
}

void branchAndBound(knapsack &k, int t){
    clock_t startTime = clock();

    int bestKnapsackValue = -1;
    knapsack bestKnapsack = knapsack(k);

    int costLimit = k.getCostLimit();
    int numObjects = k.getNumObjects();

    priority_queue<knapsack, vector<knapsack>, greater<knapsack> > knapsackQueue;

    k.setBound();
    knapsackQueue.push(k);

    while(!knapsackQueue.empty() && (float)(clock()-startTime) / CLOCKS_PER_SEC <= (float)t ){
        // Get the next instance to process
        knapsack currKnapsack = knapsackQueue.top();
        knapsackQueue.pop();

        //cout<<"Processing. Bound = "<<currKnapsack.bound<< "  Value = " << currKnapsack.getValue()<<"  ";
        //printPartialKnapsack(currKnapsack);
        // Do not continue to branch on this knapsack if its optimistic bound is worse
        // than the current
        if(currKnapsack.bound <= bestKnapsackValue){
            continue;
        }

        int depth = currKnapsack.getDepth();  //Index where undecided objects begin

        // Create knapsacks for branching
        knapsack k0 = knapsack(currKnapsack);
        knapsack k1 = knapsack(currKnapsack);
    
    // Process branch with item[depth] = unselected
        // Don't need to test 
        int b0;
        k0.depth = depth + 1;
        b0 = k0.setBound();
        // Only add to queue if there are still undecided objects
        // We didn't add any object, so if its predecessor fit, it will also fit.
        if(k0.depth < numObjects){
            knapsackQueue.push(k0);
        }

    // Process branch with item[depth] = selected
        int b1; // bound
        int v1; // value
        int c1; // cost
        k1.select(depth);
        k1.depth = depth + 1;
        b1 = k1.setBound();
        v1 = k1.getValue();
        c1 = k1.getCost();
        // Only add to queue if there are still undecided objects 
        // AND the cost limit isn't yet exceeded
        // AND the optimistic bound is better than the current best solution
        if(b1 > bestKnapsackValue && c1 <= costLimit && k1.depth <= numObjects){
            if(k1.depth < numObjects){
                knapsackQueue.push(k1);
            }
            if(v1 > bestKnapsackValue){
                bestKnapsackValue = v1;
                bestKnapsack = knapsack(k1);
            }
        }
    }
    k = knapsack(bestKnapsack);
}

int main(int argc, char* argv[])
{
    ifstream fin;
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " instance" << endl; 
        exit(1); 
    }
    fin.open(argv[1]);
    if (!fin)
    {
        cerr << "Cannot open " << argv[1] << endl;
        exit(1);
    }

    try
    {
        knapsack k(fin);
        k.sortItemsByCVRatio(); // Sort the items by cost/value ratio
        // k.sortItemsByValue(); // (Possible Alternative, seems more 'greedy') Sort the idems by value
        branchAndBound(k,600);
        cout<< "Cost Limit: "<<k.getCostLimit()<<endl;
        k.printSolution();
      
    }    
    catch (std::exception &ex) { 
        cout << ex.what() << endl;
        exit(1);
    }
}
