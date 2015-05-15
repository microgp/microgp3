// Simple set of test functions for CMA-ES
// by Alberto Tonda, 2014 <alberto.tonda@grignon.inra.fr>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define OUTPUT "fitness.out"

// comment/uncomment to turn debugging off/on
//#define DEBUG 1

using namespace std;

double f1(double const *x, int N) 
{ /* function "cigtab" */
	int i; 
	double sum = 1e4*x[0]*x[0] + 1e-4*x[1]*x[1];
	
	for(i = 2; i < N; ++i)  
		sum += x[i]*x[i]; 
	
	return sum;
}

void Tokenize(const string& str, vector<string>& tokens, const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

int main(int argc, char* argv[])
{
	// the only input is a text file with the parameters
	if( argc < 2 )
	{
		cout 	<< "Usage: " << argv[0] << " <parameters.txt>" << endl << endl
			<< "The output file will be written to \"" << OUTPUT << "\"." << endl;
		return 0;
	}

	// open file
	ifstream inputFile( argv[1] );
	
	if( !inputFile.is_open() )
	{
		cerr << "Error: cannot open file \"" << argv[1] << "\". Aborting..." << endl;
		exit(-1);
	}

	// read line
	string buffer;
	getline(inputFile, buffer);
	
	inputFile.close();
	
	// parse string
	vector<string> tokens;
	Tokenize(buffer, tokens, "; \t");

	// allocate memory for double vector
	double* x = (double*)malloc( sizeof(double) * tokens.size() );
	
	// fill vector
	for(unsigned int i = 0; i < tokens.size(); i++)
	{
		if( sscanf(tokens[i].c_str(), "%lf", &x[i]) != 1 )
		{
			cerr 	<< "Error: non-numeric value \"" << tokens[i] 
				<< "\" found in file \"" << argv[1] << "\". Aborting..." << endl;
			free(x);
			exit(-1);
		}
#if defined(DEBUG)
		cout << "x[" << i << "] = " << x[i] << endl;
#endif
	}

	// compute fitness function
	double fitness = f1(x, tokens.size());
#if defined(DEBUG)
	cout << "Fitness is " << fitness << endl;
#endif
	// write fitness to file
	ofstream outputFile(OUTPUT);
	
	if( !outputFile.is_open() )
	{
		cerr << "Error: cannot write on fitness file \"" << OUTPUT << "\". Aborting..." << endl;
		free(x);
		exit(-1);
	}

	outputFile << fitness << endl;
	outputFile.close();

	// free memory
	free(x);
	
	return 0;
}
