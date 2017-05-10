/***********************************************************************\
|                                                                       |
| Program.Run.cc                                                        |
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
| $Revision: 656 $
| $Date: 2015-04-15 14:16:06 +0200 (Wed, 15 Apr 2015) $
\***********************************************************************/

/**
 * @file Program.Run.cc
 * Implementation of the run method of the Program class.
 * @see Program.h
 * @see Program.cc
 */

#include "ugp3_config.h"
#include "Program.h"

// ADD HERE: include headers of new types of populations and genetic operators
#include "EnhancedPopulation.h"
#include "MOPopulation.h"
#include "Operators/AllopatricDifferentialOperator.h"
#include "Operators/AlterationMutation.h"
#include "Operators/BitStringOperator.h"
#include "Operators/InsertionMutation.h"
#include "Operators/LocalSearch.h"
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

// Obsolete operators
//#include "Operators/LocalScanMutation.h"
//#include "Operators/RandomWalkMutation.h"
//#include "Operators/ScanMutation.h"

// ADD HERE: include headers of group operators
#include "Operators/GroupRandomInsertionMutation.h"
#include "Operators/GroupRandomRemovalMutation.h"
#include "Operators/GroupBalancedCrossover.h"
#include "Operators/GroupUnbalancedCrossover.h"
#include "Operators/GroupKFoldScanCrossoverOperator.h"
#include "Operators/GroupKFoldScanRemovalOperator.h"
#include "Operators/GroupUnionIntersectionOperator.h"
#include <Operators/GroupDreamTeamOperator.h>


using namespace std;
using namespace ugp3;
using namespace ugp3::frontend;
using namespace ugp3::constraints;
using namespace ugp3::core;

void Program::registerOperators()
{
    _STACK;

    LOG_VERBOSE << "Registering genetic operators" << ends;
    Operator::registration(new AllopatricDifferentialOperator());
    Operator::registration(new AlterationMutationOperator());
    Operator::registration(new BitStringFlipOneMutation());
    Operator::registration(new BitStringResetOneMutation());
    Operator::registration(new BitStringSetOneMutation());
    Operator::registration(new BitStringGlobalMajorityMutation());
    Operator::registration(new BitStringGlobalMinorityMutation());
    Operator::registration(new BitStringLocalMajorityMutation());
    Operator::registration(new BitStringLocalMinorityMutation());
    Operator::registration(new BitStringFlipUniformMutation());
    Operator::registration(new BitStringFlip1Mutation());
    Operator::registration(new BitStringFlip3Mutation());
    Operator::registration(new BitStringFlip5Mutation());
    Operator::registration(new InsertionMutationOperator());
    Operator::registration(new InverOverCrossoverOperator());
    Operator::registration(new LocalSearchOperator());
    Operator::registration(new OnePointCrossoverOperator());
    Operator::registration(new OnePointImpreciseCrossoverOperator());
    Operator::registration(new RandomizerOperator());
    Operator::registration(new RemovalMutationOperator());
    Operator::registration(new ReplacementMutationOperator());
    Operator::registration(new ScanMutationBitArrayOperator());
    Operator::registration(new ScanMutationConstantOperator());
    Operator::registration(new ScanMutationFloatOperator());
    Operator::registration(new ScanMutationInnerLabelOperator());
    Operator::registration(new ScanMutationIntegerOperator());
    Operator::registration(new SimpleDifferentialOperator());
    Operator::registration(new SingleParameterAlterationMutationOperator());
    Operator::registration(new SubGraphInsertionMutationOperator());
    Operator::registration(new SubGraphRemovalMutationOperator());
    Operator::registration(new SubGraphReplacementMutationOperator());
    Operator::registration(new SwapMutationOperator());
    Operator::registration(new TwoPointCrossoverOperator());
    Operator::registration(new TwoPointImpreciseCrossoverOperator());
    Operator::registration(new UniformCrossoverOperator());
    LOG_DEBUG << "Registration complete." << endl << ends;

// Old genetic operators
//    GeneticOperator::registration(new LocalScanMutationOperator());
//    GeneticOperator::registration(new RandomWalkMutationOperator());
//    GeneticOperator::registration(new ScanMutationOperator());

    LOG_VERBOSE << "Registering group operators" << ends;
    Operator::registration(new GroupRandomInsertionMutationOperator());
    Operator::registration(new GroupRandomRemovalMutationOperator());
    Operator::registration(new GroupExternalInsertionMutationOperator());
    Operator::registration(new GroupBalancedCrossoverOperator());
    Operator::registration(new GroupUnbalancedCrossoverOperator());
    Operator::registration(new GroupKFoldScanCrossoverOperator());
    Operator::registration(new GroupKFoldScanRemovalOperator());
    Operator::registration(new GroupUnionIntersectionOperator());
    Operator::registration(new GroupDreamTeamOperator());
    LOG_DEBUG << "Registration complete." << endl << ends;
}

void Program::run()
{
    _STACK;

    // Registers the operators that can be used in the evolution.
    Program::registerOperators();

    // Pointer to the algorithm that is going to do all the evolution.
    // Instantiates a new evolutionary algorithm.
    unique_ptr< EvolutionaryAlgorithm > algorithm = unique_ptr<EvolutionaryAlgorithm>( new EvolutionaryAlgorithm() );

    // Loads parameters and names of populations.
    const vector<string> populationParametersFiles = settings.getOption("evolution", Argument::Population).toList();
    const vector<string> populationNames = settings.getOption("evolution", "populationNames").toList();
    vector<string> populationSeedingFiles = settings.getOption("evolution", "populationSeedingFiles").toList();
    LOG_DEBUG << "List of seeding files:" << ends;
    for(unsigned int i = 0; i < populationSeedingFiles.size(); i++) 
    {
	// let's trim the strings, because if they are empty, they contain " "
	populationSeedingFiles[i].erase(
			std::find_if(populationSeedingFiles[i].rbegin(), populationSeedingFiles[i].rend(), 
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(), populationSeedingFiles[i].end()
			);
	LOG_DEBUG << "\"" << populationSeedingFiles[i] << "\""<< ends;
    }
    // Loads an old saved execution to continue the evolution if necessary.
    const string& inputFile = settings.getOption("recovery", Argument::RecoveryInputAlgorithm).getValue();
    if (inputFile.empty() == false)
    // A recovery input file was specified.
    {
        LOG_INFO << "Recovering status from file \"" << inputFile << "\"..." << ends;
        // If fitness is discarded is recalulated for every individual.
        bool discardFitness = settings.getOption("recovery", Argument::RecoveryDiscardFitness).toBool();

        // Loads the state of the evolution in order to continue it.
        algorithm->fromFile(inputFile, discardFitness);

        for (unsigned int i=0; i < algorithm->getPopulationCount(); ++i)
        {
            if (const GroupPopulation* gpop = dynamic_cast<const GroupPopulation*>(&algorithm->getPopulation(i)))
            {
                LOG_INFO << "Recovered group population \"" << gpop->getName() 
                << "\" (time: " << algorithm->getElapsedTime().count()
                << "s, gen: " << gpop->getGeneration() << ")" << ends;
            } 
	    else if (const EnhancedPopulation* epop = dynamic_cast<const EnhancedPopulation*>(&algorithm->getPopulation(i)))
            {
                LOG_INFO << "Recovered enhanced population \"" << epop->getName() 
                << "\" (time: " << algorithm->getElapsedTime().count()
                << "s, gen: " << epop->getGeneration() << ")" << ends;
            } 
	    else if (const MOPopulation* mopop = dynamic_cast<const MOPopulation*>(&algorithm->getPopulation(i)))
            {
                LOG_INFO << "Recovered multi-objective population \"" << mopop->getName() 
                << "\" (gen: " << mopop->getGeneration() << ")" << ends;
            } 
	    else if (const Population* pop = dynamic_cast<const Population*>(&algorithm->getPopulation(i)))
            {
                LOG_INFO << "Recovered enhanced population \"" << pop->getName() 
                << "\" (gen: " << pop->getGeneration() << ")"  << ends;
            }
        }
    } 
    else
    // No recovery, create a new algorithm from scratch.
    {
        // Sets the value of the seed to generate random numbers.
        long initialSeed;
        if (settings.getOption("evolution", Argument::RandomSeed).getValue() == "0")
        {
            initialSeed = std::time(nullptr);
            LOG_INFO << "Random generator seed: " << initialSeed << " (time since the epoch)" << ends;
        } else
        {
            initialSeed = settings.getOption("evolution", Argument::RandomSeed).toLong();
            LOG_INFO << "Random generator seed: " << initialSeed << ends;
        }
        Random::seed(initialSeed);

        LOG_VERBOSE << "Setting up the evolutionary algorithm..." << ends;

        // Sets the parameters of the evolutionary algorithm.
        algorithm->setOverwriteOutput(
                                     settings.getOption("recovery", Argument::RecoveryOutputOverwrite).toBool()
                                     );
        algorithm->setOutputPathName(
                                    settings.getOption("recovery", Argument::RecoveryOutputAlgorithm).getValue()
                                    );
        algorithm->setStatisticsPathName(
                                        settings.getOption("evolution", Argument::StatisticsPathName)
                                        );

        // Since the algorithm is empty,
        // the user should specify at least the configuration of one population to insert in the algorithm.
        if (populationParametersFiles.empty() == true)
        {
            throw Exception("At least one population settings must be specified in settings file, or with command line option \"--" + Argument::Population + "\"",
                            LOCATION);
        }

        // Creates empty populations with the specified configurations and adds them to the algorithm.
        if (populationParametersFiles.empty() == true)
        {
            LOG_ERROR << "Can't find any population" << ends;
        }

        for (unsigned int i = 0; i < populationParametersFiles.size(); i++)
        {
            string popName;
            if (i < populationNames.size())
            {
                popName = populationNames[i];
            } 
	    else
            {
                ostringstream tmp;
                tmp << "NAMELESS-" << i;
                popName = tmp.str();
            }

            bool existingPopulation = false;
            for (unsigned int j = 0; j < algorithm->getPopulationCount(); j++)
            {
                if (popName == algorithm->getPopulation(j).getName())
                {
                    existingPopulation = true;
                }
            }   

            if (existingPopulation == true)
            {
                LOG_WARNING << "Can't add population \"" << popName << "\" (already present)" << ends;
            } 
	    else
            {
                unique_ptr<Population> population;
                try
                {
                    LOG_INFO 	<< "Adding population " << algorithm->getPopulationCount() 
				<< " \"" << popName << "\" from file \"" << populationParametersFiles[i] 
				<< "\"" << ends;
                    population = Population::fromParametersFile(*algorithm, populationParametersFiles[i]);
                    population->setName(popName);
                }
		catch (const Exception& e)
		{
			throw;
		}
                algorithm->addPopulation(std::move(population));
	
            }
        }
    }

    // Merge populations if specified
    const vector<string> populationsToMerge = settings.getOption("evolution", Argument::Merge).toList();
    for (unsigned int i = 0; i < populationsToMerge.size(); i++)
    {
        const string& populations = populationsToMerge[i];
        istringstream stream(populations);

        unsigned int index1, index2;
        stream >> index1 >> index2;

        if (index1 == index2)
        {
            throw Exception("Cannot merge the population with itself.", LOCATION);
        }

        Population& population1 = algorithm->getPopulation(index1);

        unique_ptr<Population> population2 = algorithm->removePopulation(index2);

        if (population2->getCandidateCount() == 0)
        {
            LOG_WARNING << "Merging empty population at index " << index2<< " into population " << index1 << ends;
        }
        population1.merge(std::move(population2));
    }

    // migration options
    if ( frontend::Program::settings.getContext("evolution").hasOption("migration") == true )
    {
        LOG_DEBUG << "Now reading migrator options..." << ends;
        // options parsing
        string buffer, strType, strSize, strInterval;
        size_t first, second;

        buffer = frontend::Program::settings.getContext("evolution").getOption("migration").getValue();
        first = buffer.find(";");
        second = buffer.find(";", first+1);

        if ( first == string::npos || second == string::npos)
        {
            throw Exception("Error in settings file: migrator value should be in the form \"<type>;<size>;<interval>\"", LOCATION);
        }

        strType = buffer.substr(0, first);
        strSize = buffer.substr(first+1, second-first-1);
        strInterval = buffer.substr(second+1);

        istringstream ssSize(strSize), ssInterval(strInterval);
        unsigned int size, interval;

        if (ssSize >> size && ssInterval >> interval)
        {
            LOG_DEBUG << "Type of migrator is " << strType << ", size is " << strSize << " and interval is " << strInterval << ends;

            if ( strType == ClassicalMigrator::XML_SCHEMA_TYPE )
            {
                algorithm->setMigrator(new ClassicalMigrator(size, interval));
            } else
            {
                throw Exception("Type of migrator " + strType + " not found.", LOCATION);
            }
        } else
            throw Exception("Error in settings file: migrator value should be in the form \"<type>;<size>;<interval>\", with <size> and <interval> unsigned integers.", LOCATION);
    }

    // Double check: avoid populations with the same name
    for (unsigned int i = 0; i < algorithm->getPopulationCount(); i++)
    {
        for (unsigned int j = 0; j < i ; j++)
        {
            if (algorithm->getPopulation(i).getName() == algorithm->getPopulation(j).getName())
            {
                throw Exception("Duplicated population name \"" + algorithm->getPopulation(j).getName() + "\"", LOCATION);
            }
        }
    }   
    
    // Check that cache is disabled when using dynamic fitness
    for (unsigned int i = 0; i < algorithm->getPopulationCount(); i++) 
    {
        if (algorithm->getPopulation(i).getParameters().getInvalidateFitnessAfterGeneration()
            && algorithm->getPopulation(i).getParameters().getEvaluator().getCacheSize() != 0) 
	{
            LOG_ERROR << "Dynamic fitness is enabled but the evaluation cache is enabled." << std::ends;
            LOG_ERROR << "Please disable the evaluator cache:" << std::ends;
            LOG_ERROR << "  <evaluation>" << std::ends;
            LOG_ERROR << "    <cacheSize value=\"0\" />" << std::ends;
            LOG_ERROR << "    ..." << std::ends;
            LOG_ERROR << "  </evaluation>" << std::ends;
            LOG_ERROR << "" << std::ends;
            LOG_ERROR << "or disable dynamic fitness:" << std::ends;
            LOG_ERROR << "  <invalidateFitnessAfterGeneration value=\"0\"/>" << std::ends;
            LOG_ERROR << "" << std::ends;
            throw Exception("Incoherent settings: fitness is dynamic but the evaluator cache is enabled.", LOCATION);
        }
    }

    // Sanity check
    for (unsigned int i = 0; i < algorithm->getPopulationCount(); i++)
    {
        // FIXME since DMAB, this sanity check is not reliable anymore
#if 0
        double w = 0.0;
        for (unsigned int j=0; j < algorithm->getPopulation(i).getParameters().getActivations().getDataCount(); ++j)
        {
            LOG_DEBUG << algorithm->getPopulation(i).getParameters().getActivations().getData(j).getOperatorName() << ": " << algorithm->getPopulation(i).getParameters().getActivations().getData(j).getWeight() << ends;
            w += algorithm->getPopulation(i).getParameters().getActivations().getData(j).getWeight();
        }
        if (w == 0.0)
        {
            throw Exception("No active operators in population \"" + algorithm->getPopulation(i).getName() + "\"", LOCATION);                
        }
#endif
    }

    // Initialize statistics stream for the algorithm (where all population's statistics will be written to)
    algorithm->openStatisticsStream();
    
    // Population size must be >= nu.
    LOG_VERBOSE << "Warming up..." << ends;
    for (unsigned int i = 0; i < algorithm->getPopulationCount() ; i++)
    {
        Population& population = algorithm->getPopulation(i);
    
        LOG_INFO << "* Population \"" << algorithm->getPopulation(i).getName() << "\" (kickstart)" << ends;
        
        if (settings.getOption("evolution", Argument::AllOperatorsEnabled).toBool()) 
	{
            LOG_WARNING << "Enabling all operators by default. ANYTHING CAN HAPPEN NOW." << std::ends;
            population.getParameters().getActivations().forceDefaultEnabling(true);
        } 
	else if (settings.getOption("evolution", Argument::AllOperatorsDisabled).toBool()) 
	{
            LOG_INFO << "Disabling all operators by default." << std::ends;
            population.getParameters().getActivations().forceDefaultEnabling(false);
        }

        if (population.getGeneration() == 0) 
	{
	    // if some individuals to assimilate were specified, add them to the population before starting
	    // the setup, that includes the random generation of individuals
	    if( populationSeedingFiles[i].length() > 0 )
	    {
		LOG_INFO 	<< "Seeding the population with individuals listed in file \""
				<< populationSeedingFiles[i] << "\""
				<< "..." << ends;
		population.seeding( populationSeedingFiles[i] );
	    }
            population.setupPopulation();
        }
        
        SETENV("UGP3_GENERATION", Convert::toString(population.getGeneration()).c_str());
        population.evaluateAndHandleClones();
#ifndef NDEBUG
        population.checkFitnessValidity();
#endif
        // `evaluateAndHandleClones()' is allowed to kill candidates,
        // so we must remove their corpses.
        population.removeDeadCandidates();

        if (population.getGeneration() == 0) 
	{
            // Compute statistics for generation 0 of each population.
            population.commit();
        }

        population.showStatistics();
    }
    
    // Save "step 0" of the algorithm
    algorithm->saveDumpState();
    
    // Save the statistics of "step 0"
    algorithm->writeStatisticsStream();

    // Tweaking
    if (settings.getOption("evolution", Argument::further).getValue() != "0")
    {
        int moreGen = settings.getOption("evolution", Argument::further).toLong();
        for (unsigned int i = 0; i < algorithm->getPopulationCount() ; i++)
        {
            long newLimit = algorithm->getPopulation(i).getGeneration() +moreGen;
            algorithm->getPopulation(i).getParameters().setMaximumGenerations(newLimit);
            LOG_INFO << "Maximum generation limit for population \"" << algorithm->getPopulation(i).getName() << "\" extended to " << newLimit << ends;
        }
    }

    // Run the evolution.
    LOG_DEBUG << "Starting evolution" << ends;

    algorithm->run();

    // Write best individuals to file
    for (unsigned int i = 0; i < algorithm->getPopulationCount() ; i++)
    {
        if (const EnhancedPopulation* population = dynamic_cast<const EnhancedPopulation*>(&algorithm->getPopulation(i)))
        {
            const Individual* individual = population->getBestRawIndividual();
            if (individual != nullptr)
            {
                ostringstream fileName;
                fileName << "BEST_" << population->getName() << File::getExtension(population->getParameters().getEvaluator().getInputFile());
                LOG_INFO << "Saving best individual of enhanced population " << i << " as \"" << fileName.str() << "\"" << ends;
                individual->toCode(fileName.str());  
            } 
	    else
            {
                LOG_WARNING << "No \"best individual\" defined in enhanced population \"" << population->getName() << "\"" << ends;
            }
        } 
	else if (const MOPopulation* population = dynamic_cast<const MOPopulation*>(&algorithm->getPopulation(i)))
        {
            LOG_INFO << "Pareto front of multi-objective population " << i << ":" << ends;
            for (unsigned int j=0; j < population->getIndividualCount(); j++)
            {
                if (((const MOIndividual&)population->getIndividual(j)).getLevel() == 0)
                {
                    LOG_INFO << "---- " << population->getIndividual(j) << ": " << population->getIndividual(j).getFitness() << ends;
                }
            }
            bool found = false;
            int currentLevel = 1;
            for (unsigned int j=0; j < population->getIndividualCount(); j++)
            {
                if (((const MOIndividual&)population->getIndividual(j)).getLevel() == currentLevel)
                {
                    found = true;
                }
            }
            LOG_VERBOSE << "Level 0 of multi-objective population " << i << " is the pareto front (see above)." << ends;
            while (found)
            {
                found = false;
                LOG_VERBOSE << "Level " << currentLevel << " of multi-objective population " << i << ":" << ends;
                for (unsigned int j=0; j < population->getIndividualCount(); j++)
                {
                    if (((const MOIndividual&)population->getIndividual(j)).getLevel() == currentLevel)
                    {
                        LOG_VERBOSE << "---- " << population->getIndividual(j) << ": " << population->getIndividual(j).getFitness() << ends;
                    }
                }
                ++currentLevel;
                for (unsigned int j=0; j < population->getIndividualCount(); j++)
                {
                    if (((const MOIndividual&)population->getIndividual(j)).getLevel() == currentLevel)
                    {
                        found = true;
                    }
                }
            }
            LOG_INFO << "Saving pareto front of multi-objective population " << i << " as PF_"  << population->getName() << "_*" << File::getExtension(population->getParameters().getEvaluator().getInputFile()) << ends;
            for (unsigned int j=0; j < population->getIndividualCount(); j++)
            {
                if (((const MOIndividual&)population->getIndividual(j)).getLevel() == 0)
                {
                    ostringstream fileName;
                    fileName << "PF_" << population->getName() << "_" << population->getIndividual(j).getId() << File::getExtension(population->getParameters().getEvaluator().getInputFile());
                    population->getIndividual(j).toCode(fileName.str());  
                }
            }
        } 
	else if (const GroupPopulation* population = dynamic_cast<const GroupPopulation*>(&algorithm->getPopulation(i)))
        {
            const Group* group = population->getBestRawGroup();
            if (group)
            {
                ostringstream fileName;
                fileName << "BEST_" << population->getName() << File::getExtension(population->getParameters().getEvaluator().getInputFile());
                LOG_INFO << "Saving best group of group population " << i << " as \"" << fileName.str() << "\"" << ends;
                group->toCode(fileName.str());  
            } 
        } 
	else
        {
            LOG_WARNING << "Unhandled population type!" << ends;
        }
    }

    // Free memory
    Operator::unregisterAll();
    PopulationParameters::clearConstraintsCache();

    LOG_DEBUG << "That's all" << ends;
}

string GuessExtension(string pattern)
{
	return "?";
}
