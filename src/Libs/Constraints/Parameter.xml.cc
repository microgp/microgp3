/***********************************************************************\
|                                                                       |
| Parameter.xml.cc                                                      |
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
using namespace ugp3::constraints;

const std::string Parameter::XML_NAME = "item";
const std::string Parameter::XML_ATTRIBUTE_NAME = "name";
const std::string Parameter::XML_ATTRIBUTE_TYPE = "type";
const std::string Parameter::XML_ATTRIBUTE_REF = "ref";

std::string Parameter::writeXmlAsTypeDefinition() const
{
	_STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::Parameter." << std::ends;

	std::ostringstream output;

	if(this->typeDefinition != nullptr)
	{
		output 
			<< "<" << this->getXmlName() << " xsi:type=\"definedType\" " 
			<< XML_ATTRIBUTE_REF << "=\"" << xml::Utility::transformXmlEscChar(this->typeDefinition->getName()) << "\" "
			<< XML_ATTRIBUTE_NAME << "=\"" << xml::Utility::transformXmlEscChar(this->getName()) << "\"/>" 
			<< std::endl;
	}
	else output << "";

	return output.str();
}

void Parameter::readXml(const xml::Element& element)
{
    _STACK;

    this->name = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_NAME);
    this->typeDefinition = nullptr;
}

