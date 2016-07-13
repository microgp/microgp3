/***********************************************************************\
|                                                                       |
| GenericSection.cc                                                     |
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


GenericSection::GenericSection(const GenericSection* parent, const string& id)
	: ConstrainingElement(*parent, id),
    prologue(nullptr),
	epilogue(nullptr)
{ 
	// do not check the 'parent' parameter:
	// the root element has no parent so it may be null
}

GenericSection::GenericSection(const GenericSection* parent)
	: ConstrainingElement(*parent),
	prologue(nullptr),
	epilogue(nullptr)
{
	_STACK;

    if(parent == nullptr) throw Exception("", LOCATION);
}

void GenericSection::setPrologue(Prologue& prologue)
{
	_STACK;
	
	delete this->prologue;
	this->prologue = &prologue;

	if(this != this->prologue->getParent())
    {
		throw Exception("Prologue parameter has an invalid parent object.", LOCATION);
    }
}

void GenericSection::setEpilogue(Epilogue& epilogue)
{
	_STACK;

	delete this->epilogue;
	this->epilogue = &epilogue;

	if(this != this->epilogue->getParent())
    {
		throw Exception("Epilogue parameter has an invalid parent object.", LOCATION);
    }
}

GenericSection::~GenericSection()
{
    this->clear();

	LOG_DEBUG << "Destructor: ugp3::constraints::GenericSection" << ends;
}

void GenericSection::clear()
{
    delete this->prologue;
	this->prologue = nullptr;

	delete this->epilogue;
	this->epilogue = nullptr;
}



Epilogue& GenericSection::getEpilogue() const
{
	_STACK;

    Assert(this->epilogue != nullptr);

	return *this->epilogue;
}

Prologue& GenericSection::getPrologue() const
{
	_STACK;

	Assert(this->prologue != nullptr);

	return *this->prologue;
}

