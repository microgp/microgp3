/***********************************************************************\
|                                                                       |
| GroupKFoldScanCrossoverOperator.cc |
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
 * @file GroupKFoldScanCrossoverOperator.cc
 *
 */

#include "GroupKFoldScanCrossoverOperator.h"
#include <GroupPopulationParameters.h>
#include <GroupPopulation.h>

using namespace std;

namespace ugp3 {
namespace core {

void GroupKFoldScanCrossoverOperator::generate(const std::vector< Group* >& parents,
                                           std::vector< Group* >& outChildren,
                                           GroupPopulation* population) const
{
    const GroupPopulationParameters &parameters = population->getParameters();
    double sigma = parameters.getSigma();

    LOG_DEBUG << "Now entering GroupKFoldCrossoverOperator::generate()" << ends;

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
    
    // Choose the slice size
    // Order parents: we will slice the biggest, parent1
    if (parent1->getIndividualCount() < parent2->getIndividualCount())
        std::swap(parent1, parent2);
    
    unsigned int min = parameters.getMinIndividualsPerGroup();
    unsigned int max = parameters.getMaxIndividualsPerGroup();
    unsigned int howManyParent1CanLose = parent1->getIndividualCount() - min;
    unsigned int howManyParent2CanTake = max - parent2->getIndividualCount();
    unsigned int maxSliceSize = std::min(howManyParent1CanLose, howManyParent2CanTake);
    if (maxSliceSize == 0)
        return;
    
    unsigned int sliceSize = 1;
    while (ugp3::Random::nextDouble(0, 1) < sigma && sliceSize < maxSliceSize)
        ++sliceSize;
    
    LOG_DEBUG << "min " << min << ", p1 " << parent1->getIndividualCount() << ", p2 " << parent2->getIndividualCount() << ", max " << max
    << ", maxSliceSize " << maxSliceSize << ", sliceSize " << sliceSize << ", sigma " << sigma << ends;
    
    // For each slice, clone the parents and move the slice from child1 to child2
    vector<GEIndividual*> inds1(parent1->getIndividuals());
    vector<GEIndividual*> inds2(parent2->getIndividuals());
    // NOTE/DET sort by id before shuffling
    std::sort(inds1.begin(), inds1.end(), CandidateSolution::OrderById());
    ugp3::Random::shuffle(inds1.begin(), inds1.end());
    std::sort(inds2.begin(), inds2.end(), CandidateSolution::OrderById());
    ugp3::Random::shuffle(inds2.begin(), inds2.end());
    
    const std::string& tag1 = CandidateSolution::allopatricTagCounter.toString();
    CandidateSolution::allopatricTagCounter++;
    const std::string& tag2 = CandidateSolution::allopatricTagCounter.toString();
    CandidateSolution::allopatricTagCounter++;
    auto it = inds1.begin();
    while (it != inds1.end()) {
        // clone parents
        LOG_DEBUG << "About to clone the parent1, group " << parents[0] << ends;
        unique_ptr<Group> child1(parent1->clone());
        LOG_DEBUG << "Now adding individuals to group " << *child1 << ", child of group " << parent1 << ends;
        
        LOG_DEBUG << "About to clone the parent2 group " << parents[1] << ends;
        unique_ptr<Group> child2(parent2->clone());
        LOG_DEBUG << "Now adding individuals to group " << *child2 << ", child of group " << parent2 << ends;
        
        for (unsigned int i = 0; it != inds1.end() && i < sliceSize; ++i) {
            bool done = removeIndividual(child1.get(), *it);
            assert(done);
            addIndividual(child2.get(), *it); // Maybe child2 already contains this individual, we don't care
            ++it;
        }
        
        LOG_DEBUG << "Size of the child " << *child1 << " is now " << child1->getIndividualCount() << ends;
        LOG_DEBUG << "Size of the child " << *child2 << " is now " << child2->getIndividualCount() << ends;
        
        LOG_VERBOSE << this->getName() << ": created group " << *child1 << " from parents " <<  *parent1 << " and " << *parent2 << ends;
        child1->getLineage().set(this->getName(), parents);
        child1->setAllopatricTag(tag1);
        outChildren.push_back(child1.release());
        
        LOG_VERBOSE << this->getName() << ": created group " << *child2 << " from parents " <<  *parent2 << " and " << *parent1 << ends;
        child2->getLineage().set(this->getName(), parents);
        child2->setAllopatricTag(tag2);
        outChildren.push_back(child2.release());
    }
}

const std::string GroupKFoldScanCrossoverOperator::getName() const
{
    return "groupKFoldScanCrossover";
}

const std::string GroupKFoldScanCrossoverOperator::getAcronym() const
{
    return "G.KFS.X";
}

const std::string GroupKFoldScanCrossoverOperator::getDescription() const
{
    return getName() + " selects two groups, slices the biggest in K parts, and moves each part to the a new clone of the other group. Only active during group evolution.";
}

}
}
