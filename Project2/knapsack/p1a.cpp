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


void greedyKnapsack(knapsack &k){
  int totalCost = 0;
  int i = 0;
  int costLimit = k.getCostLimit();
  cout<<"Cost Limit: "<< costLimit << endl;
  int numObjects = k.getNumObjects();
  
  // Loop until bag is full or time runs out
  while( totalCost <= costLimit && i < numObjects){
    // If next item in list is cheap enough, add it
    if(k.getCost(i) + totalCost <= costLimit){
      k.select(i);
      totalCost += k.getCost(i);
    }

    // Move on to next item in sorted list
    i++;
  }
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
        greedyKnapsack(k);

        k.printSolution();
      
    }    
    catch (std::exception &ex) { 
        cout << ex.what() << endl;
        exit(1);
    }
}
