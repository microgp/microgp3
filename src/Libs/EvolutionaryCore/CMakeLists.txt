#############################################################-*-cmake-*-#
#                                                                       #
# CMakeList for libEvolutionaryCore                                     #
#                                                                       #
# This file is part of MicroGP v3 (ugp3)                                #
# http://ugp3.sourceforge.net/                                          #
#                                                                       #
# Copyright (c) 2002-2016 Giovanni Squillero                            #
#                                                                       #
#-----------------------------------------------------------------------#
#                                                                       #
# This program is free software; you can redistribute it and/or modify  #
# it under the terms of the GNU General Public License as published by  #
# the Free Software Foundation, either version 3 of the License, or (at #
# your option) any later version.                                       #
#                                                                       #
# This program is distributed in the hope that it will be useful, but   #
# WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      #
# General Public License for more details                               #
#                                                                       #
#########################################################################
# $Revision: 644 $
# $Date: 2015-02-23 14:50:30 +0100 (Mon, 23 Feb 2015) $
#########################################################################

# ADD_SUBDIRECTORY(../Constraints)
# ADD_SUBDIRECTORY(../Graph)
# ADD_SUBDIRECTORY(../Log)
# ADD_SUBDIRECTORY(../Shared)
# ADD_SUBDIRECTORY(../XmlParser)

ADD_LIBRARY(EvolutionaryCore
  OperatorSelectors/CallData.cc
  OperatorSelectors/DMABData.cc
  OperatorSelectors/DMABOperatorSelector.cc
  OperatorSelectors/PDMABOperatorSelector.cc
  OperatorSelectors/KappaRouletteOperatorSelector.cc
  OperatorSelectors/Data.cc
  OperatorSelectors/OperatorSelector.cc
  OperatorSelectors/Performance.cc
  OperatorSelectors/RandomOperatorSelector.cc
  OperatorSelectors/SAData.cc
  OperatorSelectors/SelfAdaptiveOperatorSelector.cc
  OperatorSelectors/Statistics.cc
  
  CandidateSelection.cc
  CandidateSelection.xml.cc 
  CandidateSolution.cc
  CandidateSolution.xml.cc
  ClassicalMigrator.cc
  DeltaEntropy.cc 
  EnhancedIndividual.cc 
  EnhancedIndividual.xml.cc 
  EnhancedPopulation.cc 
  EnhancedPopulation.xml.cc 
  EnhancedPopulationParameters.cc
  EnhancedPopulationParameters.xml.cc 
  Entropy.cc 
  Evaluator.cc
  Evaluator.cc 
  Evaluator.xml.cc 
  EvaluatorCommon.cc
  EvaluatorDispatcher.cc
  EvaluatorFileDispatcher.cc
  EvaluatorLuaDispatcher.cc
  EvaluatorLuaWorker.cc
  EvolutionaryAlgorithm.cc 
  EvolutionaryAlgorithm.xml.cc 
  Fitness.cc 
  Fitness.xml.cc 
  FitnessEvaluator.cc
  FitnessEvaluator.cc 
  GEIndividual.cc
  GEIndividual.xml.cc
  GeneticOperator.cc 
  Group.cc
  Group.xml.cc 
  GroupFitnessEvaluator.cc
  GroupOperator.cc
  GroupPopulation.cc
  GroupPopulation.xml.cc
  GroupPopulationParameters.cc
  GroupPopulationParameters.xml.cc
  IMigrator.cc 
  Individual.cc 
  Individual.instantiate.cc 
  Individual.xml.cc 
  IndividualPopulation.cc
  IndividualPopulationParameters.cc
  Lineage.cc 
  Lineage.xml.cc 
  MOFitness.cc 
  MOIndividual.cc 
  MOIndividual.xml.cc 
  MOPopulation.cc 
  MOPopulation.xml.cc 
  MOPopulationParameters.cc
  MOPopulationParameters.xml.cc 
  Operator.cc
  OperatorToolbox.cc 
  Population.cc 
  Population.instantiate.cc 
  Population.xml.cc 
  PopulationParameters.cc 
  PopulationParameters.xml.cc 
  RankingSelection.cc
  RankingSelection.xml.cc
  ScaledFitness.cc 
  ScaledFitness.xml.cc 
  TournamentSelection.cc 
  TournamentSelection.xml.cc 

  Operators/AllopatricDifferentialOperator.cc 
  Operators/AlterationMutation.cc 
  Operators/BitStringOperator.cc
  Operators/Crossover.cc 
  Operators/DifferentialEvolutionOperator.cc
  Operators/GroupBalancedCrossover.cc
  Operators/GroupCrossover.cc
  Operators/GroupDreamTeamOperator.cc
  Operators/GroupExternalInsertionMutation.cc
  Operators/GroupExternalInsertionMutation.xml.cc
  Operators/GroupKFoldScanCrossoverOperator.cc
  Operators/GroupKFoldScanRemovalOperator.cc
  Operators/GroupMutation.cc
  Operators/GroupRandomInsertionMutation.cc
  Operators/GroupRandomRemovalMutation.cc
  Operators/GroupUnbalancedCrossover.cc
  Operators/GroupUnionIntersectionOperator.cc
  Operators/InsertionMutation.cc 
  Operators/InverOverCrossover.cc 
  Operators/LocalSearch.cc
  Operators/Mutation.cc 
  Operators/OnePointCrossover.cc 
  Operators/OnePointImpreciseCrossover.cc 
  Operators/RandomizerOperator.cc
  Operators/RemovalMutation.cc 
  Operators/ReplacementMutation.cc 
  Operators/ScanMutationBitArray.cc 
  Operators/ScanMutationConstant.cc 
  Operators/ScanMutationFloat.cc 
  Operators/ScanMutationInnerLabel.cc 
  Operators/ScanMutationInteger.cc 
  Operators/SimpleDifferentialOperator.cc
  Operators/SingleParameterAlterationMutation.cc 
  Operators/SubGraphInsertionMutation.cc 
  Operators/SubGraphRemovalMutation.cc 
  Operators/SubGraphReplacementMutation.cc 
  Operators/SwapMutation.cc 
  Operators/TwoPointCrossover.cc
  Operators/TwoPointImpreciseCrossover.cc
  Operators/UniformCrossover.cc
)

IF (UGP3_USE_LUA OR UGP3_USE_LUAJIT)
    IF (UGP3_USE_LUAJIT)
        TARGET_LINK_LIBRARIES(EvolutionaryCore ${LUAJIT_LIBRARIES})
    ELSE (UGP3_USE_LUAJIT)
        TARGET_LINK_LIBRARIES(EvolutionaryCore ${LUA_LIBRARIES})
    ENDIF (UGP3_USE_LUAJIT)
ENDIF (UGP3_USE_LUA OR UGP3_USE_LUAJIT)

#INSTALL(TARGETS EvolutionaryCore
#  LIBRARY DESTINATION lib
#)
