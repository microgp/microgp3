/***********************************************************************\
|                                                                       |
| CombinatorialParameter.cc                                                  |
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

// included for random_shuffle
#include <algorithm> 

#include "ugp3_config.h"
#include "Constraints.h"
using namespace std;
using namespace ugp3::constraints;

CombinatorialParameter::CombinatorialParameter()
	: DataParameter()
{ }

CombinatorialParameter::CombinatorialParameter(const string& name, const vector< string >* constants, const string& delimiter)
	: DataParameter(name)
{ 
	_STACK;

	if(constants == nullptr)
	{
		throw ArgumentNullException("constants", LOCATION);
	}

	this->constants = *constants;
	this->delimiter = delimiter;
}

const string CombinatorialParameter::randomize() const
{
	_STACK;	

	Assert(this->constants.size() > 0);

	vector<string> temp = this->constants;
	LOG_DEBUG << "About to shuffle the vector with " << temp.size() << " elements..." << ends;
	ugp3::Random::shuffle(temp.begin(), temp.end());

	// the value is the vector in a random order, with the delimiter separating each value
	LOG_DEBUG << "Vector shuffled, about to write it..." << ends;
	ostringstream ss;
	ss << temp[0];
	for(unsigned int i = 1; i < temp.size(); i++)
	{
		ss << this->delimiter << temp[i];
	}
	LOG_DEBUG << "Vector wrote to string: \"" << ss.str() << "\"" << ends;
	return ss.str();
}

bool CombinatorialParameter::validate(const string& value) const
{
	_STACK;

	// parse the value string
	vector<string> tokens = Convert::toStringVector(value, this->delimiter);
	
	LOG_DEBUG << "Validating the combinatorial value:" << ends;
	for(unsigned int t = 0; t < tokens.size(); t++)
		LOG_DEBUG << "\"" << tokens[t] << "\"" << ends;

    	// TODO: the given value should match all the ones in constants (quicker algorithm to do that?)
	for(unsigned int t = 0 ; t < tokens.size(); t++)
	{
		bool found = false;

		for(unsigned int i = 0 ; i < this->constants.size() && found == false; i++)
		{
			if(this->constants[i] == tokens[t])
			{
				found = true;
			}
		}
		
		if(found == false) return false;
	}
	return true;
}

void CombinatorialParameter::clone(Parameter*& outParameter, const string& name)
{
	_STACK;

	CombinatorialParameter* parameter = new CombinatorialParameter(name, &this->constants, this->delimiter);
	parameter->typeDefinition = (this->typeDefinition != nullptr? this->typeDefinition : this);

	outParameter = parameter;
}

CombinatorialParameter::~CombinatorialParameter()
{ }

const string CombinatorialParameter::getRegex() const
{
	// for the combinatorial parameters, it's kinda hard
	// to find a proper regex...
	LOG_WARNING << "Warning: the regex for combinatorial parameters is still a work in progress..." << ends;
	return "[A-Za-z0-9]";
}
