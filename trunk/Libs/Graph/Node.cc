/***********************************************************************\
|                                                                       |
| Node.cc                                                               |
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

/**
 * @file Node.cc
 * Implementation of the Node class.
 * @see Node.h
 * @see Node.xml.cc
 */

#include "ugp3_config.h"
#include "TaggedGraph.h"
using namespace std;
using namespace tgraph;
using namespace ugp3;

InfinityString Node::idCounter;

Node::Node()
    : id(idCounter.toString())
{
    _STACK;

    Node::idCounter++;
}

Node::~Node()
{
	// delete outgoing edges
	for(unsigned int i = 0; i < this->edges.size(); i++)
	{
		delete this->edges[i];
	}
	
#ifndef NDEBUG
    this->edges.clear();
    id = "";
#endif
    
	// do not delete incoming edges: they are already destroyed in the source node
}

void Node::addEdge(Edge& edge)
{
    _STACK;

    if(edge.getTo() != this && &edge.getFrom() != this)
    {
        throw ArgumentException("The edge is not related to the node.", LOCATION);
    }

    if(edge.getTo() == this)
    // 'this' is the target node
    {
        for(unsigned int i = 0; i < this->backEdges.size(); i++)
        { 
            if(this->backEdges[i] == &edge)
                throw ArgumentException("The edge is already contained in the node.", LOCATION);
        }

        this->backEdges.push_back(&edge);

        if(&edge.getFrom() != edge.getTo())
        {
           Node& source = edge.getFrom();
           if(source.contains(edge) == false)
           {
                source.addEdge(edge);
           }
        }
    }
    else if(&edge.getFrom() == this)
    // 'this' is the source node
    {
        for(unsigned int i = 0; i < this->edges.size(); i++)
        { 
            if(this->edges[i] == &edge)
                throw ArgumentException("The edge is already contained in the node.", LOCATION);
        }

        this->edges.push_back(&edge);

        if(&edge.getFrom() != edge.getTo() && edge.getTo() != nullptr)
        {
           Node* target = edge.getTo();
           if(target->contains(edge) == false)
           {
                target->addEdge(edge);
           }
        }
    }
}

Edge& Node::getBackEdge( unsigned int i ) const
{
    _STACK;

    if(i >= this->backEdges.size())
    {
        throw ugp3::IndexOutOfBoundsException("Node::backEdges", LOCATION);
    }

    Assert(this->backEdges[i] != nullptr);
    return *this->backEdges[i];
}

Edge& Node::getEdge( unsigned int i ) const
{
    _STACK;

    if(i >= this->edges.size())
    {
        throw ugp3::IndexOutOfBoundsException("Node::edges", LOCATION);
    }

    return *this->edges[i];
}

Edge* Node::addEdgeTo(Node* node)
{
    _STACK;

    return new Edge(*this, node);
}

bool Node::contains(const Edge& edge) const
{
    for(unsigned int i = 0; i < this->edges.size(); i++)
    {
        Edge* outgoingEdge = this->edges[i];
        if(outgoingEdge == &edge)
        {
            return true;
        }
    }

    for(unsigned int i = 0; i < this->backEdges.size(); i++)
    {
        Edge* incomingEdge = this->backEdges[i];
        if(incomingEdge == &edge)
        {
             return true;
        }
    }

    return false;
}

void Node::removeBackEdge(Edge* edge)
{
     _STACK;

    if(edge == nullptr)
    {
        throw ArgumentNullException("edge", LOCATION);
    }

    if(edge->getTo() != this)
    {
        throw ArgumentException("The target of the edge is not correct.", LOCATION);
    }

    for(unsigned int i = 0; i < this->backEdges.size(); i++)
    {
        if(this->backEdges[i] == edge)
        {
            // remove the edge
            this->backEdges.erase(this->backEdges.begin() + i);
            return;
        }
    }
    
    throw ArgumentException("Could not find the specifid edge.", LOCATION);
    
}
void Node::removeEdge(Edge* edge)
{
    _STACK;

    if(edge == nullptr)
    {
        throw ArgumentNullException("edge", LOCATION);
    }

    if(&edge->getFrom() != this)
    {
        throw ArgumentException("The target of the edge is not correct.", LOCATION);
    }

    // check if the edge is in the outgoing edges list
    for(unsigned int i = 0; i < this->edges.size(); i++)
    {
        Edge* outgoingEdge = this->edges[i];
        if(outgoingEdge == edge)
        // found the edge
        {
            // remove the edge
            this->edges.erase(this->edges.begin() + i);

            return;
        }
    }

    throw ArgumentException("Could not find the specifid edge.", LOCATION);
}

#ifdef DEBUG
bool Node::connectedTo( Node *d )
{
 vector<Edge*>::iterator edgeIterator;

 for( edgeIterator=edges.begin(); edgeIterator<edges.end(); ++edgeIterator)
 {
  if( (*edgeIterator)->getTo() == d ) return true;
 }
 return false;
}
#endif /// DEBUG

