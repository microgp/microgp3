/***********************************************************************\
|                                                                       |
| Location.cc                                                           |
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
 * @file Location.cc
 * Implementation of the Location class.
 * @see Location.h
 */

#include "ugp3_config.h"
#include <string>

#include "Location.h"
using namespace ugp3::log;

Location::Location(const std::string& file, const std::string& function, unsigned int line)
	: file(file),
	  function(function),
	  line(line)
{ }

Location::Location(const Location& location)
	: file(location.file),
	  function(location.function),
	  line(location.line)
{ }

Location& Location::operator=(const Location& location)
{
	this->file = location.file;
	this->line = location.line;
	this->function = location.function;

	return *this;
}

