/***********************************************************************\
|                                                                       |
| GroupKFoldScanRemovalOperator.cc |
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
 * @file GroupKFoldScanRemovalOperator.cc
 *
 */

#include "GroupKFoldScanRemovalOperator.h"
#include <GroupPopulationParameters.h>
#include <GroupPopulation.h>

using namespace std;

namespace ugp3 {
namespace core {

void GroupKFoldScanRemovalOperator::generate(const std::vector< Group* >& parents,
                                           std::vector< Group* >& outChildren,
                                           GroupPopulation* population) const
{
    const GroupPopulationParameters &parameters = population->getParameters();
    double sigma = parameters.getSigma();

    LOG_DEBUG << "Now entering GroupKFoldScanRemovalOperator::generate()" << ends;

    if (sigma <= 0 || sigma >= 1) {
        throw ArgumentException("sigma should be in (0, 1)", LOCATION);
    }

    if (parents.size() != this->getParentsCardinality()) {
        throw ArgumentException("The number of input arguments (parents count) for the genetic operator \"" + this->getName() + "\" is not correct.", LOCATION);
    }

    // Get the two parents
    Group *parent1 = parents[0];

    // Choose the slice size
    // Order parents: we will slice the biggest, parent1
    unsigned int min = parameters.getMinIndividualsPerGroup();
    unsigned int maxSliceSize = parent1->getIndividualCount() - min;
    if (maxSliceSize == 0)
        return;
    
    unsigned int sliceSize = 1;
    while (ugp3::Random::nextDouble(0, 1) < sigma && sliceSize < maxSliceSize)
        ++sliceSize;
    
    LOG_DEBUG << "min " << min << ", p1 " << parent1->getIndividualCount() 
    << ", maxSliceSize " << maxSliceSize << ", sliceSize " << sliceSize << ", sigma " << sigma << ends;
    
    // For each slice, clone the parents and remove the slice
    vector<GEIndividual*> inds1(parent1->getIndividuals());
    // NOTE/DET sort by id before shuffling
    std::sort(inds1.begin(), inds1.end(), CandidateSolution::OrderById());
    ugp3::Random::shuffle(inds1.begin(), inds1.end());
    
    const std::string& tag = CandidateSolution::allopatricTagCounter.toString();
    CandidateSolution::allopatricTagCounter++;
    auto it = inds1.begin();
    while (it != inds1.end()) {
        // clone parents
        LOG_DEBUG << "About to clone the parent1, group " << parents[0] << ends;
        unique_ptr<Group> child1(parent1->clone());
        LOG_DEBUG << "Now adding individuals to group " << *child1 << ", child of group " << parent1 << ends;
        
        for (unsigned int i = 0; it != inds1.end() && i < sliceSize; ++i) {
            bool done = removeIndividual(child1.get(), *it);
            assert(done);
            ++it;
        }
        
        LOG_DEBUG << "Size of the child " << *child1 << " is now " << child1->getIndividualCount() << ends;
        
        LOG_VERBOSE << this->getName() << ": created group " << *child1 << " from parent " <<  *parent1 << ends;
        child1->getLineage().set(this->getName(), parents);
        child1->setAllopatricTag(tag);
        outChildren.push_back(child1.release());
    }
}

const std::string GroupKFoldScanRemovalOperator::getName() const
{
    return "groupKFoldScanRemoval";
}

const std::string GroupKFoldScanRemovalOperator::getAcronym() const
{
    return "G.KFS.R";
}

const std::string GroupKFoldScanRemovalOperator::getDescription() const
{
    return getName() + " selects one group, slices it in K parts, and creates K subgroups each missing one of the parts. The goal is to eliminate useless individuals from the given group. Only active during group evolution.";
}

}
}
