/***********************************************************************\
|                                                                       |
| CNode.xml.cc                                                          |
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
#include "CNode.h"

using namespace std;
using namespace ugp3;
using namespace tgraph;
using namespace ugp3::constraints;
using namespace ugp3::ctgraph;
using namespace xml;

void CNode::readXml(const xml::Element& element)
{
	_STACK;
	
	if(this->getConstrain() == nullptr && dynamic_cast<const Constraints*>(this->getConstrain()) == nullptr)
	{
		throw Exception("Constrain is missing.", LOCATION);
	}
	else if(dynamic_cast<const Constraints*>(this->getConstrain()) == nullptr)
	{
		throw Exception("Constrain is missing.", LOCATION);
	}
	
	this->clear();

	Node::readXml(element);
	
	
    LOG_DEBUG << "Deserializing ugp3::ctgraph::CNode object" << ends;

    LOG_DEBUG << "Linking CNode to the Constraints" << ends;
    const string macroPath = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_CONSTRAINTSREF);

    LOG_DEBUG << "Retrieving definition of macro \"" << macroPath << "\" " << ends;
	const Constraints& constraints = (const Constraints&)*this->getConstrain();
	const GenericMacro* macro = constraints.getMacroByPath(macroPath);
	if(macro == nullptr)
    {
        throw Exception( "Could not bind the node " + this->getId() 
            + " to its macro definition since the macro " + macroPath 
            + " was not found in the constraints.", LOCATION);
    }
	this->setConstrain(*macro);

   

    LOG_DEBUG << "Node " << this << " successfully deserialized." <<  ends;
}

void CNode::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object ugp3::ctgraph::CNode " << this << ends;

    output 
        << "<" << XML_NAME << " " << XML_ATTRIBUTE_ID << "=\"" << this->getId() << "\" ";

	if(this->getConstrain() != nullptr)
	{
		output
        << XML_ATTRIBUTE_CONSTRAINTSREF << "=\"" << this->getGenericMacro().getPath() << "\"";
	}


    output << ">" << endl;
    
    if(this->next != nullptr)
    {
        Tag(CNode::TAG_NEXT, this->next->getId()).writeXml(output);
    }
    
    if(this->prev != nullptr)
    {
        Tag(CNode::TAG_PREV, this->prev->getId()).writeXml(output);
    }
    
    
    Taggable::writeXml(output);

    for(unsigned int i = 0; i < this->getEdgeCount(); i++)
    {
        Node::getEdge(i).writeXml(output);
    }

    output << "</" << XML_NAME << ">" << endl;
    
}

