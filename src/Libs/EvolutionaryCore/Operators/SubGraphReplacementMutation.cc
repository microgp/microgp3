/***********************************************************************\
|                                                                       |
| SubGraphReplacementMutationOperator.cc                                |
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
#include "Operators/SubGraphReplacementMutation.h"
using namespace std;
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;


SubGraphReplacementMutationOperator::SubGraphReplacementMutationOperator()
{ }

void SubGraphReplacementMutationOperator::generate(const std::vector< Individual* >& parents,
                          std::vector< Individual* >& outChildren,
                          IndividualPopulation& population) const
{ 
    _STACK;

    double sigma = population.getParameters().getSigma();

    // there is only one parent
    Individual& parent = *parents[0];
    Assert(parent.validate() == true);
    if (parent.getGraphContainer().getCGraphCount() == 1
        && parent.getGraphContainer().getCGraph(0).getSubGraphCount() == 1) {
        LOG_DEBUG << "Only one subgraph, do not randomize, failing instead." << std::ends;
        return;
    }
    
    // clone the parent
    unique_ptr<Individual> child = parent.clone();
    LOG_VERBOSE << this << ": created individual " << child->toString() << " from parent " <<  parent << ends;
    
    // set the lineage for the new individual
    child->getLineage().set(this->getName(), parents);

    OperatorToolbox toolbox(child->getGraphContainer());

    do
    {
        CGraph* graph = toolbox.getRandomGraph();
        CSubGraph* subGraphToReplace = toolbox.getRandomSubGraph(*graph);
        
        CSubGraph* newSubGraph = new CSubGraph(*graph);
        /*
         * Choose a subsection (constraint) for the new subgraph.
         */
        const SubSection& subsection = subGraphToReplace->getSubSection();
        const size_t subGraphCount = graph->getSubGraphs(subsection.getId()).size();
        if (subGraphCount == subsection.getMinimumInstances()) {
            /* Case 1: if we can't remove the selected subgraph, we must
             * replace it with a new instance of the same subsection type.
             */
            newSubGraph->setConstrain(subsection);
        } else {
            /* Case 2: we can remove the selected subgraph. Choose a suitable
             * subsection type for the replacement. There will always be at
             * least one possibility: use the same as before.
             */
            vector<const SubSection*> candidates{&subsection};
            for (unsigned int i = 0; i < graph->getSection().getSubSectionCount(); ++i) {
                SubSection& subsec_i = graph->getSection().getSubSection(i);
                size_t count_i = graph->getSubGraphs(subsec_i.getId()).size();
                if (&subsec_i != &subsection
                    && count_i < subsec_i.getMaximumInstances()) {
                    candidates.push_back(&subsec_i);
                }
            }
            Assert(!candidates.empty());
            auto randomSample = Random::nextUInteger(0, candidates.size() - 1);
            newSubGraph->setConstrain(*candidates[randomSample]);
        }
        newSubGraph->buildRandom();

        bool done = graph->replaceSubGraph(subGraphToReplace, newSubGraph);
        if(done == true)
        {
            delete subGraphToReplace;
        }
        else
        {
            delete newSubGraph;
            return;
        }
    }
    while(sigma > Random::nextDouble());


    bool success = child->getGraphContainer().attachFloatingEdges();
    if(success == false) return;

    Assert(child->validate() == true);

    outChildren.push_back(child.release());
}

const string SubGraphReplacementMutationOperator::getName() const
{
    return "subGraphReplacementMutation";
}

const string SubGraphReplacementMutationOperator::getAcronym() const
{
    return "SG.RPL.M";
}

const string SubGraphReplacementMutationOperator::getDescription() const
{
	
	return "" + this->getName() + " creates a child individual by cloning the parent, then selects a random subgraph (instance of a subsection) inside the child and replaces it with a new random subgraph. Subsequently, a random value in [0,1] is generated. If the value is lower than the current sigma, the process is repeated. It is important to notice that, when individuals possess only one subgraph, this operator would basically create completely random individuals, by replacing the only subgraph with a random one, so instead it fails.";
}
