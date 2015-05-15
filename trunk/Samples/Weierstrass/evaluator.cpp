#include <cmath>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#define OPTION_OUTPUT "--output"
#define OPTION_INPUT "--input"
#define OPTION_ITERATIONS "--iterations"
#define OPTION_REPETITIONS "--repetitions"

#define BIG_NUMBER 100

using namespace std;

inline float weierstrass(vector<float> x, int repetitions, int iterations)
{
	float fRes=0, fSum, b=2, h=0.5;

	for (int j = 0; j < repetitions; j++)
	{
    		fRes = 0;
    		for (int i = 0; i < x.size(); i++) 
		{
      			fSum=0;
      			for (int k = 0; k < iterations; k++) fSum += pow(b,-(float)k*h) * sin(pow(b,(float)k)*x[i]);
      			fRes += abs(fSum);
    		}
  	}
  
	return (fRes);
}

void usage(char* programName)
{
	cout 	<< "Usage: " << programName << " " << OPTION_INPUT << " <input.txt> "
		<< OPTION_OUTPUT << " <output.txt> " << OPTION_ITERATIONS << " <number-of-iterations> "
		<< OPTION_REPETITIONS << " <number-of-repetitions> " << endl
		<< "Defaults:" << endl
		<< "- iterations = 120" << endl
		<< "- repetitions = 10" << endl
		<< "- output = \"fitness.output\"" << endl;
	exit(0);
}

// ugp3 evaluator 
int main(int argc, char* argv[])
{
	// check if this is just a request for help
	if(argc == 1) usage(argv[0]);
			
	// start the program
	string inputFile;
	string outputFile = "fitness.output";
	int repetitions = 10;
	int iterations = 120;	

	// parse the input arguments
	for(unsigned int a = 0; a < argc; a++)
	{
		if( strcmp(argv[a], OPTION_OUTPUT) == 0 )
		{
			outputFile = argv[a+1];
		}
		else if( strcmp(argv[a], OPTION_INPUT) == 0 )
		{
			inputFile = argv[a+1];
		}
		else if( strcmp(argv[a], OPTION_ITERATIONS) == 0 )
		{
			if( sscanf(argv[a+1], "%d", &iterations) != 1 )
				cout << "Option \"" << OPTION_ITERATIONS << "\" specified, but cannot read number of iterations." << endl;
		}
		else if( strcmp(argv[a], OPTION_REPETITIONS) == 0 )
		{
			if( sscanf(argv[a+1], "%d", &repetitions) != 1 )
				cout << "Option \"" << OPTION_REPETITIONS << "\" specified, but cannot read number of repetitions." << endl;

		}
		else if( argv[a][0] == '-' && argv[a][1] == '-' )
		{
			cerr << "Warning: option \"" << argv[a] << "\" not recognized..." << endl;
		}
	}
	
	// check for errors
	if( inputFile.length() == 0 )
	{
		cerr << "Error: input file not provided. Aborting..." << endl;
		exit(0);
	}
	
	// read input file
	vector<float> x;
	string buffer;

	ifstream input( inputFile.c_str() );
	while( input.good() )
	{
		float temp;
		input >> temp;

		x.push_back(temp);
	}
	input.close();
	
	// if there are no values, error!
	if( x.size() == 0 )
	{
		cerr << "Error: input file should have a real value on each line. Aborting..." << endl;
		exit(0);
	}
	
	// obtain fitness
	float result = weierstrass(x, repetitions, iterations);
	float fitness = BIG_NUMBER - result; 
	
	// write fitness to output
	ofstream output( outputFile.c_str() );
	if(output.is_open())
	{
		output << fitness << " #function_value=" << result << "_individual=(";
		for(unsigned int i = 0; i < x.size(); i++)
			if( i != x.size() - 1)
				output << x[i] << ",";
			else
				output << x[i] << ")" << endl;
	}
	output.close();
	
	// return
	return 0;
}
