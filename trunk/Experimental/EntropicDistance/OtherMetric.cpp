#include "OtherMetric.h"

#include <sstream>

#include "EvolutionaryCore.h"

using namespace std;

// constructor
OtherMetric::OtherMetric()
{
	// TODO modify here the name of the problem-specific distance
	this->name = "HammingDistance";
}

// compute distance
double OtherMetric::compute(const ugp3::core::EnhancedIndividual* individual1, const ugp3::core::EnhancedIndividual* individual2)
{
	// write individuals in string form, and then try to parse them following the problem-specific knowledge
	ostringstream ss1, ss2;
	individual1->getGraphContainer().writeExternalRepresentation(ss1);
	individual2->getGraphContainer().writeExternalRepresentation(ss2);
	
	string phenotype1 = ss1.str();
	string phenotype2 = ss2.str();
	
	// TODO modify here following the problem-specific distance
	double distance = 0.0;
	
	// for the Hamming distance, tokenize the two strings, then +1 distance for each different bit in the same position
	// in this case, there is no separator ("1010001011"), so the two strings can be used as they are
	/*
	vector<string> tokens1;
	vector<string> tokens2;
	
	this->getTokens(phenotype1, tokens1, " \n\t,");
	this->getTokens(phenotype2, tokens2, " \n\t,");
	*/	

	for(int i = 0; i < phenotype1.length() && i < phenotype2.length(); i++)
	{
		// debug
		//cout 	<< "individual1[" << i << "] = \"" << phenotype1[i] 
		//	<< "\"; individual2[" << i << "] = \"" << phenotype2[i] << "\"" << endl;

		// if two corresponding bits are different, distance +1
		if( phenotype1[i] != phenotype2[i] ) distance += 1;
	}

	// TODO end of problem-specific part

	return distance;
}

// text utility
void OtherMetric::getTokens(const string& str, vector<string>& tokens, const string& delimiters)
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
