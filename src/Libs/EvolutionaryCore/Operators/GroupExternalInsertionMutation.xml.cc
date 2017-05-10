/***********************************************************************\
|                                                                       |
| GroupExternalInsertionMutationOperator.xml.cc                         |
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

#include "EvolutionaryCore.h"
#include "Operators/GroupExternalInsertionMutation.h"

using namespace std;
using namespace ugp3::core;

const string GroupExternalInsertionMutationOperator::XML_ATTRIBUTE_VALUE = "value";
const string GroupExternalInsertionMutationOperator::XML_PARAMETER_SELECTOR = "externalSelectorPath";
const string GroupExternalInsertionMutationOperator::XML_PARAMETER_INDIVIDUALS = "numberOfIndividuals";
const string GroupExternalInsertionMutationOperator::XML_PARAMETER_FITNESSHOLE = "fitnessHole";

bool GroupExternalInsertionMutationOperator::hasParameters() const
{
  return true;
}

void GroupExternalInsertionMutationOperator::writeXml(ostream& output) const
{
  output<< "    <" << XML_PARAMETER_SELECTOR << ' ' << XML_ATTRIBUTE_VALUE << "=\"" << this->selectorFileName << "\"/>" << endl;
  output<< "    <" << XML_PARAMETER_INDIVIDUALS << ' ' << XML_ATTRIBUTE_VALUE << "=\"" << this->numberOfIndividuals << "\"/>" << endl;
  output<< "    <" << XML_PARAMETER_FITNESSHOLE << ' ' << XML_ATTRIBUTE_VALUE << "=\"" << this->fitnessHole << "\"/>" << endl;
}

void GroupExternalInsertionMutationOperator::readXml(const xml::Element& element)
{
    bool selectorFound = false;
    bool individualsFound = false;
    bool fitnessHoleFound = false;

    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        string elementName = childElement->ValueStr();
        if(elementName == XML_PARAMETER_SELECTOR)
        {
            if(selectorFound == true)
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/operatorStatistics/operator/operatorParameters/externalSelectorPath", LOCATION);

            selectorFound = true;
            this->selectorFileName=xml::Utility::attributeValueToString(*childElement, XML_ATTRIBUTE_VALUE);
        }
        else if(elementName == XML_PARAMETER_INDIVIDUALS)
        {
            if(individualsFound == true)
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/operatorStatistics/operator/operatorParameters/numberOfIndividuals", LOCATION);

            individualsFound = true;
            this->numberOfIndividuals=xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE);
        }
        else if(elementName == XML_PARAMETER_FITNESSHOLE)
        {
            if(fitnessHoleFound == true)
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/operatorStatistics/operator/operatorParameters/fitnessHole", LOCATION);

            fitnessHoleFound = true;
            this->fitnessHole=xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_VALUE);
        }
        else
        {
            throw xml::SchemaException("unexpected element \"" + elementName + "\"", LOCATION);
        }

        childElement = childElement->NextSiblingElement();
    }
}

