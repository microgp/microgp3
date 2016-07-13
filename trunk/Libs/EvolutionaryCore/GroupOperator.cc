/***********************************************************************\
|                                                                       |
| GroupOperator.cc                                                      |
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
 * @file GroupOperator.cc
 * Implementation of the GroupOperator class.
 * @see GroupOperator.h
 */

#include "EvolutionaryCore.h"

using namespace ugp3::core;
using namespace std;

const std::string GroupOperator::TYPE = "groupOperator";

bool GroupOperator::addIndividual(Group* child, GEIndividual* individual) const
{
    if (child->addIndividual(individual)) {
        if (individual->safeAddGroup(child)) {
            return true;
        }
        // Rollback
        child->removeIndividual(individual);
    }
    return false;
}

bool GroupOperator::removeIndividual(Group* child, GEIndividual* individual) const
{
    if (child->removeIndividual(individual)) {
        if (individual->safeRemoveGroup(child)) {
            return true;
        }
        // Rollback
        child->addIndividual(individual);
    }
    return false;
}

bool GroupOperator::moveIndividual(Group* child1, GEIndividual* individual, Group* child2) const
{
    if (removeIndividual(child1, individual)) {
        if (addIndividual(child2, individual)) {
            return true;
        }
        addIndividual(child1, individual);
    }
    return false;
}

bool GroupOperator::swapIndividuals(
    Group* child1, GEIndividual* ind1,
    Group* child2, GEIndividual* ind2
) const
{
    LOG_DEBUG << getName() << ": Swapping individuals..." << ends;

    if (moveIndividual(child1, ind1, child2)) {
        if (moveIndividual(child2, ind2, child1)) {
            LOG_DEBUG << getName() << ": Individuals swapped by reference" << ends;
            return true;
        }
        moveIndividual(child2, ind1, child1);
    }
    LOG_DEBUG << getName() << ": Swapping individuals failed!" << ends;
    return false;
}

void GroupOperator::release(vector<Group*>& groups)
{
    for(unsigned int i= 0; i < groups.size(); i++)
    {
        delete groups[i];
    }
}

bool GroupOperator::isApplicable(const Population* population)
{
    return dynamic_cast<const GroupPopulation*>(population);
}


void GroupOperator::apply(Population& population, std::vector< CandidateSolution* >& outChildren) const
{
    GroupPopulation& groupPop = dynamic_cast<GroupPopulation&>(population);
    vector<Group*> outGroups;
    const vector<Group*> parents = groupPop.getParameters().groupSelection(getParentsCardinality(), 1.0);
    if (parents.size() != getParentsCardinality()) {
        LOG_WARNING << "Operator " << getName() << " failed because it could not select enough parents ("
        << parents.size() << " returned vs " << getParentsCardinality() << " requested)" << std::ends;
        return;
    }
    generate(parents, outGroups, &groupPop);
    // Only return valid groups
    // TODO update operators so that they only produce valid groups and turn this into an assert
    for (auto group: outGroups) {
        if (group->validate()) {
            outChildren.push_back(group);
        }
    }
}





