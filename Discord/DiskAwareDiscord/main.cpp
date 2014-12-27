#include<iostream>
#include<fstream>
#include<set>
#include<algorithm>
#include<vector>
#include<map>
#include<time.h>
using namespace std;
const int SEQ_NUM = 65536;
const int SEQ_LENGTH = 1024;
const char* FILE_NAME = "../../data/discord_rw_sw_test.txt";

double ed_distance(double* a, double* b){
	double dist = 0.;
	for (int i = 0; i < SEQ_LENGTH; i++){
		dist = dist + (a[i] - b[i])*(a[i] - b[i]);
	}
	return sqrt(dist);
}
void DC_Selection(double r, set<pair<int,double*> > &C){
	ifstream fp(FILE_NAME);
	double* seq = new double[SEQ_LENGTH];
	int idx;
	fp >> idx;
	for (int i = 0; i < SEQ_LENGTH; i++)
		fp >> seq[i];
	//set<double*> C;
	C.insert(make_pair(0,seq));
	seq = NULL;
	for (int i = 1; i < SEQ_NUM; i++){
		//read i-th seq from file
		double* tmp = new double[SEQ_LENGTH];
		fp >> idx;
		for (int k = 0; k < SEQ_LENGTH; k++)
			fp >> tmp[k];

		bool isCandidate = true;
		if (!C.empty()){
			vector<set<pair<int, double*> >::iterator> v;
			for (set<pair<int, double*> >::iterator it = C.begin(); it != C.end(); it++){
				double d = ed_distance(tmp, (*it).second);
				if (d < r){
					isCandidate = false;
					delete[](*it).second;
					v.push_back(it);
					//C.erase(*it);
				}
			}
			for (int i = 0; i < v.size(); i++)
				C.erase(*(v[i]));
		}
		if (isCandidate)
			C.insert(make_pair(i,tmp));
	}
	fp.close();
}

void DC_Refinement(double r, set<pair<int, double*> > &C, map<int,double> &C_dist){
	int C_size = C.size();
	int idx;
	ifstream fp(FILE_NAME);
	for (set<pair<int, double*> >::iterator it = C.begin(); it != C.end(); it++)
		C_dist[(*it).first] = FLT_MAX;
	for (int i = 0; i < SEQ_NUM; i++){
		double seq[SEQ_LENGTH];
		fp >> idx;
		for (int k = 0; k < SEQ_LENGTH; k++)
			fp >> seq[k];
		vector<set<pair<int, double*> >::iterator> v;
		for (set<pair<int, double*> >::iterator it = C.begin(); it != C.end(); it++){
			if ((*it).first==i)
				continue;
			double d = ed_distance((*it).second, seq);
			if (d < r){
				delete [] (*it).second;
				C_dist[(*it).first] = -1.;
				//C.erase(*it);
				v.push_back(it);
			}
			else{
				C_dist[(*it).first] = min(C_dist[(*it).first], d);
			}
		}
		for (int i = 0; i < v.size(); i++)
			C.erase(*(v[i]));
	}
	fp.close();
}


double get_range_r(){
	double mi = FLT_MAX;
	double mx = 0.;
	ifstream fp(FILE_NAME);
	double seq[SEQ_LENGTH];
	int idx;
	fp >> idx;
	for (int i = 0; i < SEQ_LENGTH; i++)
		fp >> seq[i];
	for (int i = 1; i < SEQ_NUM; i++){
		double tmp[SEQ_LENGTH];
		fp >> idx;
		for (int i = 0; i < SEQ_LENGTH; i++)
			fp >> tmp[i];
		double d = ed_distance(tmp, seq);
		if (d>mx)
			mx = d;   //10.915
		if (d < mi)
			mi = d;   //3.84567
	}
	fp.close();
	cout << "range max=" << mx << endl;
	cout << "range min=" << mi << endl;
	return (mx + mi) / 2;
}

void disk_aware_discord(set<pair<int, double*> > *pC){
	//double r = get_range_r();
	double r = 17.38;
	DC_Selection(r, *pC);
	int C_size = (*pC).size();
	//double* C_dist = new double[C_size];
	map<int, double> C_dist;
	DC_Refinement(r, *pC, C_dist);
}


int main(){
	//disc aware discord discovery
	set<pair<int, double*> > *pC = new set<pair<int, double*> >;
	int t = clock();
	disk_aware_discord(pC);
	t = clock() - t;
	for (set<pair<int, double*> >::iterator itr = (*pC).begin(); itr != (*pC).end(); itr++){
		cout << "position = " << (*itr).first << endl;
	}
	delete pC;
	cout << "time= " << t / CLOCKS_PER_SEC << endl;  //t=26
	getchar();
	return 0;
}
