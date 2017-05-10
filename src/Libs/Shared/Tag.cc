/***********************************************************************\
|                                                                       |
| Tag.cc                                                                |
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
#include "Taggable.h"
#include "Exceptions/SchemaException.h"
using namespace std;
using namespace ugp3;

const string Tag::XML_NAME = "tag";



Tag::Tag(const string& name, const string& value)
    : name(name),
    value(value)
{
    _STACK;

    if(this->name.empty() == true)
    {
        throw ugp3::ArgumentException("Cannot create a tag with an empty name.", LOCATION);
    }

#ifndef NDEBUG
    LOG_DEBUG << "Tag " << this << " created." <<  std::ends;
#endif
}

void Tag::readXml(const xml::Element& element)
{
	_STACK;

	this->clear();

    if(element.ValueStr() != XML_NAME)
    {
        throw xml::SchemaException("expected element 'tag'", LOCATION);
    }

    this->name = xml::Utility::attributeValueToString(element, "name");
    this->value = xml::Utility::attributeValueToString(element, "value");
}

bool Tag::equals(const Tag& tag) const
{
    return this->name == tag.name && this->value == tag.value;
}

void Tag::clear()
{
   this->name = "";
   this->value = "";
}

void Tag::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object Tag " << this << "." << std::ends;

    output << "<" << XML_NAME
    << " name=\"" << xml::Utility::transformXmlEscChar(this->name) << "\""
    << " value=\"" << xml::Utility::transformXmlEscChar(this->value) << "\"/>"
    << std::endl;
}

const std::string Tag::toString() const
{
    _STACK;

    	return this->name + "=" + this->value;
}

const std::string Tag::toString(double quantum) const
{
    _STACK;
	// if the value stored in the string this->value is real,
	// this function should return a quantized value, otherwise
	// entropy would have no meaning (since the symbols are computed
	// on the string returned by this function)
    
    // WARNING FIXME BUG is the comment before that still true? 
    // WARNING FIXME BUG duplicated quantization code with Graph/CNode.cc:1160

	double realValue = 0.0;
	char quantizedValue[256];

	sscanf(this->value.c_str(), "%lf", &realValue);
	realValue /= quantum;
	realValue = floor(realValue);

	sprintf(quantizedValue, "%f", realValue);
	string returnedValue(quantizedValue);

	return this->name + "=" + returnedValue;
}
