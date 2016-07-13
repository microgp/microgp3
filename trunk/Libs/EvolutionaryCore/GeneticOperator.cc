/***********************************************************************\
|                                                                       |
| GeneticOperator.cc                                                    |
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
 * @file GeneticOperator.cc
 * Implementation of the GeneticOperator class.
 * @see GeneticOperator.h
 */

#include "ugp3_config.h"
#include "EvolutionaryCore.h"
using namespace ugp3::core;
using namespace std;

const std::string GeneticOperator::TYPE = "geneticOperator";

void GeneticOperator::release(vector<Individual*>& individuals)
{
    for (unsigned int i = 0; i < individuals.size(); i++) {
        delete individuals[i];
    }
}

bool GeneticOperator::isApplicable(const Population* population)
{
    return dynamic_cast<const IndividualPopulation*>(population);
}

void GeneticOperator::apply(Population& population, std::vector< CandidateSolution* >& newGeneration) const
{
    IndividualPopulation& pop = dynamic_cast<IndividualPopulation&>(population);
    std::vector<Individual*> outChildren;
    const vector<Individual*> parents = pop.getParameters().individualSelector(getParentsCardinality(), 1.0);
    if (parents.size() != getParentsCardinality()) {
        LOG_WARNING << "Operator " << getName() << " failed because it could not select enough parents ("
        << parents.size() << " returned vs " << getParentsCardinality() << " requested)" << std::ends;
        return;
    }
    generate(parents, outChildren, pop);
    newGeneration.insert(newGeneration.end(), outChildren.begin(), outChildren.end());
}




