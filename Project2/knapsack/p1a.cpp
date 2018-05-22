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


// Treating the list of objects included/not-included as a binary number, increment the binary representation
int incrementKnapsack(knapsack &k, unsigned int numObjects){
  int idx = numObjects - 1;
  unsigned int carry = 1;
  int ret = 0;
  while(idx >= 0 && carry == 1){
    if(k.isSelected(idx)){
      k.unSelect(idx);
    } else{
      k.select(idx);
      carry = 0;
    }
    idx--;
  }
  // Return 1 if the last combination has been tested
  if(idx == -1 && carry == 1){
    ret = 1;
  }
  return ret;
}

// Exhaustive Search for maximum value knapsack
void exhaustiveKnapsack(knapsack &k, int t){
  clock_t startTime = clock();
  unsigned int numObjects = k.getNumObjects();
  int costLimit = k.getCostLimit();
  cout<<"Number of objects: "<<numObjects<<"\nCost Limit: "<<costLimit<<endl;
  int bestValue = 0;
  vector<unsigned int> bestSolution(numObjects,0);

  unsigned int iter = 0;
  // Create vector of object indices to be included
  // Uses binary representation 
  bool run = true;
  while(run){
    clock_t now = clock();
    if( (float)(now-startTime) / CLOCKS_PER_SEC > (float)t ){
      cout<<"Timed out. time = "<<now-startTime<<endl;
      break;
    }
    iter++;
    // Get next set of objects to be included
    if(incrementKnapsack(k, numObjects) == 1){
      cout<<"Tested all combinations."<<endl;
      run = false;
    }

    // Move on if cost is too high
    if(k.getCost() > costLimit){
      continue;
    }

    // Check value, save it if it's the new best
    if(k.getValue() > bestValue){
      for(int i = numObjects-1; i>=0; i--){
        if(k.isSelected(i)){
          bestSolution[i] = 1;
        }else{
          bestSolution[i] = 0;
        }
      }
      bestValue = k.getValue();
    }
  }

  // Clear knapsack and refill with best solution
  for(int i = numObjects-1; i>=0; i--){
    if(bestSolution[i] == 1){
      k.select(i);
    }else{
      k.unSelect(i);
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
        exhaustiveKnapsack(k, 600);

        k.printSolution();
      
    }    
    catch (std::exception &ex) { 
        cout << ex.what() << endl;
        exit(1);
    }
}
