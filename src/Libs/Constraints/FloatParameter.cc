/***********************************************************************\
|                                                                       |
| FloatParameter.cc                                                     |
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
#include "Constraints.h"
using namespace ugp3::constraints;
using namespace std;

FloatParameter::FloatParameter()
	: RangedParameter(),
    minValue(0),
    maxValue(0)
{ }

FloatParameter::FloatParameter(const string& name, double minValue, double maxValue)
	: RangedParameter(name),
	minValue(minValue),
	maxValue(maxValue)
{
	_STACK;

	if(minValue > maxValue)
	{
		throw ArgumentOutOfRangeException("minValue", LOCATION);
	}
}


const string FloatParameter::randomize() const
{
	_STACK;

	return Convert::toString(Random::nextDouble(this->minValue, this->maxValue));
}

bool FloatParameter::validate(const string& value) const
{
	_STACK;

	istringstream stream;
	stream.str(value);

	double number;
	stream >> number;

	if(stream.eof() == false) return false;

	return (number >= this->minValue && number <= this->maxValue);
}

string FloatParameter::getAt(double rangePercentage) const
{
	if(rangePercentage < 0 || rangePercentage > 1)
		throw ArgumentException("", LOCATION);

	return Convert::toString((double)(this->minValue + rangePercentage * (this->maxValue - this->minValue)));
}

void FloatParameter::clone(Parameter*& outParameter, const string& name)
{
	_STACK;

	FloatParameter* parameter = new FloatParameter(name, this->minValue, this->maxValue);
	parameter->typeDefinition = (this->typeDefinition != nullptr? this->typeDefinition : this);

	outParameter = parameter;
}

double FloatParameter::getPosition(const string& value) const
{
	if(this->validate(value) == false) throw ArgumentException("", LOCATION);

	double val = Convert::toDouble(value);

	val -= minValue;
	double percentage = val / (maxValue - minValue);

	return percentage;
}

// get a regular expression representing a floating point
const string FloatParameter::getRegex() const
{
	return "([-+]?[0-9]*\\.?[0-9]+.)";
}
