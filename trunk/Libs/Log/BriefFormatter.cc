/***********************************************************************\ 
|                                                                       |
| BriefFormatter.cc                                                     |
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
 * @file BriefFormatter.cc
 * Implementation of the BriefFormatter class.
 * @see BriefFormatter.h
 */

#include "BriefFormatter.h"
#include "Record.h"
using namespace ugp3::log;

const std::string BriefFormatter::XML_SCHEMA_TYPE = "brief";

const std::string BriefFormatter::format(const Record& record) const
{
    std::ostringstream tmpStream;
    
    tmpStream << std::setfill('0');
	tmpStream 
	    << "[" 
	    << std::setw(2) << record.getTime().tm_hour << ":" 
	    << std::setw(2) << record.getTime().tm_min << ":" 
	    << std::setw(2) << record.getTime().tm_sec << "] ";

	tmpStream << std::setfill(' ');
    tmpStream << record.getLevel().toString() << ":: ";	
	tmpStream << record.getMessage();
	
    return tmpStream.str();
}
