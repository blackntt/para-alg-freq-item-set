Parallelized algorithm for finding frequent Item sets

<strong>Want get database :</strong><br/>
<code>
#include "helper.cpp"
vector< Transaction > database;
database = TeamHelper::getDatabase("datasource/10k.txt");
cout << database.size() << endl;
for (int i = 0; i < database.size(); i++) {
    cout << database[i].getItemId();
    std::vector<string> tmp = database[i].getItemSet();
    cout << endl << "\t\t";<br/>
    for ( int k = 0; k < tmp.size(); k++) {
        cout << tmp[k] << "\t";
    }
    cout << endl;
}
</code>