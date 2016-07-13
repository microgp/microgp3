/***********************************************************************\
|                                                                       |
| MOPopulation.cc                                                       |
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
| $Revision: 658 $
| $Date: 2015-04-23 11:26:47 +0200 (Thu, 23 Apr 2015) $
\***********************************************************************/

#include "ugp3_config.h"
#include "EvolutionaryCore.h"

#include <float.h>

using namespace std;
using namespace ugp3::core;
using namespace ugp3::log;
using namespace ugp3::ctgraph;

struct IndividualComparatorPlaceholder
{
    bool operator()(const Individual* i1, const Individual* i2)
    {
        int result = ((MOIndividual *)i1)->getPlaceholder() - ((MOIndividual *)i2)->getPlaceholder();
        return(result > 0);
    }
};

struct IndividualComparatorPlaceholderDec
{
    bool operator()(const Individual* i1, const Individual* i2)
    {
        int result = ((MOIndividual *)i1)->getPlaceholder() - ((MOIndividual *)i2)->getPlaceholder();
        return(result < 0);
    }
};


MOPopulation::MOPopulation(
                          unique_ptr<MOPopulationParameters> parameters,
                          const EvolutionaryAlgorithm& parent)
: SpecificIndividualPopulation(unique_ptr<PopulationParameters>(std::move(parameters)), 0, parent),
m_parameters(this)
{
	_STACK;
}

MOPopulation::MOPopulation(const EvolutionaryAlgorithm& parent)
: SpecificIndividualPopulation(parent),
m_parameters(this)
{
	_STACK;
}

void MOPopulation::addIndividual(std::unique_ptr<Individual> individual)
{
    _STACK;

    Individual *individualPtr = individual.get();

    MOIndividual* ind = dynamic_cast<MOIndividual*> (individualPtr);
    if (ind == nullptr)
        throw Exception("Expected an individual of type 'multiobjective'.", LOCATION);

    ind->setPlaceholder(this->getIndividualCount()); // done here so that placeholders start at 0

    SpecificIndividualPopulation::addIndividual(std::move(individual));

    LOG_DEBUG << "Adding new individual " << individualPtr << " to clone map" << ends;
}

void MOPopulation::selectNewZombifiableCandidates()
{
    auto liveBegin = regroupAndSkipDeadCandidates(m_individuals.begin(), m_individuals.end());
    
    // Prevent the whole first level from death.
    // FIXME makes sense? maybe they won't be killed anyway.
    m_bloodMagicWaitingList.clear();
    // Save at most mu zombies FIXME chose other limit
    // Save the best according to selection criteria
    std::sort(liveBegin, m_individuals.end(),
              [this] (MOIndividual* a, MOIndividual* b) {
                  return compareForSelection(a, b);
              });
    unsigned int mu = getParameters().getMu();
    for (auto it = liveBegin; it != m_individuals.end()
            && m_bloodMagicWaitingList.size() < mu; ++it) {
        m_bloodMagicWaitingList.push_back(*it);
    }
}


void MOPopulation::age()
{
    // ages the individuals, but does not age the heroes
    // sorts the individuals and sets apart the heroes
    // TODO: 	it makes no sense to preserve N individuals in a multi-objective approach: it would be much better
    //		to interpret the "eliteSize" parameter as the number of LEVELS of individuals that do not age.

    _STACK;

    this->m_strengthValid=false;

    // sort the individuals by levels and entropy // TODO: sorting individuals is useless
    // LOG_DEBUG << "Sorting individuals before aging" << ends;
    // this->sortIndividuals(this->individuals);

    for (unsigned int i = 0; i < m_individuals.size(); i++)
    {
        MOIndividual* individual = m_individuals[i];
        
        if (individual == nullptr)
            throw Exception("Expected an individual of type 'multiobjective'.", LOCATION);

        // sets the heroes
        if (individual->isAlive()) 
	{
            if (individual->getLevel() < (int)this->getParameters().getEliteCardinality()) 
	    {
                individual->setState(Individual::HERO);
            } 
	    else 
	    {
                individual->setState(Individual::ALIVE);
            }
        }

        // ages the individual
        individual->step(true);
    }
    this->m_strengthValid=false;
}

void MOPopulation::evaluateAndHandleClones()
{
    _STACK;

    LOG_VERBOSE << "Evaluating population ..." << ends;

    // if this is the first evaluation after a recovery, we must keep prevLevels from the xml!!
    if (m_firstRecovery) 
    {
        this->setFirstRecovery(false);
    } 
    else 
    {
        for (auto ind: m_individuals) 
	{
            // copy current levels to prevLevels
            ind->setPreviousLevel(ind->getLevel());
        }
    }
    
    SpecificIndividualPopulation::evaluateAndHandleClones();
    
    computeLevels();
    computePerceivedStrength();
}

void MOPopulation::prepareForCommit()
{
    computeLevels();
    computePerceivedStrength();
    
    SpecificIndividualPopulation::prepareForCommit();
}

void MOPopulation::commit()
{
    _STACK;


    // if this is generation 0, make an exception to avoid issues
    /*if( this->generation == 0 )
    {
	this->bestIndividual = (MOIndividual*) this->individuals[0];
	this->worstIndividual = (MOIndividual*) this->individuals[ this->individuals.size() -1 ];
	
	return;
    }*/

    // sorts the individuals by crowding distance in the first and last level
    unsigned int i;
    std::vector<MOIndividual*> ind;

    LOG_DEBUG << "Finding individuals in the best and worst level" << ends;

    // find best individuals (Pareto front / level 0) and sort them by entropy
    Assert(m_individuals.size() > 0);
    for (auto individual: m_individuals) 
    {
        // PATCH! at kickstart, all inds are @ level -1 (gx)
        if (individual->getLevel() <= 0) 
	{
            ind.push_back(individual);
        }
    }
    
    // TODO: sorting multi-objective individuals is kinda useless...
    std::sort(ind.begin(), ind.end(), [this] (MOIndividual* a, MOIndividual* b) {
        return compareScaledBestWorst(a, b);
    });
    Assert(ind.size() > 0);
    setBestIndividual(ind[0]);
    
    LOG_INFO 	<< "The Pareto front (level " << getBestIndividual()->getLevel() 
		<< ") contains " << ind.size() << " individuals:" << ends;

    for(i = 0; i < ind.size(); i++)
	LOG_INFO << "-- (" << ind[i] << "): " << ind[i]->getFitness() << ends;

    // find worst individuals (level == maxLevel) and sort them by entropy as well
    ind.clear();   
    for (auto individual: m_individuals) {
        // PATCH!
        if (individual->getLevel() == this->getMaxLevel() || individual->getLevel() < 0) 
	{
            ind.push_back(individual);
        }
    }

    // TODO: sorting multi-objective individuals is kinda useless...
    std::sort(ind.begin(), ind.end(), [this] (MOIndividual* a, MOIndividual* b) {
        return compareScaledBestWorst(a, b);
    });
    
    setWorstIndividual(ind.back());

    LOG_INFO << "The worst front (level " << getWorstIndividual()->getLevel() << ") contains " << ind.size() << " individuals:" << ends;
    for(i = 0; i < ind.size(); i++)
	LOG_INFO << "-- (" << ind[i] << "): " << ind[i]->getFitness() << ends;

}

bool MOPopulation::compareForSelection(const CandidateSolution* a, const CandidateSolution* b) const
{
    // TODO check that the comparison is in the right direction 
    // TODO check that is defines a strict total order
    const MOIndividual* ind1 = dynamic_cast<const MOIndividual*>(a);
    const MOIndividual* ind2 = dynamic_cast<const MOIndividual*>(b);
    
    Assert(ind1 != nullptr);
    Assert(ind2 != nullptr);
    
    if(ind1->getLevel() == ind2->getLevel()) 
    {
        if(ind1->getPerceivedStrength() == ind2->getPerceivedStrength())
        {
            int result = ind1->getDeltaEntropy().compareTo(ind2->getDeltaEntropy());
            return useResultOrTakeOldestOrTakeFirstId(result, a, b);
        }
        else
            return (ind1->getPerceivedStrength() > ind2->getPerceivedStrength());
    } 
    else
        return (ind1->getLevel() < ind2->getLevel());
}

void MOPopulation::slaughtering()
{
    _STACK;

    LOG_VERBOSE << "Performing natural selection on the population ..." << ends;
    describePopulation("Before slaughtering", m_individuals.begin(), m_individuals.end());

    removeCorpses(m_individuals);

    // must recompute levels
    computeLevels();
    computePerceivedStrength();

    updateDeltaEntropy(m_individuals.begin(), m_individuals.end());
    
    // Zombies are not concerned by the selection process, skip them
    auto liveBegin = regroupAndSkipNotAliveCandidates(m_individuals.begin(), m_individuals.end());
    
    // TODO define the compareForSelection function for MO Population
    std::sort(liveBegin, m_individuals.end(), [this] (MOIndividual* a, MOIndividual* b) {
        return compareForSelection(a, b);
    });

#ifndef NDEBUG
    for (auto individual: m_individuals) {
        LOG_DEBUG << "Individual " << *individual << " in level " << individual->getLevel() << ends;
    }
#endif

    // keep only the best mu individuals (those in the first part of the vector)
    LOG_VERBOSE << "Saving only the best " << this->getParameters().getMu() << " individuals ..." <<  ends;

    /*
     * Slaugthering within a level is done via reverse tournament:
     * n individuals are randomly selected, then one is chosen considering
     * its relative strength
     */
    unsigned int liveInds = std::distance(liveBegin, m_individuals.end());
    unsigned int mu = this->getParameters().getMu();
    if (liveInds > mu) 
    {
        unsigned int individuals_to_remove = liveInds - mu;
        LOG_VERBOSE << "Removing " << individuals_to_remove << " individual(s)..." << ends;
        
        unsigned int first = std::distance(m_individuals.begin(), liveBegin);
        unsigned int last = m_individuals.size() - 1;
        do 
	{
            int lastLevel = m_individuals[last]->getLevel();
            unsigned int levelSize = getLevelSize(lastLevel);
            Assert(levelSize > 0);
            if (levelSize <= individuals_to_remove) 
	    {
                // Wipe level
                individuals_to_remove -= levelSize;
                while (first <= last && m_individuals[last]->getLevel() == lastLevel) 
		{
                    m_individuals[last]->setDeath(getGeneration());
                    --last;
                }
            }
	    else 
	    {
                LOG_VERBOSE << "Removing " << individuals_to_remove << " leftovers from level " << lastLevel << ends;
                
                // select all individuals belonging to level lastLevel
                vector<MOIndividual*> elements;
                for (MOIndividual* individual: m_individuals) 
		{
                    if (individual->getLevel() == lastLevel && individual->isAlive())
                        elements.push_back(individual);
                }
                LOG_VERBOSE << "Found " << elements.size() << " individuals belonging to level " << lastLevel << ends;
                
                // choose individuals within this level
                // apply reverse tournament selection
                // worst individuals are selected
                // -1.0 of selection pressure to get a reverse tournament
                std::vector<CandidateSolution*> selected = getParameters().getSelector().selectCandidates(
                    elements.begin(), elements.end(), *this, individuals_to_remove, -1.0);
                
                Assert(selected.size() == individuals_to_remove);
                individuals_to_remove = 0;
                for (auto candidate: selected) 
		{
                    candidate->setDeath(getGeneration());
                }
            }
        } while (first <= last && individuals_to_remove > 0);
    }
    
    removeCorpses(m_individuals);

    LOG_INFO << "Individuals after slaughtering: " << m_individuals.size() << ends;

    // must recompute levels...
    computeLevels();
    computePerceivedStrength();
#ifndef NDEBUG
    for (unsigned int i = 0; i < m_individuals.size(); i++)
    {
        MOIndividual* individual = m_individuals[i];
        if (individual->getLevel() > this->m_maxLevel)
        {
            LOG_DEBUG << "Individual " << individual << " has level " << individual->getLevel() << ends;
            throw Exception("Individual has higher level than its population", LOCATION);
        }
    }
#endif
    LOG_DEBUG << "Updating placeholders" << ends;
    for (unsigned int i = 0; i < m_individuals.size(); ++i) {
        // update the placeholders
        m_individuals[i]->setPlaceholder(i);
    }
    LOG_DEBUG << "placeholders updated successfully" << ends;
    
    describePopulation("After slaughtering", m_individuals.begin(), m_individuals.end());
}

bool MOPopulation::checkMaximumFitnessReached()
{
    const vector<double>& maxFitness = this->getParameters().getMaximumFitness();
    Assert(maxFitness.size() == getParameters().getFitnessParametersCount());
    
    for (auto individual: m_individuals) 
    {
        // if there are such individuals, they must be in the first level
        if (individual->getLevel() == 0) 
	{
            const vector<double>& fitness = individual->getRawFitness().getValues();
            Assert(fitness.size() == getParameters().getFitnessParametersCount());
            
            if (fitness >= maxFitness) 
	    {
                LOG_INFO << "Reached maximum fitness (individual " <<  individual << ")"<< ends;
                return true;
            }
        }
    }
    
    return false;
}

void MOPopulation::updateSteadyStateGenerations()
{
    // if steady state is enabled
    //     there is a steady state when no individuals from first level
    //     are declassed to lower levels
    //     -> individuals are removed from first level only by slaughtering!
    
    if (this->getParameters().getSteadyStateGenerationsStop() == true && this->generation > 0)
    {
        LOG_DEBUG << "Searching steady state" << ends;
        LOG_DEBUG << "MaxSteadyStateGenerations: " << this->getParameters().getMaximumSteadyStateGenerations() << ends;
        LOG_DEBUG << "SteadyStateGenerations: " << m_steadyStateGenerations << ends;
        
        // TODO:this is probably wrong; for example, if new points are arriving into the Pareto front, the algorithm is far from
        //	being in a stagnation condition.
	
	// so, let's experiment with "the size of the Pareto front did not change"
	unsigned int sizeOfPreviousPF = 0, sizeOfCurrentPF = 0;

        for (auto individual: m_individuals) 
	{
            LOG_DEBUG 	<< "Individual previous level: " << individual->getPreviousLevel() 
			<< " and current level: " << individual->getLevel() << ends;
            
            if (individual->getPreviousLevel() == 0) sizeOfPreviousPF++;
	    if (individual->getLevel() == 0) sizeOfCurrentPF++; 
        }
        
        if (sizeOfCurrentPF == sizeOfPreviousPF)
        {
            m_steadyStateGenerations++;
            LOG_INFO << "Stagnation: size of the Pareto front did not change for " 
            << m_steadyStateGenerations << " generations (stop set at " 
            << this->getParameters().getMaximumSteadyStateGenerations() << ")."
            << ends;
        }
	else
	{
            m_steadyStateGenerations = 0;
            LOG_INFO << "Stagnation: size of the Pareto front changed!" << ends; 
	}
        
        LOG_DEBUG << "SteadyGenerations: " << m_steadyStateGenerations << ends;
    }
}

void MOPopulation::computeLevels()
{
    _STACK;
    
    /*
     *  computes the level for each individual
     * For each individual i:
     * if !exists an individual j such as j covers i
     * 	 then i is in level 0
     * endif
     * remove all individuals in level 0
     * repeat cycle to find individuals in level 1, 2, etc.
     * Procedure ends when all individuals are classified.
     */
    
    for (auto individual: m_individuals) 
    {
        // invalidates current level
        individual->setLevel(-1);
    }
    
    // Dead individuals are not updated because their fitness is invalid
    // Zombies are updated so they can die when they leave the first level
    auto begin = regroupAndSkipDeadCandidates(m_individuals.begin(), m_individuals.end());
    
    // recomputes levels
    int n;
    bool end = false;
    unsigned int validated;
    
    for (n = 0 , validated = 0; !end/* there are still individuals */; n++) 
    {
        end = true;
        LOG_INFO << "Computing level " << n << Progress(validated / ((double) m_individuals.size() + 1.0)) << ends;
        
        for (auto it = begin; it != m_individuals.end(); ++it) 
	{
            MOIndividual* individual = *it;
            bool found_cover = false;
            
            // computes only individuals with level invalidated
            if (individual->getLevel() == -1) 
	    {
                end = false;
                // considers the scaled fitness
                for (auto it2 = begin; it2 != m_individuals.end(); ++it2) 
		{
                    MOIndividual* individual2 = *it2;
                    if (found_cover)
                        break;
                    
                    if (individual != individual2 && ((int)individual2->getLevel() == n || individual2->getLevel() == -1)) 
		    {
                        /* we don't consider levels above this */
                        if (individual->getFitness().compareTo(individual2->getFitness()) < 0)
                        {
                            // individual is covered by at least one other individual
                            found_cover = true;
                        }
                    }
                }
                
                if (!found_cover)
                {
                    // individual belongs to level n
                    individual->setLevel(n);
                    validated++;
                    LOG_DEBUG 	<< "Individual " << *individual << " with fitness " << individual->getFitness() 
				<< " belongs to level " << individual->getLevel() << ends;
                }
            }
        }
    }
    
    LOG_INFO << "Computing levels" << Progress::END << ends;
    
    // sets maxLevel
    this->setMaxLevel(n-2); // the for cycle makes one more loop before exiting
}

void MOPopulation::computePerceivedStrength()
{
    _STACK;
    
    // here the "strength" is the "crowding distance"; basically, it's based on the fitness-space distance between
    // an individual and the two closest neighbours of the same level. Once the two closest individuals are found, the crowding
    // distance is the area of the rectangle whose two opposite vertices are the two closest individuals.
    
    LOG_INFO << "Computing crowding distance for all individuals..." << ends;
    
    // Zombies and dead individuals are out of it
    auto begin = regroupAndSkipNotAliveCandidates(m_individuals.begin(), m_individuals.end());
    
    for (auto it = begin; it != m_individuals.end(); ++it) 
    {
        MOIndividual* individual = *it;
        // initialize the vectors
        vector<double> distancesLeft, distancesRight; 
        vector<MOIndividual*> closestNeighboursLeft, closestNeighboursRight;
        
        size_t fitnessSize = individual->getRawFitness().getValues().size();
        distancesLeft.resize(fitnessSize, -1);
        distancesRight.resize(fitnessSize, -1);
        closestNeighboursLeft.resize(fitnessSize, nullptr);
        closestNeighboursRight.resize(fitnessSize, nullptr);
        
        // so, FOR EACH OBJECTIVE, find the two closest points (one on each side of the objective), on the same front!
        // for each front, solutions at the limit of each objective are assigned an infinite value
        LOG_DEBUG << "Computing crowding distance for individual " << *individual << "..." << ends;
        for (auto it2 = begin; it2 != m_individuals.end(); ++it2) 
	{
            MOIndividual* individual2 = *it2;
            if( individual != individual2 && individual->getLevel() == individual2->getLevel() )
            {
                for (unsigned int objective = 0; objective < fitnessSize; objective++)
                {
                    double difference = 
                    individual->getRawFitness().getValues()[objective] - individual2->getRawFitness().getValues()[objective];
                    
                    if( difference < 0 )
                    {
                        if( abs(difference) < distancesLeft[objective] || distancesLeft[objective] == -1 )
                        {
                            distancesLeft[objective] = abs(difference);
                            closestNeighboursLeft[objective] = individual2;
                        }
                    }
                    else if( difference > 0 )
                    {
                        if( abs(difference) < distancesRight[objective] || distancesRight[objective] == -1 )
                        {
                            distancesRight[objective] = abs(difference);
                            closestNeighboursRight[objective] = individual2;
                        }
                    }
                    else if( difference == 0 )
                    {
                        // I actually have no idea how to manage this...
                        // maybe replace the largest? replace the closest?
                        if( distancesRight[objective] > distancesLeft[objective] )
                        {
                            distancesRight[objective] = abs(difference);
                            closestNeighboursRight[objective] = individual2;
                        }
                        else
                        {
                            distancesLeft[objective] = abs(difference);
                            closestNeighboursLeft[objective] = individual2;
                        }
                    }
                }
            }
        } // end for each other individual
        
        double hyperVolume = 1;
        for (unsigned int objective = 0; objective < fitnessSize && hyperVolume < DBL_MAX; objective++) 
	{
            if (distancesLeft[objective] == -1 || distancesRight[objective] == -1)
                hyperVolume = DBL_MAX;
            else
                hyperVolume *= (distancesLeft[objective] + distancesRight[objective]);
        }

        LOG_DEBUG 	<< "Hypervolume of crowding distance for individual " << *individual 
			<< " with fitness=" << individual->getRawFitness()
			<< " is " << hyperVolume << ends; 
        
        // now, a couple of special cases: if the hyperVolume is negative, it's one of the individuals at the limit
        // so we superboost its crowding distance, it is at the edge of the front
        if( hyperVolume < 0 ) hyperVolume = DBL_MAX;
        
        individual->setPerceivedStrength( hyperVolume );
        
    } // end for each individual
}

// computes average fitness in level n
vector<double> MOPopulation::computeLevelAverageFitness(int n) const
{
    _STACK;
    
    unsigned int lvlSize = 0;
    
    if (n < 0 || n > this->m_maxLevel)
        throw Exception("Cannot compute average fitness: invalid level.", LOCATION);
    
    // initialize the fitness vector
    vector<double> fitness(getParameters().getFitnessParametersCount(), 0.0);
    
    for (auto individual: m_individuals) {
    
        if (individual->getLevel() == n && individual->isAlive())
        {
            const vector<double>& indFitness = individual->getFitness().getValues();
            
            Assert(fitness.size() == indFitness.size());
            for (unsigned int f = 0; f < fitness.size(); f++)
            {
                fitness[f] = fitness[f] + indFitness[f];
            }
            lvlSize++;
        }
    }
    
    for (unsigned int i = 0; i <  fitness.size(); i++)
    {
        fitness[i] = fitness[i] / lvlSize;
    }
    
    return fitness;
    
}

void MOPopulation::showStatistics(void) const
{
    SpecificIndividualPopulation::showStatistics();
    
    // TODO: 	why is there only ONE level (-1) at generation 0? isn't it
    //		better to put maxLevel = 0?
    if( this->generation > 0 )
    {
        LOG_INFO << "Size of the " << this->getMaxLevel()+1 << " levels:";
        for (int i = 0; i < this->getMaxLevel()+1; ++i)
        {
            LOG_INFO << " " << this->getLevelSize(i);
        }
        LOG_INFO << ends;
    }
    
    // in fact, on the generation 0 the best level is -1! so, skip this
    if( this->generation > 0 )
    {
        // Average fitness in first level
        vector<double> averageFitnessFirst = this->computeLevelAverageFitness(0);
        LOG_INFO << "Fitness in first level (avg):";
        for (unsigned int i = 0; i <  averageFitnessFirst.size(); i++)
        {
            LOG_INFO << " " << averageFitnessFirst[i];
        }
        LOG_INFO << ends;
        // Average fitness in last level
        vector<double> averageFitnessLast = this->computeLevelAverageFitness(this->getMaxLevel());
        LOG_INFO << "Fitness in last level (avg):";
        for (unsigned int i = 0; i <  averageFitnessLast.size(); i++)
        {
            LOG_INFO << " " << averageFitnessLast[i];
        }
        LOG_INFO << ends;
    }
    
}

void MOPopulation::dumpStatisticsHeader(ostream& output) const
{
    SpecificIndividualPopulation::dumpStatisticsHeader(output);
    
    // FITNESS: AVERAGE, FIRST LEVEL
    for (unsigned int i = 0; i < this->getParameters().getFitnessParametersCount(); i++)
    {
        output << "," << this->name << "_FL_f" << i;
    }
    
    // FITNESS: AVERAGE, LAST LEVEL
    for (unsigned int i = 0; i < this->getParameters().getFitnessParametersCount(); i++)
    {
        output << "," << this->name << "_LL_f" << i;
    }
}

void MOPopulation::dumpStatistics(ostream& output) const
{
    SpecificIndividualPopulation::dumpStatistics(output);
    
    vector<double> averageFitnessFirst = this->computeLevelAverageFitness(0);
    vector<double> averageFitnessLast = this->computeLevelAverageFitness(this->getMaxLevel());
    
    // FITNESS: AVERAGE, FIRST LEVEL
    for (unsigned int i = 0; i < averageFitnessFirst.size(); i++)
    {
        output << "," << averageFitnessFirst[i];
    }
    // FITNESS: AVERAGE, LAST LEVEL
    for (unsigned int i = 0; i < averageFitnessLast.size(); i++)
    {
        output << "," << averageFitnessLast[i];
    }
}

unsigned int MOPopulation::getLevelSize(int n) const {
    
    if (n > this->getMaxLevel() || n < 0)
        throw Exception("Invalid level number.", LOCATION);
    
    unsigned int size = 0;
    for (unsigned int i = 0; i < m_individuals.size(); i++)
    {
        MOIndividual* individual = m_individuals[i];
        if (individual->getLevel() == n && individual->isAlive())
            size++;
    }
    return size;
}

MOPopulation::~MOPopulation()
{
    LOG_DEBUG << "Destructor: ugp3::core::MOPopulation" << ends;
}

void MOPopulation::handleClone(CandidateSolution& master, CandidateSolution& clone, unsigned int number, unsigned int total)
{
    if (number != 0) {
        // Kill the clone. Zombies are already dead.
        // FIXME what to do if the 
        // TODO find again the lost ending of the previous philosophical question
        // NOTE maybe it was about heroes, like: WHAT TO DO IF THE CLONE IS A HERO?
        clone.setDeath(getGeneration());
    }
}

