/***********************************************************************\
|                                                                       |
| GroupUnionIntersectionOperator.cc |
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
 * @file GroupUnionIntersectionOperator.cc
 *
 */

#include "GroupUnionIntersectionOperator.h"

#include <GroupPopulationParameters.h>

namespace ugp3 {
namespace core {
    
bool GroupUnionIntersectionOperator::doSwaps(unsigned int min, unsigned int max, double sigma,
                                             Group* child1, Group* child2,
                                             std::vector< GEIndividual* >& swappable1, std::vector< GEIndividual* >& swappable2) const
{
    // We try to move as many individuals as possible from child1 to child2
    bool didSomething = false;
    while (!swappable1.empty() && child1->getIndividualCount() > min && child2->getIndividualCount() < max) {
        didSomething = moveIndividual(child1, swappable1.back(), child2);
        assert(didSomething);
        swappable1.pop_back();
        
        if (ugp3::Random::nextDouble(0, 1) >= sigma)
            break;
    }
    return didSomething;
}

const std::string GroupUnionIntersectionOperator::getName() const
{
    return "groupUnionIntersection";
}

const std::string GroupUnionIntersectionOperator::getAcronym() const
{
    return "G.UI";
}

const std::string GroupUnionIntersectionOperator::getDescription() const
{
    return getName() + " selects two groups and tries to build their intersection and their union. Only active during group evolution.";
}

}
}