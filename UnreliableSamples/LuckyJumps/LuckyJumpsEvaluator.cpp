#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

int main(int argc, const char* argv[])
{
	ifstream ifile;
	ifile.open(argv[1]);

	if(ifile.is_open() == false)
	{
		cout << "Evaluator: could not open input file " << argv[1] << endl;
		return 1;
	}

	// read the file
	vector<string> cells;
	for(unsigned int index = 0; ifile.eof() == false; index++)
	{
		char buffer[1000];
		ifile.getline(buffer, 1000);
		cells.push_back(buffer);
	}

	// count the money for each jump
	unsigned int totalMoney = 0;
	for(unsigned int index = 0; index < cells.size() ; index++)
	{
		string cell = cells[index];

		int i = cell.find("-> ");
		if (i != string::npos)
		{
			string destination;
			destination.assign(cell, i + 3, cell.size());
			bool find = false;
			for(unsigned int index2 = 0;
				index2 < cells.size() && find == false;
				index2++)
			{
				string cell2 = cells[index2];
				if(cell2.compare(0, destination.size(), destination) == 0)
				{
					find = true;
					int j = cell2.find("$");
					if(j != string::npos)
					{
						string moneyString;
						char* dummy;
						moneyString.assign(cell2, j + 1, cell2.size());
						totalMoney += strtol(moneyString.c_str(), &dummy, 10);
					}
				}
			}
		}
	}

	ifile.close();

	int fitness = totalMoney;
	//cout << "Evaluator: fitness value is: " << fitness << endl;

	ofstream ofile;
	ofile.open("fitness.output");
    ofile << fitness;

	ofile.close();

	return 0;
}

