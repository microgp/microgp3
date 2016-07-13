/***********************************************************************\
|                                                                       |
| RankingSelection.xml.cc                                            |
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
#include "EvolutionaryCore.h"
using namespace std;
using namespace ugp3::core;

const string RankingSelection::XML_SCHEMA_TYPE = "ranking";
const string RankingSelection::XML_ATTRIBUTE_PRESSURE = "pressure";
const string RankingSelection::XML_ATTRIBUTE_PRESSUREMAX = "pressureMax";
const string RankingSelection::XML_ATTRIBUTE_PRESSUREMIN = "pressureMin";


void RankingSelection::writeXml(std::ostream& output) const
{
	output << "<" << this->getXmlName()
        << " " 
        << XML_ATTRIBUTE_TYPE << "=\"" << XML_SCHEMA_TYPE << "\""
		<< " " 
		<< XML_ATTRIBUTE_PRESSURE << "=\"" << this->getPressure() << "\""
		<< " " 
		<< XML_ATTRIBUTE_PRESSUREMIN << "=\"" << this->getPressureMin() << "\""
		<< " " 
		<< XML_ATTRIBUTE_PRESSUREMAX << "=\"" << this->getPressureMax() << "\" />" << endl;
}

void RankingSelection::readXml(const xml::Element& element)
{
	if(element.ValueStr() != this->getXmlName()) 
    {
        throw xml::SchemaException("expected element '/selection'", LOCATION);
    }

    this->pressure = xml::Utility::attributeValueToDouble(element, XML_ATTRIBUTE_PRESSURE);
    this->pressureMax = xml::Utility::attributeValueToDouble(element, XML_ATTRIBUTE_PRESSUREMAX);
    this->pressureMin = xml::Utility::attributeValueToDouble(element, XML_ATTRIBUTE_PRESSUREMIN);
}

