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
// what should ugp3 do:
// - select parent(s), using the internal selector
// Already done by the caller (apply())

// - write parent(s) to file
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
	// TODO: 	find a way to obtain a list of all files with a certain pattern, with a new method for ugp3::File;
	//		the method File::exists does not work with "name*"
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
	int returnValue = system( scriptCommandLine.c_str() );
	
	LOG_DEBUG 	<< this->getName() << ": local search process \"" << scriptCommandLine 
#include <vector>
			<< "\" terminated with return value " << returnValue << ends;
	
// - read output file(s) and assimilate them in ugp3
	// first, get the list of files that are coherent with the output pattern
	vector<string> offspringNames = File::getList( this->outputPattern + "*" );
	
	if( offspringNames.size() == 0 )
	{
		LOG_WARNING << "No individuals produced by the local search process \"" << scriptCommandLine << "\"..." << ends;
	}
	
	// TODO: write the assimilation function
	for(unsigned int i = 0; i < offspringNames.size(); i++)
	{
			/* it does not work, because the copy constructor fro Constraints is declared as private...why?
			const ugp3::constraints::Constraints& constraints = populationParameters->getConstraints();
			Individual* child = population->assimilate( offspringNames[i], &constraints );
			outChildren.push_back( child );
			*/
	}
	
// - delete parent file(s) and output file(s)
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
