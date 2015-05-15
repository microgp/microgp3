// Just an example to verify whether the new BORG is working

#include "ConsoleHandler.h"
#include "Constraints.h"
#include "EnhancedPopulation.h"
#include "EvolutionaryCore.h"
#include "EvolutionaryAlgorithm.h"
#include "Individual.h"
#include "Log.h"
#include "Program.h"

#define OP_POPULATION "--population"
#define OP_ASSIMILATE "--assimilate"

#define OUTPUT_FILE ".assimilated.output"

using namespace std;

void printUsage()
{
	cout 	<< endl << "Temporary version of BORG, created to experiment with the Population::Assimilate() function" << endl
		<< "By Alberto Tonda, 2013"
		<< endl << endl
		<< "Usage:" << endl
		<< "./borg-temp "OP_POPULATION" <population xml file> "OP_ASSIMILATE" <file to assimilate>" << endl << endl
		<< "Output file will be written on <file to assimilate>"OUTPUT_FILE << endl << endl;
}

int main (int argc, char* argv[])
{
	// open console for logging
	cout << "borg-temp v.0.01" << endl << "By Alberto Tonda, 2013" << endl;

	cout << "Opening debug log..." << endl;
        ugp3::log::ConsoleHandler* debugHandler = new ugp3::log::ConsoleHandler();
        ::log_.addHandler(*debugHandler); //::log_ is a global instance of a Log class
        debugHandler->setLevel(ugp3::log::Level::Debug);
        //debugHandler->setLevel(ugp3::log::Level::Info);
	
	// we need: a population file, a constraints file, file(s) to assimilate
	string populationFile;
	string constraintsFile;
	string toAssimilateFile;

	// parsing arguments
	LOG_INFO << "Parsing arguments..." << ends;
	for(int a = 0; a < argc; a++)
	{
		if( strcmp(argv[a], OP_POPULATION) == 0 && a+1 < argc)
		{
			populationFile = argv[a+1];
		}
		else if( strcmp(argv[a], OP_ASSIMILATE) == 0 && a+1 < argc)
		{
			toAssimilateFile = argv[a+1];
		}
		else if( argv[a][0] == '-' && argv[a][1] == '-' )
		{
			LOG_WARNING << "Option \"" << argv[a] << "\" not recognized..." << ends;
		}
	}
	
	if( populationFile.length() == 0 || toAssimilateFile.length() == 0 )
	{
		LOG_ERROR << "Both population file AND file to assimilate must be specified on command line." << ends;
		printUsage();
		return -1;
	}

	// another attempt, copying lots of code from Program.Run.cc
	
	// register operators (sigh)
	LOG_INFO << "Registering operators..." << ends;
	ugp3::frontend::Program::registerOperators();

	// pointer to evolutionary algorithm
	LOG_INFO << "Creating evolutionary algorithm..." << ends;
	unique_ptr< ugp3::core::EvolutionaryAlgorithm > algorithm = unique_ptr<ugp3::core::EvolutionaryAlgorithm>( new ugp3::core::EvolutionaryAlgorithm() );	

	// pointer to the population
	LOG_INFO << "Loading population parameters file \"" << populationFile << "\"..." << ends;
	unique_ptr<ugp3::core::Population> population = ugp3::core::Population::fromParametersFile(*algorithm, populationFile);

	// create empty constraints
	LOG_INFO << "Creating empty constraints..." << ends;
	ugp3::constraints::Constraints* constraints = new ugp3::constraints::Constraints();

	// start assimilation!
	LOG_INFO << "Calling assimilate method..." << ends;
	ugp3::core::CandidateSolution* individual = population->assimilate(toAssimilateFile, constraints);
	
	// if the individual is not nullptr, write it to file
	if( individual == nullptr )
	{
		LOG_ERROR << "Assimilated individual is not valid." << ends;
	}
	else
	{
		LOG_INFO << "Writing individual to file..." << ends;
		string outputFile = toAssimilateFile + OUTPUT_FILE;
	}
/*
	// an evolutionary algorithm instance is (unfortunately) needed for this...maybe it could be time to overload the function?
	ugp3::core::EvolutionaryAlgorithm* evolutionaryAlgorithm = new ugp3::core::EvolutionaryAlgorithm();
	
	// create population parameters from file 
	LOG_INFO << "Loading population parameters file \"" << populationFile << "\"..." << ends;
	unique_ptr<ugp3::core::PopulationParameters> populationParameters = ugp3::core::PopulationParameters::fromFile(populationFile); 
	
	// this is terrible programming, but I have been fighting against unique_ptr for too long
	ugp3::core::PopulationParameters* ppPtr = populationParameters.release();
	ugp3::core::EnhancedPopulationParameters* eppPtr = dynamic_cast<ugp3::core::EnhancedPopulationParameters*>(ppPtr);
	
	if( eppPtr == nullptr )
	{
		LOG_ERROR 	<< "There is something wrong with the population parameters, they should be in the EnhancedPopulation format."
				<< ends;
		return -1;
	}
	
	LOG_INFO << "Creating population..." << ends;
	ugp3::core::EnhancedPopulation* population = new ugp3::core::EnhancedPopulation( unique_ptr<ugp3::core::EnhancedPopulationParameters>(eppPtr), *evolutionaryAlgorithm );

	// create empty constraints
	LOG_INFO << "Creating empty constraints..." << ends;
	ugp3::constraints::Constraints* constraints = new ugp3::constraints::Constraints();
	
	// start assimilation!
	LOG_INFO << "Calling assimilate method..." << ends;
	ugp3::core::CandidateSolution* individual = population->assimilate(toAssimilateFile, constraints);
	
	// if the individual is not nullptr, write it to file
	if( individual == nullptr )
	{
		LOG_ERROR << "Assimilated individual is not valid." << ends;
	}
	else
	{
		LOG_INFO << "Writing individual to file..." << ends;
		string outputFile = toAssimilateFile + OUTPUT_FILE;
		
		//(Individual*)individual->toCode( outputFile );
	}
*/
	LOG_INFO << "Done." << ends;
	return 0;
}
