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

#include "ugp3_config.h"
#include "EvolutionaryCore.h"
#include "Operators/SwapMutation.h"
using namespace std;
using namespace ugp3::core;
using namespace ugp3::ctgraph;


SwapMutationOperator::SwapMutationOperator()
{ }


void SwapMutationOperator::generate(const std::vector< Individual* >& parents,
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

	// select the target (/graph/subGraph/macro/parameter)
	OperatorToolbox toolbox(child->getGraphContainer());

	// collect all nodes with at least one Combinatorial parameter
	vector<CNode*> nodes = toolbox.getNodesWithParameter<CombinatorialParameter>();

	// if no parameters are found, return
	if( nodes.size() == 0 )
	{
		LOG_DEBUG << this << " : no nodes with Combinatorial parameters found. Failing..." << ends;
		return;
	}

	// start mutations
	do
	{
		// select a node
		unsigned int randomNode = Random::nextUInteger(0, nodes.size() - 1);
		
		// collect all the combinatorial parameter inside that node
		vector<CombinatorialParameter*> parameters;
		for(unsigned int p = 0; p < nodes[randomNode]->getGenericMacro().getParameterCount(); p++)
			if( dynamic_cast<CombinatorialParameter*>( &nodes[randomNode]->getGenericMacro().getParameter(p) ) != nullptr )
				parameters.push_back(dynamic_cast<CombinatorialParameter*>( &nodes[randomNode]->getGenericMacro().getParameter(p) ));
		
		// choose a random parameter
		unsigned int randomParameter = Random::nextUInteger(0, parameters.size() - 1);
		CombinatorialParameter* parameter = dynamic_cast<CombinatorialParameter*>(parameters[randomParameter]);

		// obtain the current value for the parameter
		Tag& tag = nodes[randomNode]->getTag(CNode::Escape + parameter->getName());
		vector<string> value = Convert::toStringVector( tag.getValue(), parameter->getDelimiter() );	

		LOG_DEBUG
		<< this->getName() << " : original value for parameter " 
		<< parameter->getName() << " is \"" << tag.getValue() << "\"." << ends;

		// select two random indexes in the vector
		// choose two DIFFERENT values and check whether the number of values is bigger than 1 
		unsigned int index1 = Random::nextUInteger(0, value.size() - 1);
		unsigned int index2;
		unsigned int count = 0;
		
		// try for N times to pick a different index
		while( (index2 = Random::nextUInteger(0, value.size() - 1)) == index1 /* && count < 10 */) count++;
		
		// assign the two indexes to two values
		unsigned int bigIndex, smallIndex;
		if( index1 > index2 )
		{
			bigIndex = index1;
			smallIndex = index2;
		}
		else if( index2 > index1 )
		{
			bigIndex = index2;
			smallIndex = index1;
		}
		else
		{
			// the two indexes are the same
			LOG_DEBUG << this->getName()
			<< " failing: it was impossibile to pick two different indexes in the vector (" 
			<< index1 << " == " << index2 << ")." << ends;
			return;
		}
		
		LOG_DEBUG 
		<< this->getName() << " : now swapping values between the two indexes "
		<< smallIndex << " and " << bigIndex << ends;

		// swap all values between the two indexes
		while(bigIndex > smallIndex)
		{
			// swap value
			string temp;
			temp = value[bigIndex];
			value[bigIndex] = value[smallIndex];
			value[smallIndex] = temp;
			
			// increase/decrease
			bigIndex--;
			smallIndex++;
		}
		
		// convert value to string
		ostringstream ss;
		ss << value[0];
		for(unsigned int i = 1; i < value.size(); i++) ss << parameter->getDelimiter() << value[i];
		
		LOG_DEBUG 
		<< this->getName() << " : new value for parameter " 
		<< parameter->getName() << " is \"" << ss.str() << "\"." << ends;

		// assign value to child
		tag.setValue( ss.str() );
	
		// validate new value
		if(parameter->validate(ss.str()) == false )
		{
			LOG_VERBOSE 	<< this << ": cannot validate value \"" << ss.str() << "\" for CombinatorialParameter "
					<< *parameter << " in node " << *nodes[randomNode] << ends;
			return;
		}

		LOG_DEBUG << "New value is \"" << nodes[randomNode]->getTag(CNode::Escape + parameter->getName()).getValue() << "\"." << ends;
	}
	while(sigma > Random::nextDouble());

	if(child->getGraphContainer().attachFloatingEdges() == false)
	{
		LOG_VERBOSE << this << ": cannot validate the mutant produced." << ends;
		return;
	}
	Assert(child->validate() == true);
	
	LOG_VERBOSE << this << ": mutant " << child->toString() << " created" << ends;
	outChildren.push_back(child.release());
	
	return;
}

const string SwapMutationOperator::getName() const
{
	return "swapMutation";
}

const string SwapMutationOperator::getAcronym() const
{
	return "SWM.M";
}

const string SwapMutationOperator::getDescription() const
{
	return "" + this->getName() + " creates a child individual by cloning the parent, then selects a parameter of type \"combinatorial\". It chooses two random indexes inside the parameter, and swaps all values between the two indexes. Subsequently, a random value in [0,1] is generated. If the value is lower than the current sigma, the process is repeated. It is important to notice that this operator WILL NOT WORK on individuals WITHOUT \"combinatorial\" parameters.";
}
