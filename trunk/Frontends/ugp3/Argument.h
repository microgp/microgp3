/***********************************************************************\
|                                                                       |
| Argument.h                                                            |
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
 * @file Argument.h
 * Definition of the Argument class.
 * @see Argument.cc
 */

#ifndef HEADER_UGP3_FRONTEND_ARGUMENT
/** Defines that this file has been included */
#define HEADER_UGP3_FRONTEND_ARGUMENT

#include <string>

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
         * @class Argument
         * Static class that contains a list of all possible commandline string commands.
         * @author Alessandro Salomone 
         * @see ugp3::Program::parseArguments
         */
         // This class is used by the comandline parsing routine (ugp3::Program::parseArguments) to parse the commandline arguments.
        class Argument
        {
        private:
            // The default constructor. It is declared as private so the class cannot be instantiated.
            Argument();

        public:
            /** To specify a file with settings. */
            static const std::string SettingsFile;
            /** To show the license of the program. */
            static const std::string License;
            /** To show the arguments usage or a description of a specified option. */
            static const std::string Help;
            /** To the version information of the application. */
            static const std::string Version;
            /** To sets the level debug in showing information of the program. */
            static const std::string Debug;
            /** To sets the level verbose in showing information of the program. */
            static const std::string Verbose;
            /** To sets the level info in showing information of the program. */
            static const std::string Info;
            /** To sets the level warning in showing information of the program. */
            static const std::string Warning;
            /** To sets the level error in showing information of the program. */
            static const std::string Error;
            /** To sets the level silent in showing information of the program. */
            static const std::string Silent;
            /** To specify a file where write the log output of the program. */
            static const std::string Log;
            /** To specify that the log have to be written the textonly format. */
            static const std::string TextOnly;
            /** To specify that the log have to be written the brief format. */
            static const std::string Brief;
            /** To specify that the log have to be written the extended format. */
            static const std::string Extended;
            /** To print all available operators and their descriptions. */
            static const std::string Operators;

            // recovery
            /** To specify a saved state of an old execution. */
            static const std::string RecoveryInputAlgorithm;
            /** To specify an input population file. */
            static const std::string RecoveryInputPopulation;
            /** To specify a file where save the state of the execution. */
            static const std::string RecoveryOutputAlgorithm;
            /** CHECK BECAUSE IT DOESN'T WORK */
            static const std::string RecoveryOutputOverwrite;
            /** To reevaluate the fitness of the individuals in a recovery. */
            static const std::string RecoveryDiscardFitness;

            // evolution
            /** To specify the seed to use to generate the random numbers. */
            static const std::string RandomSeed;
            /** To speciffy the path where save the statistics of the evolution. */
            static const std::string StatisticsPathName;
            /** To specify a population file to add a population to the evolution. */
            static const std::string Population;
            /** To specify populations to merge before start. */
            static const std::string Merge;

            /** To specify how and when migration of individuals between populations should happen. */
            static const std::string Migration;

            // Tweaks
            static const std::string further;
            /** To enable by default all operators */
            static const std::string AllOperatorsEnabled;
            /** To disable by default all operators */
            static const std::string AllOperatorsDisabled;

            // evaluation
            /** CHECK BECAUSE IT DOESN'T WORK */
            static const std::string FitnessConcurrentEvaluations;
            /** CHECK BECAUSE IT DOESN'T WORK */
            static const std::string FitnessParametersCount;
            /** CHECK BECAUSE IT DOESN'T WORK */
            static const std::string FitnessEvaluator;
            /** CHECK BECAUSE IT DOESN'T WORK */
            static const std::string FitnessEvaluatorInput;
            /** CHECK BECAUSE IT DOESN'T WORK */
            static const std::string FitnessEvaluatorOutput;
            /** CHECK BECAUSE IT DOESN'T WORK */
            static const std::string FitnessRemoveTempFiles;
        };
	}
}


#endif
