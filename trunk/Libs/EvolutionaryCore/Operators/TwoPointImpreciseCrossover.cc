/***********************************************************************\
|                                                                       |
| TwoPointImpreciseCrossoverOperator.cc                                 |
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
#include "Operators/TwoPointImpreciseCrossover.h"
#include <IndividualPopulationParameters.h>
#include <IndividualPopulation.h>
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;
using namespace std;

TwoPointImpreciseCrossoverOperator::TwoPointImpreciseCrossoverOperator()
{ }

void TwoPointImpreciseCrossoverOperator::generate(
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
    unique_ptr<Individual> child1 = parent1->clone();
    unique_ptr<Individual> child2 = parent2->clone();
    LOG_DEBUG << this->getName() << ": created two individuals (" <<
        child1->toString() << " and " << child2->toString() << ") from parents " <<
        parent1 << " and " << parent2 << ends;


    // Set the lineage of the children
    child1->getLineage().set(this->getName(), parents);
    child2->getLineage().set(this->getName(), parents);


	// Select two sub-graphs (of the same type) on the children
	// and choose two cut points for each sub-graph
	// (excluding prologue and epilogue)
	// and swap the nodes inside the two cut points
	// between the two individuals
    OperatorToolbox toolbox1(child1->getGraphContainer());
    OperatorToolbox toolbox2(child2->getGraphContainer());
    if (toolbox1.getNodesWithParameter<ugp3::constraints::OuterLabelParameter>().empty()
        && toolbox2.getNodesWithParameter<ugp3::constraints::OuterLabelParameter>().empty()) {
        LOG_VERBOSE << "No outer label in any of the two parents, failing." << std::ends;
        return;
    }

    // step1: select a subgraph to slice on the first individual
    LOG_DEBUG << "Selecting a subgraph to slice on the first individual" << ends;
    CGraph* selectedGraph1 = toolbox1.getRandomGraph();
    CSubGraph* selectedSubGraph1 = toolbox1.getRandomSubGraph(*selectedGraph1);
    if(selectedSubGraph1 == nullptr)
        return;

    // step2: select a compatible subgraph on the other individual
    LOG_DEBUG << "Selecting a compatible subgraph on the other individual" << ends;
    CSubGraph* selectedSubGraph2 = child2->getGraphContainer().getRandomSubGraph(
        selectedGraph1->getSection().getId(), selectedSubGraph1->getSubSection().getId());
    if(selectedSubGraph2 == nullptr)
        return;

    // step3: select the position and size of the slice on the first individual
    //        (exclude prologue and epilogue)
    LOG_DEBUG << "Selecting the position and size of the slice on the first individual (subgraph " << selectedSubGraph1 << ")" << ends;
    int cutPoint1child1 = Random::nextUInteger(1, selectedSubGraph1->getSize() - 1 - 1);// exclude prologue and epilogue
    int cutPoint2child1 = Random::nextUInteger(1, selectedSubGraph1->getSize() - 1 - 1);// exclude prologue and epilogue

    if(cutPoint1child1 > cutPoint2child1)
    {
        int tempCutPoint = cutPoint1child1;
        cutPoint1child1 = cutPoint2child1;
        cutPoint2child1 = tempCutPoint;
    }

    unsigned int slice1Size = cutPoint2child1 - cutPoint1child1 + 1;
    LOG_DEBUG << "Slice on subgraph " << selectedSubGraph1 << " will be " << slice1Size << ends;

    // step4: choose a valid size for the slice on the other individual
    unsigned int maxSize = selectedSubGraph1->getSubSection().getMaximumMacroInstances(); // maximum macro count, no prologue and epilogue
    unsigned int minSize = selectedSubGraph1->getSubSection().getMinimumMacroInstances(); // minimum macro count, no prologue and epilogue
    LOG_DEBUG << "Size of the subgraphs must be in [" << minSize << ", " << maxSize << "]" << ends;

    unsigned int size1 = selectedSubGraph1->getSize() - 2; // exclude prologue and epilogue
    unsigned int size2 = selectedSubGraph2->getSize() - 2; // exclude prologue and epilogue
    LOG_DEBUG << "Actual size of the subgraphs is " << size1 << " and " << size2 << ends;

    // if one of the two subgraphs has size 0, the operators fails
    if( size1 == 0 || size2 == 0 )
    {
	LOG_WARNING << "Operator " << this->getName() << " cannot modify individuals with 0 macros in a subSection." << ends;
	return;
    }

    int maxSlice2 = min<int>(min<int>(maxSize - size1 + slice1Size , size2 + slice1Size - minSize ), size2);
    int minSlice2 = max<int>( max<int>(minSize - size1 + slice1Size , size2 + slice1Size - maxSize), 0);
    Assert( maxSlice2 >= minSlice2 );
    Assert( minSlice2 >= 0 );
    LOG_DEBUG << "The second slice must be in [" << minSlice2 << ", " << maxSlice2 << "]" << ends;

    unsigned long slice2Size = Random::nextUInteger(minSlice2, maxSlice2);
    LOG_DEBUG << "The second slice will be " << slice2Size << " nodes long" << ends;
    LOG_DEBUG << "cutpoint " << size2 - slice2Size << ends;
    unsigned long cutPoint1child2 = Random::nextUInteger(1, size2 - slice2Size + 1);
    unsigned long cutPoint2child2 = cutPoint1child2 + slice2Size - 1;

    LOG_DEBUG << "The cut points are " << cutPoint1child2 << " and " << cutPoint2child2 << ends;
    // step5: swap the slices
    LOG_DEBUG << "Swapping slices" << ends;
    this->swapSlices(
        *selectedSubGraph1, cutPoint1child1, cutPoint2child1,
        *selectedSubGraph2, cutPoint1child2, cutPoint2child2
    );

    // step6: restore the edges of the nodes involved in the replacement
    LOG_DEBUG << "Restoring the edges of the nodes" << ends;
    bool success = child1->getGraphContainer().attachFloatingEdges();
    if(success == false)
        return;

    success = child2->getGraphContainer().attachFloatingEdges();
    if(success == false)
        return;

    LOG_DEBUG << this << ": individual " << child1->toString() << " created" << ends;
    LOG_DEBUG << this << ": individual " << child2->toString() << " created" << ends;
    Assert(child1->getGraphContainer().validate() == true);
    Assert(child2->getGraphContainer().validate() == true);

    // step7: save the new individuals
    outChildren.push_back(child1.release());
    outChildren.push_back(child2.release());

    LOG_DEBUG << this << ": crossover succeeded" << ends;
}

const string TwoPointImpreciseCrossoverOperator::getName() const
{
    return "twoPointImpreciseCrossover";
}

const string TwoPointImpreciseCrossoverOperator::getAcronym() const
{
    return "2PI.X";
}

const string TwoPointImpreciseCrossoverOperator::getDescription() const
{
	
	return "" + this->getName() + " produces two children individuals from two parent individuals, by randomly selecting two points of cut inside corresponding subgraphs (instances of subsections) of the two individuals, and swapping the parts between the two points of cut. This operator DOES NOT restore external references (\"outerLabel\" parameters) correctly, differently from the not \"Imprecise\" crossover operator. It must be noted that, in absence of \"outerLabel\" parameters in the constraints, the behaviors of this operator and of its not \"Imprecise\" version would be the same, so instead this one fails.";
}
