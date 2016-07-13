/***********************************************************************\
|                                                                       |
| MOIndividual.cc                                                       |
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

using namespace std;
using namespace ugp3::core;

MOIndividual::MOIndividual(const MOIndividual& individual)
: Individual(individual),
m_level(-1),
m_previousLevel(-1),
m_strength(0)
{
}

MOIndividual::MOIndividual(const Population& population)
: Individual(population), m_level(-1), m_previousLevel(-1),m_strength(0)
{
}

MOIndividual::MOIndividual(
    long unsigned int birth, const string& geneticOperator, const vector< string >& parents, const Population& population
              )
   : Individual(birth, geneticOperator, parents, population),
   m_level(-1),
   m_previousLevel(-1),
   m_strength(0)
{
}

unique_ptr<Individual> MOIndividual::clone() const
{
    return unique_ptr<Individual> ( new MOIndividual(*this) );
}

const string MOIndividual::getDisplayTypeName() const
{
    return TypeName< MOIndividual >::name;
}

