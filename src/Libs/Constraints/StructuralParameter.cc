/***********************************************************************\
|                                                                       |
| StructuralParameter.cc                                                |
|                                                                       |
| This file is part of MicroGP v3 (ugp3)                                |
| https://github.com/squillero/microgp3                                 |
|                                                                       |
| Copyright (c) 2006-2016 Giovanni Squillero                            |
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
using namespace ugp3::constraints;


StructuralParameter::StructuralParameter()
	: Parameter()
{ }

StructuralParameter::StructuralParameter(const std::string& name)
	: Parameter(name)
{ }


StructuralParameter::~StructuralParameter()
{ 
	LOG_DEBUG << "Destructor: ugp3::constraints::StructuralParameter" << std::ends;
}

// get the regular expression for a label parameter
const std::string StructuralParameter::getRegex() const
{
	// any label can be matched by the following expression
	LOG_WARNING << "Regex for structural parameters are not supported." << std::ends;
	return "";
}
