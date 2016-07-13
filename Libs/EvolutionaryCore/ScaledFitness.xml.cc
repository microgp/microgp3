/***********************************************************************\
|                                                                       |
| ScaledFitness.xml.cc                                                |
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

const std::string ScaledFitness::XML_NAME = "scaledFitness";
const std::string ScaledFitness::XML_ATTRIBUTE_CLONECOUNT = "cloneCount";
const std::string ScaledFitness::XML_ATTRIBUTE_SCALING_FACTOR[ScaledFitness::SCALING_CAUSE_COUNT] = {
"cloneScaling",
"sharingScaling",
"contributionScaling",
};
const std::string ScaledFitness::XML_ATTRIBUTE_DESCRIPTION[ScaledFitness::SCALING_CAUSE_COUNT] = {
"cloneDesc",
"sharingDesc",
"contributionDesc",
};


void ScaledFitness::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object ugp3::core::ScaledFitness." << std::ends;

    if (isValid) {
        output << "<" << XML_NAME
            << " " << XML_ATTRIBUTE_CLONECOUNT << "=\""  << this->m_cloneCount << "\"";
        for (int cause = 0; cause < SCALING_CAUSE_COUNT; ++cause) {
            output << " " << XML_ATTRIBUTE_SCALING_FACTOR[cause] << "=\"" << this->m_scalingFactors[cause] << "\"";
            output << " " << XML_ATTRIBUTE_DESCRIPTION[cause] << "=\"" << xml::Utility::transformXmlEscChar(m_descriptions[cause]) << "\"";
        }
        output << ">" << std::endl;

        output
            << "<" << XML_CHILD_ELEMENT_DESCRIPTION << ">" 
            << xml::Utility::transformXmlEscChar(m_rawDescription) 
            << "</" << XML_CHILD_ELEMENT_DESCRIPTION << ">" << std::endl;

        for (double value: m_rawValues) {
            output 
            << "<" << XML_CHILD_ELEMENT_VALUE << ">" 
            << value 
            << "</" << XML_CHILD_ELEMENT_VALUE << ">" << std::endl;
        }
        output << "</" << XML_NAME << ">" << std::endl;
    }
}

void ScaledFitness::readXml(const xml::Element& element)
{
    _STACK;
    
    m_cloneCount = xml::Utility::attributeValueToUInt(element, XML_ATTRIBUTE_CLONECOUNT);
    for (int cause = 0; cause < SCALING_CAUSE_COUNT; ++cause) {
        if (xml::Utility::hasAttribute(element, XML_ATTRIBUTE_SCALING_FACTOR[cause])) {
            m_scalingFactors[cause].fromString(xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_SCALING_FACTOR[cause]));
        } else {
            m_scalingFactors[cause] = Scaling();
        }
        
        if (xml::Utility::hasAttribute(element, XML_ATTRIBUTE_DESCRIPTION[cause])) {
            m_descriptions[cause] = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_DESCRIPTION[cause]);
        } else {
            m_descriptions[cause] = "";
        }
    }

    // The reading of the raw values will trigger rescale and redescribe
    Fitness::readXml(element);
}

