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

#include "d_except.h"
#include "d_matrix.h"
#include "knapsack.h"


// I
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

int main()
{
   char x;
   ifstream fin;
   stack <int> moves;
   string fileName;
   
   // Read the name of the file from the keyboard or
   // hard code it here for testing.
   
  //fileName = "data/knapsack16.input";

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
      cout << "Reading knapsack instance" << endl;
      knapsack k(fin);

      exhaustiveKnapsack(k, 600);

      cout << endl << "Best solution" << endl;
      k.printSolution();
      
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

