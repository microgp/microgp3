/***********************************************************************\
|                                                                       |
| Group.xml.cc                                                          |
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
 * @file Group.xml.cc
 * Implementation of the xml related methods of the Group class.
 * @see Group.h
 * @see Group.cc
 */

#include "EvolutionaryCore.h"

using namespace ugp3::core;
using namespace ugp3::log;
using namespace std;

const std::string Group::XML_NAME = "group";
const std::string Group::XML_TYPE = "group";
const std::string Group::XML_CHILD_INDIVIDUALS = "individuals";
const std::string Group::XML_CHILD_INDIVIDUAL = "individual";
const std::string Group::XML_ATTRIBUTE_REF = "ref";

void Group::readXml(const xml::Element& element)
{
    CandidateSolution::readXml(element);
    
//    if (element.ValueStr() != Group::XML_NAME) {
//        throw xml::SchemaException("expected element '" + Group::XML_NAME + "'", LOCATION);
//    }
    
    bool individualsFound = false;

    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        string elementName = childElement->ValueStr();
        if (elementName == XML_CHILD_INDIVIDUALS)
        {
            if (individualsFound)
            {
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/groups/group/individuals", LOCATION);
            }
            
            individualsFound = true;
            
            const xml::Element* individualElement = childElement->FirstChildElement();
            while(individualElement!=nullptr)
            {
                if(individualElement->ValueStr()!=XML_CHILD_INDIVIDUAL)
                {
                    throw xml::SchemaException("expected element '" + XML_CHILD_INDIVIDUAL + "'", LOCATION);
                }
                
                GEIndividual *ind=getPopulation().getIndividualById(xml::Utility::attributeValueToString(*individualElement, XML_ATTRIBUTE_REF));
                
                if(this->addIndividual(ind))
                {
                    ind->addGroup(this);
                    LOG_DEBUG << "Assigned individual " << *ind << " to the group " << *this << "." << std::ends;
                }
                
                individualElement=individualElement->NextSiblingElement();
            }
        }
        childElement = childElement->NextSiblingElement();
    }
    
    if (!individualsFound) {
        throw xml::MissingElementSchemaException("/evolutionaryAlgorithm/populations/population/groups/group/individuals", LOCATION);
    }
    
}

void Group::writeInnerXml(ostream& output) const
{
    _STACK;
    
    LOG_DEBUG << "Serializing object ugp3::core::Group." << std::ends;
    
    output << "<" << XML_CHILD_INDIVIDUALS << ">" << endl;
    
    for (GEIndividual* ind: m_individuals) {
        output << "<" << XML_CHILD_INDIVIDUAL << " " << XML_ATTRIBUTE_REF 
        << "=\"" << ind->getId() << "\"/>" << endl;
    }
    
    output << "</" << XML_CHILD_INDIVIDUALS << ">" << endl;
}

