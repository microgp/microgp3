#include <iostream>
#include <fstream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nk-cpp/NK.h"

#define DEBUG 0

using namespace std;

void usage()
{
	cout 	<< "Usage: ./nk-landscapes --n <n> --k <k> --a <a> --input <input.txt> --output <output.txt> --randomSeed <randomSeed> [--reverse]" 
		<< "\t--n <n>: number of bits, tunes the amplitude of the search space" << endl
		<< "\t--k <k>: tunes the irregularity of the search space; if n is close to k, the landscape is very irregular" << endl
		<< "\t--a <a>: alphabet size; if a==2, it will expect strings of 0s and 1s; if a==3, strings of 0s,1s and 2s" << endl
		<< "\t--input <input.txt>: genome to evaluate, one gene per line (e.g. \"0\\n1\\n1\\n0\\n0\\n...\")" << endl
		<< "\t--output <output.txt>: file to write the fitness to" << endl
		<< "\t--randomSeed <randomSeed>: nk-landscapes are initialized with a seed, every seed gives birth to a different landscape" 
		<< endl
		<< "\t[--reverse]: when analyzing a genome X, this option gives in output (1-fitness(X)); used to switch from minimization to maximization problems" << endl
		<< endl;
}

void exitError(const char *message)
{
	cerr << "Error:: " << message << " Aborting..." << endl;
	usage();
	exit(0);
}

int main(int argc, char* argv[])
{

	string outputFile;
	string inputFile;
	int n = 0, k = 0, a = 0, randomSeed = 0;
	bool reverseFitness = false;

	// parse arguments
	for(int arg = 0; arg < argc; arg++)
	{
		if( strcmp( argv[arg], "--n" ) == 0 )
		{
			if( sscanf(argv[arg+1], "%d", &n) != 1) exitError("value \"n\" not provided.");
		}
		else if( strcmp( argv[arg], "--k" ) == 0 )
		{
			if( sscanf(argv[arg+1], "%d", &k) != 1) exitError("value \"k\" not provided.");
		}
		else if( strcmp( argv[arg], "--a" ) == 0 )
		{
			if( sscanf(argv[arg+1], "%d", &a) != 1) exitError("value \"a\" not provided.");
		}
		else if( strcmp( argv[arg], "--randomSeed" ) == 0 )
		{
			if( sscanf(argv[arg+1], "%d", &randomSeed) != 1) exitError("value \"randomSeed\" specified but not provided.");
		}
		else if( strcmp( argv[arg], "--input" ) == 0 )
		{
			inputFile = argv[arg+1];
		}
		else if( strcmp( argv[arg], "--reverse" ) == 0 )
		{
			reverseFitness = true;
		}
		else if( strcmp( argv[arg], "--output" ) == 0 )
		{
			outputFile = argv[arg+1];
		}
	}

	// check: is everything all right?
	if( n == 0 || k == 0 || a == 0 || outputFile.size() == 0 || inputFile.size() == 0 )
		exitError("one or more parameters not specified.");

	// warning for randomSeed
	if( randomSeed == 0 ) cerr << "Warning:: randomSeed is 0. Specify a new randomSeed with option \"--randomSeed\"" << endl;	

	// debug
	if(DEBUG)
	{
		cout 	<< "n=" << n << endl << "k=" << k << endl << "a=" << a << endl
			<< "input=" << inputFile << endl << "output=" << outputFile << endl;
	}

	// parse input file (expected format: one number on each line, number of lines equal to <n> specified with --n)
	string buffer;
	unsigned int in = 0;
	int *individual = (int *)malloc(sizeof(int) * n);

	ifstream input( inputFile.c_str() );	
	while( input.good() && in < n)
	{
		getline(input, buffer);
		sscanf(buffer.c_str(), "%d", &individual[in]);
		in++;
	}
	input.close();

	if(DEBUG)
	{
		cout << "Individual read is:";
		for(unsigned int i = 0; i < n; i++) cout << " " << individual[i];
		cout << endl;
	}

	// create nk-landscape (default: adjacency)
	NK_Landscape* nklandscape = new NK_Landscape(n, k, NK_Landscape::ADJ, a, randomSeed);	
	
	// evaluate individual
	double fitness = nklandscape->Fitness( individual ); 
	if( reverseFitness == true ) fitness = 1.0 - fitness;
	
	if(DEBUG) cout << "Fitness of individual is " << fitness << endl;

	// write fitness value to output
	ofstream output( outputFile.c_str() );
	if( output.good() ) 
	{
		output << fitness << " #";
		for(unsigned int i = 0; i < n; i++) output << individual[i];
	}	
	output.close();

	// free memory 
	delete( nklandscape );
	free(individual);

	// exit
	exit(0);
}
