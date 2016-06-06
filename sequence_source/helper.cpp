#include "transaction.cpp"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;


class TeamHelper {
    public:
        static std::vector<string> split(const string& s, char c) {
           string::size_type i = 0;
           string::size_type j = s.find(c);
           std::vector<string> v;
           while (j != string::npos) {
              v.push_back(s.substr(i, j-i));
              i = ++j;
              j = s.find(c, j);

              if (j == string::npos)
                 v.push_back(s.substr(i, s.length()));
           }
           return v;
        }
        static std::vector< Transaction > getDatabase(string filepath) {
          std::vector< Transaction > database;
          if (filepath.empty()) {
            std::cout << "File Path cannot be empty";
            return database;
          }
          vector<string> items;
          vector< std::vector<string> > transaction;
          ifstream myfile(filepath);
          if(!myfile){
              cout << "Error opening output file" << endl;
              return database;
          }
          string key = "";
          vector<string> item_tmp;
          for (string line; getline( myfile, line );) {
              items = TeamHelper::split(line, '|');
              if (key.empty()) {
                  key = items[0];
                  item_tmp.push_back(items[1]);
              }
              else {
                  if (key.compare(items[0]) != 0) {
                      database.push_back(Transaction(key, item_tmp));
                      item_tmp.clear();
                      key = items[0];
                  }
                  item_tmp.push_back(items[1]);
              }
          }
          return database;
        }
};
