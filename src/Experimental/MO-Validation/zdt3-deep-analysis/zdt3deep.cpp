// ZDT3 test function to validate multi-objective optimization 
// taken from Chase et al. A Benchmark Study of Multi-Objective Optimization Methods

// Modified to create an archive with all evaluated individuals

// by Alberto Tonda, 2014 <alberto.tonda@gmail.com>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/stat.h> // if file exists
#include <vector>

#define OP_MINIMIZE "--minimize"
#define OP_MAXIMIZE "--maximize"
#define OP_INDIVIDUAL "--individual"

#define DEBUG 0
#define OUTPUT_FILE "fitness.output"
#define ARCHIVE_FILE "individuals-archive.csv"
#define UGP3_ENV_GENERATION "UGP3_GENERATION"

using namespace std;

// g(x) function
float g(vector<float> x)
{
	float sum = 0;
	
	for(int i = 1; i < x.size(); i++)
	{
		sum += x[i];
	}
	
	return 1 + (9 * sum) / (x.size() - 1);
}

// usage
void printUsage(char* programName)
{
	cout 	<< "Usage: " << endl
		<< programName
		<< " (" << OP_MAXIMIZE << "|" << OP_MINIMIZE << ") "
		<< OP_INDIVIDUAL << " <individual.name>"
		<< endl
		
		<< "Where:" << endl
		<< OP_MAXIMIZE << " | " << OP_MINIMIZE << " : minimize or maximize the function. Default is maximize." << endl
		<< OP_INDIVIDUAL << " <individual.name> : file containing a set of space-separated floating point values (0,1)." << endl
		<< endl;
}		

// main
int main(int argc, char* argv[])
{
	// parse input
	string fileName;
	bool maximize = true;
	for(int a = 0; a < argc; a++)
	{
		if( strcmp(argv[a], OP_INDIVIDUAL) == 0 )
		{
			fileName = argv[a+1];
		}
		else if( strcmp(argv[a], OP_MINIMIZE) == 0 )
		{
			maximize = false;
		}
		else if( strcmp(argv[a], OP_MAXIMIZE) == 0 )
		{
			//maximize = true; // it's already true by default
		}
		else if( argv[a][0] == '-' && argv[a][1] == '-' )
		{
			cerr << "Warning: unrecognized option \"" << argv[a] << "\" will be ignored!" << endl;
		}
	}

	// check if a file is specified
	if( fileName.length() == 0 )
	{
		cerr << "No file specified. Aborting..." << endl;
		printUsage(argv[0]);
		return 0;
	}

	// open the file, read
	ifstream input( fileName.c_str() );
	
	if( input.is_open() == false )
	{
		cerr << "Cannot read file \"" << fileName << "\". Aborting..." << endl;
		return 0;
	}
	
	// take every element on the first line
	float xi;
	vector<float> x;

	while( input >> xi)
	{
		//if(DEBUG) cout << "Read value #" << x.size() << " = " << xi << endl;
		x.push_back(xi);
	}
	input.close();
	
	// compute the functions
	float objective1 = x[0];
	
	float gValue = g(x);
	float objective2 = gValue * (1.0 - sqrt(x[0] / gValue) - (x[0] / gValue) * sin(10 * M_PI * x[0])   ); 
	
	if(DEBUG) cout << "Objective1 = " << objective1 << "; Objective2 = " << objective2 << endl;

	// write to fitness file
	ofstream output( OUTPUT_FILE );
	
	if( output.is_open() == false )
	{
		cerr << "Cannot write on file \"" << OUTPUT_FILE << "\". Aborting..." << endl;
		return 0;
	}
	
	// since ugp3 is trying to maximize the objectives, we must adjust the output
	if(maximize)
		output << max(0.0, 100.0 - objective1) << " " << max(0.0, 100.0 - objective2) << endl;
	else
		output << objective1 << " " << objective2 << endl;

	output.close();
	
	// write to archive file
	// check if file exists
	struct stat buffer;
	ofstream archiveOutput;
	if( stat(ARCHIVE_FILE, &buffer) != 0 )
	{
		// file does not exist, write header
		archiveOutput.open( ARCHIVE_FILE, std::ofstream::out );
		archiveOutput << "GENERATION,FITNESS1,FITNESS2,X,Y" << endl;	

	}
	else
	{
		// file exists, append
		archiveOutput.open( ARCHIVE_FILE, std::ofstream::app );
	}
	
	// try to read the state variable
	char* pGen = getenv( UGP3_ENV_GENERATION );
	if( pGen != nullptr )
		archiveOutput << pGen << ",";
	else
		archiveOutput << "-1,";
	
	archiveOutput 	<< objective1 << "," << objective2 << endl;
	
	archiveOutput.close();

	return 0;
}	
