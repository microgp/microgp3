#include <vector>

#include "ugp3_config.h"
#include "EvolutionaryCore.h"
#include "Operators/LocalSearch.h"

using namespace std;
using namespace ugp3::core;

const std::string LocalSearchOperator::XML_NAME = "localSearch";
const std::string LocalSearchOperator::XML_ATTRIBUTE_NUMBEROFPARENTS = "numberOfParents"; 
const std::string LocalSearchOperator::XML_ATTRIBUTE_EXECUTABLE = "executable"; 
const std::string LocalSearchOperator::XML_ATTRIBUTE_OUTPUT = "outputPathName"; 

const std::string LocalSearchOperator::PARENT_FILENAME = "localSearchParent";
const std::string LocalSearchOperator::OFFSPRING_FILENAME = "localSearchOffspring";

// Methods for the LocalSearch class
LocalSearchOperator::LocalSearchOperator() : 
executable(""),
outputPattern(""),
parentsCardinality(0)
{ }

LocalSearchOperator::~LocalSearchOperator()
{ }

inline void LocalSearchOperator::writeXml(std::ostream& output) const
{

}

inline void LocalSearchOperator::readXml(const xml::Element& element)
{
// XML options for this operator include
// - name of the executable to be called
// - number of parents
// - name of the output files to assimilate (with "output-individual%s.in" if more than one is produced)
	_STACK;
	
	this->parentsCardinality = xml::Utility::attributeValueToUInt(element, XML_ATTRIBUTE_NUMBEROFPARENTS);
	this->executable = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_EXECUTABLE);
	this->outputPattern = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_OUTPUT);
	
	LOG_DEBUG 	<< "Reading properties for operator \"" << this->getName() << "\": "
			<< XML_ATTRIBUTE_NUMBEROFPARENTS << " = \"" << this->parentsCardinality << "\","
			<< XML_ATTRIBUTE_EXECUTABLE << " = \"" << this->executable << "\","
			<< XML_ATTRIBUTE_OUTPUT << " = \"" << this->outputPattern << "\"." << ends;
}

void LocalSearchOperator::generate(
    const std::vector< Individual* >& parents,
    std::vector< Individual* >& outChildren,
    IndividualPopulation& population) const
{
	vector<string> parentNames;
    
	// Generated files to be removed
	vector<string> outfiles;

	LOG_DEBUG << this->getName() << ": writing " << parents.size() << " parents individual to file." << ends;
	for(unsigned int i = 0; i < parents.size(); i++)
	{
		ostringstream fileName;
		fileName << this->PARENT_FILENAME << i;
		parentNames.push_back( fileName.str() );

		LOG_DEBUG << this->getName() << ": writing parent " << parents[i]->getId() << " as " << parentNames[i] << ends;
		parents[i]->toCode(parentNames[i]);
	}

// - delete previously produced individual files
	// TODO: check whether the lines below actually work	
	vector<string> existingOffspring = File::getList( this->outputPattern + "*" );
	for(unsigned int i = 0; i < existingOffspring.size(); i++)
	{
		LOG_DEBUG << "Removing previously generated offspring file \"" << existingOffspring[i] << "\"..." << ends;
		//File::remove( existingOffspring[i] );
	}

// - call program
	// add all parent files
	string scriptCommandLine = "";
	for(unsigned int i = 0; i < parentNames.size(); i++)
	{
		scriptCommandLine = scriptCommandLine + parentNames[i] + " ";
	}
	
	// now, add the executable and run the script
	scriptCommandLine = this->executable + " " + scriptCommandLine;
	
	LOG_DEBUG << this->getName() << ": executing command \"" << scriptCommandLine << "\"..." << ends;
	int returnValue = system( scriptCommandLine.c_str() ); // TODO is this good? compare with how the evaluator is called
	
	LOG_DEBUG 	<< this->getName() << ": local search process \"" << scriptCommandLine 
			<< "\" terminated with return value " << returnValue << ends;
	
// - read output file(s) and assimilate them in ugp3
	vector<string> offspringNames;
	if( File::exists( this->outputPattern ) )
	{
		LOG_DEBUG << "Reading individual names..." << ends;
		ifstream file(this->outputPattern);
		string buffer;
		
		while(getline(file, buffer))
		{
			offspringNames.push_back(buffer);
		}
	}
	
	if( offspringNames.size() == 0 )
	{
		LOG_WARNING << "No individuals produced by the local search process \"" << scriptCommandLine << "\"..." << ends;
	}
	
	// call the assimilation function
	for(unsigned int i = 0; i < offspringNames.size(); i++)
	{
		ugp3::constraints::Constraints* constraints = new ugp3::constraints::Constraints(); 
		CandidateSolution* child = population.assimilate( offspringNames[i], constraints );
		if( dynamic_cast<Individual*>( child ) != nullptr )
		{
			outChildren.push_back( (Individual*) child );
		}
	}
	
// - delete parent file(s) and output file(s)
	// TODO actually implement this part
#if 0 
    // FIXME BUG This operator tends to CLEAR THE CURRENT DIRECTORY. Very naughty.
	for(unsigned int i = 0; i < parentNames.size(); i++)
	{
		File::remove( parentNames[i] );
	}
	
	for(unsigned int i = 0; i < offspringNames.size(); i++)
	{
		LOG_DEBUG << "I should remove file \"" << offspringNames[i] << "\"..." << ends;
		File::remove( offspringNames[i] );
	}
#endif
}

unsigned int LocalSearchOperator::getParentsCardinality() const
{
	return this->parentsCardinality;
}

const string LocalSearchOperator::getName() const
{
	return "localSearch";
}

const string LocalSearchOperator::getAcronym() const
{
	return "LS";
}

const string LocalSearchOperator::getDescription() const
{
	return "" + this->getName() + " IS NOT WORKING YET.";
}
