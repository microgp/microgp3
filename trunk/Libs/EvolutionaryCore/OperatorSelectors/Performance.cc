/***********************************************************************\
|                                                                       |
| Performance.cc                                                        |
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
 * @file Performance.cc
 * Implementation of the Performance class.
 * @see Performance.h
 */

#include "Performance.h"

using namespace ugp3::core;

const Performance Performance::VeryGood(0, "veryGood");
const Performance Performance::Good(1, "good");
const Performance Performance::Normal(2, "normal");
const Performance Performance::Bad(3, "bad");
const Performance Performance::VeryBad(4, "veryBad");
const Performance* Performance::values[Performance::PerformanceLevelCount] = {
    &Performance::VeryGood,
    &Performance::Good,
    &Performance::Normal,
    &Performance::Bad,
    &Performance::VeryBad
};


Performance::Performance(unsigned int value, const char* description)
: Enumeration(value, description)
{ }


