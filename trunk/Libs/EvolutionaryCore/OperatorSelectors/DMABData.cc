/***********************************************************************\
|                                                                       |
| DMABData.cc |
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
 * @file DMABData.cc
 *
 */

#include "DMABData.h"

#include "Exceptions/SchemaException.h"

namespace ugp3 {
namespace core {

DMABData::DMABData()
{

}

DMABData::DMABData(Operator& op)
: Data(op)
{

}

DMABData::DMABData(const std::string& operatorName)
: Data(operatorName)
{

}

void DMABData::clear()
{
    Data::clear();
    
    nExecutions = 0;
    meanReward = 0;
    maxDeviation = 0;
    meanDeviation = 0;
    pastRewards.clear();
}

void DMABData::reset()
{
    meanReward = 0;
    nExecutions = 0;
    meanDeviation = 0;
    maxDeviation = 0;
}


double DMABData::getMABWeight() const
{
    return meanReward;
}

const std::string DMABData::XML_CHILD_ELEMENT_DMAB = "dmab";
const std::string DMABData::XML_ATTRIBUTE_NEXECTUIONS = "nexections";
const std::string DMABData::XML_ATTRIBUTE_MEAN_REWARD = "mean_reward";
const std::string DMABData::XML_ATTRIBUTE_MEAN_DEVIATION = "mean_deviation";
const std::string DMABData::XML_ATTRIBUTE_MAX_DEVIATION = "max_deviation";
const std::string DMABData::XML_ATTRIBUTE_PAST_REWARDS = "past_rewards";

void DMABData::writeInnerXml(std::ostream& output) const
{
    Data::writeInnerXml(output);
    
    std::ostringstream pastRewardsString;
    for (double r: pastRewards) {
        pastRewardsString << r;
    }
    
    // Print DMAB information
    output << "  <" << XML_CHILD_ELEMENT_DMAB << " " <<
    XML_ATTRIBUTE_NEXECTUIONS << "=\"" << nExecutions << "\" " <<
    XML_ATTRIBUTE_MEAN_REWARD << "=\"" << meanReward << "\" " <<
    XML_ATTRIBUTE_MEAN_DEVIATION << "=\"" << meanDeviation << "\" " <<
    XML_ATTRIBUTE_MAX_DEVIATION << "=\"" << maxDeviation << "\" " <<
    XML_ATTRIBUTE_PAST_REWARDS << "=\"" << pastRewardsString.str() << "\" " <<
    "/>" << std::endl;
    
}

void DMABData::readXml(const xml::Element& element)
{
    Data::readXml(element);
    
    bool dmabFound = false;
    
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        std::string elementName = childElement->ValueStr();
        if (elementName == XML_CHILD_ELEMENT_DMAB)
        {
            if (dmabFound)
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/operatorStatistics/operator/" + XML_CHILD_ELEMENT_DMAB, LOCATION);
            
            dmabFound = true;
            
            nExecutions = xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_NEXECTUIONS);
            meanReward = xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_MEAN_REWARD);
            meanDeviation = xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_MEAN_DEVIATION);
            maxDeviation = xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_MAX_DEVIATION);
            std::istringstream pastRewardsString;
            pastRewardsString.str(xml::Utility::attributeValueToString(
                *childElement, XML_ATTRIBUTE_PAST_REWARDS));
            double r;
            while (pastRewardsString >> r) {
                pastRewards.push_back(r);
            }
        }
        childElement = childElement->NextSiblingElement();
    }
}

void DMABData::dumpStatistics(std::ostream& output) const
{
    Data::dumpStatistics(output);

    output << "," << getMABWeight();
}

void DMABData::dumpStatisticsHeader(const std::string& name, std::ostream& output) const
{
    Data::dumpStatisticsHeader(name, output);

    output << "," << name << "_MAB_" << getOperatorName();
}

}
}