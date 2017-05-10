/***********************************************************************\
|                                                                       |
| ConstantParameter.xml.cc                                              |
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

const string ConstantParameter::XML_CHILD_ELEMENT_VALUE = "value";

void ConstantParameter::writeXml(ostream& output) const
{
	_STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::ConstantParameter" << ends;

	if(this->typeDefinition == nullptr)
	{
		output << "<" << this->getXmlName() << " xsi:type=\"constant\" " 
		<< XML_ATTRIBUTE_NAME << "=\"" << xml::Utility::transformXmlEscChar(this->getName()) << "\">" << endl;
		
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

void ConstantParameter::readXml(const xml::Element& element)
{
    _STACK;

    Parameter::readXml(element);

    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        if(childElement->ValueStr() != XML_CHILD_ELEMENT_VALUE)
        {
            throw xml::SchemaException("expected element /constraints/typeDefinitions/type/value", LOCATION);
        }

        string constant = xml::Utility::elementText(*childElement);
        this->constants.push_back(constant);

        childElement = childElement->NextSiblingElement();
    }

    if(constants.size() == 0)
    {
        throw xml::SchemaException("/constraints/typeDefinitions/item/value", LOCATION);
    }
}

