/***********************************************************************\
|                                                                       |
| GroupPopulation.xml.cc                                                |
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
 * @file GroupPopulation.xml.cc
 * Implementation of the xml related methods of the GroupPopulation class.
 * @see GroupPopulation.h
 * @see GroupPopulation.cc
 */

#include "EvolutionaryCore.h"
using namespace ugp3::core;
using namespace xml;

const std::string GroupPopulation::XML_SCHEMA_TYPE = "group";
const std::string GroupPopulation::XML_CHILD_ELEMENT_GROUPS = "groups";

void GroupPopulation::parseGroups(const xml::Element& element)
{
    // get the name of the xml element
    if(element.ValueStr() != XML_CHILD_ELEMENT_GROUPS)
    {
        throw xml::SchemaException("expected element '/evolutionaryAlgorithm/populations/population/groups'", LOCATION);
    }

    LOG_DEBUG << "Parsing /evolutionaryAlgorithm/populations/population/groups ..." << ends;

    // loop on child elements
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        const string& elementName = childElement->ValueStr();
        // TODO check type instead of name
        if(elementName == Group::XML_NAME)
        {
            unique_ptr<Group> group = unique_ptr<Group>(new Group(*childElement, *this));

            this->addGroup(std::move(group));
        }
        else throw xml::SchemaException("unexpected element \"" + elementName + "\"", LOCATION);

        childElement = childElement->NextSiblingElement();
    }

    if(this->m_groups.size() == 0)
    {
    	LOG_WARNING << "The population xml does not contain any group" << ends;
    }

    string bestRawGroupId = "";
    string worstRawGroupId = "";
    string bestScaledGroupId = "";
    string worstScaledGroupId = "";
    if(element.Attribute(XML_ATTRIBUTE_RAW_BEST) != nullptr)
    {
        bestRawGroupId = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_RAW_BEST);
    }
    if(element.Attribute(XML_ATTRIBUTE_RAW_WORST) != nullptr)
    {
        worstRawGroupId = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_RAW_WORST);
    }
    if(element.Attribute(XML_ATTRIBUTE_SCALED_BEST) != nullptr)
    {
        bestScaledGroupId = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_SCALED_BEST);
    }
    if(element.Attribute(XML_ATTRIBUTE_SCALED_WORST) != nullptr)
    {
        worstScaledGroupId = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_SCALED_WORST);
    }
    // assign best and worst
    for (Group* group: m_groups)
    {
        if (bestRawGroupId == group->getId()) {
            m_bestRawGroup = group;
        }
        if (worstRawGroupId == group->getId()) {
            m_worstRawGroup = group;
        }
        if (bestScaledGroupId == group->getId()) {
            m_bestScaledGroup = group;
        }
        if (worstScaledGroupId == group->getId()) {
            m_worstScaledGroup = group;
        }
    }
}

void GroupPopulation::readXml(const xml::Element& element)
{
    _STACK;

    SpecificIndividualPopulation::readXml(element);

    for (const xml::Element* populationElement = element.FirstChildElement();
        populationElement != nullptr;
        populationElement = populationElement->NextSiblingElement()) {
        const string& elementName = populationElement->ValueStr();
        if (elementName == XML_CHILD_ELEMENT_GROUPS)
        {
            this->parseGroups(*populationElement);
            
        }
    }
}

void GroupPopulation::writeInnerXml(ostream& output) const
{
    _STACK;
    
    SpecificIndividualPopulation::writeInnerXml(output);

    LOG_DEBUG << "Serializing object ugp3::core::GroupPopulation" << ends;

	output << '<' << XML_CHILD_ELEMENT_STEADYSTATE << " generations" << "='" << m_steadyStateGenerations << "' previousMaxFitness='";

	vector<double> values;
	if (m_previousGroupMaxFitness.getIsValid())
	{
		for(unsigned int i = 0; i < getParameters().getGroupFitnessParametersCount(); i++)
		{
			values.push_back(0);
		}
	}
	else values = m_previousGroupMaxFitness.getValues();

	for (unsigned int i = 0; i < values.size(); i++)
	{
		output << values[i];

		if(i < values.size() - 1)
		{
			output << ' ';
		}
	}

	output << "'/>" << endl;


    output << '<' << XML_CHILD_ELEMENT_GROUPS;
    if (m_bestRawGroup) {
        output << ' ' << XML_ATTRIBUTE_RAW_BEST << "='" << m_bestRawGroup->getId() << "'";
    }
    if(m_worstRawGroup) {
        output << ' ' << XML_ATTRIBUTE_RAW_WORST << "='" << m_worstRawGroup->getId() << "'";
    }
    if (m_bestScaledGroup) {
        output << ' ' << XML_ATTRIBUTE_SCALED_BEST << "='" << m_bestScaledGroup->getId() << "'";
    }
    if (m_worstScaledGroup) {
        output << ' ' << XML_ATTRIBUTE_SCALED_WORST << "='" << m_worstScaledGroup->getId() << "'";
    }
    output << ">" << endl;

    output << "<!-- total " << this->m_groups.size() << " groups -->" << endl;

    for(unsigned int i = 0; i < this->m_groups.size(); i++)
    {
        this->m_groups[i]->writeXml(output);
    }

    output << "</" << XML_CHILD_ELEMENT_GROUPS << '>' << endl;

}



