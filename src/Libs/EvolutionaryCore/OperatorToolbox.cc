/***********************************************************************\
|                                                                       |
| OperatorToolbox.cc                                                    |
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
using namespace std;
using namespace ugp3;
using namespace ugp3::core;
using namespace ugp3::constraints;
using namespace ugp3::ctgraph;
using namespace tgraph;

OperatorToolbox::~OperatorToolbox()
{ }

OperatorToolbox::OperatorToolbox(CGraphContainer& container)
    : container(container)
{ }

CGraph* OperatorToolbox::getRandomGraph() const
{
    _STACK;

    if(this->container.getCGraphCount() == 0)
    {
        throw Exception("No graphs available for selection.", LOCATION);
    }

    // select a random graph
    unsigned long randomSample = Random::nextUInteger(0, this->container.getCGraphCount() - 1);

    // return the chosen graph
    return &this->container.getCGraph(randomSample);
}

CSubGraph* OperatorToolbox::getRandomSubGraph(const CGraph& sectionGraph)
{
    _STACK;

    if(sectionGraph.getSubGraphCount() == 0)
    {
        LOG_DEBUG << "No sub-graph was selected" << ends;
        return nullptr;
    }

    // select a random subsection of the graph
    unsigned long randomSample = Random::nextUInteger(0, sectionGraph.getSubGraphCount() - 1);

    // return the prologue of the chosen subsection
    return &sectionGraph.getSubGraph(randomSample);
}

CNode* OperatorToolbox::getRandomNode(const CGraph& sectionGraph, bool includePrologue, bool includeEpilogue)
{
    _STACK;

    // select a random subsection
    CSubGraph* subGraph = this->getRandomSubGraph(sectionGraph);
    if(subGraph == nullptr) return nullptr;

    // select a random node inside the chosen subsection
    return this->getRandomNode(*subGraph, includePrologue, includeEpilogue);
}

CNode* OperatorToolbox::getRandomNode(const CSubGraph& subGraph, bool includePrologue, bool includeEpilogue)
{
    _STACK;

    // get the number of nodes inside the given subSection
    unsigned int nodeCount = subGraph.getSize();
    if(includePrologue == false) nodeCount--;
    if(includeEpilogue == false) nodeCount--;

    if(nodeCount == 0)
    {
        // there are no nodes
        return nullptr;
    }

    
    CNode* node = &subGraph.getPrologue();
    
    if(includePrologue == false)
    // skip the prologue
    {
        node = node->getNext();
    }

    // select a random position in the subsection
    unsigned long randomSample = Random::nextUInteger(0, nodeCount - 1);
    LOG_DEBUG << "Picking node at position " << randomSample << " ..." << ends;

    // move forward until the chosen position is reached
    unsigned int position = 0;
    while(randomSample > position)
    {
        position++;
        node = node->getNext();
    }

    Assert(node != nullptr);

    LOG_DEBUG << "Picked node " << node << ends;
    Assert(node->representsPrologue() == false || (node->representsPrologue() == true && includePrologue == true));
    Assert(node->representsEpilogue() == false || (node->representsEpilogue() == true && includeEpilogue == true));

    // return the selected node
    return node;
}

bool OperatorToolbox::insertRandomNode()
{
    _STACK;

    // select a random graph
    CGraph* graph = this->getRandomGraph();
	if(graph == nullptr) return false;

    // insert a node in the selected graph
    return this->insertRandomNode(*graph);
}

bool OperatorToolbox::insertRandomNode(ctgraph::CGraph& sectionGraph)
{
    _STACK;

    // select the prologue of a random subSection
    CSubGraph* subGraph = this->getRandomSubGraph(sectionGraph);
    if(subGraph == nullptr) return false;

    // insert a new node in a random position of the selected subsection
    return this->insertRandomNode(*subGraph);
}

bool OperatorToolbox::insertRandomNode(ctgraph::CSubGraph& subGraph)
{
    _STACK;

    LOG_DEBUG << "Inserting a new node into the sub-graph " << subGraph << "." << std::ends;
   
    // extract a random node as the ancestor of the new node
    int insertPoint = Random::nextUInteger(1, subGraph.getSlice().getSize() - 1 - 1);
    
    // extract a random macro and add it to the sub-graph
    unique_ptr<CNode> newNode (new CNode(subGraph));
	newNode->setConstrain(*subGraph.getSubSection().getRandomMacro());
	newNode->buildRandom();

    Assert(newNode->representsPrologue() == false && newNode->representsEpilogue() == false);

    // insert the new node after its ancestor
    subGraph.getSlice().spliceSlice(unique_ptr<Slice>(new Slice(std::move(newNode))), insertPoint);
    
    return true;
}

bool OperatorToolbox::removeRandomNode()
{
    return this->removeRandomNode(*this->getRandomGraph());
}

bool OperatorToolbox::removeRandomNode(ctgraph::CGraph& sectionGraph)
{
    CSubGraph* subGraph = this->getRandomSubGraph(sectionGraph);
    if(subGraph == nullptr) return false;
    
    return this->removeRandomNode(*subGraph);
}

bool OperatorToolbox::removeRandomNode(ctgraph::CSubGraph& subGraph)
{
    _STACK;


    LOG_DEBUG << "Removing a random node from the sub-graph " << subGraph << "." << std::ends;

    // cannot remove prologue or epilogue
    if(subGraph.getSlice().getSize() <= 2) return false;

    // select a node to remove
    int removePosition = Random::nextUInteger(1, subGraph.getSlice().getSize() - 1 - 1);
    
    subGraph.getSlice().cutSlice(removePosition, removePosition);

    return true;
}

bool OperatorToolbox::insertRandomSubSection()
{
    _STACK;

    return this->insertRandomSubSection(*this->getRandomGraph());
}

bool OperatorToolbox::insertRandomSubSection(ctgraph::CGraph& sectionGraph)
{
    _STACK;

    // select a random subsection of the graph
    CSubGraph* subGraph = this->getRandomSubGraph(sectionGraph);
    if(subGraph == nullptr)
    {
        return false;
    }

    // insert a new instance of the subsection
    return sectionGraph.insertSubGraph(subGraph->getSubSection());
}

bool OperatorToolbox::removeRandomSubSection()
{
    return this->removeRandomSubSection(*this->getRandomGraph());
}

bool OperatorToolbox::removeRandomSubSection(ctgraph::CGraph& sectionGraph)
{
    _STACK;

    CSubGraph* subGraph = this->getRandomSubGraph(sectionGraph);
    if(subGraph == nullptr)
    {
        return false;
    }

    bool success = sectionGraph.detachSubGraph(*subGraph);

    if(success == true)
    {
        delete subGraph;
    }

    return success;
}

CSubGraph* OperatorToolbox::getRandomSubGraph(const string& section, const string& subSection) const
{
    _STACK;

    CGraph* graph = this->container.getCGraph(section);
	if(graph == nullptr) return nullptr;

    vector<CSubGraph*> subGraphs = graph->getSubGraphs(subSection);
    if(subGraphs.empty() == true)
    {
        return nullptr;
    }

    unsigned long randomSample = Random::nextUInteger(0, (unsigned long)(subGraphs.size() - 1));
    Assert(randomSample <subGraphs.size() - 1);
    return subGraphs[randomSample];
}

// returns all nodes that can actually be changed! This means: 
// 	- prologues/epilogues with AT LEAST ONE PARAMETER (otherwise they're just immutable strings)
// 	- subsection nodes EVEN WITHOUT PARAMETERS, as long as there's more then one macro for the subsection (replacementMutation could
//	  swap one parameterless macro with another one
vector<CNode*> OperatorToolbox::getMutableNodes() const
{
	vector<CNode*> allNodes;

	LOG_VERBOSE << "OperatorToolbox: collecting all nodes that can be mutated in some way." << ends;	

	// add global prologue, iff it contains at least one parameter (otherwise, it's immutable)
	if( this->container.getPrologue().getGenericMacro().getParameterCount() > 0 )
	{
		allNodes.push_back( &this->container.getPrologue() );
		LOG_DEBUG << "Node " << this->container.getPrologue() << " added: it's the global prologue." << endl;
	}

	// for each section
	for(unsigned int s = 0; s < this->container.getCGraphCount() ; s++)
	{	
		// pointer to current section
		CGraph* section = &this->container.getCGraph(s);

		// add section prologue iff it contains at least one parameter (otherwise, it's immutable)
		if( section->getPrologue().getGenericMacro().getParameterCount() > 0 )
		{
			allNodes.push_back( &section->getPrologue() );
			LOG_DEBUG << "Node " << section->getPrologue() << " added: it's a section prologue." << ends;
		}

		// for each subsection
		for(unsigned int ss = 0; ss < section->getSubGraphCount() ; ss++)
		{
			// pointer to current subsection
			CSubGraph* subsection = &section->getSubGraph(ss);

			// add all the nodes in the subsection (prologue and epilogue included)
			// the best way to navigate the nodes is to obtain the slice of the subgraph,
			// where the node sequence is stored; one slice is stored for each subgraph
			Slice& slice = subsection->getSlice();
			for(unsigned int n = 0; n < slice.getSize(); n++)
			{
				CNode* node = &slice.getNode(n);
				
				// if the node contains a parameter, or if the section has more than one macro, store it
				if( 	node->getGenericMacro().getParameterCount() > 0 ||
					subsection->getSubSection().getMacroCount() > 1 )
				{
					LOG_DEBUG << "Subsection node " << node << " in position #" << n << " added."  << ends;
					allNodes.push_back( node );
				}
			}
		}

		// add section epilogue
		if( section->getEpilogue().getGenericMacro().getParameterCount() > 0 )
		{
			allNodes.push_back( &section->getEpilogue() );
			LOG_DEBUG << "Node " << section->getEpilogue() << " added: it's a section epilogue." << ends;
		}
	}

	// add global epilogue
	if( this->container.getEpilogue().getGenericMacro().getParameterCount() > 0 )
	{
		allNodes.push_back( &this->container.getEpilogue() );
		LOG_DEBUG << "Node " << this->container.getEpilogue() << " it's the global epilogue." << ends;
	}

	LOG_DEBUG << "OperatorToolbox::getAllNodes collected a total of " << allNodes.size() << " nodes." << ends;

	// return the vector
	return allNodes; 
}


// returns all nodes with at least one parameter
vector<CNode*> OperatorToolbox::getNodesWithParameter() const
{
	vector<CNode*> allNodes;

	LOG_VERBOSE << "OperatorToolbox: collecting all nodes containing at least one parameter." << ends;	

	// add global prologue
	if( this->container.getPrologue().getGenericMacro().getParameterCount() > 0 )
	{
		allNodes.push_back( &this->container.getPrologue() );
		LOG_DEBUG << "Node " << this->container.getPrologue() << " added: it's the global prologue." << endl;
	}

	// for each section
	for(unsigned int s = 0; s < this->container.getCGraphCount() ; s++)
	{	
		// pointer to current section
		CGraph* section = &this->container.getCGraph(s);

		// add section prologue
		if( section->getPrologue().getGenericMacro().getParameterCount() > 0 )
		{
			allNodes.push_back( &section->getPrologue() );
			LOG_DEBUG << "Node " << section->getPrologue() << " added: it's a section prologue." << ends;
		}

		// for each subsection
		for(unsigned int ss = 0; ss < section->getSubGraphCount() ; ss++)
		{
			// pointer to current subsection
			CSubGraph* subsection = &section->getSubGraph(ss);

			// add all the nodes in the subsection (prologue and epilogue included)
			// the best way to navigate the nodes is to obtain the slice of the subgraph,
			// where the node sequence is stored; one slice is stored for each subgraph
			Slice& slice = subsection->getSlice();
			for(unsigned int n = 0; n < slice.getSize(); n++)
			{
				CNode* node = &slice.getNode(n);
				
				// if the node contains a parameter, store it
				if( node->getGenericMacro().getParameterCount() > 0 )
				{
					LOG_DEBUG << "Subsection node " << node << " in position #" << n << " added."  << ends;
					allNodes.push_back( node );
				}
			}
		}

		// add section epilogue
		if( section->getEpilogue().getGenericMacro().getParameterCount() > 0 )
		{
			allNodes.push_back( &section->getEpilogue() );
			LOG_DEBUG << "Node " << section->getEpilogue() << " added: it's a section epilogue." << ends;
		}
	}

	// add global epilogue
	if( this->container.getEpilogue().getGenericMacro().getParameterCount() > 0 )
	{
		allNodes.push_back( &this->container.getEpilogue() );
		LOG_DEBUG << "Node " << this->container.getEpilogue() << " it's the global epilogue." << ends;
	}

	LOG_DEBUG << "OperatorToolbox::getAllNodes collected a total of " << allNodes.size() << " nodes." << ends;

	// return the vector
	return allNodes; 
}

// returns all nodes in the individual
vector<CNode*> OperatorToolbox::getAllNodes() const
{
	vector<CNode*> allNodes;

	LOG_VERBOSE << "OperatorToolbox: collecting all nodes in the individual." << ends;	

	// add global prologue
	allNodes.push_back( &this->container.getPrologue() );
	LOG_DEBUG << "Node " << this->container.getPrologue() << " added: it's the global prologue." << endl;

	// for each section
	for(unsigned int s = 0; s < this->container.getCGraphCount() ; s++)
	{	
		// pointer to current section
		CGraph* section = &this->container.getCGraph(s);

		// add section prologue
		allNodes.push_back( &section->getPrologue() );
		LOG_DEBUG << "Node " << section->getPrologue() << " added: it's a section prologue." << ends;

		// for each subsection
		for(unsigned int ss = 0; ss < section->getSubGraphCount() ; ss++)
		{
			// pointer to current subsection
			CSubGraph* subsection = &section->getSubGraph(ss);

			// add all the nodes in the subsection (prologue and epilogue included)
			// the best way to navigate the nodes is to obtain the slice of the subgraph,
			// where the node sequence is stored; one slice is stored for each subgraph
			Slice& slice = subsection->getSlice();
			for(unsigned int n = 0; n < slice.getSize(); n++)
			{
				CNode* node = &slice.getNode(n);
				
				LOG_DEBUG << "Subsection node " << node << " in position #" << n << " added."  << ends;
				allNodes.push_back( node );
			}
		}

		// add section epilogue
		allNodes.push_back( &section->getEpilogue() );
		LOG_DEBUG << "Node " << section->getEpilogue() << " added: it's a section epilogue." << ends;
	}

	// add global epilogue
	allNodes.push_back( &this->container.getEpilogue() );
	LOG_DEBUG << "Node " << this->container.getEpilogue() << " it's the global epilogue." << ends;

	LOG_DEBUG << "OperatorToolbox::getAllNodes collected a total of " << allNodes.size() << " nodes." << ends;

	// return the vector
	return allNodes; 
}
