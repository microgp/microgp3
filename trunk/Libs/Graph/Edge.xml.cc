/***********************************************************************\
|                                                                       |
| Edge.xml.cc                                                           |
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
#include "TaggedGraph.h"
using namespace tgraph;
using namespace ugp3;
using namespace std;

const string Edge::XML_NAME = "edge";
const string Edge::XML_ATTRIBUTE_TO = "to";

void Edge::readXml(const xml::Element& element)
{
	_STACK;

	this->clear();
	
	Taggable::readXml(element);
	
    if(element.ValueStr() != this->getXmlName())
    {
        throw xml::SchemaException("expected element 'edge'", LOCATION);
    }

    const std::string targetId = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_TO);
    this->addTag(Edge::targetTagName, targetId);
}

void Edge::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object tgraph::Edge." << std::ends;

    if(this->to != nullptr)
    {
        output << "<" << XML_NAME << " " << XML_ATTRIBUTE_TO << "=\"" << to->getId() << "\"";
    }
    else
    {
        output << "<" << XML_NAME << " " << XML_ATTRIBUTE_TO << "=\"\"";
    }


    if(this->getTagCount() > 0)
    {
        output << ">" << std::endl;
        Taggable::writeXml(output);
        output << "</" << XML_NAME << ">" << endl;
    }
    else
    {
        output << "/>" << std::endl;
    }
}

