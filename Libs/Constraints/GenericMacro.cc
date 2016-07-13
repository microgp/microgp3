/***********************************************************************\
|                                                                       |
| GenericMacro.cc                                                       |
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


GenericMacro::GenericMacro(const GenericSection& parent, const string& id)
    : ConstrainingElement(parent, id)
{ 
    _STACK;
    
    this->expression = unique_ptr<Expression>( new Expression(this) );
}

GenericMacro::GenericMacro(const GenericSection& parent, const string& id, vector<ExpressionElement*>* expressionElements, vector<Parameter*>* parameters)
    : ConstrainingElement(parent, id)
{
    _STACK;
    
    if (!parameters) {
        throw ArgumentNullException("parameters", LOCATION);
    }

    if (!expressionElements) {
        throw ArgumentNullException("expressionElements", LOCATION);
    }

    this->parameters = *parameters;
    for(unsigned int i = 0; i < this->parameters.size(); i++)
    {
        this->parametersMap[this->parameters[i]->getName()] = this->parameters[i];
    }

    this->expression = unique_ptr<Expression>(new Expression(this, expressionElements));
}

GenericMacro::GenericMacro(const GenericSection& parent)
    : ConstrainingElement(parent)
{
	_STACK;
    
    this->expression = unique_ptr<Expression>( new Expression(this) );
}

void GenericMacro::clear()
{
	this->parameters.clear();
    this->parametersMap.clear();

    this->expression = unique_ptr<Expression>( new Expression(this) );
}
 
const string GenericMacro::getPath() const
{
    _STACK;

    string path = this->getId();

    const GenericSection* parent = this->getParent();
    while(parent != nullptr)
    {
        path = parent->getId() + "/" + path;

        parent = parent->getParent();
    }

    return "/" + path;
}



Parameter& GenericMacro::getParameter(unsigned int index) const
{
    _STACK;

    if(index >= this->parameters.size())
    {
        throw IndexOutOfBoundsException("GenericMacro::parameters", LOCATION);
    }

    return *this->parameters[index];
}

const Parameter* GenericMacro::operator[](unsigned int index) const
{
    _STACK;

    if(index >= this->parameters.size())
    {
        throw IndexOutOfBoundsException("GenericMacro::parameters", LOCATION);
    }

    if(this->parameters[index] == nullptr)
    {
        throw NullReferenceException("GenericMacro::parameters[index]", LOCATION);
    }

    return this->parameters[index];
}


GenericMacro::~GenericMacro()
{
    _STACK;

    for(unsigned int j = 0; j < this->parameters.size() ; j++)
    {
        delete this->parameters[j];
        this->parameters[j] = nullptr;
    }

    LOG_DEBUG << "Destructor: ugp3::constraints::GenericMacro " << this << "" << ends;
}

