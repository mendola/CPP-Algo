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


void greedyKnapsack(knapsack &k, int t){
  clock_t startTime = clock();
  int totalCost = 0;
  int i = 0;
  bool run = true;
  int costLimit = k.getCostLimit();
  int numObjects = k.getNumObjects();
  
  // Loop until bag is full or time runs out
  while(run){
    // If next item in list is cheap enough, add it
    if(k.getCost(i) + totalCost <= costLimit){
      k.select(i);
      totalCost += k.getCost(i);
    }

    // Move on to next item in sorted list
    i++;

    // Stop if cost limit is reached
    if(totalCost >= costLimit){
      run = false;
    }

    // Stop if all items have been exhausted
    if(i >= numObjects){
      run = false;
    }

    // Stop if time is out
    clock_t now = clock();
    if( (float)(now-startTime) / CLOCKS_PER_SEC > (float)t ){
      cout<<"Timed out. time = "<<now-startTime<<endl;
      run = false;
    }
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
        greedyKnapsack(k, 600);

        k.printSolution();
      
    }    
    catch (std::exception &ex) { 
        cout << ex.what() << endl;
        exit(1);
    }
}
