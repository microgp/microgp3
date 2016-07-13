/***********************************************************************\
|                                                                       |
| EnvironmentParameter.cc                                               |
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

EnvironmentParameter::EnvironmentParameter()
	: DataParameter(),
    environmentVariableName("")
{ }

EnvironmentParameter::EnvironmentParameter(const std::string& name, const std::string& environmentVariableName)
	: DataParameter(name), 
	environmentVariableName(environmentVariableName)
{ }
			
const std::string EnvironmentParameter::randomize() const
{
	return getenv(this->environmentVariableName.c_str());
}

bool EnvironmentParameter::validate(const std::string& value) const
{
	// The environment variable's value may have changed since the EnvironmentParameter::randomize
	// method was called so the validation shouldn't check the value
	return true;
}

void EnvironmentParameter::clone(Parameter*& outParameter, const std::string& name)
{
	_STACK;

	EnvironmentParameter* parameter = new EnvironmentParameter(name, this->environmentVariableName);
	parameter->typeDefinition = (this->typeDefinition != nullptr? this->typeDefinition : this);

	outParameter = parameter;
}

EnvironmentParameter::~EnvironmentParameter()
{ }

const std::string EnvironmentParameter::getRegex() const
{
	// yes, I have no idea how to manage this
	LOG_WARNING << "Regular expression for environment parameter are not supported..." << std::ends;
	return "";
}
