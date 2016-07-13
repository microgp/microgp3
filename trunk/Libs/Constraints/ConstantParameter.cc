/***********************************************************************\
|                                                                       |
| ConstantParameter.cc                                                  |
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
#include "RegexMatch.h"

using namespace std;
using namespace ugp3::constraints;


ConstantParameter::ConstantParameter()
	: DataParameter()
{ }

ConstantParameter::ConstantParameter(const string& name, const vector<string>* constants)
	: DataParameter(name)
{ 
	_STACK;

	if(constants == nullptr)
	{
		throw ArgumentNullException("constants", LOCATION);
	}

	this->constants = *constants;
}

const string ConstantParameter::randomize() const
{
	_STACK;

    Assert(this->constants.size() > 0);

	unsigned long randomIndex = Random::nextUInteger(0, this->constants.size() - 1);
	return this->constants[randomIndex];
}

bool ConstantParameter::validate(const string& value) const
{
	_STACK;

    // the given value should match one of the values of the parameter
	for (unsigned int i = 0 ; i < this->constants.size() ; i++)
	{
		if(this->constants[i] == value)
		{
			return true;
		}
	}

	return false;
}

void ConstantParameter::clone(Parameter*& outParameter, const string& name)
{
	_STACK;

	ConstantParameter* parameter = new ConstantParameter(name, &this->constants);
	parameter->typeDefinition = (this->typeDefinition != nullptr? this->typeDefinition : this);

	outParameter = parameter;
}

ConstantParameter::~ConstantParameter()
{ }

// regular expression for a constant parameter
const string ConstantParameter::getRegex() const
{
	// our regular expression can be basically any of the possible constant values, with "|", such as:
	// (value1|value2|value...|valueN)
	// but each "value" must be a correct regex, with escaped characters
	// the whole expression goes between (), since it will be used to catch the parameter's value
	string resultingRegex = "(";
	resultingRegex += RegexMatch::stringToRegex(this->constants[0]);

	for(unsigned int i = 1; i < this->constants.size(); i++)
	{
		resultingRegex += "|";
		resultingRegex += RegexMatch::stringToRegex(this->constants[i]);
	}
	resultingRegex += ")";
	
	return resultingRegex;
}
