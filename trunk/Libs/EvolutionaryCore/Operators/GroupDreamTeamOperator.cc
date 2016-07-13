/***********************************************************************\
|                                                                       |
| GroupDreamTeamOperator.cc |
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
 * @file GroupDreamTeamOperator.cc
 *
 */

#include "GroupDreamTeamOperator.h"
#include <GroupPopulation.h>
#include <Debug.h>

namespace ugp3 {
namespace core {
    
void GroupDreamTeamOperator::generate(const std::vector< Group* >& parents, std::vector< Group* >& outChildren, GroupPopulation* population) const
{
    // TODO speak more
    Assert(parents.empty());
    
    // First: determine group size
    unsigned int min = population->getParameters().getMinIndividualsPerGroup();
    unsigned int max = population->getParameters().getMaxIndividualsPerGroup();
    unsigned int targetSize = 0;
    
    // Get an idea of a good group size by sampling some good groups from the population
    std::vector<Group*> groupSample = population->getParameters().groupSelection(5, 1.0);
    
    if (groupSample.empty()) {
        double meanSize = 0;
        for (Group* g: groupSample) {
            meanSize += g->getIndividualCount();
        }
        meanSize /= groupSample.size();
        targetSize = meanSize * Random::nextNormal(population->getParameters().getSigma());
        targetSize = std::max(min, std::min(max, targetSize)); // clamp
    } else {
        // No groups in population, weird but anyway:
        targetSize = Random::nextUInteger(min, max);
    }
    
    // Second: select individuals
    TwoStepTournamentSelectionWithFitnessHole selector;
    // TODO maybe try to take tau from population (if it uses a TournamentSelection...)
    selector.setTauMax(4);
    selector.setTau(4);
    std::vector<CandidateSolution*> selected = selector.selectCandidates(
        population->getIndividualsConstBegin(), population->getIndividualsConstEnd(),
        *population, targetSize, 1.0);
    
    unique_ptr<Group> newGroup(new Group(population->getGeneration(), *population));
    for (CandidateSolution* cand: selected) {
        GEIndividual* ind = dynamic_cast<GEIndividual*>(cand);
        Assert(ind);
        newGroup->addIndividual(ind);
        ind->addGroup(newGroup.get());
    }
    
    // TODO maybe handle the case in which there are not enough individuals in the population
    
    if (newGroup->validate()) {
        newGroup->getLineage().set(getName(), std::vector<Group*>());
        outChildren.push_back(newGroup.release());
    }
}

}
}