/***********************************************************************\
|                                                                       |
| Node.xml.cc                                                           |
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
 * @file Node.xml.cc
 * Implementation of the xml related methods of the Node class.
 * @see Node.h
 * @see Node.cc
 */

#include "ugp3_config.h"
#include "TaggedGraph.h"
using namespace std;
using namespace tgraph;
using namespace ugp3;

const string Node::XML_NAME = "node";
const string Node::XML_ATTRIBUTE_ID = "id";

void Node::readXml(const xml::Element& element)
{
	_STACK;

	this->clear();
	
	Taggable::readXml(element);

    if(element.ValueStr() != this->getXmlName())
    {
        throw xml::SchemaException("expected element 'node'.", LOCATION);
    }

    LOG_DEBUG << "Deserializing tgraph::Node object" << ends;

    this->id = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_ID);
    LOG_DEBUG << "Setting the id of the new node to " <<  this->id << ends;
    InfinityString infinityString(this->id);
    if(Node::idCounter <= infinityString)
    {
        Node::idCounter = infinityString;
        Node::idCounter++;
    }

    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        const string elementName = childElement->ValueStr();
        if(elementName == Edge::XML_NAME)
        {
            Edge* edge = new Edge(*this);
            edge->readXml(*childElement);
            this->addEdge(*edge);
        }

        childElement = childElement->NextSiblingElement();
    }
}

void Node::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object tgraph::Node " << this << "." << std::ends;


    output << "<" << this->getXmlName() 
        << " " 
        << XML_ATTRIBUTE_ID << "=\""<< this->id << "\"" 
        << ">" << endl;

    Taggable::writeXml(output);

    for( unsigned int i = 0; i < edges.size(); i++)
    {
        edges[i]->writeXml(output);
    }

    output << "</" << this->getXmlName() << ">";
}

