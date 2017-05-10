/***********************************************************************\
|                                                                       |
| EvolutionaryCore.h                                                    |
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
 * @file EvolutionaryCore.h
 * File to include all the files of the EvolutionaryCore library.
 */

#ifndef HEADER_UGP3_CORE_EVOLUTIONARYCORE
#define HEADER_UGP3_CORE_EVOLUTIONARYCORE

// standard headers
#include <list>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <memory>

#include <sys/stat.h>

// headers from other modules
#include "Environment.h"
#include "Constraints.h"
#include "ConstrainedTaggedGraph.h"
#include "IComparable.h"
#include "File.h"
#include "Convert.h"
#include "XMLIFace.h"
#include "Utility.h"
#include "Exceptions/ArgumentException.h"

// headers of this module
#include "Lineage.h"
#include "Fitness.h"
#include "Operator.h"
#include "Group.h"
#include "GroupOperator.h"
#include "Individual.h"
#include "GeneticOperator.h"
#include "Performance.h"
#include "Data.h"
#include "OperatorSelector.h"
#include "DMABOperatorSelector.h"
#include "PopulationParameters.h"
#include "Statistics.h"
#include "EnhancedPopulationParameters.h"
#include "MOPopulationParameters.h"

#include "ClassicalMigrator.h"
#include "CloneRecord.h"
#include "DeltaEntropy.h"
#include "ScaledFitness.h"
#include "EnhancedIndividual.h"
#include "EnhancedPopulation.h"
#include "Entropy.h"
#include "Evaluator.h"
#include "EvolutionaryAlgorithm.h"
#include "FitnessEvaluator.h"
#include "GEIndividual.h"
#include "GroupFitnessEvaluator.h"
#include "GroupPopulationParameters.h"
#include "GroupPopulation.h"
#include "IMigrator.h"
#include "CandidateSelection.h"
#include "MOFitness.h"
#include "MOIndividual.h"
#include "MOPopulation.h"
#include "OperatorToolbox.h"
#include "Population.h"
#include "RankingSelection.h"
#include "TournamentSelection.h"

// operators
#include "Operators/AllopatricDifferentialOperator.h"
#include "Operators/AlterationMutation.h"
#include "Operators/Crossover.h"
#include "Operators/DifferentialEvolutionOperator.h"
#include "Operators/InsertionMutation.h"
#include "Operators/InverOverCrossover.h"
#include "Operators/LocalSearch.h"
#include "Operators/Mutation.h"
#include "Operators/OnePointCrossover.h"
#include "Operators/OnePointImpreciseCrossover.h"
#include "Operators/RandomizerOperator.h"
#include "Operators/RemovalMutation.h"
#include "Operators/ReplacementMutation.h"
#include "Operators/ScanMutationBitArray.h"
#include "Operators/ScanMutationConstant.h"
#include "Operators/ScanMutationFloat.h"
#include "Operators/ScanMutationInnerLabel.h"
#include "Operators/ScanMutationInteger.h"
#include "Operators/SimpleDifferentialOperator.h"
#include "Operators/SingleParameterAlterationMutation.h"
#include "Operators/SubGraphInsertionMutation.h"
#include "Operators/SubGraphRemovalMutation.h"
#include "Operators/SubGraphReplacementMutation.h"
#include "Operators/SwapMutation.h"
#include "Operators/TwoPointCrossover.h"
#include "Operators/TwoPointImpreciseCrossover.h"
#include "Operators/UniformCrossover.h"

// group operators
#include "Operators/GroupMutation.h"
#include "Operators/GroupCrossover.h"
#include "Operators/GroupExternalInsertionMutation.h"
#include "Operators/GroupRandomInsertionMutation.h"
#include "Operators/GroupRandomRemovalMutation.h"

#endif
