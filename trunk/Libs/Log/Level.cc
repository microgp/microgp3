/***********************************************************************\
|                                                                       |
| Level.cc                                                              |
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
 * @file Level.cc
 * Implementation of the Level class.
 * @see Level.h
 */

#include "ugp3_config.h"
#include "Level.h"
#include <stdexcept>
using namespace ugp3::log;

const Level Level::Silent(0, "SILENT");
const Level Level::Error(10, "ERROR");
const Level Level::Warning(30, "WARNING");
const Level Level::Info(50, "INFO");
const Level Level::Verbose(70, "VERBOSE");
const Level Level::Debug(90, "DEBUG");

Level::Level(unsigned int value, const char* description)
	: Enumeration(value, description)
{ }

Level Level::parse(const std::string& value)
{
    if(value == "error" || value == Level::Error.toString())
    {
        return Level::Error;
    }
    else if(value == "warning" || value == Level::Warning.toString())
    {
        return Level::Warning;
    }
    else if(value == "message")
    {
        return Level::Info;
    }
    else if(value == "info" || value == Level::Info.toString())
    {
        return Level::Info;
    }
    else if(value == "verbose" || value == Level::Verbose.toString())
    {
        return Level::Verbose;
    }
    else if(value == "debug" || value == Level::Debug.toString())
    {
        return Level::Debug;
    }
    else if(value == "silent" || value == Level::Silent.toString())
    {
        return Level::Silent;
    }

    throw std::runtime_error("Unexpected value " + value +
        " for log level");
}

