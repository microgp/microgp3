/***********************************************************************\
|                                                                       |
| Macro.xml.cc                                                          |
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
#include "Debug.h"
#include "Constraints.h"
using namespace std;
using namespace ugp3::constraints;

const std::string Macro::XML_NAME = "macro";

void Macro::writeXml(ostream& output) const
{
	_STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::Macro" << ends;

	output << "<" << this->getXmlName() << " " << XML_ATTRIBUTE_ID << "=\"" << this->getId() << "\" weight=\"" << this->weight << "\">" << endl;
	output << this->getInnerXmlDescription();
	output << "</" << this->getXmlName() << ">" << endl;
}

void Macro::readXml(const xml::Element& element)
{
	_STACK;
	
	this->clear();
	
	GenericMacro::readXml(element);
	
	LOG_DEBUG << "Parsing ./macro" << ends;

	// get the name of the element
	string elementString = element.ValueStr();
	if(elementString != this->getXmlName())
		throw xml::SchemaException("expected element '" + this->getXmlName() + "'.", LOCATION);

	const char* nameAttributeChar = element.Attribute("weight");
	if(nameAttributeChar != nullptr)
	{
		this->weight = Convert::toDouble(nameAttributeChar);	
	}
}

