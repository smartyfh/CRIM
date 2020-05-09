#pragma once
#ifndef BCRIM_H_
#define BCRIM_H_

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <queue>
#include <list>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstring>
#include <stdlib.h>
#include <iomanip>
#include <random>
using namespace std;

struct neighbourB {
	int neighbournode;
	double probability;
};

class BasicCRIM {
private:
	int comm_num; // #community
	int round; // #simulation
	int seed_size; // #seeds
	int node_num; // #nodes
	int *hub; // mark if a node is a hub node: yes 1; no 0
	int *node; //store node's name, e.g., node[10] = 10;
	int *isSeed; // mark if a node is included in seed set: yes 1; no 0
	int *visit; // if activate
	int *comm; // comm[i] = c means node i in comm c
	int *comm_update; // whether update or not
	double *threshold; // threshold[i] recordes the activation threshold of node i
	double *weight; // weight[i] recordes the current weighted fraction of i's neighbour nodes
	double *weight2;  // record the current  weighted fraction of i's neighbour nodes with the same community
	double *IS; // the current influence of each comm
	double **MIS; // the marginal influence spread of each community of a new seed node
	vector< int > seedset; // seed set
	vector< int > *commseed; // the chosen seed nodes in a comm
	vector< int > *commset; // the nodes in a comm
	vector< int > *H; // the extended nodes in a comm
	vector< neighbourB > *nbr; // network cut by commuunities
	vector< neighbourB > *G; // the whole network (hub)
	vector< neighbourB > *GG; // the whole network
public:
	BasicCRIM();
	void initialization(int t, int k, int n, int c, string M);
	void load_graph(string path, string M);
	double IC(int t, int cm);
	double LT(int t, int cm);
	double CIC(int u);
	double CLT(int u);
	void ExtendSeedsIC(int u);
	void ExtendSeedsLT(int u);
	void output(string path, string M, int k, double timecost, double influencespread);
	void outputIS(string path, string M, int k, double influencespread, double hubratio);
	void influence_maximization(string path, int n, int k, int t, int c, string M);
	double RandCasIC(int k, int t); // calculate influence spread on the whole network
	double RandCasLT(int k, int t); // calculate influence spread on the whole network
	void clr();
};

#endif // !BCRIM_H_
