/***********************************************************************\
|                                                                       |
| IntegerParameter.xml.cc                                               |
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
using namespace std;

void IntegerParameter::writeXml(ostream& output) const
{
	_STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::IntegerParameter" << ends;

	if(this->typeDefinition == nullptr)
	{
		output << "<item xsi:type=\"integer\" name=\"" << xml::Utility::transformXmlEscChar(this->getName()) << "\"";

        output << " minimum=\"" << this->minValue << "\" maximum=\"" << this->maxValue << "\"/>" << endl;
	}
	else output << this->writeXmlAsTypeDefinition();
}

void IntegerParameter::readXml(const xml::Element& element)
{
    _STACK;

    Parameter::readXml(element);

    this->minValue = xml::Utility::attributeValueToLong(element, "minimum");
    this->maxValue = xml::Utility::attributeValueToLong(element, "maximum");
}

