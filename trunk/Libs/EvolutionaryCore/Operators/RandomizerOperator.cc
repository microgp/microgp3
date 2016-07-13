/***********************************************************************\
|                                                                       |
| RandomizerOperator.h                                                  |
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
#include "EvolutionaryCore.h"
#include "Operators/RandomizerOperator.h"
using namespace std;
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;


RandomizerOperator::RandomizerOperator()
{ }

void RandomizerOperator::generate(const std::vector< Individual* >& parents, std::vector< Individual* >& outChildren, IndividualPopulation& population) const
{
    _STACK;

    // there is only one parent, used to access the population
    LOG_VERBOSE << "Creating a random individual..." << ends;
    
    // Create a new random individual.
    unique_ptr<Individual> individual;
    double attempts = 0;
    do {
        attempts++;
        individual = population.newRandomIndividual();
    } while (!individual->validate());
    
    // set lineage (useless, but kept for coherence with the rest)
    individual->getLineage().set(this->getName(), std::vector<Individual*>{});
    
    LOG_VERBOSE << this << ": succeeded after " << attempts << " attempts" << ends;
	
    outChildren.push_back(individual.release());
}

const string RandomizerOperator::getName() const
{
    return "randomizer";
}

const string RandomizerOperator::getAcronym() const
{
    return "RND";
}

unsigned int RandomizerOperator::getParentsCardinality() const
{
    return 0;
}

const string RandomizerOperator::getDescription() const
{
	return "" + this->getName() + " generates a new random individual starting from the constraints' description, just as individuals in \"population 0\" are created. As the evolution goes on, individuals created by this operator are probably going to be killed very soon, as their fitness will not be on par with individuals in the current population. Nevertheless, it could be used to maintain diversity inside the population. As a general advice, it's best to limit the activation probability of this operator, setting both a minimum and a maximum threshold: otherwise, it might not be activated very often in later generations.";
}
