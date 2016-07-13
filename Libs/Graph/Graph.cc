/***********************************************************************\
|                                                                       |
| Graph.cc                                                              |
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
#include "Graph.h"
#include "Edge.h"

using namespace std;
using namespace ugp3;
using namespace tgraph;

Graph::Graph()
{ }

Graph::~Graph()
{
    _STACK;

    for(unsigned int i = 0; i < this->nodes.size(); i++)
    {
		
        delete this->nodes[i];
 
 #ifndef NDEBUG
        this->nodes[i] = nullptr;
 #endif
    }
    
#ifndef NDEBUG
    this->nodes.clear();
#endif
}

Graph::Graph(const xml::Element* element)
{
    _STACK;

    if(element == nullptr)
    {
        throw ArgumentNullException("element", LOCATION);
    }

    this->buildFromXml(element);
}

Node* Graph::createNode(const xml::Element *element)
{
    Node* node = new Node();
    node->readXml(*element);
    
    return node;
}

void Graph::removeNode(Node& node)
{
    _STACK;

    for(unsigned int i = 0; i < this->nodes.size(); i++)
    {
        if(&node == this->nodes[i])
        {
            this->nodes.erase(this->nodes.begin() + i);
            return;
        }
    }

    throw ArgumentException("The specified node does not belong to the graph.", LOCATION);
}

void Graph::addNode(Node& node)
{
    _STACK;

    for(unsigned int i = 0; i < this->nodes.size(); i++)
    {
        if(this->nodes[i] == &node)
        {
            throw ArgumentException("The specified node already belongs to the graph.", LOCATION);
        }
    }

    this->nodes.push_back(&node);
}

bool Graph::addEdge( Node *f, Node *t )
{
    _STACK;

    bool fFound = false, tFound = false;

    for( unsigned int i=0; !fFound && i<nodes.size(); i++ )
        if( nodes[i] == f ) fFound = true;
        
    for( unsigned int i=0; !tFound && i<nodes.size(); i++ )
        if( nodes[i] == t ) tFound = true;

    if( fFound && tFound )
        f->addEdge( *new Edge( *f, t ) ); 

    return (fFound & tFound);
}

Node *Graph::getNode( unsigned int p ) const
{
    return nodes[p];
}


