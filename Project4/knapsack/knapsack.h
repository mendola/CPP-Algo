#include "../utils/d_except.h"
#include <algorithm>

class knapsack
{
   public:
      knapsack(ifstream &fin);
      knapsack(const knapsack &);
      int getCost(int) const;
      int getValue(int) const;
      int getCost() const;
      int getValue() const;
      int getNumObjects() const;
      int getCostLimit() const;
      void printSolution();
      void select(int);
      void unSelect(int);
      bool isSelected(int) const;
      void sortItemsByCVRatio();
      int nextBestItemIdx(int i);
      int setBound();
      int getDepth() const;
      int bound;
      int depth;

   private:
      int numObjects;
      int costLimit;
      vector<int> value;
      vector<int> cost;
      vector<bool> selected;
      int totalValue;
      int totalCost;
};

knapsack::knapsack(ifstream &fin)
// Construct a new knapsack instance using the data in fin.
{
   int n, b, j, v, c;
   
   fin >> n;  // read the number of objects
   fin >> b;  // read the cost limit

   numObjects = n;
   costLimit = b;
   
   value.resize(n);
   cost.resize(n);
   selected.resize(n);
   
   for (int i = 0; i < n; i++)
   {
      fin >> j >> v >> c;
      value[j] = v;
      cost[j] = c;
      unSelect(j);
   }

   totalValue = 0;
   totalCost = 0;
   bound = 0;
   depth = 0;
}

knapsack::knapsack(const knapsack &k)
// Knapsack copy constructor.
{
   int n = k.getNumObjects();
   
   value.resize(n);
   cost.resize(n);
   selected.resize(n);
   numObjects = k.getNumObjects();
   costLimit = k.getCostLimit();

   bound = k.bound;
   depth = k.depth;

   for (int i = 0; i < n; i++)
   {
      value[i] = k.getValue(i);
      cost[i] = k.getCost(i);
      if (k.isSelected(i))
	 select(i);
      else
	 unSelect(i);
   }
   totalCost = k.getCost();
   totalValue = k.getValue();
}

int knapsack::getNumObjects() const
{
   return numObjects;
}

int knapsack::getCostLimit() const
{
   return costLimit;
}


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

int knapsack::getCost() const
// Return the cost of the selected objects.
{
   return totalCost;
}

int knapsack::getValue() const
// Return the value of the selected objects.
{
   return totalValue;
}

int knapsack::getDepth() const
{
      return depth;
}

ostream &operator<<(ostream &ostr, const knapsack &k)
// Print all information about the knapsack.
{
   cout << "------------------------------------------------" << endl;
   cout << "Num objects: " << k.getNumObjects() << " Cost Limit: " << k.getCostLimit() << endl;

   int totalValue = 0;
   int totalCost = 0;
   
   for (int i = 0; i < k.getNumObjects(); i++)
   {
      totalValue += k.getValue(i);
      totalCost += k.getCost(i);
   }

   cout << "Total value: " << totalValue << endl;
   cout << "Total cost: " << totalCost << endl << endl;

   for (int i = 0; i < k.getNumObjects(); i++)
      cout << i << "  " << k.getValue(i) << " " << k.getCost(i) << endl;

   cout << endl;

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
      cout << v[i] << endl;

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


void knapsack::sortItemsByCVRatio(){

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

// Return index of next best item not already selected, starting at idx i
int knapsack::nextBestItemIdx(int i){
      // Assumes presorted by V/C ratio
      int idx = i;
      while( idx < numObjects && isSelected(idx)){
            idx++;
      }
      return idx;
}

int knapsack::setBound(){ 
      double b = 0;
      int maxCost = getCostLimit();
      int currCost = 0;
      int currItemIdx = depth;

      for(int i = 0; i < depth;i++){
            if(isSelected(i)){
                  b += (double)value[i];
                  currCost += cost[i];
            }
      }

      // Add all full items that fit (presorted in order of V/C ratio)
      while(currItemIdx < numObjects && currCost + cost[nextBestItemIdx(currItemIdx)] < maxCost){
            currItemIdx = nextBestItemIdx(currItemIdx);
            b += (double)value[currItemIdx];
            currCost += cost[currItemIdx];
            currItemIdx++;
      }

      // Add last item fractionally
      if(currItemIdx < numObjects){
            b += (double)(maxCost - currCost) / (double)cost[currItemIdx] * (double)value[currItemIdx];
            bound = (int)b;
      } else{
            bound = (int)b;
      }
      return bound;  // Truncate to integer value
}

