/***********************************************************************\
|                                                                       |
| InverOverCrossoverOperator.cc                                 |
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
#include "Operators/InverOverCrossover.h"
#include <IndividualPopulationParameters.h>
#include <IndividualPopulation.h>

using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;
using namespace tgraph;
using namespace std;

InverOverCrossoverOperator::InverOverCrossoverOperator()
{ }

void InverOverCrossoverOperator::generate(
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
	
	// create a copy of the parent
	unique_ptr<Individual> child = parent1->clone();

	// set the lineage for the new individual
	child->getLineage().set(this->getName(), parents);

	// InverOver is a special crossover, that works on combinatorial parameters only: so, the first thing
	// is to find a valid combinatorial parameter inside the child

	// select the target (/graph/subGraph/macro/parameter)
	OperatorToolbox toolbox(child->getGraphContainer());
	
	// select all nodes with combinatorial parameters
	vector<CNode*> nodes1 = toolbox.getNodesWithParameter<CombinatorialParameter>();
	
	// if no nodes with combinatorial parameters are found 
	if( nodes1.size() == 0 )
	{
		LOG_DEBUG << this << " : no appropriate parameters found. Failing..." << ends;
		return;
	}
	
	// now, choose randomly the parameter to operate on and behave accordingly
	unsigned int nodeIndex = Random::nextUInteger(0, nodes1.size() - 1);
	CNode* childNode = nodes1[nodeIndex];

	// collect all combinatorial parameters in node n
	vector<CombinatorialParameter*> combinatorialParametersChild;
	for(unsigned int i = 0; i < childNode->getGenericMacro().getParameterCount(); i++)
	{
		if( dynamic_cast<CombinatorialParameter*>( & childNode->getGenericMacro().getParameter(i) ) != nullptr ) 
			combinatorialParametersChild.push_back( dynamic_cast<CombinatorialParameter*>( & childNode->getGenericMacro().getParameter(i) ) );
	}
	
	// TODO: intentional exception raise, to test exception catch
	// childNode->getGenericMacro().getParameter(-1);
	
	// get one parameter
	unsigned int p = Random::nextUInteger(0, combinatorialParametersChild.size() - 1);
	CombinatorialParameter* childParameter = dynamic_cast<CombinatorialParameter*>( &childNode->getGenericMacro().getParameter(p) );

	// get the original value
	vector<string> childValue = Convert::toStringVector( childNode->getTag(CNode::Escape + childParameter->getName()).getValue(), childParameter->getDelimiter() );
	
	LOG_DEBUG << this << " : original value for the chosen combinatorial parameter is \"" << childValue[0];
	for(unsigned int i = 1; i < childValue.size(); i++) LOG_DEBUG << childParameter->getDelimiter() << childValue[i];
	LOG_DEBUG <<  "\"." << ends;

	// InverOver procedure
	string current, successor;
	do
	{
		// eventually select new "parent2"
		if( parent2 == nullptr )
		{
			// TODO: do something 
		}

		// find corresponding node and parameter in "parent2"
		OperatorToolbox toolboxParent2(parent2->getGraphContainer());
		vector<CNode*> nodes2 = toolboxParent2.getNodesWithParameter<CombinatorialParameter>();

		// if the number of combinatorial elements in the two individuals is different, warning
		if( nodes1.size() != nodes2.size() )
			LOG_WARNING << this << " : different number of combinatorial parameters in individuals; it might not work properly." << ends;

		if( nodeIndex >= nodes2.size() )
		{
			LOG_DEBUG << this << " : structure of the individuals not coherent. Failing..." << ends;
			return;
		}
		
		CNode* parent2Node = nodes2[nodeIndex];
		CombinatorialParameter* parent2Parameter = dynamic_cast<CombinatorialParameter*>( &parent2Node->getGenericMacro().getParameter(p) );
		if( parent2Parameter == nullptr )
		{
			LOG_DEBUG << this << " : structure of the individuals not coherent. Failing..." << ends;
			return;
		}

		vector<string> parent2Value = Convert::toStringVector( parent2Node->getTag(CNode::Escape + parent2Parameter->getName()).getValue(), parent2Parameter->getDelimiter() );
		
		LOG_DEBUG << this << " : value for same parameter in parent2 is \"" << parent2Value[0];
		for(unsigned int i = 1; i < parent2Value.size(); i++) LOG_DEBUG << parent2Parameter->getDelimiter() << parent2Value[i];
		LOG_DEBUG << "\"." << ends;

		// check if the two parameters are coherent
		// TODO: how?
		
		// select current value in child
		unsigned int currentIndex = Random::nextUInteger(0, childValue.size() - 1);
		current = childValue[currentIndex];
		
		LOG_DEBUG << this << " : current value in child is \"" << current << "\"." << ends;

		// find successor of current value in "parent2"
		unsigned int i;
		for(i = 0; i < parent2Value.size() && current.compare(parent2Value[i]) != 0; i++);
		
		if( current.compare(parent2Value[i]) != 0 )
		{
			LOG_DEBUG << this << " : structure of the individuals not coherent. Failing..." << ends;
			return;
		}
		
		// if the current value in parent2 is at the end, choose 0 as the successor
		if( i != parent2Value.size() - 1 ) 
			successor = parent2Value[i+1];
		else
			successor = parent2Value[0];
		
		LOG_DEBUG << this << " : successor value in parent2 is \"" << successor << "\"." << ends;
		
		// find successor in child 
		unsigned int successorIndex;
		for(successorIndex = 0; i < childValue.size() && successor.compare(childValue[successorIndex]) != 0; successorIndex++);
		
		if( successor.compare(childValue[successorIndex]) != 0 )
		{
			LOG_DEBUG << this << " : structure of the individuals not coherent. Failing..." << ends;
			return;
		}
		
		unsigned int bigIndex, smallIndex;
		if( currentIndex > successorIndex)
		{
			bigIndex = currentIndex;
			smallIndex = successorIndex + 1;
		}
		else
		{
			bigIndex = successorIndex;
			smallIndex = currentIndex + 1;
		}
		
		// invert all values between current and successor
		while( bigIndex > smallIndex )
		{
			// switch two values
			string temp = childValue[bigIndex];
			childValue[bigIndex] = childValue[smallIndex];
			childValue[smallIndex] = temp;

			// update counters
			bigIndex--;
			smallIndex++;
		}
		
		// set parent2 as null for next loop
		parent2 = nullptr;
	}
	while( false /*Random::nextDouble(0,1) < parameters->getSigma() && successor != current*/);

	// write new value to string
	ostringstream ss;
	ss << childValue[0];
	for(unsigned int i = 1; i < childValue.size(); i++)
		ss << childParameter->getDelimiter() << childValue[i];

	LOG_DEBUG << this << " : new value for parameter " << childParameter->getName() << " is \"" << ss.str() << "\"." << ends;
	
	// validate the modified value
	childParameter->validate( ss.str() );

	// set the value
	childNode->getTag(CNode::Escape + childParameter->getName()).setValue( ss.str() );

	// save the results
	outChildren.push_back(child.release());

	LOG_DEBUG << this << ": crossover succeeded" << ends;
}


int InverOverCrossoverOperator::getValidCutPoint(
    const CSubGraph& selectedSubGraph1, unsigned int cutPoint1,
    const CSubGraph& selectedSubGraph2) const
{
    _STACK;

    const int maxSize = selectedSubGraph1.getSubSection().getMaximumMacroInstances() + 1; // no prologue
    LOG_DEBUG << "The maximum size of the slice is " << maxSize << ends;

    const int minSize = selectedSubGraph1.getSubSection().getMinimumMacroInstances() + 1; // no prologue
    LOG_DEBUG << "The minimum size of the slice is " << minSize << ends;

    const unsigned int size1 = selectedSubGraph1.getSize() - 1; // no prologue
    LOG_DEBUG << "The size of the sub-graph " << selectedSubGraph1 << " is " << size1 << ends;

    const unsigned int size2 = selectedSubGraph2.getSize() - 1; // no prologue
    LOG_DEBUG << "The size of the sub-graph " << selectedSubGraph2 << " is " << size2 << ends;

    const int slice1Size = selectedSubGraph1.getSize() - cutPoint1;
    LOG_DEBUG << "The size of the slice on sub-graph " << selectedSubGraph1 << " is " << slice1Size << ends;

    unsigned int maxSlice2 = min<unsigned int>(min<unsigned int>(maxSize - size1 + slice1Size , size2 + slice1Size - minSize ), size2);
    LOG_DEBUG << "The new maximum size of the slice is " << maxSlice2 << ends;

    unsigned int minSlice2 = max<int>( max<int>(minSize - size1 + slice1Size , size2 + slice1Size - maxSize), 1);
    LOG_DEBUG << "The new minimum size of the slice is " << minSlice2 << ends;
    Assert( maxSlice2 >= minSlice2 );
    Assert( minSlice2 >= 1 );

    int slice2Size = Random::nextSInteger(minSlice2, maxSlice2);
    LOG_DEBUG << "The size of the slice on sub-graph " << selectedSubGraph2 << " is " << slice2Size << ends;

    const int cutPoint2 = size2 - slice2Size + 1;

    return cutPoint2;
}

const string InverOverCrossoverOperator::getName() const
{
    return "inverOverCrossover";
}

const string InverOverCrossoverOperator::getAcronym() const
{
    return "INV.X";
}

const string InverOverCrossoverOperator::getDescription() const
{
	return "" + this->getName() + " works on parameters of type \"combinatorial\". It applies the InverOverCrossover on corresponding parameters of two different individuals. For more details on the inver-over crossover, see \"Inver-over operator for the TSP\", by Tao and Michalewicz, 1998. It is important to notice that this operator WILL NOT WORK in absence of \"combinatorial\" parameters.";

}
