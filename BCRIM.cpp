//#include "stdafx.h"
#include "BCRIM.h"

std::default_random_engine RDM((unsigned int)(time(NULL)));
std::uniform_real_distribution<double> dis(0.0, 1.0);

BasicCRIM::BasicCRIM()
{
	hub = NULL;
	node = NULL;
	isSeed = NULL;
	visit = NULL;
	comm = NULL;
	comm_update = NULL;
	threshold = NULL;
	weight = NULL;
	weight2 = NULL;
	IS = NULL;
	MIS = NULL;
	commseed = NULL;
	commset = NULL;
	H = NULL;
	nbr = NULL;
	G = NULL;
	GG = NULL;
}

void BasicCRIM::initialization(int t, int k, int n, int c, string M)
{
	seedset.clear();
	comm_num = c; // the number of communities
	round = t;
	seed_size = k;
	node_num = n;

	hub = new int[node_num + 2];
	node = new int[node_num + 2];
	isSeed = new int[node_num + 2];
	visit = new int[node_num + 2];
	comm = new int[node_num + 2];
	if (M.compare("LT") == 0) {
		threshold = new double[node_num + 2];
		weight = new double[node_num + 2];
		weight2 = new double[node_num + 2];
	}

	comm_update = new int[comm_num + 2];
	IS = new double[comm_num + 2];
	MIS = new double*[2];
	for (int i = 0; i < 2; i++)
		MIS[i] = new double[comm_num + 2];
	commseed = new vector< int >[comm_num + 2];
	commset = new vector< int >[comm_num + 2];
	H = new vector< int >[comm_num + 2];

	nbr = new vector< neighbourB >[node_num + 2];
	G = new vector< neighbourB >[node_num + 2];
	GG = new vector< neighbourB >[node_num + 2];

	for (int i = 0; i < node_num + 2; i++) {
		comm[i] = -1;
		hub[i] = 0;
		node[i] = -1;
		isSeed[i] = 0;
	}
	for (int i = 0; i < comm_num + 2; i++) {
		commseed[i].clear();
		commset[i].clear();
		IS[i] = 0;
	}
}

void BasicCRIM::load_graph(string path, string M)
{
	cout << "Beginning loading graph..." << endl;

	string pathn = path + "node_comm.txt";
	string pathm = path + M + "-edges_pp.txt";
	string pathh = path + "hub.txt";

	int u, v;
	char c;
	ifstream ifile;
	ifile.open(pathn.c_str());
	if (ifile) {
		while (!ifile.eof()) {
			ifile >> u >> v;
			if (u == -1) break;
			comm[u] = v;
			commset[v].push_back(u);
			node[u] = u;
			u = -1;
		}
	}
	else {
		cout << "Error occurred when opening: " + pathn << endl;
		exit(1);
	}
	ifile.close();

    // hub
	ifstream hfile;
	hfile.open(pathh.c_str());
	if (hfile) {
		while (!hfile.eof()) {
			hfile >> u >> v;
			if (u == -1) break;
			hub[u] = v;
			u = -1;
		}
	}
	else {
		cout << "Error occurred when opening: " + pathh << endl;
		exit(1);
	}
	hfile.close();

	double p;
	ifstream fin;
	fin.open(pathm.c_str());
	if (fin) {
		while (!fin.eof()) {
			fin >> u >> v >> p;
			if (u == -1) break;
			neighbourB nb;
			nb.neighbournode = v;
			nb.probability = p;
			GG[u].push_back(nb);
			if (comm[u] == comm[v]) {
				nbr[u].push_back(nb);
				G[u].push_back(nb);
			}
			else if (hub[u]) {
				G[u].push_back(nb);
			}
			//else {
			//	hub[u] = 1;
			//}

			u = -1;
		}
	}
	else {
		cout << "Error occurred when opening: " + pathm << endl;
		exit(1);
	}
	fin.close();

	cout << "Finished loading graph!" << endl;
}

double BasicCRIM::IC(int t, int cm)
{
	//default_random_engine random((unsigned int)(time(NULL)));
	//std::uniform_real_distribution<double> dis(0.0, 1.0);

	double influencespread = 0.0;
	queue< int > que;
	while (!que.empty()) que.pop();
	for (int i = 0; i < (int)commset[cm].size(); i++) {
		visit[commset[cm][i]] = 0;
	}

	//push starting nodes into queue
	for (int i = 0; i < (int)H[cm].size(); i++) {
		que.push(H[cm][i]);
		visit[H[cm][i]] = 1;
	}
	influencespread += (int)H[cm].size();
	
	//if (H[cm].size() > commset[cm].size())
	//	cout << "comm " << cm << " wrong!" << endl;
	//if (H[cm].size() < commseed[cm].size())
	//	cout << "comm " << cm << " less!" << endl;

	while (!que.empty()) { //BFS
		int curNode = que.front();
		que.pop();

		for (int j = 0; j < nbr[curNode].size(); j++) {
			int w = nbr[curNode][j].neighbournode; // neighbour
			double p = nbr[curNode][j].probability;
			if (!visit[w]) {
				double pw = dis(RDM);
				if (pw <= p) { // activate successfully
					que.push(w);
					visit[w] = 1;
					influencespread += 1;
				}
			}
		}
	}

	return influencespread;
}

double BasicCRIM::LT(int t, int cm)
{
	double influencespread = 0.0;
	queue< int > que;
	while (!que.empty()) que.pop();
	//for (int i = 0; i < (int)commset[cm].size(); i++) {
	//	visit[commset[cm][i]] = 0;
		//weight[commset[cm][i]] = 0.0;
	//}

	//push starting nodes into queue
	for (int i = 0; i < (int)H[cm].size(); i++) {
		que.push(H[cm][i]);
		//visit[H[cm][i]] = 1;
	}
	influencespread += (int)H[cm].size();

	while (!que.empty()) { //BFS
		int curNode = que.front();
		que.pop();

		for (int j = 0; j < nbr[curNode].size(); j++) {
			int w = nbr[curNode][j].neighbournode; // neighbour
			double p = nbr[curNode][j].probability;
			if (!visit[w]) {
				weight[w] += p; // sum of activated neighbours
				if (weight[w] >= threshold[w]) { // activate successfully
					que.push(w);
					visit[w] = 1;
					influencespread += 1;
				}
			}
		}
	}
	return influencespread;
}

double BasicCRIM::CIC(int u)
{ // compute community based marginal influence spread
	double gain = 0.0; // marginal influence spread
	for (int i = 0; i < comm_num; i++)
		MIS[1][i] = 0.0;

	for (int tt = 1; tt <= round; tt++) {
		ExtendSeedsIC(u);
		for (int j = 0; j < comm_num; j++) {
			if (comm_update[j]) {
				double influencespread = IC(1, j);
				double delta = influencespread - IS[j];
				//delta = max(0.0, delta);
				gain += delta;
				MIS[1][j] += delta;
			}
		}
	}

	for (int i = 0; i < comm_num; i++)
		MIS[1][i] /= round;

	return gain / round;
}

double BasicCRIM::CLT(int u)
{ // compute community based marginal influence spread
	double gain = 0.0; // marginal influence spread
	for (int i = 0; i < comm_num; i++)
		MIS[1][i] = 0.0;

	for (int tt = 1; tt <= round; tt++) {
		ExtendSeedsLT(u);
		for (int j = 0; j < comm_num; j++) {
			if (comm_update[j]) {
				double influencespread = LT(1, j);
				double delta = influencespread - IS[j];
				//delta = max(0.0, delta);
				gain += delta;
				MIS[1][j] += delta;
			}
		}
	}

	for (int i = 0; i < comm_num; i++)
		MIS[1][i] /= round;

	return gain / round;
}

void BasicCRIM::ExtendSeedsIC(int u)
{
	queue< int > que;
	while (!que.empty()) que.pop();

	visit[u] = 0;
	for (int i = 0; i < (int)G[u].size(); i++) {
		int w = G[u][i].neighbournode;
		visit[w] = 0;
	}
	for (int i = 0; i < comm_num; i++) {
		H[i].clear();
		comm_update[i] = 0; // initially, each community is not influenced
		for (int j = 0; j < (int)commseed[i].size(); j++) {
			int v = commseed[i][j];
			visit[v] = 1;
			H[i].push_back(v);
			for (int jj = 0; jj < (int)G[v].size(); jj++) {
				int w = G[v][jj].neighbournode; // neighbour
				if (!isSeed[w]) visit[w] = 0;
			}
			que.push(v);
		}
	}

	while (!que.empty()) {
		int curNode = que.front();
		que.pop();
		for (int j = 0; j < (int)G[curNode].size(); j++) {
			int w = G[curNode][j].neighbournode; // neighbour
			double p = G[curNode][j].probability;
			if ((comm[w] != comm[curNode]) && (!visit[w])) {
				double pw = dis(RDM);
				if (pw <= p) { // activate successfully (extended)
					visit[w] = 1;
					H[comm[w]].push_back(w);
				}
			}
		}
	}
	if (!visit[u]) {
		H[comm[u]].push_back(u);
		comm_update[comm[u]] = 1;
	} else
		comm_update[comm[u]] = 0;
	if (hub[u]) {
		for (int i = 0; i < (int)G[u].size(); i++) {
			int w = G[u][i].neighbournode;
			double p = G[u][i].probability;
			if ((comm[w] != comm[u]) && (!visit[w])) {
				double pw = dis(RDM);
				if (pw <= p) { // activate successfully (extended)
					visit[w] = 1;
					H[comm[w]].push_back(w);
					comm_update[comm[w]] = 1;
				}
			}
		}
	}
}

void BasicCRIM::ExtendSeedsLT(int u)
{
	for (int i = 0; i < node_num; i++) {
		threshold[i] = dis(RDM); // reset activation threshold
		weight[i] = 0.0;
		visit[i] = 0;
	}

	queue< int > que;
	while (!que.empty()) que.pop();

	visit[u] = 2;
	weight2[u] = 0.0;
	for (int i = 0; i < (int)G[u].size(); i++) {
		int w = G[u][i].neighbournode;
		visit[w] = 2; // mark u's neighbour as 2
		weight2[w] = 0.0;
	}
	for (int i = 0; i < comm_num; i++) {
		H[i].clear();
		comm_update[i] = 0; // initially, each community is not influenced
		for (int j = 0; j < (int)commseed[i].size(); j++) {
			int v = commseed[i][j];
			visit[v] = 1;
			H[i].push_back(v);
			que.push(v);
		}
	}

	while (!que.empty()) {
		int curNode = que.front();
		que.pop();
		for (int j = 0; j < (int)G[curNode].size(); j++) {
			int w = G[curNode][j].neighbournode; // neighbour
			double p = G[curNode][j].probability;
			if ((comm[w] != comm[curNode]) && (visit[w] != 1)) {
				weight[w] += p;
				if (weight[w] >= threshold[w]) { // activate successfully (extended)
					visit[w] = 1;
					H[comm[w]].push_back(w);
				}
			}
			else if (visit[w] == 2) {
				weight2[w] += p;
			}
		}
	}
	if (weight[u] + weight2[u] >= threshold[u]) visit[u] = 1;
	if (visit[u] != 1) {
		visit[u] = 1;
		H[comm[u]].push_back(u);
		comm_update[comm[u]] = 1;
	}
	else
		comm_update[comm[u]] = 0;
	if (hub[u]) {
		for (int i = 0; i < (int)G[u].size(); i++) {
			int w = G[u][i].neighbournode;
			double p = G[u][i].probability;
			if ((comm[w] != comm[u]) && (visit[w] == 2)) {
				weight[w] += p;
				if ((weight[w] + weight2[w] - p < threshold[w]) && (weight[w] + weight2[w] >= threshold[w])) { // activate successfully (extended)
					visit[w] = 1;
					H[comm[w]].push_back(w);
					comm_update[comm[w]] = 1;
				}
				else
					visit[w] = 0;
			}
			else if (visit[w] != 1)
				visit[w] = 0;
		}
	}
}

void BasicCRIM::output(string path, string M, int k, double timecost, double influencespread)
{
	path = path + M + "-BCRIM-results.txt";
	ofstream ofile;
	ofile.open(path, ios::out | ios::app);
	if (ofile.good()) cout << "success" << endl;
	else cout << "fail" << endl;

	ofile << setiosflags(ios::fixed);
	ofile << path << endl;
	ofile << "size: " << k << endl;
	ofile << "time: " << setprecision(6) << timecost << " " << "community influence: " << influencespread << endl;
	for (int i = 0; i < (int)seedset.size(); i++)
		ofile << seedset[i] << " ";
	ofile << endl;
	ofile << "=============================================" << endl;
	ofile.close();
}

void BasicCRIM::outputIS(string path, string M, int k, double influencespread, double hubratio)
{
	path = path + M + "-BCRIM-results.txt";
	ofstream ofile;
	ofile.open(path, ios::out | ios::app);
	if (ofile.good()) cout << "success" << endl;
	else cout << "fail" << endl;

	ofile << setiosflags(ios::fixed);
	ofile << "hub ratio: " << hubratio << endl;
	ofile << "k: " << k << setprecision(8) << "        influence spread: " << influencespread << endl;
	ofile << "*******************************************************" << endl;
	if (k == seed_size)
		ofile << endl;
	ofile.close();
}

void BasicCRIM::influence_maximization(string path, int n, int k, int t, int c, string M)
{
	if ((M.compare("IC") != 0) && (M.compare("LT") != 0)) {
		cout << "Wrong diffusion model!" << endl;
		return;
	}

	initialization(t, k, n, c, M);
	load_graph(path, M);

	int hubs = 0;
	for (int i = 0; i < node_num; i++)
		if (hub[i]) hubs++;
	cout << "hubs: " << hubs << " hub ratio: " << hubs * 1.0 / node_num << endl;

	clock_t stt, edd, edd2;
	stt = clock();

	double totalinfluence = 0.0;
	for (int i = 1; i <= seed_size; i++) {
		int prenode = -1;
		double preinfluence = -1.0 * n;
		for (int j = 0; j < comm_num; j++) MIS[0][j] = MIS[1][j] = 0.0;
		for (int j = 0; j < node_num; j++) {
			int v = node[j];
			//exist and not included in seed set
			if ((v != -1) && (!isSeed[v])) {
				double sv = 0.0;
				if (M.compare("IC") == 0) {
					sv = CIC(v);
				}
				else if (M.compare("LT") == 0) {
					sv = CLT(v);
				}
				//cout << v << " " << sv << endl;
				if (sv > preinfluence) {
					prenode = v;
					preinfluence = sv;
					for (int jj = 0; jj < comm_num; jj++) {
						MIS[0][jj] = MIS[1][jj];
					}
				}
			}
		}
		if (prenode != -1) {
			seedset.push_back(prenode); // new seed node
			if (preinfluence < 0) cout << "???!" << endl;
			totalinfluence += preinfluence;
			isSeed[prenode] = 1;
			commseed[comm[prenode]].push_back(prenode);
			//cout << i << " " << preinfluence << endl;
			for (int jj = 0; jj < comm_num; jj++) {
				IS[jj] += MIS[0][jj];
			}
		}
		else
			break;
		// record the time cost of finding every five seed nodes
		double dt = 0;
		if ((i == 1) || (i % 5 == 0)) {
			edd2 = clock();
			dt = (double)(edd2 - stt) / CLOCKS_PER_SEC;
			output(path, M, i, dt, totalinfluence);
		}

		if (((dt > 50000) && (i < seed_size / 2)) || (dt >= 100000))
		{
			seed_size = i;
			break;
		}
	}

	edd = clock();
	double dt = (double)(edd - stt) / CLOCKS_PER_SEC;

	output(path, M, seed_size, dt, totalinfluence);

	//int pp[30] = {272,7,169,616,742,451,689,19,855,549,634,498,56,58,402,761,147,379,735,708,117,535,415,14,16,839,49,31,24,840};
	//int pp[30] = {430,169,535,131,780,7,49,616,549,56,761,735,544,229,350,35,87,258,24,708,689,190,265,536,61,28,18,10,36,764};
	//int pp[30] = {398,430,7,549,169,535,272,203,761,198,735,415,561,634,806,56,544,122,838,503,87,855,229,696,97,616,665,355,455,605};
	//for (int i = 0; i < 30; i++)
		//seedset.push_back(pp[i]);

	for (int i = 1; i <= seed_size; i++) {
		if ((i == 1) || (i % 5 == 0) || (i == seed_size)) {
			double ifs = 0.0;
			if (M.compare("IC") == 0) ifs = RandCasIC(i, 100);
			else if (M.compare("LT") == 0) ifs = RandCasLT(i, 100);
			outputIS(path, M, i, ifs, hubs * 1.0 / node_num);
			cout << i << " IS: " << ifs << endl;
		}
	}

	clr();
}

double BasicCRIM::RandCasIC(int k, int t)
{
	double totalinfluencespread = 0.0;

	//default_random_engine random((unsigned int)(time(NULL)));
	//std::uniform_real_distribution<double> dis(0.0, 1.0);

	for (int r = 1; r <= t; r++) {
		queue< int > que;
		while (!que.empty()) que.pop();
		for (int i = 0; i < node_num; i++) {
			visit[i] = 0;
		}

		for (int i = 0; i < k; i++) {
			visit[seedset[i]] = 1;
			que.push(seedset[i]);
		}
		totalinfluencespread += k;

		while (!que.empty()) { //BFS
			int curNode = que.front();
			que.pop();

			for (int j = 0; j < G[curNode].size(); j++) {
				int w = G[curNode][j].neighbournode; // neighbour
				double p = G[curNode][j].probability;
				if (!visit[w]) {
					double pw = dis(RDM);
					if (pw <= p) {//activate successfully
						que.push(w);
						visit[w] = 1;
						totalinfluencespread += 1;
					}
				}
			}
		}
	}

	return totalinfluencespread / t;
}

double BasicCRIM::RandCasLT(int k, int t)
{
	double totalinfluencespread = 0.0;

	for (int r = 1; r <= t; r++) {
		queue< int > que;
		while (!que.empty()) que.pop();
		for (int i = 0; i < node_num; i++) {
			visit[i] = 0;
			weight[i] = 0.0;
			threshold[i] = dis(RDM);
		}

		for (int i = 0; i < k; i++) {
			visit[seedset[i]] = 1;
			que.push(seedset[i]);
		}
		totalinfluencespread += k;

		while (!que.empty()) { //BFS
			int curNode = que.front();
			que.pop();

			for (int j = 0; j < GG[curNode].size(); j++) {
				int w = GG[curNode][j].neighbournode; // neighbour
				double p = GG[curNode][j].probability;
				if (!visit[w]) {
					weight[w] += p;
					if (weight[w] >= threshold[w]) {//activate successfully
						que.push(w);
						visit[w] = 1;
						totalinfluencespread += 1;
					}
				}
			}
		}
	}

	return totalinfluencespread / t;
}

void BasicCRIM::clr()
{
	seedset.clear();
	delete[] hub;
	delete[] node;
	delete[] isSeed;
	delete[] visit;
	delete[] comm;
	delete[] comm_update;
	delete[] threshold;
	delete[] weight;
	delete[] weight2;
	delete[] IS;
	for (int i = 0; i < 2; i++)
		delete[] MIS[i];
	delete[] MIS;
	for (int i = 0; i < node_num + 2; i++) {
		nbr[i].clear();
		G[i].clear();
	}
	for (int i = 0; i < comm_num + 2; i++) {
		commseed[i].clear();
		commset[i].clear();
		H[i].clear();
	}
	delete[] nbr;
	delete[] G;
	delete[] GG;
	delete[] commseed;
	delete[] commset;
	delete[] H;
}
