/***********************************************************************\
|                                                                       |
| OnePointCrossoverOperator.cc                                          |
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
#include "Operators/OnePointCrossover.h"
#include "EvolutionaryCore.h"
using namespace ugp3::constraints;
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace tgraph;
using namespace std;


OnePointCrossoverOperator::OnePointCrossoverOperator()
{ }

void OnePointCrossoverOperator::generate(const std::vector< Individual* >& parents,
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
    LOG_DEBUG << this << ": created two individuals (" <<
        child1->toString() << " and " << child2->toString() << ") from parents " <<
        parent1 << " and " << parent2 << ends;

    // Set the lineage of the children
    child1->getLineage().set(this->getName(), parents);
    child2->getLineage().set(this->getName(), parents);

    OperatorToolbox toolbox1(child1->getGraphContainer());

    // ---- first slice
    CGraph* selectedGraph1 = toolbox1.getRandomGraph();
    CSubGraph* selectedSubGraph1 = toolbox1.getRandomSubGraph(*selectedGraph1);
    if(selectedSubGraph1 == nullptr) 
        return;

    // ---- second slice
    LOG_DEBUG 
        << "Selecting a random sub-graph on individual  " << child2->toString() 
        << " compatible with the previously selected sub-graph " << selectedGraph1 << " of individual " 
        << child1->toString() << ends;
        
    CSubGraph* selectedSubGraph2 = child2->getGraphContainer().getRandomSubGraph(
        selectedGraph1->getSection().getId(), selectedSubGraph1->getSubSection().getId());
    if(selectedSubGraph2 == nullptr) 
        return;

    // check the actual size of the two chosen subGraphs
    unsigned int size1 = selectedSubGraph1->getSize() - 2; // exclude prologue and epilogue
    unsigned int size2 = selectedSubGraph2->getSize() - 2; // exclude prologue and epilogue
    LOG_DEBUG << "Actual size of the subgraphs is " << size1 << " and " << size2 << ends;

    // if one of the two subgraphs has size 0, the operators fails
    if( size1 == 0 || size2 == 0 )
    {
	LOG_WARNING << "Operator " << this->getName() << " cannot modify individuals with 0 macros in a subSection." << ends;
	return;
    }
    
    // the cut point will be the index of the first node of the second half of the slice
    LOG_DEBUG << "Selecting a random cut point of subgraph " << selectedSubGraph1 << ends;
    unsigned int cutPoint1 = Random::nextUInteger(1, selectedSubGraph1->getSize() - 1);
    unsigned int cutPoint2 = this->getValidCutPoint(*selectedSubGraph1, cutPoint1, *selectedSubGraph2);

    LOG_DEBUG << "Cut point chosen: Individual1 -> " << cutPoint1 << ", Individual2 -> " << cutPoint2 << ends; 
    
    int slice1Size = selectedSubGraph1->getSize() - cutPoint1;
    int slice2Size = selectedSubGraph2->getSize() - cutPoint2;
    
    // collect and copy subgraphs
	bool success = this->swapSubGraphs(
	    *child1, *child2, 
	    *selectedSubGraph1, *selectedSubGraph2, 
	    cutPoint1, slice1Size, cutPoint2, slice2Size);
	    
	if(success == false) return;

    this->swapSlices(
        *selectedSubGraph1, cutPoint1, selectedSubGraph1->getSize() - 1,
        *selectedSubGraph2, cutPoint2, selectedSubGraph2->getSize() - 1);


    // important: restore the edges of the nodes involved in the replacement
    success &= child1->getGraphContainer().attachFloatingEdges();
    success &= child2->getGraphContainer().attachFloatingEdges();

    if(success == false)
    {
        LOG_DEBUG << "One or more edges of the children could not be restored" << ends;
        return;
    }

    LOG_DEBUG << this << ": individual " << child1->toString() << " created" << ends;
    LOG_DEBUG << this << ": individual " << child2->toString() << " created" << ends;

    // save the results
    outChildren.push_back(child1.release());
    outChildren.push_back(child2.release());
}

const string OnePointCrossoverOperator::getDescription() const
{
	return "" + this->getName() + " takes two parent individuals, and creates two children by recombining the parents using a single cut point. This operator restores external references (\"outerLabel\" parameters) correctly, differently from the \"Imprecise\" crossover operators. It must be noted that, in absence of \"outerLabel\" parameters in the constraints, the behaviors of this operator and of its \"imprecise\" version are the same."; 
}
