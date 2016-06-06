#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;
#include "helper.cpp"
vector< Transaction > database;

int main()
{
    database = TeamHelper::getDatabase("10k.txt");
    cout << database.size() << endl;
    for (int i = 0; i < database.size(); i++) {
        cout << database[i].getItemId();
        std::vector<string> tmp = database[i].getItemSet();
        cout << endl << "\t\t";
        for ( int k = 0; k < tmp.size(); k++) {
            cout << tmp[k] << "\t";
        }
        cout << endl;
    }

}
