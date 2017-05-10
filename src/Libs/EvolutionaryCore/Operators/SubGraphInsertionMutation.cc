/***********************************************************************\
|                                                                       |
| SubGraphInsertionMutationOperator.cc                                  |
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
#include "Operators/SubGraphInsertionMutation.h"
using namespace std;
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;


SubGraphInsertionMutationOperator::SubGraphInsertionMutationOperator()
{ }

void SubGraphInsertionMutationOperator::generate(const std::vector< Individual* >& parents,
                          std::vector< Individual* >& outChildren,
                          IndividualPopulation& population) const
{ 
    _STACK;

    double sigma = population.getParameters().getSigma();

    // there is only one parent
    Individual& parent = *parents[0];
    Assert(parent.validate() == true);

    // clone the parent
    unique_ptr<Individual> child = parent.clone();
    LOG_VERBOSE << this << ": created individual " << child->toString() << " from parent " <<  parent << ends;

    // set the lineage for the new individual
    child->getLineage().set(this->getName(), parents);

    OperatorToolbox toolbox(child->getGraphContainer());
    do
    {
    	// select a graph of the individual
        CGraph* graph = toolbox.getRandomGraph();
        Assert(graph != nullptr);

		// randomly select a subSection type from those available for the selected graph
        unsigned int randomSample = Random::nextUInteger(0, graph->getSection().getSubSectionCount() - 1);
        const SubSection& subSection = graph->getSection().getSubSection(randomSample);

		// create a subGraph from the selected subSection and attach it to the graph
        CSubGraph* subGraph = new CSubGraph(*graph);
		subGraph->setConstrain(subSection);
		subGraph->buildRandom();
        bool success = graph->attachSubGraph(*subGraph);
       
		if(success == false)
        {
            delete subGraph;
			return;
        }
    }
    while(sigma > Random::nextDouble());

    bool success = child->getGraphContainer().attachFloatingEdges();
    if(success == false)
    {
    	 return;
    }

    LOG_VERBOSE << this << ": mutant " << child->toString() << " created" << ends;
    Assert(child->validate() == true);

    outChildren.push_back(child.release());
}

const string SubGraphInsertionMutationOperator::getName() const
{
    return "subGraphInsertionMutation";
}

const string SubGraphInsertionMutationOperator::getAcronym() const
{
    return "SG.INS.M";
}

const string SubGraphInsertionMutationOperator::getDescription() const
{
	return "" + this->getName() + " creates a child individual by cloning the parent, then inserts a whole random subgraph (instance of a subsection) at a random valid position. Subsequently, a random value in [0,1] is generated. If the value is lower than the current sigma, the process is repeated. If the resulting individual show a non-valid number of subgraphs, the operator fails. It is important to notice that this operator WILL NOT WORK on individuals with a single subsection, or with a fixed number of subsections.";
}
