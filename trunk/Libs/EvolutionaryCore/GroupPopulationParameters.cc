/***********************************************************************\
|                                                                       |
| GroupPopulationParameters.cc                                          |
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
 * @file GroupPopulationParameters.cc
 * Implementation of the GroupPopulationParameters class.
 * @see GroupPopulationParameters.h
 * @see GroupPopulationParameters.xml.cc
 */

#include "GroupPopulationParameters.h"
#include "GroupPopulation.h"

using namespace ugp3::core;
using namespace std;

GroupPopulationParameters::GroupPopulationParameters(GroupPopulation* population)
: SpecificIndividualPopulationParameters(population)
{
}

GroupPopulationParameters::~GroupPopulationParameters()
{
    
}

std::vector< Group* > GroupPopulationParameters::groupSelection(
    unsigned int cardinality, double pressureMultiplier) const
{
    GroupPopulation* groupPop = dynamic_cast<GroupPopulation*>(getPopulation());
    Assert(groupPop);
    std::vector<CandidateSolution*> selected = getSelector().selectCandidates(
        groupPop->getGroupsConstBegin(), groupPop->getGroupsConstEnd(),
        *getPopulation(), cardinality, pressureMultiplier);
    std::vector<Group*> selectedGroups;
    for (auto candidate: selected) {
        Group* group = dynamic_cast<Group*>(candidate);
        Assert(group);
        selectedGroups.push_back(group);
    }
    return selectedGroups;
}


    
    