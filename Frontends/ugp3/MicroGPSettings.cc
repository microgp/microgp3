/***********************************************************************\
|                                                                       |
| MicroGPSettings.cc                                                    |
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
| the Free Software Foundation, either version 3 of the License, or (at |
| your option) any later version.                                       |
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
 * @file MicroGPSettings.cc
 * Implementation of the MicroGPSettings class.
 * @see MicroGPSettings.h
 * @see Settings.h
 */

#include "ugp3_config.h"

#include <time.h>
#include <string>
#include <sstream>

#include "MicroGPSettings.h"

using namespace ugp3::frontend;
using namespace std;



void MicroGPSettings::initialize()
{
    _STACK;
 	unique_ptr<SettingsContext> context;
 	unique_ptr<Option> option;


    context = unique_ptr<SettingsContext>( new SettingsContext("evolution") );

    context->addOption(Argument::further, "0", "integer");
    context->getOption(Argument::further).setDescription(
        "Run for additional generations.");

    context->addOption(Argument::AllOperatorsEnabled, "false", "boolean");
    context->getOption(Argument::AllOperatorsEnabled).setDescription(
        "Enable all operators by default. WARNING: remember the part of the license agreement about the lack of any kind of warranty?");

    context->addOption(Argument::AllOperatorsDisabled, "false", "boolean");
    context->getOption(Argument::AllOperatorsDisabled).setDescription(
        "Disable all operators by default.");

    context->addOption(Argument::RandomSeed, "0", "integer");
    context->getOption(Argument::RandomSeed).setDescription(
        "Seed for the pseudo-random number generator.");

	context->addOption(Argument::Population, "", "string");
	context->getOption(Argument::Population).setDescription(
	    "File defining population parameters.");

	context->addOption(Argument::StatisticsPathName, "", "string");
	context->getOption(Argument::StatisticsPathName).setDescription(
	    "File where evolution statistics are to be saved.");

    	// Is it true? Indexes or "names"???? (!)20090717gx
    	context->addOption(Argument::Merge, "", "string");
	context->getOption(Argument::Merge).setDescription(
	    "Merges populations. Indexes are specified separated by whitespaces (e.g. '0 1; 1 2' tells to merge population 1 into population 0 and then population 2 in population 1.)");

        //context->addOption(Argument::Migration, "", "string");
        //context->getOption(Argument::Migration).setDescription(
        //    "(optional) Type of migration between multiple populations.");

	this->addContext(std::move(context));

	// recovery
	context = unique_ptr<SettingsContext>( new SettingsContext("recovery") );
	context->addOption(Argument::RecoveryInputAlgorithm, "");
	context->getOption(Argument::RecoveryInputAlgorithm).setDescription(
	    "File containing the state of a previous evolution to restore.\n"
	    "If not set, no evolution is restored.");

    // option removed
    //context->addOption(Argument::RecoveryInputPopulation, "");
	//context->getOption(Argument::RecoveryInputPopulation).setDescription(
	//    "The name of a file containing a population that will be merged with the first one."
	//    "\nIf not set, no population is loaded.");

	context->addOption(Argument::RecoveryOutputAlgorithm, "status.xml");
	context->getOption(Argument::RecoveryOutputAlgorithm).setDescription(
	    "The name of the file where the state of the algorthm is saved at the end of every generation.");

	context->addOption(Argument::RecoveryOutputOverwrite, "true", "boolean");
	context->getOption(Argument::RecoveryOutputOverwrite).setDescription(
	    "When set to true, overwrites the previous state file, otherwise saves it to another file.");

	context->addOption(Argument::RecoveryDiscardFitness, "true", "boolean");
	context->getOption(Argument::RecoveryDiscardFitness).setDescription(
	    "When set to true, discards the fitness contained in the state file and re-evaluates the individuals.");
	this->addContext(std::move(context));

	// logging
    context = unique_ptr<SettingsContext>( new SettingsContext("logging") );
    context->addOption("std::cout", "info; brief");
    context->getOption("std::cout").setDescription(
        "Only essential information on standard output");

    this->addContext(std::move(context));
}

void MicroGPSettings::initializeSettingsFile()
{
    _STACK;
 	unique_ptr<SettingsContext> context;
 	unique_ptr<Option> option;

 	context = unique_ptr<SettingsContext>( new SettingsContext("evolution") );

	context->addOption(Argument::RandomSeed, "0", "integer");
	context->getOption(Argument::RandomSeed).setDescription(
	    "The seed for the pseudo-random number generator.");

	context->addOption(Argument::Population, "population.settings.xml;population2.settings.xml", "string");
	context->addOption("populationNames", "population1;population2", "string");
	context->getOption(Argument::Population).setDescription(
	    "For each population, its name and the file where the population parameters are defined.");

	context->addOption(Argument::StatisticsPathName, "statistics.csv", "string");
	context->getOption(Argument::StatisticsPathName).setDescription(
	    "The file where the statistics on the evolution are saved.");

    context->addOption(Argument::Merge, "", "string");
	context->getOption(Argument::Merge).setDescription(
	    "Merges two populations together specifying their indexes separated by whitespaces (e.g. '0 1; 1 2' tells to merge population 1 into population 0 and then population 2 in population 1.)");

	this->addContext(std::move(context));

	// recovery
	context = unique_ptr<SettingsContext>( new SettingsContext("recovery") );
	context->addOption(Argument::RecoveryInputAlgorithm, "");
	context->getOption(Argument::RecoveryInputAlgorithm).setDescription(
	    "The name of the file containing a previous evolution to restore.\n"
	    "If not set, no evolution is restored.");

    // removed option
	//context->addOption(Argument::RecoveryInputPopulation, "");
	//context->getOption(Argument::RecoveryInputPopulation).setDescription(
	//    "The name of a file containing a population that will be merged with the first one."
	//    "\nIf not set, no population is loaded.");

	context->addOption(Argument::RecoveryOutputAlgorithm, "status.xml");
	context->getOption(Argument::RecoveryOutputAlgorithm).setDescription(
	    "The name of the file where the state of the algorthm is saved at the end of every generation.");

	context->addOption(Argument::RecoveryOutputOverwrite, "true", "boolean");
	context->getOption(Argument::RecoveryOutputOverwrite).setDescription(
	    "When set to true, overwrites the previous state file, otherwise saves it to another file.");

	context->addOption(Argument::RecoveryDiscardFitness, "true", "boolean");
	context->getOption(Argument::RecoveryDiscardFitness).setDescription(
	    "When set to true, discards the fitness contained in the state file and re-evaluates the individuals.");
	this->addContext(std::move(context));

	// logging
    context = unique_ptr<SettingsContext>( new SettingsContext("logging") );
    context->addOption("std::cout", "info; brief");
    context->getOption("std::cout").setDescription(
        "Only essential information on standard output");
    context->addOption("verbose.log", "verbose; brief");
    context->getOption("verbose.log").setDescription(
        "An extensive report will be written in a file called 'verbose.log'");
    context->addOption("debug.log", "debug; brief");
    context->getOption("debug.log").setDescription(
        "Debug information, stored in 'debug.log'");

    this->addContext(std::move(context));
}
