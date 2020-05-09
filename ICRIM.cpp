//#include "stdafx.h"
#include "ICRIM.h"

std::default_random_engine IRDM((unsigned int)(time(NULL)));
std::uniform_real_distribution<double> idis(0.0, 1.0);

ImpCRIM::ImpCRIM()
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
	TN = NULL;
	TC = NULL;
	commseed = NULL;
	commset = NULL;
	H = NULL;
	nbr = NULL;
	G = NULL;
	GG = NULL;
}

void ImpCRIM::initialization(int t, int k, int n, int c, string M)
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
	MIS = new double[comm_num + 2];
	TN = new int[node_num + 2];
	TC = new int[comm_num + 2];
	commseed = new vector< int >[comm_num + 2];
	commset = new vector< int >[comm_num + 2];
	H = new vector< int >[comm_num + 2];

	nbr = new vector< neighbourI >[node_num + 2];
	G = new vector< neighbourI >[node_num + 2];
	GG = new vector< neighbourI >[node_num + 2];

	for (int i = 0; i < node_num + 2; i++) {
		comm[i] = -1;
		hub[i] = 0;
		node[i] = -1;
		isSeed[i] = 0;
		TN[i] = 0;
	}
	for (int i = 0; i < comm_num + 2; i++) {
		commseed[i].clear();
		commset[i].clear();
		IS[i] = 0;
		TC[i] = 0;
	}
}

void ImpCRIM::load_graph(string path, string M)
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
			neighbourI nb;
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

double ImpCRIM::IC(int t, int cm)
{
	double influencespread = 0.0;
	queue< int > que;
	while (!que.empty()) que.pop();
	for (int i = 0; i < (int)commset[cm].size(); i++) {
		visit[commset[cm][i]] = 0;
	}

	for (int i = 0; i < (int)H[cm].size(); i++) {
		que.push(H[cm][i]);
		visit[H[cm][i]] = 1;
	}
	influencespread += (int)H[cm].size();

	while (!que.empty()) { //BFS
		int curNode = que.front();
		que.pop();

		for (int j = 0; j < nbr[curNode].size(); j++) {
			int w = nbr[curNode][j].neighbournode; // neighbour
			double p = nbr[curNode][j].probability;
			if (!visit[w]) {
				double pw = idis(IRDM);
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

double ImpCRIM::LT(int t, int cm)
{
	double influencespread = 0.0;
	queue< int > que;
	while (!que.empty()) que.pop();
	
	for (int i = 0; i < (int)H[cm].size(); i++) {
		que.push(H[cm][i]);
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

double ImpCRIM::CIC(int u)
{
	double gain = 0.0; // marginal influence spread
	for (int i = 0; i < comm_num; i++)
		MIS[i] = 0.0;

	for (int tt = 1; tt <= round; tt++) {
		ExtendSeedsIC(u);
		for (int j = 0; j < comm_num; j++) {
			if (comm_update[j]) {
				double influencespread = IC(1, j);
				double delta = influencespread - IS[j];
				//delta = max(0.0, delta);
				gain += delta;
				MIS[j] += delta;
			}
		}
	}

	for (int i = 0; i < comm_num; i++)
		MIS[i] /= round;

	return gain / round;
}

double ImpCRIM::CLT(int u)
{
	double gain = 0.0; // marginal influence spread
	for (int i = 0; i < comm_num; i++)
		MIS[i] = 0.0;

	for (int tt = 1; tt <= round; tt++) {
		ExtendSeedsLT(u);
		for (int j = 0; j < comm_num; j++) {
			if (comm_update[j]) {
				double influencespread = LT(1, j);
				double delta = influencespread - IS[j];
				//delta = max(0.0, delta);
				gain += delta;
				MIS[j] += delta;
			}
		}
	}

	for (int i = 0; i < comm_num; i++)
		MIS[i] /= round;

	return gain / round;
}

void ImpCRIM::ExtendSeedsIC(int u)
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
				double pw = idis(IRDM);
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
	}
	else
		comm_update[comm[u]] = 0;
	if (hub[u]) {
		for (int i = 0; i < (int)G[u].size(); i++) {
			int w = G[u][i].neighbournode;
			double p = G[u][i].probability;
			if ((comm[w] != comm[u]) && (!visit[w])) {
				double pw = idis(IRDM);
				if (pw <= p) { // activate successfully (extended)
					visit[w] = 1;
					H[comm[w]].push_back(w);
					comm_update[comm[w]] = 1;
				}
			}
		}
	}
}

void ImpCRIM::ExtendSeedsLT(int u)
{
	for (int i = 0; i < node_num; i++) {
		threshold[i] = idis(IRDM); // reset activation threshold
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

int ImpCRIM::CheckUpdate(int u, int tau)
{
	int f = 0;
	if (tau < TC[comm[u]])
		f = 1;
	else if (hub[u]) {
		for (int i = 0; i < (int)G[u].size(); i++) {
			int v = G[u][i].neighbournode;
			if (tau < TC[comm[v]]) {
				f = 1;
				break;
			}
		}
	}
	return f;
}

void ImpCRIM::output(string path, string M, int k, double timecost, double influencespread)
{
	path = path + M + "-ICRIM-results.txt";
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

void ImpCRIM::outputIS(string path, string M, int k, double influencespread)
{
	path = path + M + "-ICRIM-results.txt";
	ofstream ofile;
	ofile.open(path, ios::out | ios::app);
	if (ofile.good()) cout << "success" << endl;
	else cout << "fail" << endl;

	ofile << setiosflags(ios::fixed);
	ofile << "k: " << k << setprecision(8) << "        influence spread: " << influencespread << endl;
	ofile << "*******************************************************" << endl;
	if (k == seed_size)
		ofile << endl;
	ofile.close();
}

void ImpCRIM::influence_maximization(string path, int n, int k, int t, int c, string M)
{
	if ((M.compare("IC") != 0) && (M.compare("LT") != 0)) {
		cout << "Wrong diffusion model!" << endl;
		return;
	}

	initialization(t, k, n, c, M);
	load_graph(path, M);

	clock_t stt, edd, edd2;
	stt = clock();

	double totalinfluence = 0.0;
	while (!pq.empty()) pq.pop();

	for (int i = 0; i < node_num; i++) {
		int vnode = node[i];
		if (vnode != -1) {
			pnode pd;
			pd.node = vnode;
			pd.ms = 0.0;
			if (M.compare("IC") == 0) {
					pd.ms = CIC(vnode);
			}
			else if (M.compare("LT") == 0) {
				pd.ms = CLT(vnode);
			}
			for (int j = 0; j < comm_num; j++)
				pd.CMIS[j] = MIS[j];
			pq.push(pd);
		}
	}

	int i = 0;
	int flag = 1;
	while (i < seed_size) {
		pnode pnext = pq.top();
		pq.pop();
		int u = pnext.node;
		int tau = TN[u];

		if (CheckUpdate(u, tau)) {
			if (M.compare("IC") == 0) {
				pnext.ms = CIC(u);
			}
			else if (M.compare("LT") == 0) {
				pnext.ms = CLT(u);
			}
			for (int j = 0; j < comm_num; j++)
				pnext.CMIS[j] = MIS[j];
			pq.push(pnext);
			TN[u] = i;
		}
		else {
			seedset.push_back(u); // new seed node
			totalinfluence += pnext.ms;
			isSeed[u] = 1;
			commseed[comm[u]].push_back(u);
			i++;
			for (int jj = 0; jj < comm_num; jj++) {
				IS[jj] += pnext.CMIS[jj];
			}
			TC[comm[u]] = i; //when add u, u's comm is influenced
			if (hub[u]) {
				for (int j = 0; j < (int)G[u].size(); j++) {
					int v = G[u][j].neighbournode;
					TC[comm[v]] = i; // hub seed node's neighbnour comms are influenced
				}
			}
		}

		// record the time cost of finding every five seed nodes
		double dt = 0;
		if ((flag == 1) && ((i == 1) || (i % 5 == 0))) {
			edd2 = clock();
			dt = (double)(edd2 - stt) / CLOCKS_PER_SEC;
			output(path, M, i, dt, totalinfluence);
			flag = 0;
		}
		if (i > 1 && i % 5 != 0) flag = 1;
		if (((dt > 50000) && (i < seed_size / 2)) || (dt >= 100000))
		{
			seed_size = i;
			break;
		}
	}

	edd = clock();
	double dt = (double)(edd - stt) / CLOCKS_PER_SEC;

	output(path, M, seed_size, dt, totalinfluence);


	for (int i = 1; i <= seed_size; i++) {
		if ((i == 1) || (i % 5 == 0) || (i == seed_size)) {
			double ifs = 0.0;
			if (M.compare("IC") == 0) ifs = RandCasIC(i, 100);
			else if (M.compare("LT") == 0) ifs = RandCasLT(i, 100);
			outputIS(path, M, i, ifs);
			cout << i << " IS: " << ifs << endl;
		}
	}

	clr();
}

double ImpCRIM::RandCasIC(int k, int t)
{
	double totalinfluencespread = 0.0;

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
					double pw = idis(IRDM);
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

double ImpCRIM::RandCasLT(int k, int t)
{
	double totalinfluencespread = 0.0;

	for (int r = 1; r <= t; r++) {
		queue< int > que;
		while (!que.empty()) que.pop();
		for (int i = 0; i < node_num; i++) {
			visit[i] = 0;
			weight[i] = 0.0;
			threshold[i] = idis(IRDM);
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

void ImpCRIM::clr()
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
	delete[] MIS;
	delete[] TN;
	delete[] TC;
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
