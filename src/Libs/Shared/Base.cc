/***********************************************************************\
|                                                                       |
| Base.cc                                                               |
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
 * @file Base.cc
 * Implementation of the Base class.
 * @see Base.h
 */

#include "ugp3_config.h"
#include "Base.h"
using namespace ugp3;

const Base Base::Hexadecimal(0, "hex");
const Base Base::Decimal(1, "dec");
const Base Base::Octal(2, "oct");
const Base Base::Binary(4, "bin");

Base::Base(unsigned int value, const char* description )
	: Enumeration(value, description)
{ }

Base::Base(const Base& base)
	: Enumeration(base)
{ }
