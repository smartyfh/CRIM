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
	string model = "LT";
	int k = 30;
	//int t = 50;
	
	path[0] = "WikiVote/";

	
	nodenum[0] = 889;
	c[0] = 8;

	nodenum[1] = 4039;
	c[1] = 10;

	nodenum[2] = 75888;
    c[2] = 20;

    nodenum[3] = 425957;
    c[3] = 40;

    nodenum[4] = 1134890;
    c[4] = 50;

    nodenum[5] = 1191805;
    c[5] = 50;

    nodenum[6] = 154908;
    c[6] = 30;

    // IC model
	//Basic_CRIM(path[0], nodenum[0], k, 100, c[0], model);
	//Basic_CRIM(path[1], nodenum[1], k, 100, c[1], model);
    
    //Basic_CRIM(path[2], nodenum[2], k, 50, c[2], model);
	//Basic_CRIM(path[3], nodenum[3], k, 50, c[3], model);

	//Basic_CRIM(path[6], nodenum[6], k, 50, c[6], model);
    
    //Basic_CRIM(path[4], nodenum[4], k, 25, c[4], model);
	//Basic_CRIM(path[5], nodenum[5], k, 25, c[5], model);

    
    //Imp_CRIM(path[0], nodenum[0], k, 100, c[0], model);
	//Imp_CRIM(path[1], nodenum[1], k, 100, c[1], model);
    
    //Imp_CRIM(path[2], nodenum[2], k, 100, c[2], model);
	//Imp_CRIM(path[3], nodenum[3], k, 100, c[3], model);

	//Imp_CRIM(path[6], nodenum[6], k, 100, c[6], model);
    
    //Imp_CRIM(path[4], nodenum[4], k, 100, c[4], model);
	//Imp_CRIM(path[5], nodenum[5], k, 100, c[5], model);


	// for LT model 
	//Imp_CRIM(path[0], nodenum[0], k, 100, c[0], model);
	//Imp_CRIM(path[1], nodenum[1], k, 100, c[1], model);
    
    //Imp_CRIM(path[2], nodenum[2], k, 100, c[2], model);
	//Imp_CRIM(path[3], nodenum[3], k, 100, c[3], model);

	//Imp_CRIM(path[6], nodenum[6], k, 100, c[6], model);

	//Imp_CRIM(path[4], nodenum[4], k, 20, c[4], model);
	//Imp_CRIM(path[5], nodenum[5], k, 20, c[5], model);


	//Basic_CRIM(path[0], nodenum[0], k, 100, c[0], model);
	//Basic_CRIM(path[1], nodenum[1], k, 100, c[1], model);
    
    Basic_CRIM(path[2], nodenum[2], k, 50, c[2], model);
	//Basic_CRIM(path[3], nodenum[3], k, 50, c[3], model);

	//Basic_CRIM(path[6], nodenum[6], k, 50, c[6], model);
     
    //Basic_CRIM(path[4], nodenum[4], k, 10, c[4], model);
	//Basic_CRIM(path[5], nodenum[5], k, 10, c[5], model);
	
    return 0;
}

