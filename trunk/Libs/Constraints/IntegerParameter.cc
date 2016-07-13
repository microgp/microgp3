/***********************************************************************\
|                                                                       |
| IntegerParameter.cc                                                   |
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

IntegerParameter::IntegerParameter()
	: RangedParameter(),
	minValue(0),
	maxValue(0)
{ }

IntegerParameter::IntegerParameter(const string& name, long int minValue, long int maxValue)
	: RangedParameter(name),
	minValue(minValue),
	maxValue(maxValue)
{ }

string IntegerParameter::getAt(double rangePercentage) const
{
	if(rangePercentage < 0 || rangePercentage > 1)
		throw ArgumentException("", LOCATION);
		
	return Convert::toString((long int)(this->minValue + rangePercentage * (this->maxValue - this->minValue)));
}

double IntegerParameter::getPosition(const string& value) const
{
	if(this->validate(value) == false) throw ArgumentException("", LOCATION);
	
	long int val = Convert::toLong(value);
	
	val -= this->minValue;
	double percentage = (double) val / (maxValue - minValue);
	
	return percentage;
}

const string IntegerParameter::randomize() const
{
	_STACK;

    return Convert::toString(Random::nextSInteger(this->minValue, this->maxValue));
}

bool IntegerParameter::validate(const string& value) const
{
	_STACK;

	istringstream stream;
	stream.str(value);

	signed long int number;
	stream >> number;

	if(stream.eof() == false) return false;

	return number >= this->minValue && number <= this->maxValue;
}


void IntegerParameter::clone(Parameter*& outParameter, const string& name)
{
	_STACK;

	IntegerParameter* parameter = new IntegerParameter(name, this->minValue, this->maxValue);
	parameter->typeDefinition = (this->typeDefinition != nullptr? this->typeDefinition : this);

	outParameter = parameter;
}

const string IntegerParameter::getRegex() const
{
	// any integer can be matched by the following expression
	return "([0-9]+)";
}
