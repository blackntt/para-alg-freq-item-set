#include <vector>
#include <string>
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
        string getItemId() {
            return this->id;
        }
        vector<string> getItemSet(){
            return this->itemSet;
        }

};
