/***********************************************************************\
|                                                                       |
| Format.xml.cc                                                         |
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

const string LabelFormat::XML_NAME = "labelFormat";
const string IdentifierFormat::XML_NAME = "identifierFormat";
const string CommentFormat::XML_NAME = "commentFormat";
const string UniqueTagFormat::XML_NAME = "uniqueTagFormat";
const string Format::XML_CHILD_ELEMENT_VALUE = "value";

void Format::writeXml(std::ostream& output) const
{
	_STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::Format" << ends;

    output << "<" << this->getXmlName() << ">";
    
    output << textBefore << "<" << XML_CHILD_ELEMENT_VALUE << " />" << textAfter;
    
    output << "</" << this->getXmlName() << ">" << endl;
}

void Format::readXml(const xml::Element& element)
{
    // get element name
    string elementString = element.ValueStr();

    if(elementString != this->getXmlName())
        throw xml::SchemaException("expected element './" + this->getXmlName() + "'", LOCATION);

    bool expressionFound = false;
    const xml::TiXmlNode* childNode = element.FirstChild();
    while(childNode != nullptr)
    {
        if(childNode->ToElement() != nullptr)
        {
            const xml::TiXmlElement* childElement = childNode->ToElement();
            string childName = childElement->ValueStr();

            if(childName != XML_CHILD_ELEMENT_VALUE)
                throw xml::SchemaException("expected element './value'", LOCATION);

            if(expressionFound == true)
                throw xml::DuplicateElementSchemaException("./value", LOCATION);

            expressionFound = true;
        }
        else if(childNode->ToText() != nullptr)
        {
        	const string text = childNode->ToText()->Value();
           
            if(expressionFound == false)
            {
            	this->textBefore = text;
            }
            else this->textAfter = text;
        }

        childNode = element.IterateChildren(childNode);
    }

    if(expressionFound == false)
        throw xml::MissingElementSchemaException("./value", LOCATION);	
}

void LabelFormat::writeXml(std::ostream& output) const
{
	_STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::LabelFormat" << ends;

    output << "<" << this->getXmlName() << ">";
    
    output << textBefore ;
    if(textAfter != "")
    {
    	output << "<" << XML_CHILD_ELEMENT_VALUE << ">" << textAfter;
    }
    
    output << "</" << this->getXmlName() << ">" << endl;
}
    		
void LabelFormat::readXml(const xml::Element& element)
{
    // get element name
    string elementString = element.ValueStr();

    if(elementString != this->getXmlName())
        throw xml::SchemaException("expected element './" + this->getXmlName() + "'", LOCATION);

    bool expressionFound = false;
    const xml::TiXmlNode* childNode = element.FirstChild();
    while(childNode != nullptr)
    {
        if(childNode->ToElement() != nullptr)
        {
            const xml::TiXmlElement* childElement = childNode->ToElement();
            string childName = childElement->ValueStr();

            if(childName != XML_CHILD_ELEMENT_VALUE)
                throw xml::SchemaException("expected element './value'", LOCATION);

            if(expressionFound == true)
                throw xml::DuplicateElementSchemaException("./value", LOCATION);

            expressionFound = true;
        }
        else if(childNode->ToText() != nullptr)
        {
        	const string text = childNode->ToText()->Value();
           
            if(expressionFound == false)
            {
            	this->textBefore = text;
            }
            else this->textAfter = text;
        }

        childNode = element.IterateChildren(childNode);
    }
}

