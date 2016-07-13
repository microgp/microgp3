/***********************************************************************\
|                                                                       |
| Section.cc                                                            |
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
#include "Constraints.h"
using namespace std;
using namespace ugp3::constraints;

Section::Section(Constraints* constraints, const std::string& id, bool isCompulsoryPrologueEpilogue)
	: GenericSection(constraints, id),
	isCompulsoryPrologueEpilogue(isCompulsoryPrologueEpilogue)
{
	_STACK;

	if(constraints == nullptr)
	{
		throw ArgumentNullException("constraints", LOCATION);
	}


	if(constraints->getSection(this->id) != nullptr)
	{
		throw Exception("Another Section with the id " + this->id + " already exists in the Constraints " + constraints->getId() + ".", LOCATION);
	}
}

Section::Section(Constraints* constraints)
	: GenericSection(constraints),
	isCompulsoryPrologueEpilogue(false)
{
	_STACK;

	if(constraints == nullptr)
	{
		throw ArgumentNullException("constraints", LOCATION);
	}
}

bool Section::validate() const
{
    _STACK;

    if(this->prologue == nullptr || this->epilogue == nullptr)
    {
        return false;
    }

    for(unsigned int i = 0; i < this->subSections.size(); i++)
    {
        if(this->subSections[i]->validate() == false)
        {
            return false;
        }
    }

    return true;
}

GenericMacro* Section::getMacro(const std::string& id) const
{
    _STACK;

    for(unsigned int i = 0; i < this->subSections.size(); i++)
    {
        Macro* macro = this->subSections[i]->getMacro(id);
        if(macro != nullptr)
        {
            return macro;
        }
    }

    if(this->prologue->getId() == id)
    {
        return this->prologue;
    }
    else if(this->epilogue->getId() == id)
    {
        return this->epilogue;
    }

    return nullptr;
}
const GenericMacro* Section::getMacroByPath(const std::string& path) const
{
    _STACK;

    if(path.empty() == true)
    {
        throw ArgumentException("The argument 'path' cannot be an empty string.", LOCATION);
    }

    string p = path.substr(1); // remove leading "/"

    string::size_type index = p.find_first_of('/');

    if(index == string::npos)
    {
        if(this->getPrologue().getId() == p)
        {
            return &this->getPrologue();
        }
        else if(this->getEpilogue().getId() == p)
        {
            return &this->getEpilogue();
        }
    }
    else
    {
        string subSectionName = p.substr(0, index);
        const SubSection* subSection = this->getSubSection(subSectionName);
        if(subSection != nullptr)
        {
            string macroId = p.substr(index + 1);
            Macro* macro = subSection->getMacro(macroId);
            if(macro != nullptr)
            {
                return macro;
            }
            else if(macroId == subSection->getPrologue().getId())
            {
                return &subSection->getPrologue();
            }
            else if(macroId == subSection->getEpilogue().getId())
            {
                return &subSection->getEpilogue();
            }
        }
    }

    return nullptr;
}

bool Section::contains(const GenericMacro& macro) const
{
    if(this->prologue == &macro || this->epilogue == &macro)
    {
        return true;
    }

    for(unsigned int i = 0; i < this->subSections.size(); i++)
    {
        if(this->subSections[i]->contains(macro))
        {
            return true;
        }
    }

    return false;
}

bool Section::contains(const SubSection& subSection) const
{
    for(unsigned int i = 0; i < this->subSections.size(); i++)
    {
        if(this->subSections[i] == &subSection)
        {
            return true;
        }
    }

    return false;
}

Section::~Section()
{
	_STACK;

	for(unsigned int i = 0 ; i < this->subSections.size() ; i++)
	{
		delete this->subSections[i];
		this->subSections[i] = nullptr;
	}
}

void Section::addSubSection(SubSection* subSection)
{
	_STACK;

	if(subSection == nullptr)
	{
		throw ArgumentNullException("subSection", LOCATION);
	}

    if(this->getSubSection(subSection->getId()) != nullptr)
    {
        throw ArgumentException("Another subsection with the same name already exists.", LOCATION);
    }

	this->subSections.push_back(subSection);
}

SubSection* Section::getSubSection(const std::string& name) const
{
	_STACK;

	for(unsigned int i = 0; i < this->subSections.size(); i++)
	{
		if(this->subSections[i]->getId() == name)
		{
			return this->subSections[i];
		}
	}

	return nullptr;
}

SubSection& Section::getSubSection(unsigned int index) const
{
	_STACK;

	if(index >= subSections.size())
	{
		throw IndexOutOfBoundsException("Section::subSections", LOCATION);
	}

	Assert(this->subSections[index] != nullptr);

	return *this->subSections[index];
}

