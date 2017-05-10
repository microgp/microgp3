/***********************************************************************\
|                                                                       |
| EnhancedPopulation.cc                                                 |
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
 * @file EnhancedPopulation.cc
 * Implementation of the EnhancedPopulation class.
 * @see EnhancedPopulation.h
 * @see EnhancedPopulation.xml.cc
 */

#include "ugp3_config.h"
#include "EvolutionaryCore.h"
#include "EnhancedPopulationParameters.h"
#include <Distances.h>
using namespace std;
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace ugp3::log;

EnhancedPopulation::EnhancedPopulation(unique_ptr<EnhancedPopulationParameters> parameters,
                                       const EvolutionaryAlgorithm& parent)
  : SpecificIndividualPopulation(std::move(unique_ptr<PopulationParameters>(std::move(parameters))), 0, parent),
  m_parameters(this)
{
    _STACK;
}

EnhancedPopulation::EnhancedPopulation(const EvolutionaryAlgorithm& parent)
  : SpecificIndividualPopulation(parent),
  m_parameters(this)
{
    _STACK;

}


/**** squillero is working on it!

void EnhancedPopulation::evaluate()
{
    _STACK;

    LOG_VERBOSE << "About to evaluate the whole population..." << ends;

    // Step one: 
    // check which individuals must be evaluated (clone individuals won't be considered)
    unsigned int evaluatedIndividuals = 0;
    for(int i = 0; i < this->getIndividualCount(); ++i)
    {
        if(this->getIndividual(i).getFitness().getIsValid() == true) {
            LOG_INFO << "Individual " << this->getIndividual(i) << " fitness: " << this->getIndividual(j).getFitness() << ends;
            continue; // already has a fitness
        }
        for(int j = 0; i < j; ++j) {
            if(this->getIndividual(i).getHashCode(purpose) == this->getIndividual(j).getHashCode(purpose)) {
                LOG_INFO << "Individual " << this->getIndividual(i) << " is a clone of " << this->getIndividual(j) << ends;
                continue; // clone of i!
            }
        }

        evaluatedIndividuals++;
        this->getIndividual(i).getFitness().invalidate();
        this->getIndividual(i).getScaledFitness().invalidate();     
        ((FitnessEvaluator*)this->getParameters().getEvaluator())->evaluate(this->getIndividual(i));
    }
    // force the evaluation of the individuals
    this->getParameters().getEvaluator()->flush();
}

****/

void EnhancedPopulation::handleClone(CandidateSolution& master, CandidateSolution& clone, unsigned int number, unsigned int total)
{
    LOG_DEBUG << "Computing fitness for clones of candidate solution " << master << "..." << ends;
    
    if (number == 0) {
        // For the master, the fitness is exactly the raw fitness
        master.getFitness().setScalingFactor(ScaledFitness::CLONE_SCALING, 1);
        master.getFitness().setAdditionalDescription(ScaledFitness::CLONE_SCALING,
            (total > 1 ? " [master of " + ugp3::Convert::toString(total) + " clones]" : ""));
    } else {
        // !! if you want to change this formula you must change also the inverted formula in natural selection
        double scalingFactor = pow(getParameters().getCloneScalingFactor(), number);
        // TODO check that the formula is still the same in natural selection
        clone.getFitness().setScalingFactor(ScaledFitness::CLONE_SCALING, scalingFactor);
        clone.getFitness().setCloneCount(total); // FIXME why? Who uses it?
        clone.getFitness().setAdditionalDescription(ScaledFitness::CLONE_SCALING,
            " [clone #" + ugp3::Convert::toString(total) + " of " + master.toString() + "]");
    }
}

void EnhancedPopulation::prepareForCommit()
{
    handleClones(m_individuals.begin(), m_individuals.end());
    
    SpecificIndividualPopulation::prepareForCommit();
}


EnhancedPopulation::~EnhancedPopulation()  
{
    _STACK;
    
    LOG_DEBUG << "Destructor: ugp3::core::EnhancedPopulation" << ends;
}

