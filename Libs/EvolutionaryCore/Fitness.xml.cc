/***********************************************************************\
|                                                                       |
| Fitness.xml.cc                                                        |
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
using namespace ugp3::core;
using namespace std;

const std::string Fitness::XML_NAME = "fitness";
const std::string Fitness::XML_CHILD_ELEMENT_VALUE = "value";
const std::string Fitness::XML_CHILD_ELEMENT_DESCRIPTION = "description";

void Fitness::readXml(const xml::Element& element)
{
    _STACK;

    this->clear();

    // get the inner elements
    bool descriptionFound = false;

    vector<double> fitnessValues;
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        string elementName = childElement->ValueStr();
        if(elementName == XML_CHILD_ELEMENT_DESCRIPTION)
        {
            if(descriptionFound == true)
            {
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/individuals/individual/fitness/description", LOCATION);
            }

            descriptionFound = true;

            if(childElement->FirstChild() == nullptr || childElement->FirstChild()->ToText() == nullptr || childElement->FirstChild()->ToText()->Value() == nullptr)
            {
                LOG_DEBUG << "Could not retrieve the description of the fitness" << ends;
            }
            else this->setDescription(childElement->FirstChild()->ToText()->Value());

        }
        else if(elementName == XML_CHILD_ELEMENT_VALUE)
        {
            if(childElement->FirstChild() == nullptr || childElement->FirstChild()->ToText() == nullptr || childElement->FirstChild()->ToText()->Value() == nullptr)
            {
                throw Exception("Could not retrieve the value of the fitness", LOCATION);
            }

            double value = 0;
            istringstream stream;
            stream.str(childElement->FirstChild()->ToText()->Value());

            stream >> value;

            fitnessValues.push_back(value);
        }
        else
        {
            throw xml::SchemaException("unexpected element \"" + elementName + "\"", LOCATION);
        }

        childElement = childElement->NextSiblingElement();
    }

    this->setValues(fitnessValues);
}

void Fitness::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object ugp3::core::Fitness." << std::ends;

    if(this->isValid == true)
    {
        output << "<" << XML_NAME  << ">" << std::endl;

        output
            << "<" << XML_CHILD_ELEMENT_DESCRIPTION << ">" 
            << xml::Utility::transformXmlEscChar(this->description) 
            << "</" << XML_CHILD_ELEMENT_DESCRIPTION << ">" << std::endl;


        for(unsigned int i = 0 ; i < this->m_values.size() ; i++)
        {
            output 
                << "<" << XML_CHILD_ELEMENT_VALUE << ">" 
                << this->m_values[i] 
            << "</" << XML_CHILD_ELEMENT_VALUE << ">" << std::endl;
        }

        output << "</" << XML_NAME << ">" << std::endl;
    } else {
        LOG_DEBUG << "Not serializing an invalid fitness." << std::ends;
    }
}

