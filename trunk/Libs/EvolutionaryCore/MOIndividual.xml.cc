/***********************************************************************\
|                                                                       |
| MOIndividual.xml.cc                                                   |
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

const string MOIndividual::XML_ATTRIBUTE_LEVEL = "paretoLevel";
const string MOIndividual::XML_ATTRIBUTE_PREVIOUS = "previous";
const string MOIndividual::XML_ATTRIBUTE_CURRENT = "current";
const string MOIndividual::XML_ATTRIBUTE_VALUE = "value";
const string MOIndividual::XML_TYPE = "multiObjective";
const string MOIndividual::XML_CHILD_ELEMENT_PLACEHOLDER = "placeholder";
			
void MOIndividual::writeInnerXml(ostream& output) const
{
    _STACK;
    
    Individual::writeInnerXml(output);

    LOG_DEBUG << "Serializing object ugp3::core::MOIndividual" << ends;
    
    output
        << "<" << XML_CHILD_ELEMENT_PLACEHOLDER 
        << " " 
        << XML_ATTRIBUTE_VALUE << "=\"" << this->getPlaceholder() << "\"" 
        << "/>"<< endl;

	output 
		<< "<" << XML_ATTRIBUTE_LEVEL
        << " " 
        << XML_ATTRIBUTE_CURRENT << "='" << this->m_level << "'" 
        << " " 
        << XML_ATTRIBUTE_PREVIOUS << "='" << this->m_previousLevel << "'" 
        << "/>"<< endl;
	
}

void MOIndividual::readXml(const xml::Element& element)
{
    _STACK;

    LOG_DEBUG << "Deserializing object ugp3::core::MOIndividual" << ends;
    
    Individual::readXml(element);

    bool placeFound = false;
    const xml::Element* childElement = element.FirstChildElement();
 	while(childElement != nullptr) 
 	{
        string elementName = childElement->ValueStr();
        
        if (elementName == XML_ATTRIBUTE_LEVEL)
        {
        	this->m_level = xml::Utility::attributeValueToLong(*childElement, XML_ATTRIBUTE_CURRENT);
        	this->m_previousLevel = xml::Utility::attributeValueToLong(*childElement, XML_ATTRIBUTE_PREVIOUS);
        }
        else if(elementName == XML_CHILD_ELEMENT_PLACEHOLDER)
        {
            if(placeFound == true)
            {
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/individuals/individual/placeholder", LOCATION);
            }

            placeFound = true;
            this->m_placeholder = xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE);
        }
        else if (elementName == MOFitness::XML_NAME) {
            this->m_fitness.readXml(*childElement);    
        }
        
        childElement = childElement->NextSiblingElement();
    }
    
    if(placeFound == false)
    {
        throw xml::MissingElementSchemaException("/evolutionaryAlgorithm/populations/population/individuals/individual/placeholder", LOCATION);
    }
}

