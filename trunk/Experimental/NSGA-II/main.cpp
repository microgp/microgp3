// SHARK-like NSGA-II with ugp3-like XML interface
// by Alberto Tonda, 2013 <alberto.tonda@gmail.com>

// so, the basic idea is first to read the XML configuration file using tinyxml
// parse the meaningful information, store it and call NSGA-II

// standard includes
#include <iostream>

// ugp3 includes
#include "Log.h"
#include "ConsoleHandler.h"
#include "File.h"
#include "FileHandler.h"
#include "BriefFormatter.h"
#include "ExtendedFormatter.h"
#include "PlainFormatter.h"

// SHARK includes
#include <Array/Array.h>
#include <FileUtil/Params.h>
#include <MOO-EALib/PopulationMOO.h>
#include <MOO-EALib/ArchiveMOO.h>

// my includes
#include "NSGA2Wrapper.h"

// defines
#define OP_HELP "--help"
#define OP_MAXIMIZE "--maximize"
#define OP_OUTPUT "--output"
#define OP_POPULATION "--population"
#define OP_SAVE_POPULATION "--save-population"
#define OP_SEED "--seed"
#define SETTINGS_FILE "ugp3.settings.xml"

// unfortunately, it is more convenient to declare the console handler as a global variable
ugp3::log::ConsoleHandler* consoleHandler = nullptr;

using namespace std;
using namespace ugp3;
	
// usage
void printUsage(char* programName)
{
	LOG_INFO 	<< "Usage:" << ends;
	LOG_INFO	<< programName << " "OP_POPULATION" <ugp3.population.settings.xml>" << ends;
	LOG_INFO	<< "Options:" << ends;
	LOG_INFO	<< OP_POPULATION" <ugp3.population.settings.xml> : population's settings file, in ugp3 XML format. If no file is specified by command line, a file named \"ugp3.settings.xml\" will be parsed for a population's settings name." << ends;
	LOG_INFO 	<< OP_SEED" <seed> : initial random seed." << ends;
	LOG_INFO 	<< OP_SAVE_POPULATION" <population.file.csv> : CSV file with all the population. If no file is specified, only the last population is saved." << ends;
	
}

// main
int main(int argc, char* argv[])
{
	// initialize console
        consoleHandler = new ugp3::log::ConsoleHandler();
        //consoleHandler->setLevel(ugp3::log::Level::Debug); // TODO change to ugp3::log::Level::Debug for debugging
        consoleHandler->setLevel(ugp3::log::Level::Info); 
        ::log_.addHandler(*consoleHandler); //::log_ is a global instance of a Log class

	// print something?
	cout 	<< "Wrapper for the NSGA-II implementation of the Shark library, with ugp3 XML interface." 
		<< endl
		<< "By Alberto Tonda, 2013 <alberto.tonda@gmail.com>"
		<< endl;

	// parse the command line and get the options
	unsigned seed = 0;
	bool minimize = true;
	string outputFile = "nsga2-output.csv", populationFile, savePopulationFile;

	for(int a = 0; a < argc; a++)
	{
		if( strcmp(OP_POPULATION, argv[a]) == 0 )
		{
			populationFile = argv[a+1];
		}
		else if( strcmp(OP_OUTPUT, argv[a]) == 0 )
		{
			outputFile = argv[a+1];
		}
		else if( strcmp(OP_SEED, argv[a]) == 0 )
		{
			sscanf(argv[a+1], "%u", &seed);
		}
		else if( strcmp(OP_SAVE_POPULATION, argv[a]) == 0 )
		{
			savePopulationFile = argv[a+1];
		}
		else if( strcmp(OP_MAXIMIZE, argv[a]) == 0 )
		{
			minimize = false;
		}
		else if( strcmp(OP_HELP, argv[a]) == 0 )
		{
			printUsage(argv[0]);
			return 0;
		}
		else if( argv[a][0] == '-' && argv[a][1] == '-' )
		{
			LOG_WARNING << "Option \"" << argv[a] << "\" not recognized." << ends;
		}
	}
		
	// TODO if no population is specified, read the ugp3.settings.xml configuration file and extract the name of the population
	if(populationFile.size() == 0)
	{
		if( ugp3::File::exists( SETTINGS_FILE ) )
		{
			LOG_INFO << "Settings file \""SETTINGS_FILE"\" exists. Searching for population name..." << ends;
			// read the file with the tinyxml wrapper
			xml::Document xmlSettings;
			xmlSettings.LoadFile( SETTINGS_FILE );
			
			// find significant parts here
			xml::Element* populationElement = xmlSettings.RootElement()->FirstChildElement();
			// search for a context with name="evolution"
			bool found = false;
			while( !found && populationElement != nullptr )
			{
				const char* name = populationElement->Attribute("name");
				if( strcmp(name, "evolution") == 0 )
				{
					LOG_DEBUG << "Context \"evolution\" found!" << ends;
					xml::Element* optionElement = populationElement->FirstChildElement("option");
					while( optionElement != nullptr && strcmp(optionElement->Attribute("name"), "populations") != 0)
					{
						optionElement = optionElement->NextSiblingElement("option");
					}
			
					xml::Element* populationFileElement = nullptr;
					if( optionElement != nullptr )
						populationFileElement = optionElement->FirstChildElement("population");

					if( populationFileElement != nullptr )
					{
						LOG_DEBUG << "Element \"population\" found!" << ends;
						const char* populationFileFromXML = populationFileElement->Attribute("value");
						if( populationFileFromXML != nullptr )
						{
							found = true;
							populationFile = populationFileFromXML;
							LOG_DEBUG << "Population file is \"" << populationFile << "\"." << ends;
						}
					}
				}
				// on to the next element at the same level
				populationElement = populationElement->NextSiblingElement();
			}
		}
	}

	// if no population file is found, die
	if(populationFile.size() == 0)
	{
		LOG_ERROR << "No population file specified. Aborting..." << ends;
		printUsage(argv[0]);
		return 0;
	}

	// create instance of NSGA2Wrapper, by reading the file
	NSGA2Wrapper nsga2( populationFile );

	// set seed
	nsga2.seed = seed;
	
	// if the population must be saved, do it!
	nsga2.savePopulationFile = savePopulationFile;
	
	// run the optimization process
	nsga2.run( minimize );
	
	// save the final result
	nsga2.populationToCSV( outputFile );

	// finally, exit the program
	return 0;
}
