/***********************************************************************\
|                                                                       |
| ScanMutationInnerLabelOperator.cc                                               |
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
#include "Operators/ScanMutationInnerLabel.h"
using namespace std;
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;


ScanMutationInnerLabelOperator::ScanMutationInnerLabelOperator()
{ }

// scanMutation is currently working with:
// - inner(Forward|Backward)Label (generate one child for each possible target for the jump)
void ScanMutationInnerLabelOperator::generate(const std::vector< Individual* >& parents,
                          std::vector< Individual* >& outChildren,
                          IndividualPopulation& population) const
{
	_STACK;

	// there is only one parent
	Individual& parent = *parents[0];
	Assert(parent.validate() == true);

	// select the target (/graph/subGraph/macro/parameter)
	OperatorToolbox toolbox(parent.getGraphContainer());

	// select a graph
	CGraph* graph =  toolbox.getRandomGraph();
	if(graph == nullptr)
	{
		LOG_INFO << "No graph selected" << ends;
		return;
	}
	const string& sectionName = graph->getSection().getId();

	// select a subgraph
	CSubGraph* subGraph = toolbox.getRandomSubGraph(*graph);
	if(subGraph == nullptr)
	{
		LOG_INFO << "No subGraph selected" << ends;
		return;
	}

	// find index of chosen subgraph (used later to find the corresponding part in the children)
	unsigned int subGraphIndex = 0;
	for(unsigned int i = 0; i < graph->getSubGraphCount(); i++)
	{
		if(&graph->getSubGraph(i) == subGraph)
		{
			subGraphIndex = i;
		}
	}

	// select all candidate targets
	vector<CNode*> candidates;
	CNode* node = &subGraph->getPrologue();
	do
	{
		candidates.push_back(node);
    	}
	while((node = node->getNext()) != nullptr);

	// collect all the parameters in a random node (until at least one parameter is found)
	vector<Parameter*> params;
	do
	{
		// choose random node
		unsigned long randomSample = Random::nextUInteger(0, (unsigned long)(candidates.size() - 1));
		Assert(randomSample <= candidates.size() - 1);
		node = candidates[randomSample];
		
		// remove node from candidates
		candidates.erase(candidates.begin() + randomSample);
		params.clear();

		// collect all the available parameters for the selected macro
		for(unsigned int i = 0; i < node->getGenericMacro().getParameterCount(); i++)
		{
			// the only classes of parameters usable are: 
			// - InnerLabelParameter
			if( dynamic_cast<InnerLabelParameter*>(&node->getGenericMacro().getParameter(i)) != nullptr )
				params.push_back(&node->getGenericMacro().getParameter(i));
		}
	}
	while(params.size() == 0 && candidates.size() > 0);
	
	// if no parameters are found, return
	if( params.size() == 0 )
	{
		LOG_DEBUG << this << " : no appropriate parameters found" << ends;
		return;
	}

	// get the index of the selected node (used later to find the corresponding part in the children)
	unsigned int nodeIndex = 0;
	CNode* cursor = &subGraph->getPrologue();
	while(cursor != nullptr && cursor != node)
	{
		cursor = cursor->getNext();
		nodeIndex++;
	}

	// now, choose randomly the parameter to operate on and behave accordingly
	unsigned long randomSample = Random::nextUInteger(0, (unsigned long)(params.size() - 1));
	Assert(randomSample <= params.size() - 1);

	// first of all, the allopatric tag shared by all children individuals is chosen
	string allopatricTag = Individual::allopatricTagCounter.toString();
	Individual::allopatricTagCounter++;
	LOG_DEBUG << "All children produced by " << this << " will share the allopatric tag \"" << allopatricTag << "\"" << ends;

	// Terse output!
    	std::ostringstream terse;
	if( dynamic_cast<InnerLabelParameter*>(params[randomSample]) != nullptr ) 
	{
		// inner label parameter (to be checked if forward or backward)
		InnerLabelParameter* parameter = dynamic_cast<InnerLabelParameter*>(params[randomSample]);
		
		// find the range of indexes for all possible targets of the edge
		unsigned int minIndex = 0;
		unsigned int maxIndex = 0;
		
		cursor = &subGraph->getPrologue();
		while( cursor != nullptr ) 
		{
			maxIndex++;
			cursor = cursor->getNext();
		}

		// check if the prologue/epilogue are valid points
		if( ! parameter->getPrologueIsValid() ) minIndex++;
		if( ! parameter->getEpilogueIsValid() ) maxIndex--;	

		// is the label innerForward or innerBackward?
		if( ! parameter->getBackwardJumpIsValid() ) minIndex = nodeIndex;
		if( ! parameter->getForwardJumpIsValid() ) maxIndex = nodeIndex;

		LOG_DEBUG << this << " : possible values for innerLabel parameter: [" << minIndex << "," << maxIndex << "]" << ends;
	
		// for all legal nodes, except possibly the same node if itself == false
		for(unsigned int index = minIndex; index < maxIndex; index++)
		if( parameter->getItselfIsValid() == true || index != nodeIndex )
		{
			// clone the parent
			unique_ptr<Individual> child = parent.clone();
			terse << " " << child->toString();
		        LOG_DEBUG << this->getName() << ": created individual " << child->toString() << " from parent " <<  parent << ends;

			// set the lineage for the new individual
			child->getLineage().set(this->getName(), parents);

			// set allopatric tag
			child->setAllopatricTag(allopatricTag);
			LOG_DEBUG << "Individual " << child->toString() << " has now allopatric tag " << child->getAllopatricTag() << ends;

			// find corresponding parameter in the child
			CGraph* childGraph = child->getGraphContainer().getCGraph(sectionName);
			CSubGraph& childSubGraph = childGraph->getSubGraph(subGraphIndex);
			CNode* childCursor = &childSubGraph.getPrologue();
			unsigned int childNodeIndex = 0;
			while (childCursor && childNodeIndex != nodeIndex)
			{
			    childCursor = childCursor->getNext();
			    childNodeIndex++;
			}

			// find target for the jump
			CNode* targetCursor = &childSubGraph.getPrologue();
			unsigned int targetNodeIndex = 0;
			while (targetCursor && targetNodeIndex != index)
			{
			    targetCursor = targetCursor->getNext();
			    targetNodeIndex++;
			}
			
			if (childCursor && targetCursor) {
                // find edge in the child and set it to another value
                tgraph::Edge* edge = childCursor->getEdge( parameter->getName() );
                // TODO: maybe here the old edge must be removed, first
                
                bool success = child->getGraphContainer().attachFloatingEdges();
                if(success == true)
                {
                    edge->setTo( targetCursor );			
                    outChildren.push_back(child.release());
                    LOG_DEBUG << this << " : created child with arc to node #" << targetNodeIndex << ends;
                }
            }
		}
	}
	else
	{
		LOG_DEBUG << this << " : parameter " << params[randomSample]->toString() << " does not belong to the manageable parameters."  << ends;
		return;
	}

	LOG_VERBOSE << this->getName() << ": created individuals" << terse.str() << " from parent " <<  parent << ends;
}	

const string ScanMutationInnerLabelOperator::getName() const
{
	return "scanMutationINNERLABEL";
}

const string ScanMutationInnerLabelOperator::getAcronym() const
{
	return "SNC.M";
}

const string ScanMutationInnerLabelOperator::getDescription() const
{
	return "" + this->getName() + " performs a local quasi-exhaustive search in the proximity of the parent individual. One parameter of type \"innerLabel\" is chosen at random in the individual, and a child is produced for each possible value of the constant parameter (each other node the label can legally point to). All individuals produced will share the same allopatric tag. This means that (at the most) ONE of the offspring will survive in the slaughtering step."; 
}
