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

const int numOfCPU = 2;


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

	Last login: Wed Jun 15 08:57:53 on ttys004
You have new mail.
MinhVN:sequence_source MinhVN$ ssh ubuntu@192.168.20.201
Welcome to Ubuntu 14.04.3 LTS (GNU/Linux 3.19.0-25-generic x86_64)

 * Documentation:  https://help.ubuntu.com/

  System information as of Mon Jun 13 13:42:11 ICT 2016

  System load:  0.0               Processes:           248
  Usage of /:   5.0% of 74.40GB   Users logged in:     0
  Memory usage: 1%                IP address for eth0: 192.168.20.201
  Swap usage:   0%

  Graph this data and manage this system at:
    https://landscape.canonical.com/

141 packages can be updated.
70 updates are security updates.

Last login: Sun Jun 12 00:14:42 2016 from 192.168.20.12
ubuntu@ubuntu:~$ ls lai
ls: cannot access lai: No such file or directory
ubuntu@ubuntu:~$ ps
  PID TTY          TIME CMD
 2019 pts/0    00:00:00 bash
 2052 pts/0    00:00:00 ps
ubuntu@ubuntu:~$ clear





ubuntu@ubuntu:~$ ls
demos  WebGoat-Workspace  www
ubuntu@ubuntu:~$ mkdir DTMD
ubuntu@ubuntu:~$ cd DTMD/
ubuntu@ubuntu:~/DTMD$ ls
ubuntu@ubuntu:~/DTMD$ sudo apt-get install git
[sudo] password for ubuntu:
Reading package lists... Done
Building dependency tree
Reading state information... Done
git is already the newest version.
0 upgraded, 0 newly installed, 0 to remove and 137 not upgraded.
ubuntu@ubuntu:~/DTMD$ clear




















ubuntu@ubuntu:~/DTMD$ ls
ubuntu@ubuntu:~/DTMD$ git clone https://gitlab.com/blackntt/para_alg_freq_item_set.git
Cloning into 'para_alg_freq_item_set'...
Username for 'https://gitlab.com': minhvn
Password for 'https://minhvn@gitlab.com':
remote: Counting objects: 226, done.
remote: Compressing objects: 100% (131/131), done.
remote: Total 226 (delta 105), reused 195 (delta 91)
Receiving objects: 100% (226/226), 17.58 MiB | 771.00 KiB/s, done.
Resolving deltas: 100% (105/105), done.
Checking connectivity... done.
ubuntu@ubuntu:~/DTMD$ ls
para_alg_freq_item_set
ubuntu@ubuntu:~/DTMD$ cd para_alg_freq_item_set/
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set$ git checkout parallel
Branch parallel set up to track remote branch parallel from origin.
Switched to a new branch 'parallel'
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set$ ls status
ls: cannot access status: No such file or directory
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set$ git status
On branch parallel
Your branch is up-to-date with 'origin/parallel'.

nothing to commit, working directory clean
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set$ ls
datasource  para_source  Plan-CGRID-1.txt  README.md  sequence_source
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set$ cd sequence_source/
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/sequence_source$ ls
helper.cpp  main.cpp  transaction.cpp
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/sequence_source$ cd ../datasource/
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/datasource$ ls
100k.txt  10k.txt  20k.txt  50k.txt  540k.txt  5k.txt  test.txt
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/datasource$ clear

ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/datasource$ ls
100k.txt  10k.txt  20k.txt  50k.txt  540k.txt  5k.txt  test.txt
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/datasource$ ls
100k.txt  10k.txt  20k.txt  50k.txt  540k.txt  5k.txt  test.txt
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/datasource$ ls
100k.txt  10k.txt  20k.txt  50k.txt  540k.txt  5k.txt  test.txt
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/datasource$ clear


























ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/datasource$ ls
100k.txt  10k.txt  20k.txt  50k.txt  540k.txt  5k.txt  test.txt
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/datasource$ git pull origin parallel
Username for 'https://gitlab.com': minhvn
Password for 'https://minhvn@gitlab.com':
remote: Counting objects: 10, done.
remote: Compressing objects: 100% (10/10), done.
remote: Total 10 (delta 5), reused 0 (delta 0)
Unpacking objects: 100% (10/10), done.
From https://gitlab.com/blackntt/para_alg_freq_item_set
 * branch            parallel   -> FETCH_HEAD
   f912ebb..88746bd  parallel   -> origin/parallel
Updating f912ebb..88746bd
Fast-forward
 datasource/1k.txt | 1000 ++++++++++++++++++
 datasource/2k.txt | 2000 ++++++++++++++++++++++++++++++++++++
 datasource/3k.txt | 3000 +++++++++++++++++++++++++++++++++++++++++++++++++++++
 datasource/4k.txt |    1 +
 datasource/5k.txt | 8367 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++------------------------------------------------------------
 5 files changed, 11001 insertions(+), 3367 deletions(-)
 create mode 100644 datasource/1k.txt
 create mode 100644 datasource/2k.txt
 create mode 100644 datasource/3k.txt
 create mode 100644 datasource/4k.txt
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/datasource$ ls
100k.txt  10k.txt  1k.txt  20k.txt  2k.txt  3k.txt  4k.txt  50k.txt  540k.txt  5k.txt  test.txt
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/datasource$ cd ../
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set$ ls
datasource  para_source  Plan-CGRID-1.txt  README.md  sequence_source
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set$ clear



ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set$ ls
datasource  para_source  Plan-CGRID-1.txt  README.md  sequence_source
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set$ git checkout parallel
Already on 'parallel'
Your branch is up-to-date with 'origin/parallel'.
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set$ ls
datasource  para_source  Plan-CGRID-1.txt  README.md  sequence_source
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set$ clear

























ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set$ ls
datasource  para_source  Plan-CGRID-1.txt  README.md  sequence_source
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set$ cd para_source/
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/para_source$ ls
helper.cpp  main.cpp  transaction.cpp
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/para_source$ vi main.cpp
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/para_source$
ubuntu@ubuntu:~/DTMD/para_alg_freq_item_set/para_source$ packet_write_wait: Connection to 192.168.20.201: Broken pipe
MinhVN:sequence_source MinhVN$ ssh ubuntu@192.168.20.202
Welcome to Ubuntu 14.04.3 LTS (GNU/Linux 3.19.0-25-generic x86_64)

 * Documentation:  https://help.ubuntu.com/

  System information as of Wed Jun 15 15:43:12 ICT 2016

  System load:  0.0               Processes:           208
  Usage of /:   2.2% of 74.40GB   Users logged in:     0
  Memory usage: 0%                IP address for eth0: 192.168.20.202
  Swap usage:   0%

  Graph this data and manage this system at:
    https://landscape.canonical.com/

142 packages can be updated.
71 updates are security updates.

Last login: Wed Jun 15 15:43:12 2016 from 192.168.14.90
ubuntu@ubuntu:~$ ls
DTDM
ubuntu@ubuntu:~$ cd DTDM/para_alg_freq_item_set/sequence_source/
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ ls
1_result.txt  2_result.txt  3_result.txt  4_result.txt  5_result.txt  6_result.txt  7_result.txt  app  helper.cpp  main.cpp  transaction.cpp
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ cat 7_result.txt
1659260msubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ cat 2_result.txt
3msubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ clear

ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ ls -lai
total 160
3932366 drwxrwxr-x 2 ubuntu ubuntu   4096 Jun 15 16:40 .
3932173 drwxrwxr-x 5 ubuntu ubuntu   4096 Jun  9 11:11 ..
3932595 -rw-rw-r-- 1 ubuntu ubuntu      3 Jun 15 16:12 1_result.txt
3932615 -rw-rw-r-- 1 ubuntu ubuntu      3 Jun 15 16:12 2_result.txt
3932616 -rw-rw-r-- 1 ubuntu ubuntu      3 Jun 15 16:12 3_result.txt
3932617 -rw-rw-r-- 1 ubuntu ubuntu      3 Jun 15 16:12 4_result.txt
3932618 -rw-rw-r-- 1 ubuntu ubuntu      3 Jun 15 16:12 5_result.txt
3932619 -rw-rw-r-- 1 ubuntu ubuntu      4 Jun 15 16:12 6_result.txt
3932620 -rw-rw-r-- 1 ubuntu ubuntu      9 Jun 15 16:40 7_result.txt
3932493 -rwxrwxr-x 1 ubuntu ubuntu 108914 Jun 15 16:12 app
3932367 -rw-rw-r-- 1 ubuntu ubuntu   1875 Jun 15 09:06 helper.cpp
3932368 -rw-rw-r-- 1 ubuntu ubuntu   5594 Jun 15 16:11 main.cpp
3932369 -rw-rw-r-- 1 ubuntu ubuntu    460 Jun  9 11:11 transaction.cpp
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ rm 1_result.txt 2_result.txt 3_result.txt 4_result.txt 5_result.txt 6_result.txt 7_result.txt
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ ls
app  helper.cpp  main.cpp  transaction.cpp
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ ./app &
[1] 3578
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ 1_result.txt
2_result.txt
3_result.txt
4_result.txt
5_result.txt
6_result.txt
7_result.txt
^C
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ ps
  PID TTY          TIME CMD
 3535 pts/0    00:00:00 bash
 3578 pts/0    00:00:50 app
 3579 pts/0    00:00:00 ps
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ ls
1_result.txt  2_result.txt  3_result.txt  4_result.txt  5_result.txt  6_result.txt  app  helper.cpp  main.cpp  transaction.cpp
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ clear

ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ ls
1_result.txt  2_result.txt  3_result.txt  4_result.txt  5_result.txt  6_result.txt  app  helper.cpp  main.cpp  transaction.cpp
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ ls
1_result.txt  2_result.txt  3_result.txt  4_result.txt  5_result.txt  6_result.txt  app  helper.cpp  main.cpp  transaction.cpp
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ ps
  PID TTY          TIME CMD
 3535 pts/0    00:00:00 bash
 3578 pts/0    00:02:04 app
 3587 pts/0    00:00:00 ps
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ ps
  PID TTY          TIME CMD
 3535 pts/0    00:00:00 bash
 3578 pts/0    00:05:48 app
 3588 pts/0    00:00:00 ps
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ ls -lai
total 156
3932366 drwxrwxr-x 2 ubuntu ubuntu   4096 Jun 16 08:15 .
3932173 drwxrwxr-x 5 ubuntu ubuntu   4096 Jun  9 11:11 ..
3932595 -rw-rw-r-- 1 ubuntu ubuntu      3 Jun 16 08:15 1_result.txt
3932615 -rw-rw-r-- 1 ubuntu ubuntu      3 Jun 16 08:15 2_result.txt
3932616 -rw-rw-r-- 1 ubuntu ubuntu      3 Jun 16 08:15 3_result.txt
3932617 -rw-rw-r-- 1 ubuntu ubuntu      3 Jun 16 08:15 4_result.txt
3932618 -rw-rw-r-- 1 ubuntu ubuntu      3 Jun 16 08:15 5_result.txt
3932619 -rw-rw-r-- 1 ubuntu ubuntu      4 Jun 16 08:15 6_result.txt
3932493 -rwxrwxr-x 1 ubuntu ubuntu 108914 Jun 15 16:12 app
3932367 -rw-rw-r-- 1 ubuntu ubuntu   1875 Jun 15 09:06 helper.cpp
3932368 -rw-rw-r-- 1 ubuntu ubuntu   5594 Jun 15 16:11 main.cpp
3932369 -rw-rw-r-- 1 ubuntu ubuntu    460 Jun  9 11:11 transaction.cpp
ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ clear




ubuntu@ubuntu:~/DTDM/para_alg_freq_item_set/sequence_source$ vi main.cpp


































































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
	for (int r = 1; r <= 7; r++) {
    //read database
    string s_r = to_string(r);
    string file_name_input = "../datasource/" + s_r + ".txt";
    string file_name_output = s_r + "_result.txt";
    cout<< file_name_output << endl;
    database = TeamHelper::getDatabase(file_name_input);

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
    outFile << (endTime - startTime) <<"ms";
    outFile.close();
	}
	return 0;
}
