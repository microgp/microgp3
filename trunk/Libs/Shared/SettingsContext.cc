/***********************************************************************\
|                                                                       |
| SettingsContext.cc                                                    |
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
 * @file SettingsContext.cc
 * Implementation of the ContextSettings class.
 * @see ContextSettings.h
 * @see ContextSettings.xml.cc
 */

#include "ugp3_config.h"
#include "SettingsContext.h"
#include "Exceptions/SchemaException.h"
#include "Exceptions/ArgumentException.h"
using namespace ugp3;
using namespace std;

Option& SettingsContext::getOption(const string& name)
{
	_STACK;

	for(unsigned int i = 0; i < this->options.size(); i++)
	{
		if(this->options[i]->getName() == name)
		{
			return *this->options[i];
		}
	}

	throw Exception("Option '" + name + "' not found in the context '" + this->name + "'.", LOCATION);
}

SettingsContext::~SettingsContext()
{
    for(unsigned int i = 0; i < this->options.size(); i++)
    {
        delete this->options[i];
    }
}

bool SettingsContext::hasOption(const string& name)
{
	_STACK;

	for(unsigned int i = 0; i < this->options.size(); i++)
	{
		if(this->options[i]->getName() == name)
		{
			return true;
		}
	}

	return false;
}

void SettingsContext::addOption(unique_ptr< Option > option)
{
	_STACK;

	if(option.get() == nullptr) throw ArgumentException("Cannot add a nullptr option.", LOCATION);

	if(this->hasOption(name) == true)
	{
		throw Exception("Option '" + name + "' already set for context '" + this->name + "'.", LOCATION);
	}

	this->options.push_back(option.release());
}

void SettingsContext::removeOption(const string& name)
{
	_STACK;

	for(unsigned int i = 0; i < this->options.size(); i++)
	{
		if(this->options[i]->getName() == name)
		{
			delete this->options[i];
			this->options.erase(this->options.begin() + i);
		}
	}

	return;
}
