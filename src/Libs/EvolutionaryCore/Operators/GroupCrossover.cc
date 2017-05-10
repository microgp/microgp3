/***********************************************************************\
|                                                                       |
| GroupCrossoverOperator.cc                                             |
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

#include "Operators/GroupCrossover.h"
#include <GroupPopulationParameters.h>
#include <GEIndividual.h>
#include <GroupPopulation.h>

#include <vector>
#include <algorithm>

using namespace std;
using namespace ugp3::core;


GroupCrossoverOperator::GroupCrossoverOperator()
{ }



void GroupCrossoverOperator::generate(const std::vector< Group* >& parents, std::vector< Group* >& outChildren, GroupPopulation* population) const
{
    const GroupPopulationParameters &parameters = population->getParameters();
    double sigma = parameters.getSigma();

    LOG_DEBUG << "Now entering GroupMultiPointCrossoverOperator::generate()" << ends;

    if (sigma <= 0 || sigma >= 1) {
        throw ArgumentException("sigma should be in (0, 1)", LOCATION);
    }

    if (parents.size() != this->getParentsCardinality()) {
        throw ArgumentException("The number of input arguments (parents count) for the genetic operator \"" + this->getName() + "\" is not correct.", LOCATION);
    }

    // Get the two parents
    Group *parent1 = parents[0];
    Group *parent2 = parents[1];

    if (parent1->getId() == parent2->getId()) {
        return;
    }

    // clone parents
    LOG_DEBUG << "About to clone the parent1, group " << parents[0] << ends;
    unique_ptr<Group> child1(parent1->clone());
    LOG_DEBUG << "Now adding individuals to group " << *child1 << ", child of group " << parent1 << ends;

    LOG_DEBUG << "About to clone the parent2 group " << parents[1] << ends;
    unique_ptr<Group> child2(parent2->clone());
    LOG_DEBUG << "Now adding individuals to group " << *child2 << ", child of group " << parent2 << ends;

    LOG_DEBUG << "Sigma is " << sigma << ends;
    
    /*
     * Determine individuals eligible for swapping
     *      ┌──────────────────────┐
     *      │  A   B   C   D       │
     *      │             ┏━━━━━━━━┿━━━━━━━━━━━━┓
     *      │ parent1     ┃ E  F   │    parent2 ┃
     *      └─────────────╂────────┘            ┃
     *                    ┃      G    H   I     ┃
     *                    ┗━━━━━━━━━━━━━━━━━━━━━┛
     * swappable1 = {A, B, C, D}
     * swappable2 = {G, H, I}
     */
    vector<GEIndividual*> inds1(parent1->getIndividuals());
    vector<GEIndividual*> inds2(parent2->getIndividuals());
    // NOTE/DET use ids for the following operations to get deterministic results
    sort(inds1.begin(), inds1.end(), CandidateSolution::OrderById());
    sort(inds2.begin(), inds2.end(), CandidateSolution::OrderById());
    vector<GEIndividual*> swappable1;
    set_difference(inds1.begin(), inds1.end(), inds2.begin(), inds2.end(), back_inserter(swappable1), CandidateSolution::OrderById());
    vector<GEIndividual*> swappable2;
    set_difference(inds2.begin(), inds2.end(), inds1.begin(), inds1.end(), back_inserter(swappable2), CandidateSolution::OrderById());
    
    if (swappable1.empty() || swappable2.empty()) {
        // Nothing to swap (one group is included in the other)
        LOG_DEBUG << "GroupCrossoverOperator: nothing to swap.";
        return;
    }
    
    ugp3::Random::shuffle(swappable1.begin(), swappable1.end());
    ugp3::Random::shuffle(swappable2.begin(), swappable2.end());
    
    unsigned int min = parameters.getMinIndividualsPerGroup();
    unsigned int max = parameters.getMaxIndividualsPerGroup();
    
    bool didSomething = doSwaps(min, max, sigma, child1.get(), child2.get(), swappable1, swappable2);
    if (!didSomething)
        return;
    
    LOG_DEBUG << "Size of the child " << *child1 << " is now " << child1->getIndividualCount() << ends;
    LOG_DEBUG << "Size of the child " << *child2 << " is now " << child2->getIndividualCount() << ends;

    LOG_VERBOSE << this->getName() << ": created group " << *child1 << " from parents " <<  *parent1 << " and " << *parent2 << ends;
    child1->getLineage().set(this->getName(), parents);
    outChildren.push_back(child1.release());
    
    LOG_VERBOSE << this->getName() << ": created group " << *child2 << " from parents " <<  *parent2 << " and " << *parent1 << ends;
    child2->getLineage().set(this->getName(), parents);
    outChildren.push_back(child2.release());

}

