/***********************************************************************\
|                                                                       |
| Option.xml.cc                                                         |
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
| the Free Software Foundation, either version 3 of the License, or     |
| (at your option) any later version.                                   |
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
 * @file Option.xml.cc
 * Implementation of the xml related methods of the Option class.
 * @see Option.h
 * @see Option.cc
 * @see XMLIFace.h
 */

#include "ugp3_config.h"
#include "Option.h"
#include "StackTrace.h"
#include "Convert.h"
#include "Exceptions/SchemaException.h"
#include "Log.h"
using namespace ugp3;
using namespace std;

const string Option::XML_NAME = "option";
const string Option::XML_ATTRIBUTE_NAME = "name";
const string Option::XML_ATTRIBUTE_VALUE = "value";
const string Option::XML_ATTRIBUTE_TYPE = "type";
const string Option::XML_POPULATIONS_NAME = "populations";

void Option::writeXml(std::ostream& output) const
{
	_STACK;

	output
	// Lines added by AT
        << "<!-- " << this->getDescription() << " -->" << std::endl
	// end
		<< "<" << this->getXmlName()
		<< " " << XML_ATTRIBUTE_NAME << "=\"" << this->name << "\""
		<< " " << XML_ATTRIBUTE_VALUE << "=\"" << this->value << "\""
		<< " />" << std::endl;
}

void Option::readXml(const xml::Element& element)
{
	_STACK;

	LOG_DEBUG << "Parsing ./option ..." << ends;

	// get element name
	string elementString = element.ValueStr();
	if(elementString != this->getXmlName())
    	throw xml::SchemaException("expected element './option'", LOCATION);

	this->name = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_NAME);
	this->value = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_VALUE);

	if( this->name == "randomSeed" || this->name == "concurrentEvaluations")
	{
        this->type = "integer";
    }
    else if( this->name == "removeTempFiles" || this->name == "recoveryOverwriteOutput" || this->name == "recoveryDiscardFitness" )
    {
        this->type = "boolean";
    }
    else
    {
        this->type = "string";
    }

	if(this->validate() == false)
	{
		throw Exception("value attribute is not valid for option '"
		    + this->name + "'.", LOCATION);
	}
}

