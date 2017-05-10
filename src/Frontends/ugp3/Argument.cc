/***********************************************************************\
|                                                                       |
| Argument.cc                                                           |
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
 * @file Argument.cc
 * Implementation of the Argument class.
 * @see Argument.h
 */

#include "ugp3_config.h"
#include "Argument.h"
using namespace ugp3::frontend;
using namespace std;

const string Argument::License = "license";
const string Argument::Help = "help";
const string Argument::Operators = "operators";
const string Argument::Version = "version";

const string Argument::SettingsFile = "settingsFile";
const string Argument::Debug = "debug";
const string Argument::Verbose = "verbose";
const string Argument::Info = "info";
const string Argument::Warning = "warning";
const string Argument::Error = "error";
const string Argument::Silent = "silent"; // MadMax loved it
const string Argument::TextOnly = "text-only";
const string Argument::Brief = "brief";
const string Argument::Extended = "extended";
const string Argument::Log = "log";

// evolution
const string Argument::RandomSeed = "randomSeed";
const string Argument::StatisticsPathName = "statisticsPathName";
const string Argument::Population = "population";
const string Argument::Merge = "merge";
const string Argument::Migration = "migration";

// evaluation
const string Argument::FitnessEvaluator = "evaluatorPathName";
const string Argument::FitnessEvaluatorInput = "evaluatorInputPathName";
const string Argument::FitnessEvaluatorOutput = "evaluatorOutputPathName";
const string Argument::FitnessConcurrentEvaluations = "concurrentEvaluations";
const string Argument::FitnessParametersCount = "parametersCount";
const string Argument::FitnessRemoveTempFiles = "removeTempFiles";

// recovery
const string Argument::RecoveryInputAlgorithm = "recoveryInput";
//const string Argument::RecoveryInputPopulation = "recoveryInputPopulations";
const string Argument::RecoveryOutputAlgorithm = "recoveryOutput";
const string Argument::RecoveryOutputOverwrite = "recoveryOverwriteOutput";
const string Argument::RecoveryDiscardFitness = "recoveryDiscardFitness";

// tweaks
const string Argument::further = "further";
const string Argument::AllOperatorsEnabled = "allOperatorsEnabled";
const string Argument::AllOperatorsDisabled = "allOperatorsDisabled";

Argument::Argument()
{ }

