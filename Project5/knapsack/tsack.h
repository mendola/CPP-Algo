#include "../utils/d_except.h"
#include <algorithm>
#include <stdexcept>

class knapsack
{
private:
	int numObjects;
	int costLimit;
	std::vector<int> value;
	std::vector<int> cost;
	std::vector<bool> selected;
	int totalValue;
	int totalCost;
   
public:
	knapsack(std::ifstream &fin);
	knapsack(const knapsack &);
	int getCost(int) const;
	int getValue(int) const;
	int getCost() const { return totalCost; }
	int getValue() const { return totalValue; }
	int getNumObjects() const { return numObjects; }
	int getCostLimit() const { return costLimit; }
	void printSolution();
	void select(int);
	void unSelect(int);
	bool isSelected(int) const;
	void sortItemsByCVRatio();
};

// Construct a new knapsack instance using the data in fin.
knapsack::knapsack(std::ifstream &fin)
{
	fin >> this->numObjects >> this->costLimit;
	this->value.resize(this->numObjects);
	this->cost.resize(this->numObjects);
	this->selected.resize(this->numObjects);
   
	for (int i = 0; i < this->numObjects; i++)
	{
		int index;
		fin >> index;
		fin >> this->value[index] >> this->cost[index];
		unSelect(index);
	}

	totalValue = 0;
	totalCost = 0;
}


// Knapsack copy constructor.
knapsack::knapsack(const knapsack &k)
	: numObjects(k.numObjects), value(k.value), cost(k.cost),
	  selected(k.selected), costLimit(k.costLimit),
	  totalCost(k.totalCost), totalValue(k.totalValue)
{ }


int knapsack::getValue(int i) const
// Return the value of the ith object.
{
   if (i < 0 || i >= getNumObjects())
      throw rangeError("Bad value in knapsack::getValue");
   
   return value[i];
}

int knapsack::getCost(int i) const
// Return the cost of the ith object.
{
   if (i < 0 || i >= getNumObjects())
      throw rangeError("Bad value in knapsack::getCost");

   return cost[i];
}


ostream &operator<<(ostream &ostr, const knapsack &k)
// Print all information about the knapsack.
{
   ostr << "------------------------------------------------" << endl;
   ostr << "Num objects: " << k.getNumObjects() << " Cost Limit: " << k.getCostLimit() << endl;

   int totalValue = 0;
   int totalCost = 0;
   
   for (int i = 0; i < k.getNumObjects(); i++)
   {
      totalValue += k.getValue(i);
      totalCost += k.getCost(i);
   }

   ostr << "Total value: " << totalValue << endl;
   ostr << "Total cost: " << totalCost << endl << endl;

   for (int i = 0; i < k.getNumObjects(); i++)
      ostr << i << "  " << k.getValue(i) << " " << k.getCost(i) << endl;

   ostr << endl;

   return ostr;
}


void knapsack::printSolution()
// Prints out the solution.
{
   cout << "------------------------------------------------" << endl;

   cout << "Total value: " << getValue() << endl;
   cout << "Total cost: " << getCost() << endl << endl;

   // Print out objects in the solution
   for (int i = 0; i < getNumObjects(); i++)
      if (isSelected(i))
	 cout << i << "  " << getValue(i) << " " << getCost(i) << endl;

   cout << endl;
}

ostream &operator<<(ostream &ostr, vector<bool> v)
// Overloaded output operator for vectors.
{
   for (int i = 0; i < v.size(); i++)
      ostr << v[i] << endl;

   return ostr;
}

void knapsack::select(int i)
// Select object i.
{
   if (i < 0 || i >= getNumObjects())
      throw rangeError("Bad value in knapsack::Select");

   if (selected[i] == false)
   {
      selected[i] = true;
      totalCost = totalCost + getCost(i);
      totalValue = totalValue + getValue(i);
   }
}

void knapsack::unSelect(int i)
// unSelect object i.
{
   if (i < 0 || i >= getNumObjects())
      throw rangeError("Bad value in knapsack::unSelect");

   if (selected[i] == true)
   {
      selected[i] = false;
      totalCost = totalCost - getCost(i);
      totalValue = totalValue - getValue(i);
   }
}

bool knapsack::isSelected(int i) const
// Return true if object i is currently selected, and false otherwise.
{
   if (i < 0 || i >= getNumObjects())
      throw rangeError("Bad value in knapsack::getValue");

   return selected[i];
}


static bool compareCVRatio(const pair<int,int> &a, const pair<int,int> &b){
      return( ((float)a.first/(float)a.second) < ((float)b.first/(float)b.second) );
}


void knapsack::sortItemsByCVRatio() {
      vector<pair<int,int> > pairs(numObjects);
      for(int i = 0; i < numObjects; i++){
            pairs[i] = make_pair(cost[i],value[i]);
      }
      sort(pairs.begin(), pairs.end(), &compareCVRatio);
      for(int i = 0; i < numObjects; i++){
            cost[i] = pairs[i].first;
            value[i] = pairs[i].second;
      }
}
