// Main for the CMA-ES wrapper, using ugp3's logging system and tinyxml XML interface
// by Alberto Tonda, 2013 <alberto.tonda@gmail.com>

// CMA-ES C++ wrapper, based on Hansen's code 2007
#include "CMAES.h"

// ugp3 classes (including a wrapper for tinyxml)
#include "Log.h"
#include "ConsoleHandler.h"
#include "FileHandler.h"
#include "BriefFormatter.h"
#include "ExtendedFormatter.h"
#include "PlainFormatter.h"

// standard libraries
#include <iostream>

#include <string.h>

// defines
#define DEBUG 0 

#define OP_CONFIGURATION "--xml"

using namespace std;

void printUsage(char* programName)
{
	cout	<< "Usage:" << endl
		<< programName << " "OP_CONFIGURATION" <configuration-file>"
		<< endl;
}

int main(int argc, char* argv[])
{
	char* programName = argv[0];
	
	// header
	cout 	<< "CMA-ES by Nicolas Hansen (wrapper by Alberto Tonda, 2013)" << endl;

	// if not enough arguments are provided, return
	if( argc < 3 )
	{
		printUsage( programName );
		return 0;
	}
	
	// open consoles to log
        ugp3::log::ConsoleHandler* consoleHandler = new ugp3::log::ConsoleHandler();
        consoleHandler->setLevel(ugp3::log::Level::Info); 
        ::log_.addHandler(*consoleHandler); //::log_ is a global instance of a Log class
	
	if( DEBUG )
	{
		// set level to debug
		consoleHandler->setLevel(ugp3::log::Level::Debug); 
	}

	// parse the arguments
	string configurationFile;
	for(int a = 0; a < argc; a++)
	{
		if( strcmp(argv[a], OP_CONFIGURATION) == 0 && a+1 < argc )
			configurationFile = argv[a+1];
	}
	
	// if the configuration file is not specified, return
	if( configurationFile.length() == 0 )
	{
		// TODO replace with LOG_DEBUG
		LOG_ERROR << " :: Error: configuration file not specified!" << endl;
		printUsage( programName );
		return -1;
	}

	// set up the CMA-ES
	CMAES cmaes;
	
	// read parameters from file
	cmaes.readXml( configurationFile );
	
	// run!
	cmaes.run();

	return 0;
}
