/***********************************************************************\
|                                                                       |
| SAData.cc |
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

/**
 * @file SAData.cc
 *
 */

#include "SAData.h"
#include <ArgumentOutOfRangeException.h>
#include "SchemaException.h"

namespace ugp3 {
namespace core {
    
SAData::SAData(Operator& op)
: Data(op)
{

}

SAData::SAData(const std::string& operatorName)
: Data(operatorName)
{

}

SAData::SAData()
{

}

void SAData::clear()
{
    Data::clear();
    
    this->minWeight = 0;
    this->maxWeight = 1;
    this->weight = 1;
}

double SAData::getWeight() const
{
    return this->weight;
}

double SAData::getMinWeight() const
{
    return this->minWeight;
}

double SAData::getMaxWeight() const
{
    return this->maxWeight;
}

/**
 * @def MY_PATCH_NOBOUNDS
 * If not defined, the application will throw an exception if the activation weight of an operator exceeds the limits.
 */
#define MY_PATCH_NOBOUNDS

void SAData::setWeight(double value)
{
    _STACK;

    if(value < this->minWeight || value > this->maxWeight)
    {
#ifndef MY_PATCH_NOBOUNDS
        throw ArgumentOutOfRangeException("value = " + Convert::toString(value), LOCATION);
#endif
        LOG_WARNING
            << "Value " << value << "for " << this->operatorName <<" is out of bounds ["
            << this->minWeight << "," << this->maxWeight <<
            "] in Statistics::setWeight()" << std::ends;
    }
    this->weight = value;
}

void SAData::setMaxWeight(double value)
{
    if(value < this->minWeight || value > 1.0)
    {
        LOG_ERROR << "Value: " << value << " is not in [" << this->minWeight << ", 1.0]." << std::ends;
        throw ArgumentOutOfRangeException("value", LOCATION);
    }

    this->maxWeight = value;
}


void SAData::setMinWeight(double value)
{
    if(value < 0.0 || value > this->maxWeight)
    {
        throw ArgumentOutOfRangeException("value", LOCATION);
    }

    this->minWeight = value;
}


const std::string SAData::XML_CHILD_ELEMENT_WEIGHT = "weight";
const std::string SAData::XML_ATTRIBUTE_MAXIMUM = "maximum";
const std::string SAData::XML_ATTRIBUTE_CURRENT = "current";
const std::string SAData::XML_ATTRIBUTE_MINIMUM = "minimum";

void SAData::readXml(const xml::Element& element)
{
    ugp3::core::Data::readXml(element);
    
    bool weightFound = false;
    
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        std::string elementName = childElement->ValueStr();
        if(elementName == XML_CHILD_ELEMENT_WEIGHT)
        {
            if (weightFound == true)
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/operatorStatistics/operator/weight", LOCATION);

            weightFound = true;

            this->setMaxWeight(xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_MAXIMUM));
            this->setWeight(xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_CURRENT));
            this->setMinWeight(xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_MINIMUM));
        }
        
        childElement = childElement->NextSiblingElement();
    }
}

void SAData::writeInnerXml(std::ostream& output) const
{
    Data::writeInnerXml(output);
    
    output
        << "  <" << XML_CHILD_ELEMENT_WEIGHT << " "
        << XML_ATTRIBUTE_CURRENT << "=\"" << this->weight << "\" "
        << XML_ATTRIBUTE_MINIMUM << "=\"" << this->minWeight << "\" "
        << XML_ATTRIBUTE_MAXIMUM << "=\"" << this->maxWeight << "\""
        << "/>"
        << std::endl;
}

void SAData::dumpStatistics(std::ostream& output) const
{
    ugp3::core::Data::dumpStatistics(output);

    output << "," << getWeight();
}
    
void SAData::dumpStatisticsHeader(const std::string& name, std::ostream& output) const
{
    ugp3::core::Data::dumpStatisticsHeader(name, output);

    output << "," << name << "_SA_" << getOperatorName();
}

}
}