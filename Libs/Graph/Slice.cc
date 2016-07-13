/***********************************************************************\
|                                                                       |
| Slice.cc                                                              |
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

using namespace std;
using namespace ugp3;
using namespace tgraph;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;

const int Slice::START = -1;
const int Slice::END = -2;

Slice::Slice()
    : id(idCounter.toString())
{
    _STACK;
    
    Slice::idCounter++;
    
}

Slice::Slice(unique_ptr<CNode> node)
    : id(idCounter.toString())
{
    _STACK;
    
    Slice::idCounter++;
    
    Assert(node.get() != nullptr);
    
    nodeSequence.push_back(node.release());
}

Slice::~Slice()
{
    for (unsigned int i = 0; i < this->nodeSequence.size(); i++)
        delete this->nodeSequence[i];
}

void Slice::clear()
{
    for (unsigned int i = 0; i < this->nodeSequence.size(); i++)
        delete this->nodeSequence[i];
        
    this->nodeSequence.clear();
}

void Slice::spliceSlice(unique_ptr<Slice> additional, int position)
{
    LOG_DEBUG << "Slice " << this->id << ": inserting slice " << additional->id << " at position " << position << ends;
    LOG_DEBUG << "Slice " << this->id << ": " << this->toString() << ends;
    
    _STACK;
    vector<CNode *>::iterator start;
    
    switch (position)
    {
    case Slice::START :
        start = nodeSequence.begin();
        break;
    case Slice::END :
        start = nodeSequence.end();
        break;
    default :
        Assert(position >= 0 && position <= (int)nodeSequence.size());
        start = nodeSequence.begin() + position;
        break;
    }
    
    nodeSequence.insert(start,additional->nodeSequence.begin(),additional->nodeSequence.end());
    LOG_DEBUG << "Slice " << this->id << ": " << this->toString() << ends;
    // avoid deleting the nodes as the slice is destroyed
    additional->nodeSequence.clear();
}

void Slice::swapNodes(int position1, int position2)
{
    _STACK;
    
    Assert(position1 >= 0 && position1 < (int)nodeSequence.size());
    Assert(position2 >= 0 && position2 < (int)nodeSequence.size());
    
    nodeSequence[position1]->detachInnerLabels();
    nodeSequence[position1]->detachOuterLabels();
    nodeSequence[position2]->detachInnerLabels();
    nodeSequence[position2]->detachOuterLabels();
    
    CNode* temp = nodeSequence[position1];
    nodeSequence[position1] = nodeSequence[position2];
    nodeSequence[position2] = temp;
}

void Slice::invertSubSequence(int position1, int position2)
{
    _STACK;
    int first, last;
    
    if (position1 < position2)
    {
        first = position1;
        last = position2;
    }
    else
    {
        first = position2;
        last = position1;
    }
    
    while (first < last)
    {
        this->swapNodes(first, last);
        first++;
        last--;
    }
}

void Slice::attachNextAndPrev()
{
    _STACK;
    
    LOG_DEBUG 	<< "Slice: Resolving next and prev references for a nodeSequence of size " 
		<< this->nodeSequence.size() << ends;

    for(unsigned int i = 0; i < this->nodeSequence.size(); i++)
    {
        Assert(nodeSequence[i] != nullptr);
        CNode& node = *nodeSequence[i];
	
	LOG_DEBUG << "Now analyzing node " << node.getId() << "..." << ends; 

        if(i < this->nodeSequence.size() - 1)
        // exclude the last node
        {
            Assert(this->nodeSequence[i + 1] != nullptr);
            CNode* nextNode = this->nodeSequence[i + 1];

            node.setNext(nextNode);
            nextNode->setPrev(&node);
        }
        
        if(node.containsTag(CNode::TAG_PLACE) == false)
        {
            node.addTag(CNode::TAG_PLACE, Convert::toString(i));
        }
        else node.getTag(CNode::TAG_PLACE).setValue(Convert::toString(i));
    }
    
    LOG_DEBUG << "Next and prev references restored" << ends;
}

unique_ptr<Slice> Slice::cutSlice(unsigned int first, unsigned int last)
{
    _STACK;
    
    LOG_DEBUG << "Slice " << this->id << ": cutting from " << first << " to " << last << ends;
    LOG_DEBUG << "Slice " << this->id << " before cutting" << endl << this->toString() << ends;
    
    unique_ptr<Slice> newSlice (new Slice());
    
    Assert(first <= last);
    last++;
    newSlice->nodeSequence.insert(
        newSlice->nodeSequence.begin(),
        this->nodeSequence.begin() + first,
        this->nodeSequence.begin() + last);
        
    for (unsigned int i = first; i < last; i++)
    {
        Assert(nodeSequence[i] != nullptr);
        
        nodeSequence[i]->detachInnerLabels();
        nodeSequence[i]->detachOuterLabels();
        
        nodeSequence[i]->setNext(nullptr);
        nodeSequence[i]->setPrev(nullptr);
    }
    
    this->nodeSequence.erase(
        this->nodeSequence.begin() + first,
        this->nodeSequence.begin() + last);
    
    LOG_DEBUG << "Slice " << this->id << " after cutting" << endl << this->toString() << ends;
    
    LOG_DEBUG << "Slice " << newSlice->id << ": extracted" << ends;
    LOG_DEBUG << "Slice " << newSlice->id << " " << newSlice->toString() << ends;
    
    return newSlice;
}

const std::string Slice::toString()
{
    ostringstream stream;
    
    for(unsigned int i = 0; i < this->getSize(); i++)
    {
        Assert(this->nodeSequence[i] != nullptr);
        stream << this->nodeSequence[i]->toString() << (i < this->getSize() - 1? "-" : "");
    }
    
    return stream.str();
}

/*
// get node position in the slice, starting from id
unsigned int Slice::getPosition(std::string id)
{
	for(unsigned int i = 0; i < this->nodeSequence.size(); i++)
	{
		if( this->nodeSequence[i]->getId().compare(id) == 0 )
			return i;
	}
	
	throw
}
*/
