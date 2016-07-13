/***********************************************************************\
|                                                                       |
| Individual.instantiate.cc                                             |
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
#include "Individual.h"
#include "MOIndividual.h"
#include "EnhancedIndividual.h"
#include "GEIndividual.h"

// ADD HERE: include the headers of the additional individuals types
using namespace ugp3::core;
using namespace std;

unique_ptr<Individual> Individual::instantiate(
    const string& type, 
    const Population& population)
{
    _STACK;
    
    LOG_DEBUG << "Instantiating individual type \"" << type << "\" ... " << ends;

    Individual* individual = nullptr;
    
    if (type == EnhancedIndividual::XML_TYPE)
	{
		individual = new EnhancedIndividual(population);
	}
    else if (type == MOIndividual::XML_TYPE)
    {
        individual = new MOIndividual(population);  
    }
    else if (type == GEIndividual::XML_TYPE)
    {
        individual = new GEIndividual(population);  
    }
    // ADD HERE: a new 'if' block for each new individual
    
    if(individual == nullptr)
    {
        throw ugp3::Exception("The individual type '" + type + "' is unknown.", LOCATION);
    }
    
    return unique_ptr<Individual> (individual);
}

unique_ptr<Individual> Individual::instantiate(
    const xml::Element& element, 
    const Population& population)
{
	_STACK;

	if(element.ValueStr() != Individual::XML_NAME) 
	{
		throw xml::SchemaException("expected element '" + Individual::XML_NAME + "', found " + element.ValueStr(), LOCATION);
	}

	const string& type = xml::Utility::attributeValueToString(element, "type");
    
    unique_ptr<Individual> individual = Individual::instantiate(type, population);
    
    individual->readXml(element);

    return individual;
}
