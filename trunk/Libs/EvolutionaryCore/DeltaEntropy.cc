/***********************************************************************\
|                                                                       |
| DeltaEntropy.cc                                                       |
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
#include "DeltaEntropy.h"
#include "Debug.h"

using namespace ugp3::core;
using namespace std;

DeltaEntropy::DeltaEntropy(double value)
{
    _STACK;

    this->value = value;
}

DeltaEntropy::~DeltaEntropy()
{ }

bool DeltaEntropy::equals(const DeltaEntropy& other) const
{
    _STACK;

    if(this->getValue() == other.getValue())
        return 1;
    else
        return 0;
}

void DeltaEntropy::setValue(double value)
{
    _STACK;

    this->value = value;
}

int DeltaEntropy::compareTo(const DeltaEntropy& other) const
{
    _STACK;

    if(this->getValue() < other.getValue())
        return -1;
    else if(this->getValue() > other.getValue())
        return 1;
    else
        return 0;
}
