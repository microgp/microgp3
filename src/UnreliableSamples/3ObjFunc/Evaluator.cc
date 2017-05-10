#include <iostream>
#include <fstream>
#include <math.h>
using namespace std;

int main(int argc, const char* argv[])
{
	ofstream ofile;
	ofile.open("fitness.output");
	double x, y;

	for(unsigned int indexFile = 1 ; indexFile < argc ; indexFile++)
	{
		ifstream ifile;
		ifile.open(argv[indexFile]);

		if(ifile.is_open() == false)
		{
			cerr << "Evaluator: could not open input file " << argv[indexFile] << endl;
			return 1;
		}

	    ifile >> x;
		ifile >> y;
		    
		ifile.close();

		double fitness1, fitness2, fitness3;

		fitness1 = 1000.0 + (0.5*(pow(x, 2) + pow(y, 2))) + sin(pow(x, 2) + pow(y, 2));
		fitness2 = 1000.0 +  ((pow(3, 2)*x) - 2*y + 4)/8 + ((pow(x, 2) - y + 1)/27) + 15;
		fitness3 = 1000.0 + (1/(pow(x, 2) + pow(y, 2) + 1)) - (1.1*exp(-pow(x, 2) - pow(y, 2)));


		ofile << fitness1 << " " << fitness2 << " " << fitness3 << std::endl;
	}

	ofile.close();

	return 0;
}

