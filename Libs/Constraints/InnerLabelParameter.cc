/***********************************************************************\
|                                                                       |
| InnerLabelParameter.cc                                                |
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

InnerLabelParameter::InnerLabelParameter()
    : StructuralParameter(),
    prologueIsValid(false),
	epilogueIsValid(false),
	itselfIsValid(false),
	backwardJumpIsValid(false),
	forwardJumpIsValid(false)
{ }

InnerLabelParameter::InnerLabelParameter(const string& name, bool prologueIsValid, bool epilogueIsValid, bool itselfIsValid, bool backwardJumpIsValid, bool forwardJumpIsValid)
	: StructuralParameter(name),
	prologueIsValid(prologueIsValid),
	epilogueIsValid(epilogueIsValid),
	itselfIsValid(itselfIsValid),
	backwardJumpIsValid(backwardJumpIsValid),
	forwardJumpIsValid(forwardJumpIsValid)
{ 	
	_STACK;

	if(backwardJumpIsValid == false && prologueIsValid == true)
	{
		throw ArgumentException("The parameter 'prologueIsValid' cannot be true when 'backwardJumpIsValid' is false.", LOCATION);
	}
	else if(forwardJumpIsValid == false && epilogueIsValid == true)
	{
		throw ArgumentException("The parameter 'epilogueIsValid' cannot be true when 'forwardJumpIsValid' is false.", LOCATION);
	}
}

InnerLabelParameter::~InnerLabelParameter()
{ }

void InnerLabelParameter::clone(Parameter*& outParameter, const string& name)
{
	InnerLabelParameter* parameter = new InnerLabelParameter(name, prologueIsValid, epilogueIsValid, itselfIsValid, backwardJumpIsValid, forwardJumpIsValid);

	parameter->typeDefinition = (this->typeDefinition != nullptr? this->typeDefinition : this);
	outParameter = parameter;
}

// get the regular expression for a label parameter
const string InnerLabelParameter::getRegex() const
{
	// any label can be matched by the following expression
	return "[a-zA-Z0-9]+";
}
