/***********************************************************************\
|                                                                       |
| SingleParameterMutationOperator.cc                                    |
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

//#define PATCH_SELFADAPTING 

#include "ugp3_config.h"
#include "EvolutionaryCore.h"
#include "Operators/SingleParameterAlterationMutation.h"
using namespace std;
using namespace ugp3::core;
using namespace ugp3::ctgraph;


SingleParameterAlterationMutationOperator::SingleParameterAlterationMutationOperator()
{ }


void SingleParameterAlterationMutationOperator::generate(const std::vector< Individual* >& parents,
                          std::vector< Individual* >& outChildren,
                          IndividualPopulation& population) const
{ 
	_STACK;

	double sigma = population.getParameters().getSigma();

	LOG_DEBUG << this << ": generating new mutant" << ends;

	Individual* parent = parents[0];
	Assert(parent->validate() == true);

	// create a copy of the parent
	unique_ptr<Individual> child = parent->clone();

	// set the lineage for the new individual
    child->getLineage().set(this->getName(), parents);

	// get the vector of all parameters
	OperatorToolbox toolbox( child->getGraphContainer() );
	vector<CNode*> allNodes = toolbox.getNodesWithParameter();

	// check: are there actually nodes with at least one parameter in the individual?
	if( allNodes.size() == 0 )
	{
		LOG_VERBOSE << this << ": individual " << *child  <<  " has no parameters to randomize!" << ends;
		return;
	}

	// map, to have nodes weighted with the number of parameters they possess; nodes with more
	// parameters are more likely to be selected for randomization of a parameter
	unsigned int w = 0;
	map<int,CNode*> weightedNodes;
	for(unsigned int n = 0; n < allNodes.size(); n++)
	{
		for(unsigned int p = 0; p < allNodes[n]->getGenericMacro().getParameterCount(); p++)
		{
			weightedNodes[w] = allNodes[n];
			w++;
		}
	}
	LOG_DEBUG << "Collected " << allNodes.size() << " nodes, with a total of " << weightedNodes.size() << " parameters." << ends;

	// if there are parameters, start randomizing! Iterations based on sigma
	do
	{
		unsigned int randomNode = Random::nextUInteger( 0, weightedNodes.size() - 1);
		CNode* node = weightedNodes[randomNode];
		bool done = false;

#ifdef PATCH_SELFADAPTING
		// select a random parameter inside the node
		unsigned int randomParameter = Random::nextUInteger( 0, node->getGenericMacro().getParameterCount() - 1);
		
		// this patch only works for float parameters
		if( dynamic_cast<FloatParameter*>(&node->getGenericMacro().getParameter(randomParameter)) == nullptr )
		{
			done = node->randomizeParameter();
		}
		else
		{
			// get current value of the parameter from the tag
			FloatParameter* parameter = dynamic_cast<FloatParameter*>(&node->getGenericMacro().getParameter(randomParameter));
			LOG_DEBUG 	<< "Self-adapting patch for " << this->getName() 
					<< " activated. Retrieving current value for float parameter " << parameter->getName() 
			<< ends;

			Tag& tag = node->getTag(CNode::Escape + parameter->getName() );
			double currentPosition = parameter->getPosition( tag.getValue() );
			double newPosition = Random::nextDouble(0,1);
			double position = sigma * newPosition + (1 - sigma) * currentPosition;
			
			tag.setValue( parameter->getAt(position) );
			
			LOG_DEBUG 	<< "Old value of the parameter was " << parameter->getAt(currentPosition) << " (position "
					<< currentPosition << "); current value of the parameter is " << parameter->getAt(position)
					<< " (position " << position << ")" << ends;
			
			done = true;
		}
#else
		done = node->randomizeParameter();
#endif

		if (!done) 
		{
			LOG_VERBOSE << this << ": parameter was not randomized successfully" << ends;
			return;
		}
	}
	while(sigma > Random::nextDouble());
	
	// attach floating edges and validate individual
	if(child->getGraphContainer().attachFloatingEdges() == false) return;
	Assert(child->validate() == true);

	// return offspring
	LOG_VERBOSE << this << ": mutant " << child->toString() << " created" << ends;
	outChildren.push_back(child.release());
}

const string SingleParameterAlterationMutationOperator::getName() const
{
	return "singleParameterAlterationMutation";
}

const string SingleParameterAlterationMutationOperator::getAcronym() const
{
	return "SPA.M";
}

const string SingleParameterAlterationMutationOperator::getDescription() const
{
	return "" + this->getName() + " creates a child individual by cloning the parent, then randomly selects a node (macro instance) inside the child, and a parameter inside that node. The current value of that parameter is than randomized. Subsequently, a random value in [0,1] is generated. If the value is lower than the current sigma, another random node is selected and the operation is repeated. It is important to notice that this operator WILL NOT WORK if all macros in your constraints file have no parameters.";
}
