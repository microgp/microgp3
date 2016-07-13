/***********************************************************************\
|                                                                       |
| Population.xml.cc                                                     |
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
 * @file Population.xml.cc
 * Implementation of the xml related methods of the Population class.
 * @see Population.h
 * @see Population.cc
 * @see Population.instantiate.cc
 */

#include "ugp3_config.h"
#include "EvolutionaryCore.h"
using namespace std;
using namespace ugp3::core;

const string Population::XML_NAME = "population";
const string Population::XML_ATTRIBUTE_GENERATION = "generation";
const string Population::XML_ATTRIBUTE_TYPE = "type";
const string Population::XML_ATTRIBUTE_NAME = "name";
const string Population::XML_CHILD_ELEMENT_STEADYSTATE = "steadyState";
const string Population::XML_ATTRIBUTE_RAW_BEST = "rawBest";
const string Population::XML_ATTRIBUTE_SCALED_BEST = "scaledBest";
const string Population::XML_ATTRIBUTE_RAW_WORST = "rawWorst";
const string Population::XML_ATTRIBUTE_SCALED_WORST = "scaledWorst";
const string Population::XML_CHILDELEMENT_ENTROPY = "entropy";


void Population::readXml(const xml::Element& element)
{
	_STACK;

    LOG_DEBUG << "Parsing xml element " << element.ValueStr()  << ends;

    // get the name of the element
    if(element.ValueStr() != this->getXmlName())
    {
        throw xml::SchemaException("expected element 'population'", LOCATION);
    }

    this->generation = xml::Utility::attributeValueToUInt(element, XML_ATTRIBUTE_GENERATION);
    this->name = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_NAME);

    // get the inner elements
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        const string& elementName = childElement->ValueStr();
        if(elementName == PopulationParameters::XML_NAME)
        {
            getParameters().readXml(*childElement);
        }
        else if (elementName == XML_CHILD_ELEMENT_STEADYSTATE)
        {
            m_steadyStateGenerations = xml::Utility::attributeValueToUInt(*childElement, "generations");
            if (!xml::Utility::attributeValueToString(*childElement, "previousMaxFitness").empty()) {
                // TODO variable
            m_previousMaxFitness = Fitness::parse(xml::Utility::attributeValueToString(*childElement, "previousMaxFitness"));
            }
        }
    else if (elementName == XML_CHILDELEMENT_ENTROPY)
    {
        this->m_entropy = xml::Utility::attributeValueToDouble(*childElement, "value");
    }
        childElement = childElement->NextSiblingElement();
    }
}

void Population::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object ugp3::core::Population" << ends;

    output
        << "<" << XML_NAME
        << " "
        << XML_ATTRIBUTE_GENERATION <<"=\"" << this->generation << "\""
        << " "
        << XML_ATTRIBUTE_NAME <<"=\"" << this->name << "\""
        << " "
        << XML_ATTRIBUTE_TYPE << "=\"" << this->getType()
        << "\">" << endl;

    output
    << "<!-- Entropy of the whole population -->" << endl
    << "<" << XML_CHILDELEMENT_ENTROPY
    << " value=\"" << this->getEntropy() << "\" />"
    << endl;

    getParameters().writeXml(output);
    
    writeInnerXml(output);

    output << "</" << XML_NAME << ">" << endl;
}

void Population::writeInnerXml(ostream& output) const
{
    output << '<' << XML_CHILD_ELEMENT_STEADYSTATE << " generations" << "='" << m_steadyStateGenerations << "' previousMaxFitness='";

    vector<double> values;
    if (m_previousMaxFitness.getValues().empty()) {
        values.resize(getParameters().getFitnessParametersCount(), 0.0);
    }
    else values = m_previousMaxFitness.getValues();

    // TODO move this loop to the Fitness class
    for (unsigned int i = 0; i < values.size(); i++)
    {
        output << values[i];

        if (i < values.size() - 1)
        {
            output << ' ';
        }
    }

    output << "'/>" << endl;
}
