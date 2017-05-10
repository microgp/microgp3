/***********************************************************************\
|                                                                       |
| RemovalMutationOperator.cc                                            |
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
#include "EvolutionaryCore.h"
#include "Operators/RemovalMutation.h"
using namespace std;
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;


RemovalMutationOperator::RemovalMutationOperator()
{ }

void RemovalMutationOperator::generate(const std::vector< Individual* >& parents,
                          std::vector< Individual* >& outChildren,
                          IndividualPopulation& population) const
{ 
    _STACK;

    double sigma = population.getParameters().getSigma();

    // there is only one parent
    Individual* parent = parents[0];
    Assert(parent->getGraphContainer().validate() == true);

    // clone the parent
    unique_ptr<Individual> child = parent->clone();
    LOG_DEBUG << this->getName() << ": created individual " << child->toString() << " from parent " <<  parent << ends;

    // set the lineage for the new individual
    child->getLineage().set(this->getName(), parents);

    OperatorToolbox toolbox(child->getGraphContainer());
    CGraph* graph = toolbox.getRandomGraph();
    CSubGraph* subGraph = toolbox.getRandomSubGraph(*graph);
    if(subGraph == nullptr) 
        return;
	
    // mute the individual again and again, as long as sigma > rand(0,1)
    do
    {
        bool done = toolbox.removeRandomNode(*subGraph);

        if(done == false) return;
    }
    while(sigma > Random::nextDouble());

    if(subGraph->validateConstraints() == false) return;
    
    bool success = child->getGraphContainer().attachFloatingEdges();
    if(success == false) 
        return;

    outChildren.push_back(child.release());
}

const string RemovalMutationOperator::getName() const
{
    return "removalMutation";
}

const string RemovalMutationOperator::getAcronym() const
{
    return "RMV.M";
}

const string RemovalMutationOperator::getDescription() const
{
	return "" + this->getName() + " creates a child individual by cloning the parent, then randomly selects a node (macro instance) inside the child, and removes it. Subsequently, a random value in [0,1] is generated. If the value is lower than the current sigma, the operation is repated. If the resulting individual is smaller than the minimum allowed size, the operator fails. It is important to notice that this operator WILL NOT WORK with fixed-size individuals.";
}
