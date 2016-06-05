#include <vector>
#include <string>
#include <iostream>
using namespace std;

class Transaction{
	private:
		string id;
		vector<string> itemSet;
	public:
		Transaction(){}
		Transaction(string id, vector<string> itemSet){
			this->id = id;
			this->itemSet = itemSet;
		}
		vector<string> getItemSet(){
			return this->itemSet;
		}
		
};

//Global database
vector< Transaction > database;

//set minSup
int minSup;


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
			bool isOk = true; 
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
	
	//set minsup
	minSup = 2;
	
	//create database
	vector<string> item1;
	item1.push_back("a");
	item1.push_back("b");
	item1.push_back("c");
	vector<string> item2;
	item2.push_back("a");
	item2.push_back("b");
	item2.push_back("c");
	vector<string> item3;
	item3.push_back("a");
	item3.push_back("d");
	item3.push_back("c");
	database.push_back(Transaction("1",item1));
	database.push_back(Transaction("2",item2));
	database.push_back(Transaction("3",item3));
	
	
	//create testing itemSet
	vector< vector<string> > itemSets;
	vector<string> item4;
	item4.push_back("a");
	item4.push_back("b");
	vector<string> item5;
	item5.push_back("a");
	item5.push_back("c");
	vector<string> item6;
	item6.push_back("a");
	item6.push_back("d");
	itemSets.push_back(item4);
	itemSets.push_back(item5);
	itemSets.push_back(item6);
	
	vector< vector<string> > result = removeAllItemSet_BelowMinSupp(itemSets);
	
	for(int i = 0; i< result.size(); i++){
		for(int j = 0; j < result[i].size(); j++){
			cout << result[i][j] << " ";
		}
		cout <<endl;
	}
	cout << result.size();
	return 0;
}