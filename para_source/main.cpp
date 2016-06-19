#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <sys/time.h>

#include "helper.cpp"
#include <thread>
using namespace std;

//Global database
vector< Transaction > database;
vector< vector<string> > frequentSet;
//set minSup
int minSup;

const int numOfCPU = 16;


void removeAllItemSet_BelowMinSupp_para_unit(
	vector< vector<string> > itemSets,
	vector< vector<string> > &result
	){

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
}


//function for remove all item sets lower than the minsup
vector< vector<string> > removeAllItemSet_BelowMinSupp(
	vector< vector<string> > itemSets){


	vector< vector<string> > finalResults;

	int subSize = itemSets.size()/numOfCPU;

	vector< vector<string> > subSets[numOfCPU];
	vector< vector<string> > results[numOfCPU];

	int j=0;
	for(int i=0;i<numOfCPU;i++){

		int curMaxIndex = i*subSize+subSize;
		if(curMaxIndex<itemSets.size())
			curMaxIndex = itemSets.size();
		for(;j<curMaxIndex;j++)
			subSets[i].push_back(itemSets[j]);
	}
	vector< thread > threads;
	for(int i=0;i<numOfCPU;i++){
		threads.push_back(thread(removeAllItemSet_BelowMinSupp_para_unit,subSets[i],std::ref(results[i])));
	}

	for(int i=0;i<numOfCPU;i++){
		threads[i].join();
	}

	for(int i=0;i<numOfCPU;i++){
		for(j=0;j<results[i].size();j++){
			finalResults.push_back((results[i]).at(j));
		}
	}

	return finalResults;

}

//DUCDM create large item set with one element
void getL1Parallel(vector< Transaction > db, vector <string> &result)
{
	for (int j = 0; j < db.size(); j++)
	{
		vector<string> trans = (db.at(j).getItemSet());
		for(int i = 0; i < trans.size(); i++)
		{
			result.push_back(trans.at(i));
		}
	}
}
//DUCDM create large item set with one element
vector< vector<string> > getL1FromDatabase()
{
	int subSize = database.size()/numOfCPU;
	vector<Transaction> dataSubSets[numOfCPU];
	vector <string> results[numOfCPU];
	int j=0;

	for(int i=0;i<numOfCPU;i++){
		int curMaxIndex = i*subSize+subSize;
		if(curMaxIndex < database.size())
			curMaxIndex = database.size();
		for(;j<curMaxIndex;j++)
			dataSubSets[i].push_back(database.at(j));
	}
	vector< thread > threads;
	for(int i=0; i<numOfCPU; i++){
		threads.push_back(thread(&getL1Parallel, dataSubSets[i], std::ref(results[i])));
	}

	for(int i=0;i<numOfCPU;i++){
		threads[i].join();
	}
	set <string> C1;
	for(int i=0;i<numOfCPU;i++){
		for(j=0;j<results[i].size();j++){
			C1.insert((results[i]).at(j));
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

// HaHV: get subset recursion function
void getSubsets(vector<string> superSet, int k, int idx, set<string>* current,vector< vector<string> >* solution) {
    //successful stop clause
    if ((*current).size() == k) {
		vector<string> temp(k);
		std::copy((*current).begin(), (*current).end(), temp.begin());
		(*solution).push_back(temp);
        return;
    }
    //unseccessful stop clause
    if (idx == superSet.size()) return;
	string x = superSet.at(idx);
	(*current).insert(x);
    //"guess" x is in the subset
    getSubsets(superSet, k, idx+1, current, solution);
	(*current).erase(x);
    //"guess" x is not in the subset
    getSubsets(superSet, k, idx+1, current, solution);
}

// get k-subset of superset with (k+1) elements
vector< vector<string> >* getSubsets(const vector<string>& superSet, int k) {
    vector< vector<string> > *res = new vector< vector<string> >();
	set<string> *current = new set<string>();

	getSubsets(superSet, k, 0, current, res);

	delete current;

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
		for(int j = i+1; j < size; j++)
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
	for (int r = 1; r <= 10; r++) {
    //read database
    string s_r = to_string(r);
    string file_name_input = "../datasource/" + s_r + ".txt";
    string file_name_output = s_r + "_result.txt";
    database = TeamHelper::getDatabase(file_name_input);
    frequentSet.clear();
		//set minSup
		minSup = 3;

		struct timeval benchmark;
		gettimeofday(&benchmark, NULL);
		long startTime = benchmark.tv_sec * 1000 + benchmark.tv_usec / 1000;
		//to find frequent item set
		vector< vector<string> > L1 = getL1FromDatabase();
		 while(L1.size() > 0)
		 {
		 	L1 = aprioriGen(L1);
		 }
		gettimeofday(&benchmark, NULL);
		long endTime = benchmark.tv_sec * 1000 + benchmark.tv_usec / 1000;
		ofstream outFile;
		outFile.open (file_name_output.c_str());
		outFile << (endTime - startTime) << "ms" << endl;
		for (int k = 0; k < frequentSet.size(); k++) {
			outFile << frequentSet[k] << endl;
		}
		outFile.close();
	}
	return 0;
}
