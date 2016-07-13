/***********************************************************************\
|                                                                       |
| OuterLabelParameter.cc                                                |
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
using namespace std;
using namespace ugp3::constraints;


OuterLabelParameter::OuterLabelParameter()
{ }

OuterLabelParameter::OuterLabelParameter(const std::string& name, const std::vector<std::string>* subSectionNames)
	: StructuralParameter(name)
{ 
	_STACK;

	if(subSectionNames == nullptr)
		throw ArgumentNullException("subSectionNames", LOCATION);


	this->subSectionNames = *subSectionNames;
	
}

const std::string OuterLabelParameter::randomize() const
{
	_STACK;

	unsigned long randomIndex = Random::nextUInteger(0, (unsigned long)(this->subSectionNames.size() - 1));

    Assert(randomIndex <= this->subSectionNames.size() - 1);

	return this->subSectionNames.at(randomIndex);
}

OuterLabelParameter::~OuterLabelParameter()
{ }

bool OuterLabelParameter::validate(const std::string& value) const
{
	for(unsigned int i = 0; i < this->subSectionNames.size(); i++)
	{
		if(this->subSectionNames[i] == value)
			return true;
	}

	return false;
}

void OuterLabelParameter::clone(Parameter*& outParameter, const string& name)
{
	_STACK;

	OuterLabelParameter* parameter = new OuterLabelParameter(name, &this->subSectionNames);
	parameter->typeDefinition = (this->typeDefinition != nullptr? this->typeDefinition : this);

	outParameter = parameter;
}

// get the regular expression for a label parameter
const string OuterLabelParameter::getRegex() const
{
	// any label can be matched by the following expression
	return "[a-zA-Z0-9]+";
}
