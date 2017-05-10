/***********************************************************************\
|                                                                       |
| ReplacementMutationOperator.cc                                        |
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
#include "Operators/ReplacementMutation.h"
using namespace std;
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;


ReplacementMutationOperator::ReplacementMutationOperator()
{ }

void ReplacementMutationOperator::generate(const std::vector< Individual* >& parents,
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

    // set the lineage for the new individual
    child->getLineage().set(this->getName(), parents);

	// create a new OperatorToolbox with the child
	OperatorToolbox toolbox(child->getGraphContainer());
	
	// get the vector of all nodes containing at least one parameter
	vector<CNode*> nodes = toolbox.getAllNodes();

	// check: are there actually nodes with at least one parameter in the individual?
	if( nodes.size() == 0 )
	{
		LOG_VERBOSE << this << ": individual " << *child  <<  " has no nodes with at least a parameter to randomize!" << ends;
		return;
	}

	// if there are nodes with parameters, let's randomize!
	// TODO: 	l'originale si salva un sacco di problemi legati ai nodi. Quando viene applicata una splice, tutti i 
	//		collegamenti previous/next vengono perduti e/o invalidati. Quindi non si possono recuperare tutti i nodi,
	//		uno alla volta, come fa OperatorToolbox::getAllNodes, perché ad un certo punto si perde il puntatore al
	//		nodo successivo. I nodi sono conservati con una mappa all'interno del CSubGraph, ma esiste un modo per averli?
	do
	{
		// select one node
		unsigned int randomNode = Random::nextUInteger( 0, nodes.size() - 1);
		CNode* node = nodes[randomNode];

		// now, there are two possibilities: either the node is a prologue/epilogue (global, section, subsection);
		// or it's a node inside a subsection. In the first case, there are no alternative macros, so the node
		// will be simply randomized (all parameters are randomized); otherwise, replace node with new node		
		if( 	node->representsPrologue() || 
			node->representsEpilogue() ||
			&node->getParentContainer()->getPrologue() == node ||
			&node->getParentContainer()->getEpilogue() == node)
		{
			// the node is a prologue or epilogue: randomize all parameters
			LOG_DEBUG << "Replacing node " << *node << ", which is a prologue/epilogue." << ends;
			bool done = node->randomize();
			if(done == false) return;	
		}
		else
		{
			// the node is not a prologue or epilogue
			LOG_DEBUG << "Replacing node \"" << *node << "\", which is a regular subsection node." << ends;

			CSubGraph* subGraph = nullptr;
			
			// check: if the node container is NOT a CSubGraph, then something is wrong
			if( (subGraph = dynamic_cast<CSubGraph*>( node->getParentContainer() )) == nullptr )
			{
				LOG_VERBOSE 	<< "Cannot recover subgraph for node \"" << *node
						<< "\", even if the node should not be a prologue or epilogue." << ends;
				return;
			}

			// create a new node
			unique_ptr<CNode> newNode (new CNode(*subGraph));
			newNode->setConstrain(*subGraph->getSubSection().getRandomMacro());
			newNode->buildRandom();
			
			// create a slice, containing the new node
			unique_ptr<Slice> slice (new Slice(std::move(newNode)));

			// find the position of the selected node in the subgraph: since some of the nodes might have been
			// replaced in a previous iteration of this very same loop, the CNode::getNext() function might not
			// return a correct value. Thus, it's important to iterate using the Slice inside the CSubGraph
			LOG_DEBUG << "Looking for the position of node " << *node << " inside the subgraph..." << ends;
			unsigned int position = 0;
			Slice& currentSlice = subGraph->getSlice();
			for(position = 0; position < currentSlice.getSize() && &currentSlice.getNode(position) != node; position++)
			{}
			
			// if the node is not found, we have a problem
			if( &currentSlice.getNode(position) != node )
			{
				LOG_VERBOSE 	<< this << ": node " << *node << " not found in subgraph " 
						<< subGraph << ". Failing..." << ends;
				return;
			}

			// replace the old node with the new one 
			LOG_DEBUG 	<< this << ": replacing node " << *node << " of subGraph " 
					<< subGraph << " at position " << position << ", with new node "
					<< slice->getNode(0) << "." << ends;

			subGraph->getSlice().cutSlice(position, position);
			subGraph->getSlice().spliceSlice(std::move(slice), position);
		
			//the valid nodes need to be recollected, since actually the node of the individual changed?
			nodes = toolbox.getMutableNodes(); 
		}
	}
	while(sigma > Random::nextDouble());

	// important: restore the edges of the nodes involved in the replacement
	bool success = child->getGraphContainer().attachFloatingEdges();
	if(success == false) return;

	LOG_DEBUG << this << ": mutant " << child->toString() << " created" << ends;
	Assert(child->getGraphContainer().validate() == true);

	outChildren.push_back(child.release());
}

const string ReplacementMutationOperator::getName() const
{
	return "replacementMutation";
}

const string ReplacementMutationOperator::getAcronym() const
{
	return "RPL.M";
}

const string ReplacementMutationOperator::getDescription() const
{
	return "" + this->getName() + " creates a child individual by cloning the parent and replacing a random node (macro instance) with a new, randomly generated one. Then, a random value [0,1] is generated. If the value is lower than the current sigma, the process is repeated. It is interesting to notice that, if there is only ONE type of macro in the constraints, the behavior of this operator is identical to \"alterationMutation\"."; 
}
