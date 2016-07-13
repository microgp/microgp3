/***********************************************************************\
|                                                                       |
| Population.instantiate.cc                                             |
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
 * @file Population.instantiate.cc
 * Implementation of the instantiate methods of the Population class.
 * @see Population.h
 * @see Population.cc
 * @see Population.xml.cc
 */

#include "ugp3_config.h"
#include "Population.h"
#include "EnhancedPopulation.h"
#include "MOPopulation.h"
#include "GroupPopulation.h"
// ADD HERE: include the headers of the additional populations types

using namespace ugp3::core;
using namespace std;

unique_ptr<Population> Population::instantiate(
                                            const EvolutionaryAlgorithm& parent,
                                            const std::string& type)
{
    Population* population = nullptr;

    if(type == EnhancedPopulation::XML_SCHEMA_TYPE)
    {
        population = new EnhancedPopulation(parent);
    }
    else if(type == MOPopulation::XML_SCHEMA_TYPE)
    {
        population = new MOPopulation(parent);
    }
    else if(type == GroupPopulation::XML_SCHEMA_TYPE)
    {
        population = new GroupPopulation(parent);
    }
    // ADD HERE: a new 'if' block for each new population

    if(population == nullptr)
    {
        throw Exception("The population type '" + type + "' is unknown.", LOCATION);
    }

    return unique_ptr<Population> (population);
}

unique_ptr<Population> Population::instantiate(
                                            const xml::Element& element,
                                            const EvolutionaryAlgorithm& parent)
{
    const string& elementName = element.ValueStr();
    if(elementName != Population::XML_NAME)
    {
        throw xml::SchemaException("expected element 'population' (found '" + elementName + "').", LOCATION);
    }

    const string& type = xml::Utility::attributeValueToString(element, Population::XML_ATTRIBUTE_TYPE);

    unique_ptr<Population> population = Population::instantiate(parent, type);

    population->readXml(element);

    return population;
}

