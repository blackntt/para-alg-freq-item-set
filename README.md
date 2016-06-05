Parallelized algorithm for finding frequent Item sets

Want get database :
#include "helper.cpp"
vector< Transaction > database;
database = TeamHelper::getDatabase("datasource/10k.txt");
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