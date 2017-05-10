/***********************************************************************\
|                                                                       |
| Settings.cc                                                           |
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
 * @file Settings.cc
 * Implementation of the Settings class.
 * @see Settings.h
 * @see Settings.xml.cc
 * @see XMLIFace.h
 */

#include "ugp3_config.h"
#include "Settings.h"
#include "Debug.h"
using namespace ugp3;
using namespace std;



Settings::Settings()
{ }

void Settings::load(const string& xmlFile)
{
    _STACK;

    // open the xml document
    xml::Document settingsFile;
    settingsFile.LoadFile(xmlFile);

	this->readXml(*settingsFile.RootElement());
}

bool Settings::hasContext(const std::string& name)
{
	for(unsigned int i = 0; i < this->contexts.size(); i++)
	{
		if(this->contexts[i]->getName() == name)
		{
			return true;
		}
	}

	return false;
}

void Settings::save(const string& xmlFile)
{
    _STACK;

    ofstream stream;
    stream.open(xmlFile.c_str());

    if(stream.is_open() == false)
    {
        throw Exception("Cannot access file " + xmlFile, LOCATION);
    }

    stream << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl;
	this->writeXml(stream);

	stream.close();
}

SettingsContext& Settings::getContext(const std::string& name)
{
	_STACK;

	for(unsigned int i = 0; i < this->contexts.size(); i++)
	{
		Assert(this->contexts[i] != nullptr);

		if(this->contexts[i]->getName() == name)
		{
			return *this->contexts[i];
		}
	}

	throw Exception("Context '" + name + "' not found.", LOCATION);
}

void Settings::clear()
{
    _STACK;

    for(unsigned int i = 0; i < this->contexts.size(); i++)
	{
		delete this->contexts[i];
	}

    this->contexts.clear();
}

void Settings::addContext(unique_ptr< SettingsContext > context)
{
	_STACK;

	if(this->hasContext(context->getName()) == true)
	{
		throw Exception("Context '" + context->getName() + "' already exists.", LOCATION);
	}

	this->contexts.push_back(context.release());
}
