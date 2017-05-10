#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, const char* argv[])
{
	ofstream ofile;
	ofile.open("fitness.output");

	for(unsigned int indexFile = 1 ; indexFile < argc ; indexFile++)
	{
		ifstream ifile;
		ifile.open(argv[indexFile]);

		if(ifile.is_open() == false)
		{
			cerr << "Evaluator: could not open input file " << argv[indexFile] << endl;
			return 1;
		}


		unsigned int ones = 0, zeros = 0;
		bool valid = true;
		while(ifile.eof() == false && valid)
		{
			char* buffer = new char[10*1024*1024];
			ifile.getline(buffer, 10*1024*1024);
			string line = buffer;

			
			for(unsigned int i = 0; i < line.length(); i++)
			{
				if(line.at(i) == '1') ones++;
				else if(line.at(i) == '0') zeros++;
				else if(line.at(i) != '\n' || line.at(i) != '\r')
				{
					cerr << "Evaluator: unexpected character." << endl;
					//return 1;
					ones = 0;
					valid = false;
					break;
				}
			}
			
			delete [] buffer;
		}
	

		ifile.close();
	
		int fitness = ones;

		//cout << "Evaluator: fitness value is: " << fitness << endl;

		ofile << fitness << " good Questo non deve essere memorizzato!@#" << std::endl;
		
	}

   // ofile <<  "#stop" << std::endl;

	ofile.close();

	return 0;
}

