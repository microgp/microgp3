/***********************************************************************\
|                                                                       |
| InsertionMutationOperator.cc                                          |
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
#include "Operators/InsertionMutation.h"
using namespace std;
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;


InsertionMutationOperator::InsertionMutationOperator()
{ }

void InsertionMutationOperator::generate(const std::vector< Individual* >& parents,
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
    LOG_DEBUG << this << ": created individual " << child->toString() << " from parent " <<  parent << ends;

    // set the lineage for the new individual
    child->getLineage().set(this->getName(), parents);

    OperatorToolbox toolbox(child->getGraphContainer());
    CGraph* graph = toolbox.getRandomGraph();
    CSubGraph* subGraph = toolbox.getRandomSubGraph(*graph);
    if(subGraph == nullptr) 
        return;
        
    bool success;
    do
    {
        success = toolbox.insertRandomNode(*subGraph);
		if(success == false)
			return;
    }
    while(sigma > Random::nextDouble());

    if(subGraph->validateConstraints() == false) return;
    
    success = child->getGraphContainer().attachFloatingEdges();
    if(success == false)
    {
        return;
    }

    LOG_DEBUG << this << ": mutant " << child->toString() << " created" << ends;

    outChildren.push_back(child.release());
}

const string InsertionMutationOperator::getName() const
{
    return "insertionMutation";
}

const string InsertionMutationOperator::getAcronym() const
{
    return "INS.M";
}

const string InsertionMutationOperator::getDescription() const
{
	return "" + this->getName() + " creates a new individual by cloning the parent and adding a randomly generated node (macro instance) in a random point. Then, a random value [0,1] is generated. If the value is lower than the current sigma, the process is repeated. If the resulting individual is too big (with respect to the constraints) it will be killed. It is important to notice that this operator WILL NOT WORK with individuals of FIXED LENGTH.";

}
