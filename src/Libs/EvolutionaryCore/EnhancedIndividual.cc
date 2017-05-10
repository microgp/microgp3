/***********************************************************************\
|                                                                       |
| Enhancedindividual.cc                                                 |
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

#include "ugp3_config.h"

#include "EnhancedIndividual.h"
#include "Population.h"
#include "IndividualPopulation.h"

using namespace std;
using namespace ugp3::core;

EnhancedIndividual::EnhancedIndividual(const EnhancedIndividual& individual)
    : Individual(individual),
    m_scaledFitness(individual.getPopulation().getParameters().getFitnessParametersCount())
{ }

EnhancedIndividual::EnhancedIndividual(
    unsigned long birth, 
    const string& geneticOperator, 
    const vector<string>& parents, 
    const Population& population)
    : Individual(birth, geneticOperator, parents, population),
    m_scaledFitness(population.getParameters().getFitnessParametersCount())
{ }

EnhancedIndividual::EnhancedIndividual(const Population& population)
    : Individual(population),
    m_scaledFitness(population.getParameters().getFitnessParametersCount())
{ }


unique_ptr< Individual > EnhancedIndividual::clone() const
{
    return unique_ptr<Individual> ( new EnhancedIndividual(*this) );
}

// this is required for island models; the fitness structure could change from 
// one island to the next, so everything must be reset
void EnhancedIndividual::setFitnessStructure(Fitness fitness)
{
    Individual::setFitnessStructure(fitness);
    
    m_scaledFitness = ScaledFitness(this->getFitness().getValues().size()); 
}

const string EnhancedIndividual::getDisplayTypeName() const
{
    return TypeName< EnhancedIndividual >::name;
}

