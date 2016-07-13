/***********************************************************************\
|                                                                       |
| CombinatorialParameter.xml.cc                                              |
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

const string CombinatorialParameter::XML_CHILD_ELEMENT_VALUE = "value";
const string CombinatorialParameter::XML_ATTRIBUTE_DELIMITER = "delimiter";

void CombinatorialParameter::writeXml(ostream& output) const
{
	_STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::CombinatorialParameter" << ends;

	if(this->typeDefinition == nullptr)
	{
		output << "<" << this->getXmlName() << " type=\"combinatorial\" " 
		<< XML_ATTRIBUTE_NAME << "=\"" << xml::Utility::transformXmlEscChar(this->getName()) << "\""
		<< " " << XML_ATTRIBUTE_DELIMITER << "=\"" << xml::Utility::transformXmlEscChar(this->delimiter)  << "\""
		<< ">" << endl;
		
		for(unsigned int i = 0; i < this->constants.size(); i++)
		{
			output 
			<< "<" << XML_CHILD_ELEMENT_VALUE << ">" 
			<< xml::Utility::transformXmlEscChar(this->constants[i]) 
			<< "</" << XML_CHILD_ELEMENT_VALUE << ">" << endl;
		}

		output << "</" << this->getXmlName() << ">" << endl;
	}
	else output << this->writeXmlAsTypeDefinition();
}

void CombinatorialParameter::readXml(const xml::Element& element)
{
    _STACK;

    Parameter::readXml(element);

    // read attribute delimiter
    try
    {
        this->delimiter = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_DELIMITER);
    }
    catch(const Exception& e)
    {
        throw;
    }
    
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        if(childElement->ValueStr() == XML_CHILD_ELEMENT_VALUE)
        {
		string constant = xml::Utility::elementText(*childElement);
        	this->constants.push_back(constant);
        }
	else
	{
            	throw xml::SchemaException("expected elements \"value\" or \"delimiter\" for parameter of type \"combinatorial\"", LOCATION);
	}

        childElement = childElement->NextSiblingElement();
    }

    if(constants.size() == 0 || this->delimiter.length() == 0)
    {
        throw xml::SchemaException("expected elements \"value\" and \"delimiter\" for parameter of type \"combinatorial\"", LOCATION);
    }

    if(constants.size() < 2)
    {
	throw xml::SchemaException("a combinatorial parameter must have at least 2 values.", LOCATION);
    }
}

