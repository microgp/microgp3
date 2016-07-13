/***********************************************************************\
|                                                                       |
| Individual.xml.cc                                                     |
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

const std::string CandidateSolution::XML_ATTRIBUTE_ID = "id";
const std::string CandidateSolution::XML_ATTRIBUTE_TYPE = "type";
const std::string CandidateSolution::XML_ATTRIBUTE_ALLOPATRIC_TAG = "allopatricTag";
const std::string CandidateSolution::XML_ATTRIBUTE_DELTAENTROPY = "deltaEntropy";
const std::string CandidateSolution::XML_CHILD_ELEMENT_LIFE = "life";
const std::string CandidateSolution::XML_ATTRIBUTE_STATE = "state";
const std::string CandidateSolution::XML_ATTRIBUTE_BIRTH = "birth";
const std::string CandidateSolution::XML_ATTRIBUTE_DEATH = "death";
const std::string CandidateSolution::XML_ATTRIBUTE_AGE = "age";

void CandidateSolution::readXml(const xml::Element& element)
{
    _STACK;
    
    m_id = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_ID);
    m_allopatricTag = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_ALLOPATRIC_TAG);
    m_deltaEntropy.setValue(xml::Utility::attributeValueToDouble(element, XML_ATTRIBUTE_DELTAENTROPY));
    
    // get the inner elements
    bool lifeFound = false;
    bool rawFitnessFound = false;
    bool scaledFitnessFound = false;
    bool lineageFound = false;
    
    const xml::Element* childElement = element.FirstChildElement();
    while (childElement) {
        const string& elementName = childElement->ValueStr();
        if (elementName == XML_CHILD_ELEMENT_LIFE) {
            if (lifeFound) {
                throw xml::DuplicateElementSchemaException(
                    "/evolutionaryAlgorithm/populations/population/individuals/individual/life", LOCATION);
            }
            lifeFound = true;
    
            m_state = static_cast<State>(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_STATE));
            if (STATE_COUNT <= m_state) {
                throw xml::SchemaException("Candidate solution state is not recognized: " + ugp3::Convert::toString((int)m_state), LOCATION);
            }
            m_birthGen = xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_BIRTH);
            m_age = xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_AGE);
            if (xml::Utility::hasAttribute(*childElement, XML_ATTRIBUTE_DEATH)) {
                m_deathGen = xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_DEATH);
            }
        }
        else if (elementName == Fitness::XML_NAME) {
            if (rawFitnessFound) {
                throw xml::DuplicateElementSchemaException(
                    "/evolutionaryAlgorithm/populations/population/individuals/individual/fitness", LOCATION);
            }
            rawFitnessFound = true;
            
            m_rawFitness.readXml(*childElement);
        } else if (elementName == getFitness().getXmlName()) {
            if (scaledFitnessFound) {
                throw xml::DuplicateElementSchemaException(
                    "/evolutionaryAlgorithm/populations/population/individuals/individual/scaledFitness", LOCATION);
            }
            scaledFitnessFound = true;
            
            getFitness().readXml(*childElement);
        } else if (elementName == Lineage::XML_NAME) {
            if (lineageFound) {
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/individuals/individual/lineage", LOCATION);
            }
            lineageFound = true;

            getLineage().readXml(*childElement);
        }
        
        childElement = childElement->NextSiblingElement();
    }
    
    if (!lifeFound) {
        throw xml::MissingElementSchemaException("/evolutionaryAlgorithm/populations/population/individuals/individual/life", LOCATION);
    }
    if (!lineageFound) {
        throw xml::MissingElementSchemaException("/evolutionaryAlgorithm/populations/population/individuals/individual/lineage", LOCATION);
    }
    if (rawFitnessFound && !scaledFitnessFound) {
        getFitness().setValues(getRawFitness().getValues());
        getFitness().setDescription(getRawFitness().getDescription());
    }
}

void CandidateSolution::writeXml(ostream& output) const
{
    _STACK;
    
    LOG_DEBUG << "Serializing object ugp3::core::CandidateSolution, with XML_NAME=" << getXmlName() << std::ends;
    
    output 
    << "<" << getXmlName()
    << " " << XML_ATTRIBUTE_ID <<"=\"" << xml::Utility::transformXmlEscChar(this->getId()) << "\""
    << " " << XML_ATTRIBUTE_ALLOPATRIC_TAG << "=\"" << xml::Utility::transformXmlEscChar(this->getAllopatricTag()) << "\""
    << " " << XML_ATTRIBUTE_TYPE << "=\"" << xml::Utility::transformXmlEscChar(this->getType()) << "\""
    << " " << XML_ATTRIBUTE_DELTAENTROPY << "=\"" << m_deltaEntropy.getValue() << "\""
    << ">" << endl;
    
    output
    << "<" << XML_CHILD_ELEMENT_LIFE << " "
    << XML_ATTRIBUTE_STATE << "='" << static_cast<int>(m_state) << "' "
    << XML_ATTRIBUTE_BIRTH << "='" << getBirth() << "' "
    << XML_ATTRIBUTE_DEATH << "='" << getDeath() << "' "
    << XML_ATTRIBUTE_AGE   << "='" << getAge() << "' "
    << "/>" << endl;
    
    LOG_DEBUG << "Writing candidate solution lineage..." << ends;
    this->getLineage().writeXml(output);
    
    LOG_DEBUG << "Writing candidate solution raw fitness..." << ends;
    this->getRawFitness().writeXml(output);
    
    LOG_DEBUG << "Writing candidate solution scaled fitness..." << ends;
    this->getFitness().writeXml(output);
    
    writeInnerXml(output);
    
    output << "</" << getXmlName() << ">" << endl;
}
