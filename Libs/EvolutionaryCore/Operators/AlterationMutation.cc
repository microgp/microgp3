/***********************************************************************\
|                                                                       |
| AlterationMutationOperator.cc                                         |
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
#include "Operators/AlterationMutation.h"
using namespace std;
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;


AlterationMutationOperator::AlterationMutationOperator()
{ }

void AlterationMutationOperator::generate(const std::vector< Individual* >& parents,
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

	// create operator toolbox to manipulate the child
	OperatorToolbox toolbox(child->getGraphContainer());

	// collect all nodes with at least one parameter
	vector<CNode*> nodes = toolbox.getNodesWithParameter();

	// check: are there actually nodes with at least one parameter in the individual?
	if( nodes.size() == 0 )
	{
		LOG_VERBOSE << this << ": individual " << *child  <<  " has no nodes with at least a parameter to randomize!" << ends;
		return;
	}

	// choose a random node between those available and randomize it (replacing the macro with a new one)
	do
	{
		unsigned int randomNode = Random::nextUInteger( 0, nodes.size() - 1);
		CNode* node = nodes[randomNode];

		bool done = node->randomize();
		if(done == false) return;
	}
	while(sigma > Random::nextDouble());

	// check if the child is valid
	if(child->getGraphContainer().attachFloatingEdges() == false) 
	    return;

	LOG_DEBUG << this << ": mutant " << child->toString() << " created" << ends;
	outChildren.push_back(child.release());
}

const string AlterationMutationOperator::getName() const
{
    return "alterationMutation";
}

const string AlterationMutationOperator::getAcronym() const
{
    return "ALT.M";
}

const string AlterationMutationOperator::getDescription() const
{
	return 	"" + this->getName() + " generates a single child individual. It copies the parent, then selects a random node "
		"(macro instance) inside the child individual, and sets ALL parameters of the "
		"chosen node to a random value, within the constriants. Then, a random number [0,1] is generated: if the random "
		"value is less than the current sigma, the process is iterated. When sigma is high, this operator favors "
		"exploration, generating offspring that is quite different from the parent; when sigma is low, the children "
		"individual is closer to the parent, thus favoring exploitation. It is interesting to notice that, if all macros " 
		"have a single parameter, the behavior is identical to singleParameterAlterationMutation.";
}
