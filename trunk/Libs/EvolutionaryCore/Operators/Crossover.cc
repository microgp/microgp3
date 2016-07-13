/***********************************************************************\
|                                                                       |
| CrossoverOperator.cc                                                  |
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
#include <set>

#include "Operators/Crossover.h"
#include "CSubGraph.h"
using namespace std;
using namespace ugp3::ctgraph;
using namespace tgraph;
using namespace ugp3::constraints;
using namespace ugp3::core;


CrossoverOperator::CrossoverOperator()
{ }

void CrossoverOperator::collectLinkedSubGraphs(
    const std::set<CNode*>& nodes, 
    set<CSubGraph*>& subGraphs) const
{
    _STACK;
    
    // collect all the subgraphs that are referenced by the nodes in the specified set
    set<CNode*>::const_iterator iterator = nodes.begin();
    while(iterator != nodes.end())
    {
        // for each node in the set
        const CNode* node = *iterator;
        for(unsigned int i = 0; i < node->getGenericMacro().getParameterCount(); i++)
        {
            // for each parameter of the node
            const Parameter& parameter = node->getGenericMacro().getParameter(i);
            const OuterLabelParameter* outerLabel = dynamic_cast<const OuterLabelParameter*>(&parameter);
            
            // consider outer labels only    
            if(outerLabel == nullptr) 
                continue;

            // get the containers of the node
            Assert(dynamic_cast<CSubGraph*>(node->getParentContainer()) != nullptr);
            CSubGraph* nodeSubGraph = (CSubGraph*) node->getParentContainer();
           
            Assert(dynamic_cast<CGraph*>(nodeSubGraph->getParentContainer()) != nullptr);
            CGraph* nodeGraph = (CGraph*) nodeSubGraph->getParentContainer();

            Assert(dynamic_cast<CGraphContainer*>(nodeGraph->getParentContainer()) != nullptr);
            CGraphContainer* nodeContainer = (CGraphContainer*) nodeGraph->getParentContainer();

            // get the edge corresponding to the outer label parameter
            LOG_DEBUG << "Following outer label " << outerLabel<< " of node " << node << " " << ends;
            tgraph::Edge* edge = node->getEdge(outerLabel->getName());
            Assert(edge != nullptr);
         
            // retrieve the target information from the edge
            const Tag& offsetTag = edge->getTag(Edge::offsetTagName);
            const Tag& sectionTag = edge->getTag("section");

            // retrieve the target prologue
            CGraph* targetGraph = nodeContainer->getCGraph(sectionTag.getValue());
            CSubGraph& targetSubGraph = targetGraph->getSubGraph(
                Convert::toUInt(offsetTag.getValue()));
            CNode& subGraphPrologue = targetSubGraph.getPrologue();
            Assert(subGraphPrologue.representsPrologue());
            
            LOG_DEBUG << "Outer label points to sub-graph " << targetSubGraph << ends;
            if(subGraphs.find(&targetSubGraph) == subGraphs.end())
            {
                subGraphs.insert(&targetSubGraph);

                // recursion on the new sub-graph
                this->collectLinkedSubGraphs(&targetSubGraph, subGraphs);
            }
        }

        ++iterator;
    }
}

void CrossoverOperator::swapSlices(
    CSubGraph& subGraph1, unsigned int cutPoint11, unsigned int cutPoint12, 
    CSubGraph& subGraph2, unsigned int cutPoint21, unsigned int cutPoint22) const
{
    LOG_DEBUG << "Swapping slices" << ends;
    
    unique_ptr<Slice> slice1;
    if(cutPoint11 <= cutPoint12)
    {
        slice1 = subGraph1.getSlice().cutSlice(cutPoint11, cutPoint12);
        // Cutting a slice will also detach all edges.
    }
    else slice1 = unique_ptr<Slice> (new Slice());

    unique_ptr<Slice> slice2;
    if(cutPoint21 <= cutPoint22)
    {
        slice2 = subGraph2.getSlice().cutSlice(cutPoint21, cutPoint22);
        // Cutting a slice will also detach all edges.
    }
    else slice2 = unique_ptr<Slice>(new Slice());

    subGraph1.getSlice().spliceSlice(std::move(slice2), cutPoint11);
    subGraph2.getSlice().spliceSlice(std::move(slice1), cutPoint21);
    
    LOG_DEBUG << "Slices swapped" << ends;
}

void CrossoverOperator::collectLinkedSubGraphs(CSubGraph* rootSubGraph, std::set<ugp3::ctgraph::CSubGraph*>& subGraphs) const
{
    _STACK;
    
    if(rootSubGraph == nullptr)
    {
        throw ArgumentNullException("rootSubGraph", LOCATION);
    }

    if(subGraphs.find(rootSubGraph) != subGraphs.end()) return;

    LOG_DEBUG << "Following outer labels of nodes in sub-graph " << rootSubGraph << ends;
    set<CNode*> nodes;
    CNode* cursor = &rootSubGraph->getPrologue();
    while(cursor != nullptr)
    {
        nodes.insert(cursor);
        cursor = cursor->getNext();
    }

    this->collectLinkedSubGraphs(nodes, subGraphs);
}

bool CrossoverOperator::transferSubGraph(
    const CSubGraph& selectedSubGraph, 
    set<CSubGraph*>& subGraphs, 
    Individual& otherChild) const
{

    if(subGraphs.empty()) 
        return true;
        
    // a map with <original_subgraph_prologue_id, cloned_subgraph_instance>
    // for every cloned subgraph, store the id of the prologue of the original subgraph and
    // associate it to the cloned subgraph
    map<string, CSubGraph*> clonedSubGraphs;

    // clone the sub-graphs
    set<CSubGraph*>::iterator iterator = subGraphs.begin();
    while(iterator != subGraphs.end())
    {
        CSubGraph* subGraph = *iterator;
        ++iterator;

        // do not clone the original subgraph
        if(subGraph == &selectedSubGraph) 
            continue;
        
        
        // create a copy of the sub-graph to be inserted in the target individual
        CSubGraph* clonedSubGraph = subGraph->clone().release();
        clonedSubGraphs[subGraph->getPrologue().getId()] = clonedSubGraph;

        // get the graph that contains the sub-graph
        Assert(dynamic_cast<CGraph*> (subGraph->getParentContainer()) != nullptr);
        CGraph* parentGraph = (CGraph*) subGraph->getParentContainer();
        

        // search for the same graph in the target individual
        CGraph* targetGraph = otherChild.getGraphContainer().getCGraph(parentGraph->getSection().getId());
        if(targetGraph == nullptr) 
            return false;

        // attach the copied sub-graph in the target graph
        bool success = targetGraph->attachSubGraph(*clonedSubGraph);
        if(success == false) 
            return false;
    }

    // remove the sub-graphs from the original individual if not referenced anymore
    iterator = subGraphs.begin();
    while(iterator != subGraphs.end())
    {
        CSubGraph* subGraph = *iterator;
        ++iterator;
        
        // do not remove the original subgraph
        if(subGraph == &selectedSubGraph) 
            continue;
        
        // search for outer labels incoming from sub-graphs that are not contained in the "subGraphs" set
        int externalOuterLabels = 0;
        CNode* prologue = &subGraph->getPrologue();
        for(unsigned int e = 0; e < prologue->getBackEdgeCount(); e++)
        {
            // follow the incoming edges
            const Edge& edge = prologue->getBackEdge(e);
            const CNode& sourceNode = (const CNode&) edge.getFrom();
            
            if(edge.containsTag(Edge::parameterTagName))
            {
                const string& parameterName = edge.getTag(Edge::parameterTagName);
                const Parameter* parameter = 
                    sourceNode.getGenericMacro().getParameter(parameterName);
                
                if(dynamic_cast<const OuterLabelParameter*>(parameter) != nullptr)
                // the edge represents an outer label
                {
                    // check if the source node of the label is contained in one of the sub-graphs
                    // of the "subGraphs" set
                    CSubGraph* sourceSubGraph = dynamic_cast<CSubGraph*>(sourceNode.getParentContainer());
                    if(sourceSubGraph != nullptr && subGraphs.find(sourceSubGraph) == subGraphs.end())
                    {
                        externalOuterLabels++;
                    }
                }
            }
        }
        
        if(externalOuterLabels == 0)
        // no relevant references to the sub-graph were found: it can be removed
        {
            // get the graph that contains the sub-graph
            Assert(dynamic_cast<CGraph*> (subGraph->getParentContainer()) != nullptr);
            CGraph* parentGraph = (CGraph*) subGraph->getParentContainer();
            

            // detach and destroy the original sub-graph
            bool success = parentGraph->detachSubGraph(*subGraph);
            
            if(success == false) 
                return false;
                
            delete subGraph;
        }
    }
    subGraphs.clear();

    // rename all the old references (outer labels) on the cloned sub-graphs(so that the edges can be restored correctly)
    // from original prologue id to cloned prologue id
    map<string, CSubGraph*>::iterator mapIterator = clonedSubGraphs.begin();
    while(mapIterator != clonedSubGraphs.end())
    {
        // for every cloned sub-graph ...
        CSubGraph* newSubGraph = mapIterator->second;

        // scan all the nodes and update the old prologue targets with the new ones
        CNode* cursor = &newSubGraph->getPrologue();
        while(cursor != nullptr)
        {
            for(unsigned int i = 0; i < cursor->getEdgeCount(); i++)
            {
                tgraph::Node* node = (tgraph::Node*)cursor;
                tgraph::Edge& edge = node->getEdge(i);
                if(edge.getTo() == nullptr && edge.containsTag(Edge::targetTagName))
                // the edge is floating and has a record of the id of the previous attached node
                {
                    Tag& tag = edge.getTag(Edge::targetTagName);
                    
                    // remap the target record to the id of the cloned subgraph
                    if(tag.getValue() == mapIterator->first)
                    {
                        tag.setValue(newSubGraph->getPrologue().getId());
                    }
                }
            }
        
            // move to the next node of the subGraph
            cursor = cursor->getNext();
        }

        // move to the next subGraph
        ++mapIterator;
    }
    return true;
}

bool CrossoverOperator::swapSubGraphs(
    Individual& child1, 
    Individual& child2, 
    const CSubGraph& selectedSubGraph1, 
    const CSubGraph& selectedSubGraph2, 
    unsigned int cutPoint1, 
    unsigned int slice1Size, 
    unsigned int cutPoint2, 
    unsigned int slice2Size) const
{
    _STACK;

    // STEP1: collect the subgraphs pointed by the nodes of the slices
    // slice 1
    LOG_DEBUG 
        << "Collecting the nodes of the slice on sub-graph " 
        << selectedSubGraph1 << " to follow outer labels" << ends;
    set<CSubGraph*> subGraphs1;
    {
        // collect the nodes of the slice #1
        CNode* cursor = &selectedSubGraph1.getPrologue();
        for (unsigned int i = 0; i < cutPoint1; ++i) {
            Assert(cursor != nullptr);
            cursor = cursor->getNext();
        }
        set<CNode*> nodes;
        for (unsigned int i = 0; i < slice1Size; ++i) {
            Assert(cursor != nullptr);
            nodes.insert(cursor);
            cursor = cursor->getNext();
        }
        
        // collect the subgraphs pointed by the nodes of the slice #1
        this->collectLinkedSubGraphs(nodes, subGraphs1);
    }

    // slice 2
    LOG_DEBUG << "Collecting the nodes of the slice on sub-graph " << selectedSubGraph2 << " to follow outer labels" << ends;
    set<CSubGraph*> subGraphs2;
    {
        // collect the nodes of the slice #2
        // TODO same as before
        CNode* cursor = &selectedSubGraph2.getPrologue();
        for (unsigned int i = 0; i < cutPoint2; ++i) {
            Assert(cursor != nullptr);
            cursor = cursor->getNext();
        }
        set<CNode*> nodes;
        for(unsigned int i = 0; i < slice2Size; ++i) {
            Assert(cursor != nullptr);
            nodes.insert(cursor);
            cursor = cursor->getNext();
        }

        // collect the subgraphs pointed by the nodes of the slice #2
        this->collectLinkedSubGraphs(nodes, subGraphs2);
    }
    
    // STEP2: swap the subgraphs
    // copy the subgraphs pointed by the slice1 into the child2
    bool success = this->transferSubGraph(selectedSubGraph1, subGraphs1, child2);
    if(success == false) return false;

    // copy the subgraphs pointed by the slice2 into the child1
    success = this->transferSubGraph(selectedSubGraph2, subGraphs2, child1);
    if(success == false) return false;

    return true;
}

unsigned int CrossoverOperator::getParentsCardinality() const
{
	return 2;
}
