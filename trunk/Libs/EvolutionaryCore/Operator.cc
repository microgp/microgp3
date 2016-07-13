/***********************************************************************\
|                                                                       |
| Operator.cc                                                           |
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

/**
 * @file Operator.cc
 * Implementation of the Operator class.
 */

#include "Operator.h"

namespace ugp3
{
namespace core
{

std::vector<Operator*> Operator::operators;
const std::string Operator::NO_OPERATOR = "NO_OPERATOR";

Operator::Operator()
{ }

unsigned int Operator::getOperatorCount()
{
    return (unsigned int)Operator::operators.size();
}

Operator* Operator::getOperator(unsigned int index)
{
    return Operator::operators.at(index);
}

Operator* Operator::getOperator(const std::string& name)
{
    _STACK;

    if(name.empty() == true)
    {
        throw ArgumentException("The parameter 'name' cannot be an empty string", LOCATION);
    }

    for(unsigned int i= 0; i < Operator::operators.size(); i++)
    {
        if(Operator::operators[i]->getName() == name)
        {
            return Operator::operators[i];
        }
    }

    return nullptr;
}

void Operator::registration(Operator* op)
{
    if(op == nullptr)
    {
        throw ArgumentNullException("op", LOCATION);
    }

    if(Operator::getOperator(op->getName()) != nullptr)
    {
        throw Exception("The operator \"" + op->toString() + "\" has already been inserted.", LOCATION);
    }

    Operator::operators.push_back(op);

    LOG_DEBUG << "    operator " << op << " registered." << std::ends;
}

void Operator::unregisterAll()
{
    for(unsigned int i= 0; i < Operator::operators.size(); i++)
    {
        delete Operator::operators[i];
    }
}

const std::string Operator::toString() const
{
    // return "\"" + this->getName() + "\""; why??? (!)20120116
    return this->getName();
}

Operator::~Operator()
{ }

}

}
