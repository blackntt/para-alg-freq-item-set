#include <vector>
#include <string>
#include <iostream>
#include "helper.cpp"
using namespace std;

//Global database
vector< Transaction > database;

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

//testing main
int main(int argc, char* argv[]){
	
	//read database
	database = TeamHelper::getDatabase("../datasource/10k.txt");
	
	//set minSup
	minSup = 3;
	//to find frequent item set
	
	return 0;
}