// CRIM.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "BCRIM.h"
#include "ICRIM.h"

void Basic_CRIM(string path, int n, int k, int t, int c, string M) {
	BasicCRIM *app = new BasicCRIM();
	app->influence_maximization(path, n, k, t, c, M);
	delete app;
	app = NULL;
}

void Imp_CRIM(string path, int n, int k, int t, int c, string M) {
	ImpCRIM *app = new ImpCRIM();
	app->influence_maximization(path, n, k, t, c, M);
	delete app;
	app = NULL;
}

int main()
{ 
	string path[10];
	int nodenum[10];
	int c[10];
	
	string model = "LT"; // IC or LT
	int k = 30;
	
	path[0] = "Epinions/";
	nodenum[0] = 75888;
	c[0] = 20;

	// for LT model 
	Imp_CRIM(path[0], nodenum[0], k, 100, c[0], model);
	Basic_CRIM(path[0], nodenum[0], k, 100, c[0], model);
	
        return 0;
}

