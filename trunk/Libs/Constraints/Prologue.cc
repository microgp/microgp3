/***********************************************************************\
|                                                                       |
| Prologue.cc                                                           |
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

const std::string Prologue::XML_NAME = "prologue";

Prologue::Prologue(GenericSection& parent, const string& id)
	: GenericMacro(parent, id)
{ 
	_STACK;
}

Prologue::Prologue(GenericSection& parent, const string& id, vector<ExpressionElement*>* expressionElements, vector<Parameter*>* parameters)
	: GenericMacro(parent, id, expressionElements, parameters)
{ 
	_STACK;
}

Prologue::Prologue(GenericSection& parent)
	: GenericMacro(parent)
{ 
	_STACK;
}

Prologue::~Prologue()
{
	LOG_DEBUG << "Destructor: ugp3::constraints::Prologue \"" << this->getId() << "\"" << ends;
}

