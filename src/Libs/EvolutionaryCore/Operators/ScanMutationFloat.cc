/***********************************************************************\
|                                                                       |
| ScanMutationFloatOperator.cc                                               |
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
#include "Operators/ScanMutationFloat.h"
using namespace std;
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;


ScanMutationFloatOperator::ScanMutationFloatOperator()
{ }

// scanMutation is currently working with:
// - ranged float (generate several children, sampling the range, two for each iteration on sigma)
void ScanMutationFloatOperator::generate(const std::vector< Individual* >& parents,
                          std::vector< Individual* >& outChildren,
                          IndividualPopulation& population) const
{
	_STACK;

	double sigma = population.getParameters().getSigma();

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

	// select all the nodes that contain at least one integer parameter
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
			// the only class of parameters usable is: 
			// - FloatParameter
			if( dynamic_cast<FloatParameter*>(&node->getGenericMacro().getParameter(i)) != nullptr )
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
	if( dynamic_cast<FloatParameter*>(params[randomSample]) != nullptr ) 
	{
		// float ranged parameter
		FloatParameter* parameter = dynamic_cast<FloatParameter*>(params[randomSample]);

		// create a finite number of children, based on a sampling of the interval of the ranged parameter
		// and on the current value of sigma: the higher sigma, the more children produced

		LOG_DEBUG << this << ": possible values for float parameter: (TODO)" << ends;

		// two children individuals are created at each iteration
		do
		{
			// clone the parent
			unique_ptr<Individual> child1 = parent.clone();
			unique_ptr<Individual> child2 = parent.clone();
            		LOG_VERBOSE << " " << child1->toString() << " " << child2->toString();
            		LOG_DEBUG << this->getName() << ": created individual " << child1->toString() << " from parent " <<  parent << ends;
            		LOG_DEBUG << this->getName() << ": created individual " << child2->toString() << " from parent " <<  parent << ends;

            		// set the lineage for the new individualscreated individual
			child1->getLineage().set(this->getName(), parents);
			child2->getLineage().set(this->getName(), parents);

			// set the allopatric tag for the new individuals
			child1->setAllopatricTag(allopatricTag);
			child2->setAllopatricTag(allopatricTag);

			// retrieve the node
			CGraph* childGraph1 = child1->getGraphContainer().getCGraph(sectionName);
			CGraph* childGraph2 = child2->getGraphContainer().getCGraph(sectionName);

			Assert(childGraph1 != nullptr);
			Assert(childGraph2 != nullptr);

			CSubGraph& childSubGraph1 = childGraph1->getSubGraph(subGraphIndex);
			CSubGraph& childSubGraph2 = childGraph2->getSubGraph(subGraphIndex);

			CNode* cursor1 = &childSubGraph1.getPrologue();
			CNode* cursor2 = &childSubGraph2.getPrologue();
			unsigned int pos = 0;
			while(pos < nodeIndex)
			{
			    cursor1 = cursor1->getNext();
			    cursor2 = cursor2->getNext();
			    pos++;
			}

			CNode& childCursor1 = *cursor1;
			CNode& childCursor2 = *cursor2;

			Assert(childCursor1.getGenericMacro().getParameter(parameter->getName()) != nullptr);
			Assert(childCursor1.containsTag(CNode::Escape + parameter->getName()) == true);
			Assert(childCursor2.getGenericMacro().getParameter(parameter->getName()) != nullptr);
			Assert(childCursor2.containsTag(CNode::Escape + parameter->getName()) == true);

			// change the value for child 1
			Tag& tag1 = childCursor1.getTag(CNode::Escape + parameter->getName());
			double position1 = parameter->getPosition( tag1.getValue() );

			// choose a new value
			double newPosition = 0;
			do
			{
			    // use the mutation strength as the sigma of the distribution
			    newPosition = position1 + Random::nextNormal(sigma);
			}
			// take values between 0% and 100% of the range of the parameter
			while(newPosition > 1 || newPosition < 0);

			tag1.setValue(parameter->getAt(newPosition));
			if(child1->getGraphContainer().attachFloatingEdges() == true)
			{
			    outChildren.push_back(child1.release());
			    LOG_DEBUG << this << ": created child1 with value " << tag1.getValue() << ends;
			}

			// change the value for child 2
			Tag& tag2 = childCursor2.getTag(CNode::Escape + parameter->getName());
			// TODO: this is probably redundant, position1 == position2
			double position2 = parameter->getPosition( tag2.getValue() ); 

			// choose a new value
			newPosition = 0;
			do
			{
			    // use the mutation strength as the sigma of the distribution
			    newPosition = position2 - Random::nextNormal(sigma);
			}
			// take values between 0% and 100% of the range of the parameter
			while(newPosition > 1 || newPosition < 0);

			tag2.setValue(parameter->getAt(newPosition));
			if(child2->getGraphContainer().attachFloatingEdges() == true)
			{
			    outChildren.push_back(child2.release());
			    LOG_DEBUG << this << ": created child2 with value " << tag2.getValue() << ends;
			}
		
		}while(Random::nextDouble() <= sigma); 
	}
	else
	{
		LOG_DEBUG << this << " : parameter " << params[randomSample]->toString() << " does not belong to the manageable parameters."  << ends;
		return;
	}

    LOG_VERBOSE << this->getName() << ": created individuals " << terse.str() << " from parent " <<  parent << ends;
}	

const string ScanMutationFloatOperator::getName() const
{
    return "scanMutationFLOAT";
}

const string ScanMutationFloatOperator::getAcronym() const
{
    return "SCF.M";
}

const string ScanMutationFloatOperator::getDescription() const
{
	return "" + this->getName() + " performs a local quasi-exhaustive search in the proximity of the parent individual. One parameter of type \"float\" is chosen at random in the individual, and several children are produced with values of the parameter close to the parent's. The number of individuals generated depends on sigma: from two to several tens of individuals are produced, with a sampling of the range specified in the parameters. All individuals produced will share the same allopatric tag. This means that (at the most) ONE of the offspring will survive in the slaughtering step."; 
}
