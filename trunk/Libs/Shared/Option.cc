/***********************************************************************\
|                                                                       |
| Option.cc                                                             |
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
| the Free Software Foundation, either version 3 of the License, or     |
| (at your option) any later version.                                   |
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
 * @file Option.cc
 * Implementation of the Option class.
 * @see Option.h
 * @see Option.xml.cc
 * @see XMLIFace.h
 * @see IString.h
 */

#include "ugp3_config.h"
#include "Option.h"
#include "StackTrace.h"
#include "Convert.h"
#include "Exceptions/SchemaException.h"
#include "Log.h"
using namespace ugp3;
using namespace std;

Option::Option(const std::string& name, const std::string& value, const std::string& type)
	: name(name),
	value(value),
	type(type)
{ 
	_STACK;
	if(this->validate() == false)
	{
		throw Exception("value attribute is not valid for option '" + this->name + "'.", LOCATION);
	}
}

std::vector<std::string> Option::toList() const
{
	istringstream stream;
	stream.str(this->value);

	std::vector<std::string> results;
	
	while(stream.eof() == false)
	{
        char* buffer = new char[10000];
        stream.getline(buffer, 10000, ';');
		const std::string token = buffer;
        delete [] buffer;

		if(token.empty() == false)
		    results.push_back(token);
	}

	return results;
}
	
bool Option::validate()
{
	_STACK;
	
	try
	{
		if(this->type == "string")
		{
			return true;
		}
		else if(this->type == "integer")
		{
			Convert::toLong(this->value);
		}
		else if(this->type == "boolean")
		{
			Convert::toBool(this->value);
		}
		else if(this->type == "float")
		{
			Convert::toDouble(this->value);
		}
		else if(this->type != "") 
		{
			LOG_WARNING << "Unrecongnized data type '" << this->type << "' for settings" << ends;
			return false;
		}
	}
	catch(Exception& e)
	{
		return false;
	}
	
	return true;
}

