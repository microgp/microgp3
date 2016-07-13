/***********************************************************************\
|                                                                       |
| Program.h                                                             |
|                                                                       |
| This file is part of MicroGP v3 (ugp3)                                |
| http://ugp3.sourceforge.net/                                          |
|                                                                       |
| Copyright (c) 2002-2016 Giovanni Squillero                            |
|                                                                       |
|-----------------------------------------------------------------------|
|                                                                       |
| This program is free software; you can redistribute it and/or modify  |
| it under the terms of the GNU General Public License as published by  |
| the Free Software Foundation, either version 3 of the License, or     |
| (at your option) any later version.                                   |
|                                                                       |
| This program is distributed in the hope that it will be useful, but   |
| WITHOUT ANY WARRANTY; without even the implied warranty of            |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      |
| General Public License for more details                               |
|                                                                       |
|***********************************************************************'
| $Revision: 644 $
| $Date: 2015-02-23 14:50:30 +0100 (Mon, 23 Feb 2015) $
\***********************************************************************/

/**
 * @file Program.h
 * Definition of the Program class.
 * @see Program.cc
 * @see Program.Run.cc
 */

#ifndef HEADER_UGP3_FRONTEND_PROGRAM
/** Defines that this file has been included */
#define HEADER_UGP3_FRONTEND_PROGRAM

#include <string>
#include <vector>
#include <exception>
#include <cstdlib>
#include <memory>

#include "Debug.h"
#include "StackTrace.h"
#include "Random.h"
#include "Exception.h"
#include "Convert.h"
#include "File.h"
#include "EvolutionaryCore.h"
#include "Environment.h"
#include "Argument.h"
#include "MicroGPSettings.h"
#include "Info.h"

/**
 * ugp3 namespace
 */
namespace ugp3
{

/**
 * ugp3::frontend namespace
 */
namespace frontend
{

/**
 * @class Program
 * Static class that loads and initiates the application.
 * Allows to load the configuration files and to start the application.
 */
class Program
{
private:
    /** Constructor of the class. It does nothing because the class is static and this method is private, so is not allowed to instantiate the class. */
    Program();
    /** Reads the arguments and loads the configuration consequently. Returns true if exit program is required */
    static bool parseArguments(unsigned int argumentCount, char** arguments);
    /** Initiates the logging handlers based on the configuration established by arguments. */
    static void setupLogger();
    /**Instantiates and adds a new log handler. */
    static void addLogHandler(
        const std::string& fileName, //name of the file where the log is going to be written.
        const std::string& verbosity, //level of details.
        const std::string& format); //format to use.
    /**Shows to the user how to use the arguments of the application. */
    static void displayArgumentsUsage();
    /**Shows the license of the application. */
    static void displayLicense();
    /** Shows operators available in this version */
    static void displayOperators();
    /**Shows the copyright note and a welcome to the user. */
    static void displayStartupMessage();
    /**Shows details about the compilation, platform and version of the application. */
    static void displayVersion();
    /**Registers the genetic operators, loads the evolutionary algorithm, loads the populations and starts the evolution. Continues an old execution if specified. */
    static void run();
    /**Shows the state of an option. */
    static void printOption(const Option& defaultOption);


private: // static fields
    /** File with the ugp settings. */
    static const std::string defaultSettingsFile;
    /** Configuration file of logging. */
    static const std::string defaultLoggingFile;
    /** MicroGPSettings class with all the settings of the application. */
    static MicroGPSettings settings;

public:
    /** 
     * Initialites and starts the algorithm.
     * Initialites the algorithm with the arguments and configuration stablished.
     * @param argumentCount Numbers of arguments.
     * @param arguments Arguments.
     * @see Argument
     */
    static int start(unsigned int argumentCount = 0, char** arguments = nullptr);
    /**Instantiates and registers the operators that can be used in the evolution. 
    ° Made public because it's used by ugp3-extractor as well	
    */
    static void registerOperators();
};

}

}

#endif
