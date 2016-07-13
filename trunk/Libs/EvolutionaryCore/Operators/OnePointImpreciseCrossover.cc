/***********************************************************************\
|                                                                       |
| OnePointImpreciseCrossoverOperator.cc                                 |
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
#include "Operators/OnePointImpreciseCrossover.h"
#include <IndividualPopulationParameters.h>
#include <IndividualPopulation.h>
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace tgraph;
using namespace std;


OnePointImpreciseCrossoverOperator::OnePointImpreciseCrossoverOperator()
{ }

void OnePointImpreciseCrossoverOperator::generate(
    const std::vector< Individual* >& parents,
    std::vector< Individual* >& outChildren,
    IndividualPopulation& population) const
{
    _STACK;

    // Get the parents
    Individual *parent1 = parents[0];
    Individual *parent2 = parents[1];
    Assert(parent1->getGraphContainer().validate() == true);
    Assert(parent2->getGraphContainer().validate() == true);

    // Clone parents
    std::unique_ptr<Individual> child1 = parent1->clone();
    std::unique_ptr<Individual> child2 = parent2->clone();
    LOG_DEBUG << this << ": created two individuals (" <<
        child1->toString() << " and " << child2->toString() << ") from parents " <<
        parent1 << " and " << parent2 << std::ends;

    // Set the lineage
    child1->getLineage().set(this->getName(), parents);
    child2->getLineage().set(this->getName(), parents);

    OperatorToolbox toolbox1(child1->getGraphContainer());
    OperatorToolbox toolbox2(child2->getGraphContainer());
    if (toolbox1.getNodesWithParameter<ugp3::constraints::OuterLabelParameter>().empty()
        && toolbox2.getNodesWithParameter<ugp3::constraints::OuterLabelParameter>().empty()) {
        LOG_VERBOSE << "No outer label in any of the two parents, failing." << std::ends;
        return;
    }

    // ---- first slice
    LOG_DEBUG << "Selecting a random sub-graph on individual " << child1->toString() << ends;
    CGraph* selectedGraph1 = toolbox1.getRandomGraph();
    CSubGraph* selectedSubGraph1 = toolbox1.getRandomSubGraph(*selectedGraph1);
    if(selectedSubGraph1 == nullptr)
        return;
    LOG_DEBUG << "Selected the sub-graph " << selectedSubGraph1 << ends;

    // ---- second slice
    LOG_DEBUG
        << "Selecting a random sub-graph on individual  " << child2->toString()
        << " compatible with the previously selected sub-graph " << selectedGraph1 << " of individual "
        << child1->toString() << ends;
    CSubGraph* selectedSubGraph2 = child2->getGraphContainer().getRandomSubGraph(
        selectedGraph1->getSection().getId(), selectedSubGraph1->getSubSection().getId());
    if(selectedSubGraph2 == nullptr)
        return;
    LOG_DEBUG << "Selected the sub-graph " << selectedSubGraph2 << ends;

    // check the actual size of the two chosen subGraphs
    unsigned int size1 = selectedSubGraph1->getSize() - 2; // exclude prologue and epilogue
    unsigned int size2 = selectedSubGraph2->getSize() - 2; // exclude prologue and epilogue
    LOG_DEBUG << "Actual size of the subgraphs is " << size1 << " and " << size2 << ends;

    // if one of the two subgraphs has size 0, the operators fails
    if (size1 == 0 || size2 == 0)
    {
        LOG_WARNING << "Operator " << this->getName() << " cannot modify individuals with 0 macros in a subSection." << ends;
        return;
    }
    
    // the cut point is the index of the first node of the second half of the slice
    unsigned int cutPoint1 = Random::nextUInteger(1, selectedSubGraph1->getSize() - 1);

    // get the cut point on the second slice 
    unsigned int cutPoint2 = this->getValidCutPoint(*selectedSubGraph1, cutPoint1, *selectedSubGraph2);
	
    this->swapSlices(
        *selectedSubGraph1, cutPoint1, selectedSubGraph1->getSize() - 1,
        *selectedSubGraph2, cutPoint2, selectedSubGraph2->getSize() - 1);

    // important: restore the edges of the nodes involved in the replacement
    bool success = true;
    success &= child1->getGraphContainer().attachFloatingEdges();
    success &= child2->getGraphContainer().attachFloatingEdges();

    if(success == false)
    {
        LOG_DEBUG << "One or more edges of the children could not be restored" << ends;
        return;
    }

    LOG_DEBUG << this << ": individual " << child1->toString() << " created" << ends;
    LOG_DEBUG << this << ": individual " << child2->toString() << " created" << ends;
    Assert(child1->getGraphContainer().validate() == true);
    Assert(child2->getGraphContainer().validate() == true);

    // save the results
    outChildren.push_back(child1.release());
    outChildren.push_back(child2.release());

    LOG_DEBUG << this << ": crossover succeeded" << ends;
}



int OnePointImpreciseCrossoverOperator::getValidCutPoint(
    const CSubGraph& selectedSubGraph1, unsigned int cutPoint1,
    const CSubGraph& selectedSubGraph2) const
{
    _STACK;

    const int maxSize = selectedSubGraph1.getSubSection().getMaximumMacroInstances() + 1; // no prologue
    LOG_DEBUG << "The maximum size of the slice is " << maxSize << ends;

    const int minSize = selectedSubGraph1.getSubSection().getMinimumMacroInstances() + 1; // no prologue
    LOG_DEBUG << "The minimum size of the slice is " << minSize << ends;

    const unsigned int size1 = selectedSubGraph1.getSize() - 1; // no prologue
    LOG_DEBUG << "The size of the sub-graph " << selectedSubGraph1 << " is " << size1 << ends;

    const unsigned int size2 = selectedSubGraph2.getSize() - 1; // no prologue
    LOG_DEBUG << "The size of the sub-graph " << selectedSubGraph2 << " is " << size2 << ends;

    const int slice1Size = selectedSubGraph1.getSize() - cutPoint1;
    LOG_DEBUG << "The size of the slice on sub-graph " << selectedSubGraph1 << " is " << slice1Size << ends;

    unsigned int maxSlice2 = min<unsigned int>(min<unsigned int>(maxSize - size1 + slice1Size , size2 + slice1Size - minSize ), size2);
    LOG_DEBUG << "The new maximum size of the slice is " << maxSlice2 << ends;

    unsigned int minSlice2 = max<int>( max<int>(minSize - size1 + slice1Size , size2 + slice1Size - maxSize), 1);
    LOG_DEBUG << "The new minimum size of the slice is " << minSlice2 << ends;
    Assert( maxSlice2 >= minSlice2 );
    Assert( minSlice2 >= 1 );

    int slice2Size = Random::nextSInteger(minSlice2, maxSlice2);
    LOG_DEBUG << "The size of the slice on sub-graph " << selectedSubGraph2 << " is " << slice2Size << ends;

    const int cutPoint2 = size2 - slice2Size + 1;

    return cutPoint2;
}

const string OnePointImpreciseCrossoverOperator::getName() const
{
    return "onePointImpreciseCrossover";
}

const string OnePointImpreciseCrossoverOperator::getAcronym() const
{
    return "1PI.X";
}

const string OnePointImpreciseCrossoverOperator::getDescription() const
{
	return "" + this->getName() + " takes two parent individuals, and creates two children by recombining the parents using a single cut point. This operators DOES NOT restore external references (\"outerLabel\" parameters) correctly, differently from the non-\"Imprecise\" crossover operators. It must be noted that, in absence of \"outerLabel\" parameters in the constraints, the behaviors of this operator and of its \"precise\" version would be the same, so instead this one fails."; 
}
