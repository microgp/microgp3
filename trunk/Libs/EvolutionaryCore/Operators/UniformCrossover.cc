/***********************************************************************\
|                                                                       |
| UniformCrossoverOperator.cc                                          |
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

#include <sstream> // just for debugging purposes

#include "ugp3_config.h"
#include "Operators/UniformCrossover.h"
#include "EvolutionaryCore.h"
using namespace ugp3::constraints;
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace tgraph;
using namespace std;


UniformCrossoverOperator::UniformCrossoverOperator()
{ }

void UniformCrossoverOperator::generate(const std::vector< Individual* >& parents,
                          std::vector< Individual* >& outChildren,
                          IndividualPopulation& population) const
{ 
     _STACK;

    // Get the parents
    Individual *parent1 = parents[0];
    Individual *parent2 = parents[1];
    Assert(parent1->getGraphContainer().validate() == true);
    Assert(parent2->getGraphContainer().validate() == true);

    // Clone parents
    unique_ptr<Individual> child1 = parent1->clone();
    unique_ptr<Individual> child2 = parent2->clone();
    LOG_DEBUG << this << ": created two individuals (" <<
        child1->toString() << " and " << child2->toString() << ") from parents " <<
        parent1 << " and " << parent2 << ends;

    // Set the lineage of the children
    child1->getLineage().set(this->getName(), parents);
    child2->getLineage().set(this->getName(), parents);

    // Set the toolboxes that will be used to operate on the individuals 
    OperatorToolbox toolbox1(child1->getGraphContainer());
    OperatorToolbox toolbox2(child2->getGraphContainer());

    // Now, this is tricky: we should go through the common parts of the two individuals, replacing node by node; however, in order to do that, we should also take into account the missing instances of subsections that may/may not appear in both individuals
    
    // It's probably better to work on corresponding subgraphs
    // ---- first slice
    CGraph* selectedGraph1 = toolbox1.getRandomGraph();
    CSubGraph* selectedSubGraph1 = toolbox1.getRandomSubGraph(*selectedGraph1);

    if(selectedSubGraph1 == nullptr) 
        return;
    
    // ---- second slice
    LOG_DEBUG 
        << "Selecting a random sub-graph on individual  " << child2->toString() 
        << " compatible with the previously selected sub-graph " << selectedGraph1 << " of individual " 
        << child1->toString() << ends;
        
    CSubGraph* selectedSubGraph2 = child2->getGraphContainer().getRandomSubGraph(
        selectedGraph1->getSection().getId(), selectedSubGraph1->getSubSection().getId() );

    if(selectedSubGraph2 == nullptr) 
        return;

    // check the actual size of the two chosen subGraphs
    unsigned int size1 = selectedSubGraph1->getSize() - 2; // exclude prologue and epilogue
    unsigned int size2 = selectedSubGraph2->getSize() - 2; // exclude prologue and epilogue
    LOG_DEBUG << "Actual size of the subgraphs is " << size1 << " and " << size2 << ends;

    // if one of the two subgraphs has size 0, the operators fails
    if( size1 == 0 || size2 == 0 )
    {
	LOG_WARNING << "Operator " << this->getName() << " cannot modify individuals with 0 macros in a subSection." << ends;
	return;
    }

    // check which one is the longest
    CSubGraph* longestSubGraph = selectedSubGraph1;
    CSubGraph* shortestSubGraph = selectedSubGraph2;

    if( size2 > size1 )
    {
	longestSubGraph = selectedSubGraph2;
	shortestSubGraph = selectedSubGraph1;
    }

    // now, let's go over the longest graph and randomly replace loci with the alleles on the other
    // replacing a node, however, is quite complex: we basically need to create a new Slice, and swap it with the Slice containing only the node to be replaced (created from the SubGraph of the original individual) 
    // NOTE: we skip the prologue and epilogue (?)
    unsigned int position = 0;
    for(position = 1; position < shortestSubGraph->getSize(); position++)
    {
	// will the node at this position be replaced?
	if( Random::nextDouble(0,1) >= 0.5 )
	{
		// create a new node, cloning the one that will replace the node at this position
		unique_ptr<CNode> newNode = shortestSubGraph->getSlice().getNode(position).clone();
		// create a slice, containing the new node
		unique_ptr<Slice> newSlice (new Slice(std::move(newNode)));
		// cut the original node and replace it
		longestSubGraph->getSlice().cutSlice(position, position);
		longestSubGraph->getSlice().spliceSlice(std::move(newSlice), position);
	}
    }
    
    // the process ends by randomly keeping or removing the remaining nodes
    // NOTE: again, we are skipping the epilogue
    for(; position < longestSubGraph->getSize() - 1; position++)
    {
	// will the node at this position be removed?
	if( Random::nextDouble(0,1) >= 0.5 )
	{
		// cut the slice containing only the current node
		longestSubGraph->getSlice().cutSlice(position, position);
		// advance the index
		position++;
	}
    }
	
    // TODO: some special cases for prologue and epilogue?

    // important: restore the edges of the nodes involved in the replacement
    bool success = child1->getGraphContainer().attachFloatingEdges();
    if( size2 > size1 )
	success = child2->getGraphContainer().attachFloatingEdges();

    if(success == false)
    {
        LOG_DEBUG << "One or more edges of the child could not be restored" << ends;
        return;
    }

    // TODO: remove this, it's just some paranoid debugging to see if everything was ok
    stringstream ss;
    ss << parent1->getExternalRepresentation();
    LOG_DEBUG << "Parent 1: \"" << ss.str() << "\"" << ends;
    ss.str(std::string()); // clear the ss

    ss << parent2->getExternalRepresentation();
    LOG_DEBUG << "Parent 2: \"" << ss.str() << "\"" << ends;
    ss.str(std::string()); // clear the ss

    if( size1 > size2 ) ss << child1->getExternalRepresentation();
    else ss << child2->getExternalRepresentation();
    LOG_DEBUG << "Resultin child: \"" << ss.str() << "\"" << ends;


    if( size1 > size2 )
    {
	LOG_DEBUG << this << ": individual " << child1->toString() << " created" << ends;
	outChildren.push_back(child1.release());
    }
    else
    {
	LOG_DEBUG << this << ": individual " << child2->toString() << " created" << ends;
	outChildren.push_back(child2.release());
    }

    // the individual that is NOT added to the outChildren vector is actually deleted at the end of this method
    return;
}


const string UniformCrossoverOperator::getDescription() const
{
	return "" + this->getName() + " takes two parent individuals, and creates one child by recombining the parents. For every locus, one of the two alleles is randomly taken from one of the parents. If two parents are of different length, for each exceeding locus, either the allele of the longest parent or nothing is taken. This operator restores external references (\"outerLabel\" parameters) correctly, differently from the \"Imprecise\" crossover operators."; 
}
