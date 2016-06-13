#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <sys/time.h>

#include "helper.cpp"
using namespace std;

//Global database
vector< Transaction > database;
vector< vector<string> > frequentSet;
//set minSup
int minSup;

//function for remove all item sets lower than the minsup
vector< vector<string> > removeAllItemSet_BelowMinSupp(
	vector< vector<string> > itemSets){

	vector< vector<string> > result;

	for(int i = 0;i < itemSets.size();i++){

		vector<string> * curSet = &itemSets[i];


		//Count the supp

		int count=0;//count of current itemset

		for (int j = 0;j < database.size(); j++){


			//check if the current transaction contains the current itemset
			vector<string> currentTransaction = (database[j].getItemSet());
			bool isOk = true;//true: the transaction contains the itemset - false: the transaction doesn't contains the itemset
			for(int k = 0; k < curSet->size(); k++){
				bool isContain = false;

				for(int l = 0; l < currentTransaction.size();l++){
					if(curSet->at(k).compare(currentTransaction.at(l))==0){
						isContain = true;
						break;
					}
				}

				if(!isContain){
					isOk = false;
					break;
				}
			}

			if(isOk && curSet->size()>0 && currentTransaction.size()>0) count++;

		}

		if(count >= minSup)
			result.push_back(*curSet);
	}
	return result;
}

//DUCDM create large item set with one element
vector< vector<string> > getL1FromDatabase()
{
	set <string> C1;
	for (int j = 0; j < database.size(); j++)
	{
		vector<string> trans = (database[j].getItemSet());
		for(int i = 0; i < trans.size(); i++)
		{
			C1.insert(trans.at(i));
		}
	}
	vector< vector<string> > c1Vector;
	set<string>::iterator it;
  	for (it = C1.begin() ; it != C1.end(); it++ )
	{
		vector<string> temp;
		temp.push_back(*it);
		c1Vector.push_back(temp);
	}
	return removeAllItemSet_BelowMinSupp(c1Vector);
}


// get k-subset of superset with (k+1) elements
vector< vector<string> >* getSubsets(const vector<string>& superSet, int k) {

	vector< vector<string> > *res = new vector< vector<string> >();

	// k-subset will be generate by selecting k in total (k+1) elements in superSet
	for (int i = 0; i < superSet.size(); i++)
	{
		vector<string> tempKsubset;
		//eliminate an element to create a k-subset.
		for (int j = 0; j < superSet.size(); j++)
		{
			if (i == j)
			{
				continue;
			} else
			{
				tempKsubset.push_back(superSet.at(j));
			}
		}

		(*res).push_back(tempKsubset);
	}

    return res;
}

//check if any k-subset doesn't exist in L(k)
bool hasInFrequentSubset(const vector<string> &candidateSetKplus1, vector< vector<string> >& largeItemSetK)
{
	long k = candidateSetKplus1.size() - 1;

	//get all k-item-subsets of  C(k+1)
	vector< vector<string> >* subsets = getSubsets(candidateSetKplus1, k);

	bool result = false;

	//check if any k-subset doesn't exist in L(k)
	for(int i = 0; i < (*subsets).size(); i++) {

		vector<string> sub = (*subsets).at(i);

		bool foundSubInLK = false;

		for(int j = 0; j < largeItemSetK.size(); j++) {

			vector<string> kItemSet = largeItemSetK.at(j);
			if(sub == kItemSet) {

				foundSubInLK = true;
				break;
			}
		}

		// k-subitem doest exists in Lk
		if(foundSubInLK == false) {

			result = true;
			break;
		}
	}

	delete subsets;

	return result;
}

//DUCDM gen Large Item from Item Set K element
vector< vector<string> > aprioriGen(vector< vector<string> > largeItemSetK)
{
	vector< vector<string> > result;
	int size = largeItemSetK.size();
	int k = largeItemSetK.at(0).size();
	for(int i = 0; i < size; i++)
	{
		//Add to frequent set (Global)
		frequentSet.push_back(largeItemSetK.at(i));
		for(int j = i + 1; j < size; j++)
		{
			vector<string> l1 = largeItemSetK.at(i);
			vector<string> l2 = largeItemSetK.at(j);
			bool haveSamePrefix = true;
			for(int t = 0; t < (k-1); t++)
			{
				if(l1.at(t) != l2.at(t))
				{
					haveSamePrefix = false;
					break;

				}
			}
			if(haveSamePrefix)
			{
				if(l1.at(k-1).compare(l2.at(k-1)) != 0)
				{
					vector<string> temp = l1;
					temp.push_back(l2.at(k-1));
					bool isInValid = hasInFrequentSubset(temp, largeItemSetK);
					if(!isInValid)
					{
						result.push_back(temp);
					}
				}
			}
		}
	}
	return removeAllItemSet_BelowMinSupp(result);

}

//for test
template<typename T>
ostream& operator<< (ostream& out, const vector<T>& v) {
    out << "[";
    size_t last = v.size() - 1;
    for(size_t i = 0; i < v.size(); ++i) {
        out << v[i];
        if (i != last)
            out << ", ";
    }
    out << "]";
    return out;
}


//testing main
int main(int argc, char* argv[]){

	//read database
	database = TeamHelper::getDatabase("../datasource/test.txt");

	//set minSup
	minSup = 3;

	struct timeval benchmark;
	gettimeofday(&benchmark, NULL);
	long startTime = benchmark.tv_sec * 1000 + benchmark.tv_usec / 1000;
	// //to find frequent item set
	int count_step = 1;
	vector< vector<string> > L1 = getL1FromDatabase();
	 while(L1.size() > 0)
	 {
	 	L1 = aprioriGen(L1);
	 	cout<<"Step: "<< count_step++ << endl;
	 }
	gettimeofday(&benchmark, NULL);
	long endTime = benchmark.tv_sec * 1000 + benchmark.tv_usec / 1000;
	ofstream outFile;
	outFile.open ("result_10k.txt");
	outFile << (endTime - startTime) <<"ms";
	outFile.close();
	cout<<"Time to run: "<<endTime - startTime<<"ms" << endl;
	return 0;
}
