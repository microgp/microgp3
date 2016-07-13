/***********************************************************************\
|                                                                       |
| BitArrayParameter.xml.cc                                              |
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
#include "Constraints.h"
using namespace std;
using namespace ugp3::constraints;
 
const string BitArrayParameter::XML_ATTRIBUTE_PATTERN = "pattern";
const string BitArrayParameter::XML_ATTRIBUTE_BASE = "base";
const string BitArrayParameter::XML_ATTRIBUTE_INIT = "initNull";

void BitArrayParameter::readXml(const xml::Element& element)
{
    _STACK;

    Parameter::readXml(element);
   
    if(element.Attribute("pattern") != nullptr)
    {
        this->pattern = xml::Utility::attributeValueToString(element, "pattern");

        for(unsigned int i = 0; i < this->pattern.length(); i++)
        {
             if(pattern[i] != '0' && pattern[i] != '1' && pattern[i] != '-')
             {
                throw Exception("The pattern is not valid.", LOCATION);
             }
        }
    }
    else 
    {
        unsigned int size = xml::Utility::attributeValueToUInt(element, "length");
        this->pattern = "";
        for(unsigned int i = 0; i < size; i++)
        {
            this->pattern += "-";
        }
    }
    
    if (xml::Utility::hasAttribute(element, XML_ATTRIBUTE_INIT)) {
        initNull = true;
    }
    
   
    const string base = xml::Utility::attributeValueToString(element, "base");
    if(base == Base::Binary.toString())
    {
        this->base = Base::Binary;
    }
    else if(base == Base::Hexadecimal.toString())
    {
        this->base = Base::Hexadecimal;
    }
    else if(base == Base::Octal.toString())
    {
        this->base = Base::Octal;
    }
    else throw Exception("The value " + base + " is not a valid base.", LOCATION);
}

void BitArrayParameter::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::FloatParameter" << ends;


	if (this->typeDefinition) {
        output << this->writeXmlAsTypeDefinition();
    } else {
		output 
			<< "<" << this->getXmlName() 
			<< " " 
			<< "xsi:type=\"bitArray\"" 
			<< " " 
			<< XML_ATTRIBUTE_NAME << "=\"" 
			<< xml::Utility::transformXmlEscChar(this->getName()) << "\"" 
			<< " " 
			<< XML_ATTRIBUTE_PATTERN << "=\"" << this->pattern << "\"" 
			<< " " 
			<< XML_ATTRIBUTE_BASE << "=\"" << this->base.toString() << "\""
            << " "
            << XML_ATTRIBUTE_INIT << "=\"" << initNull << "\""
			<< "/>" << endl;
	}
}
