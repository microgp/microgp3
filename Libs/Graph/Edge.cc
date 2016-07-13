/***********************************************************************\
|                                                                       |
| Edge.cc                                                               |
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
#include "Edge.h"

#include "Log.h"
#include "StackTrace.h"
#include "Exceptions/SchemaException.h"

using namespace tgraph;
using namespace ugp3;
using namespace std;


const string Edge::offsetTagName = "offset";
const string Edge::parameterTagName = "parameter";
const string Edge::targetTagName = "target";

Edge::Edge(Node& from, Node* to)
    : from(from),
    to(to)
{
    _STACK;

    // the edge is also added to the target node
    this->from.addEdge(*this);
}

Edge::Edge(Node& sourceNode)
    : from(sourceNode),
	to(nullptr)
{ }

void Edge::clear()
{
	Taggable::clear();

	this->to = nullptr;
}

Edge::~Edge()
{
    _STACK;

#ifndef NDEBUG // speed up when not in debug mode
    this->to = nullptr;
#endif
}

const std::string Edge::toString() const
{
    return
        "{from=" + this->from.toString() +
        " to=" + (this->to != nullptr? this->to->toString() : "null") + "}";
}

void Edge::setTo(Node *to)
{
    _STACK;

    Node* oldTo = this->to;
    Node* newTo = to;

    if(oldTo == newTo) return;

    if(oldTo != nullptr)
    {
       oldTo->removeBackEdge(this);
    }

    this->to = newTo;

    if(newTo != nullptr)
    {
        newTo->addEdge(*this);
    }
}

