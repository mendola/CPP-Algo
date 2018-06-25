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

knapsack randomSolution(knapsack &k, double fillRate){
    int numObjects = k.getNumObjects();

    knapsack newKnapsack = knapsack(k);

    // Generate random solution with % objects approximately equal to fillRate
    for(int i = 0; i < numObjects; i++){
        if( (double) rand() / (double) RAND_MAX <= fillRate){
            newKnapsack.select(i);
        }else{
            newKnapsack.unSelect(i);
        }
    }
    cout<<"Value for new knapsack = "<<newKnapsack.getValue()<<".  Cost for new knapsack = "<<newKnapsack.getCost()<<endl;
    
    return newKnapsack;
}

knapsack mutateKnapsack(knapsack &k, int numMutations){
    knapsack newKnapsack = knapsack(k);
    int numObjects = k.getNumObjects();
    for(int i = 0; i < numMutations; i++){
        int objIdx = rand() % numObjects;
        if(newKnapsack.isSelected(objIdx)){
            newKnapsack.unSelect(objIdx);
        }else{
            newKnapsack.select(objIdx);
        }
    }
    return newKnapsack;
}

knapsack crossoverKnapsack(knapsack &k1, knapsack &k2){
    int numObjects = k1.getNumObjects();
    int xOverIdx = rand() % numObjects;
    knapsack newKnapsack = knapsack(k1);

    for(int i = 0; i < numObjects; i++){
        if(i < xOverIdx){
            if(k1.isSelected(i)){
                newKnapsack.select(i);
            }else{
                newKnapsack.unSelect(i);
            }
        }else{
            if(k2.isSelected(i)){
                newKnapsack.select(i);
            }else{
                newKnapsack.unSelect(i);
            }
        }
    }

    return newKnapsack;
}

bool operator>(const knapsack& lhs, const knapsack& rhs){
    int maxCost = lhs.getCostLimit();

    double fitnessLH = (double)lhs.getValue();
    if(lhs.getCost() > maxCost){
        fitnessLH -= (double)(lhs.getCost() - maxCost) / (double)maxCost * (double)fitnessLH;
    }

    double fitnessRH = (double)rhs.getValue();
    if(rhs.getCost() > maxCost){
        fitnessRH -= (double)(rhs.getCost() - maxCost) / (double)maxCost * (double)fitnessRH;
    }

    return (fitnessLH < fitnessRH);
}

double getFitness(knapsack &k){
    int maxCost = k.getCostLimit();
    double fitness = (double)k.getValue();
    if(k.getCost() > maxCost){
        fitness -= (double)(k.getCost() - maxCost) / (double)maxCost * (double)fitness;
    }
    return fitness;
}


void geneticKnapsack(knapsack &k, int t, int initPopulation, double initFillRate, double mutateRate, double breedRate, int numMutations){
    srand(time(NULL));
    clock_t startTime = clock();
    int maxCost = k.getCostLimit();
// Generate initial population of random solutions
    // Initialize population matrix
    vector< knapsack > population(initPopulation,k);
    priority_queue<knapsack, vector<knapsack>, greater<knapsack> > populationSorted;
    // Fill matrix with random solutions
    double bestFitness = 0.0;

    for(int i = 0; i < initPopulation; i++){
        population[i] = randomSolution(k, initFillRate);
        populationSorted.push(population[i]);
        double fitness = getFitness(population[i]);
        if(fitness > bestFitness){
            bestFitness = fitness;
        }
    }

// Simulate evolution until time limit expires
    int iter = 0;
    while((float)(clock()-startTime) / CLOCKS_PER_SEC <= (float)t ){
        iter++;
        int populationSize = population.size();
        int numMutations = (int) (mutateRate * (double) populationSize);
        int numBreeds = (int) (breedRate* (double) populationSize);

        vector<int> mutateList(numMutations);
        vector< pair<int,int> > breedList(numBreeds);

        // Perform mutations
        for(int i = 0; i < numMutations; i++){
            int parentIdx = rand() % populationSize;
            knapsack newKnapsack = mutateKnapsack(population[parentIdx],numMutations);
            population.push_back(newKnapsack);
            populationSorted.push(newKnapsack);
            double fitness = getFitness(newKnapsack);
            if(fitness > bestFitness){
                bestFitness = fitness;
            }
        }

        // Perorm crossover breedings
        for(int i = 0; i < numBreeds; i++){
            int parentIdxA = rand() % populationSize;
            int parentIdxB = parentIdxA;
            while(parentIdxB == parentIdxA){
                parentIdxB = rand() % populationSize;    
            }
            
            // Breed probabilistically based on fitnesses
            if(getFitness(population[parentIdxA]) + getFitness(population[parentIdxB]) < 1.25*bestFitness){
                if( (double) rand() / (double) RAND_MAX < breedRate){
                    knapsack newKnapsack = crossoverKnapsack(population[parentIdxA], population[parentIdxB]);
                    population.push_back(newKnapsack);
                    populationSorted.push(newKnapsack);
                    double fitness = getFitness(newKnapsack);
                    if(fitness > bestFitness){
                        bestFitness = fitness;
                    }
                }
            }else{
                knapsack newKnapsack = crossoverKnapsack(population[parentIdxA], population[parentIdxB]);
                population.push_back(newKnapsack);
                populationSorted.push(newKnapsack);
                double fitness = getFitness(newKnapsack);
                if(fitness > bestFitness){
                    bestFitness = fitness;
                }
            }
        }
        
        // Choose survivors from parents and new children
        vector<knapsack> newPopulation;
        int popSize = population.size();
        for(int i = 0; i < popSize; i++){
            //knapsack currKnapsack = population[i];
            knapsack currKnapsack = populationSorted.top();
            populationSorted.pop();

            if( (double)rand() / (double) RAND_MAX > 0.7 *(double)i/(double)popSize){
                if(newPopulation.size() < 5000){
                    newPopulation.push_back(currKnapsack);
                }
            }
        }

        for(int i = 0; i<newPopulation.size(); i++){
            knapsack currKnapsack = newPopulation[i];
            populationSorted.push(currKnapsack);
        }
/*
            double fitness = getFitness(currKnapsack);
            if(fitness > 0.75*bestFitness){
                newPopulation.push_back(currKnapsack);
            }else{
                if( (double) rand() / (double)RAND_MAX * bestFitness < fitness){
                    newPopulation.push_back(currKnapsack);                 
                }
            }
        }
*/
        population = newPopulation;
        cout<<"Iteration #"<<iter<<endl;
        cout<<"Best fitness is: "<<bestFitness<<endl;
        cout<<"Population is: "<<population.size()<<endl;
        cout<<"Sorted Population is:"<<populationSorted.size()<<endl;
    }
    
    int bestValue = 0;
    int bestIdx;
    for(int i = 0; i< population.size(); i++){
        knapsack currKnapsack = population[i];
        if(currKnapsack.getCost() < maxCost){
            if(currKnapsack.getValue() > bestValue){
                bestIdx = i;
                bestValue = currKnapsack.getValue();
            }
        }
    }
    k = population[bestIdx];
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
        //k.sortItemsByCVRatio(); // Sort the items by cost/value ratio
        // k.sortItemsByValue(); // (Possible Alternative, seems more 'greedy') Sort the idems by value
        //greedyKnapsack(k);
        geneticKnapsack(k, 300, 5000, 0.2, 0.2,0.4,2);
        k.printSolution();
      
    }    
    catch (std::exception &ex) { 
        cout << ex.what() << endl;
        exit(1);
    }
}
