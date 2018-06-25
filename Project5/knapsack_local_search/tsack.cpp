#include <stdlib.h>
#include <iostream>
#include <limits.h>
#include <list>
#include <fstream>
#include <memory>
#include <optional>
#include <queue>
#include <vector>
#include <stack>
#include <time.h>

#include "tsack.h"

// Explore the exclusion of each item and inclusion of other items in the space it frees via
// greedy knapsack

class NeighborhoodNode {
	typedef std::shared_ptr<knapsack> kptr_t;
	kptr_t kptr;
	int num;

	// Memoized!
	mutable std::optional<int> next_num = {};
	void calcNextNum(void) const {
		for (int i = this->num + 1; i < this->kptr->getNumObjects(); i++) {
			if (this->kptr->isSelected(i)) {
				this->next_num = i;
				return;
			}
		}
		this->next_num = -1;
	}

	// Memoized!
	// Assumes K is prior sorted by CV
	mutable std::optional<std::pair<std::vector<bool>, int>> descent = {};
	void calcDescent(void) const {
		int total_cost=0, value=0;
		std::vector<bool> selected(this->kptr->getNumObjects());

		// Load in all selections besides the current
		for (int i = 0; i < this->kptr->getNumObjects(); i++) {
			if (this->kptr->isSelected(i) && i != this->num) {
				selected[i] = true;
				total_cost += this->kptr->getCost(i);
				value      += this->kptr->getValue(i);
			}
		}
  
		for (int i = 0; i < this->kptr->getNumObjects(); i++) {
			// Don't reinclude what we just excluded
			if ( ! selected[i] && i != this->num &&
					this->kptr->getCost(i) + total_cost < this->kptr->getCostLimit()) {
				selected[i] = true;
				total_cost += this->kptr->getCost(i);
				value      += this->kptr->getValue(i);
			}
		}

		this->descent = std::make_pair(selected, value);
	}
public:
	// Memoized!
	int getNextNum(void) const {
		if (! this->next_num.has_value()) {
			this->calcNextNum();
		}
		return this->next_num.value();
	}

	// Memoized!
	std::vector<bool> getSelected(void) const {
		if (! this->descent.has_value()) {
			this->calcDescent();
		}
		return this->descent.value().first;
	}
	
	// Memoized!
	int getValue(void) const {
		if (! this->descent.has_value()) {
			this->calcDescent();
		}
		return this->descent.value().second;
	}

	NeighborhoodNode(const kptr_t &kptr, int num) : kptr(kptr), num(num) {};
	NeighborhoodNode(knapsack k) : NeighborhoodNode(std::make_shared<knapsack>(k), 0) {};

	bool done(void) const {
		return this->getNextNum() < 0;
	}

	NeighborhoodNode next(void) const {
		return NeighborhoodNode(this->kptr, this->getNextNum());
	}
};


void greedyKnapsack(knapsack &k) {
	int totalCost = 0;

	for(int i = 0; i < k.getNumObjects(); i++) {
		if(k.getCost(i) + totalCost <= k.getCostLimit()) {
			k.select(i);
			totalCost += k.getCost(i);
		}
	}
}


NeighborhoodNode neighborhoodSearch(const knapsack &k) {
	NeighborhoodNode n(k), cur = n;
	while ( ! cur.done()) {
		cur = cur.next();
		if (n.getValue() < cur.getValue()) {
			n = cur;
		}
	}
	return n;
}

void steepestDescent(knapsack &k) {
	auto n = neighborhoodSearch(k);

	if (n.getValue() < k.getValue()) {
		std::cout << "Neighborhood search generated value " << n.getValue() << " which is less than " << k.getValue() << std::endl;
		return;
	}

	do {
		std::cout << "Increasing Value from " << k.getValue() << " to " << n.getValue() << std::endl;
		for(int i = 0; i < k.getNumObjects(); i++) {
			if (n.getSelected()[i]) {
				k.select(i);
			}
			else {
				k.unSelect(i);
			}
		}
		n = neighborhoodSearch(k);
	} while (n.getValue() > k.getValue());
		std::cout << "Neighborhood search generated value " << n.getValue() << " which is less than " << k.getValue() << std::endl;
}

int main(int argc, char* argv[])
{
    std::ifstream fin;
    if (argc != 2) {
	std::cout << "Usage: " << argv[0] << " instance" << std::endl; 
        exit(1); 
    }
    fin.open(argv[1]);
    if (!fin)
    {
	std::cerr << "Cannot open " << argv[1] << std::endl;
        exit(1);
    }

    try
    {
        knapsack k(fin);
        k.sortItemsByCVRatio(); // Sort the items by cost/value ratio
	greedyKnapsack(k);
	steepestDescent(k);	
        k.printSolution();
      
    }    
    catch (std::exception &ex) { 
	std::cout << ex.what() << std::endl;
        exit(1);
    }
	}
