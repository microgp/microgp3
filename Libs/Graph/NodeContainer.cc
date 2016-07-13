/***********************************************************************\
|                                                                       |
| NodeContainer.cc                                                      |
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
#include "ConstrainedTaggedGraph.h"
using namespace ugp3::ctgraph;
using namespace tgraph;
using namespace std;

void NodeContainer::setAsParent(CNode* node, NodeContainer* newParent) const
{
    _STACK;

    if(node == nullptr)
    {
        throw ArgumentNullException("node", LOCATION);
    }

    node->parentContainer = newParent;
}

void NodeContainer::setPrologue(std::unique_ptr<CNode>& value)
{
	if(this->prologue.get() != nullptr)
		this->prologue->detachOuterLabels();
	
	this->prologue.reset(value.release());
}

void NodeContainer::setEpilogue(std::unique_ptr<CNode>& value)
{
	if(this->epilogue.get() != nullptr)
		this->epilogue->detachOuterLabels();

	this->epilogue.reset(value.release());
}

// overloaded methods added by Alberto Tonda, 2015
void NodeContainer::setPrologue(CNode* node)
{
	if( this->prologue.get() != nullptr )
		this->prologue->detachOuterLabels();
	
	this->prologue.reset( node );
}

void NodeContainer::setEpilogue(CNode* node)
{
	if( this->epilogue.get() != nullptr )
		this->epilogue->detachOuterLabels();
	
	this->epilogue.reset( node );
}
