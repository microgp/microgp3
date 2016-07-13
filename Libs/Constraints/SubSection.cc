/***********************************************************************\
|                                                                       |
| SubSection.cc                                                         |
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
#include "Debug.h"
#include "Constraints.h"
using namespace std;
using namespace ugp3::constraints;


SubSection::SubSection(Section* section, const string& id)
    : GenericSection(section, id)
{ 
    _STACK;

    if(section == nullptr)
    {
        throw ArgumentNullException("section", LOCATION);
    }

    if(id.empty())
    {
        throw ArgumentException("The parameter id cannot be an empty string", LOCATION);
    }
}

SubSection::SubSection(Section* section, const string& id, unsigned int minimumInstances, unsigned int maximumInstances)
    : GenericSection(section, id),
    maximumInstances(maximumInstances),
    minimumInstances(minimumInstances)
{
    _STACK;

    if(section == nullptr)
    {
        throw ArgumentNullException("section", LOCATION);
    }

    if(maximumInstances < minimumInstances)
    {
        throw ArgumentException("", LOCATION);
    }

    if(maximumInstances == 0)
    {
        LOG_WARNING << "The SubSection \"" << id << "\" will never be instantiated since its maximum instance cardinality is zero" << ends;
    }

    for(unsigned int i = 0; i < section->getSubSectionCount(); i++)
    {
        if(section->getSubSection(i).getId() == this->getId())
        {
            throw Exception("Another SubSection with the id " + this->getId() + " already exists in the Section " + section->getId() + ".", LOCATION);
        }
    }
}

SubSection::~SubSection()
{
    _STACK;

    map<string, Macro*>::iterator iterator = this->macros.begin();
    while(iterator != this->macros.end())
    {
        delete (*iterator).second;
        ++iterator;
    }
}

bool SubSection::contains(const GenericMacro& macro) const
{
    _STACK;

    if(this->prologue == &macro || this->epilogue == &macro)
    {
        return true;
    }

    map<string, Macro*>::const_iterator iterator = this->macros.find(macro.getId());
    return iterator != this->macros.end() && (*iterator).second == &macro;
}

Macro* SubSection::getRandomMacro() const
{
    double total = 0.0;
    map<string, Macro*>::const_iterator iterator = this->macros.begin();
    while(iterator != this->macros.end())
    {
        total += (*iterator).second->getWeight();

        ++iterator;
    }

    double random = Random::nextDouble(0, total);

    double cumulative = 0.0;
    iterator = this->macros.begin();
    while(iterator != this->macros.end())
    {
        Macro* macro = (*iterator).second;
        cumulative += macro->getWeight();

        if(random <= cumulative)
        {
            return macro;
        }

        ++iterator;
    }

    // should never happen
    Assert(false);
    return nullptr;
}

void SubSection::addMacro(Macro* macro)
{
    _STACK;

    if(macro == nullptr)
    {
        throw ArgumentNullException("macro", LOCATION);
    }

    if(this->macros.count(macro->getId()) > 0)
    {
         throw ArgumentException("Another macro with the same id already exists.", LOCATION);
    }

    this->macros[macro->getId()] = macro;
}

SubSection::SubSection(Section* section)
    : GenericSection(section)
{
    _STACK;

    if(section == nullptr)
    {
        throw ArgumentNullException("section", LOCATION);
    }
}

Macro* SubSection::getMacro(const string& id) const
{
    _STACK;

    map<string, Macro*>::const_iterator iterator = this->macros.find(id);
    
    return iterator != this->macros.end() ? (*iterator).second : nullptr;
}

bool SubSection::validate() const
{
    _STACK;

    if(this->prologue == nullptr || this->epilogue == nullptr)
    {
        return false;
    }
    
    map<string, Macro*>::const_iterator iterator = this->macros.begin();
    while(iterator != this->macros.end())
    {
        Macro* macro = (*iterator).second;

        if(macro->validate() == false)
        {
            return false;
        }

        ++iterator;
    }

    if(this->expand == true && this->maximumReferences > 1)
    {
        LOG_ERROR << "When sub-section expansion is enabled, no more than one reference per sub-section (attribute maxReferences) is available" << ends;
        return false;
    }

    if( this->getMacro(this->prologue->getId()) != nullptr || 
        this->getMacro(this->epilogue->getId()) != nullptr)
    {
        return false;
    }

    return true;
}

Macro& SubSection::getMacro(unsigned int i) const
{
	unsigned int index = 0;
	map<string, Macro*>::const_iterator iterator = this->macros.begin();
    while(index < i)
    {
        ++iterator;
		++index;
    }

	return *(*iterator).second;
}

