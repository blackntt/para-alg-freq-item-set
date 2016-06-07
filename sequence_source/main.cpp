#include <vector>
#include <string>
#include <iostream>
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

//DUCDM gen Large Item from Item Set K element
vector< vector<string> > aprioriGen(vector< vector<string> > largeItemSetK)
{
	vector< vector<string> > result;
	long k = largeItemSetK.at(0).size();
	for(int i = 0; i < k; i++)
	{
		for(int j = 1; j < k; j++) 
		{
			vector<string> l1 = largeItemSetK.at(i);
			//Add to frequent set (Global)
			frequentSet.push_back(l1);
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
					//call to Ha (temp.push_back(l2.at(k-1)), largeItemSetK)
					result.push_back(temp);
				}
			}
		}
	}
	return result;
}

bool hasInFrequentSubset(vector<string> &c, vector< vector<string> > largeItemSetK){
	long k = largeItemSetK.at(0).size();
	bool existInFrenquent = false;
	int count;
	for(int i = 0; i < k; i++){
		count = 0;
		vector<string> itemSet = largeItemSetK.get(i);
		for(int j = 0; j < itemSet.size(); j++){
			if(c.contains(itemSet.at(j))){
				count++;
				if(count == k)
					return true;
			}else{
				break;
			}
				
		}
	}
}
//testing main
int main(int argc, char* argv[]){
	
	//read database
	database = TeamHelper::getDatabase("../datasource/10k.txt");
	
	//set minSup
	minSup = 3;
	//to find frequent item set
	
	return 0;
}