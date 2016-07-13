/***********************************************************************\
|                                                                       |
| ConstrainingElement.cc                                                |
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


ConstrainingElement::ConstrainingElement()
    : id(""),
    commentFormat(nullptr),
    labelFormat(nullptr),
    uniqueTagFormat(nullptr),
    identifierFormat(nullptr),
    parent(nullptr)
{ }

ConstrainingElement::ConstrainingElement(const GenericSection& parent, const string& id)
    : id(id),
    commentFormat(nullptr),
    labelFormat(nullptr),
    uniqueTagFormat(nullptr),
    identifierFormat(nullptr),
    parent(&parent)
{ 
    _STACK;

    if(id.empty() == true)
    {
        throw ArgumentException("The parameter id cannot be an empty string", LOCATION);
    }
}

ConstrainingElement::ConstrainingElement(const GenericSection& parent)
    :commentFormat(nullptr),
    labelFormat(nullptr),
    uniqueTagFormat(nullptr),
    identifierFormat(nullptr),
    parent(&parent)
{
    _STACK;
}

void ConstrainingElement::clear()
{
    this->setLabelFormat(nullptr);
    this->setCommentFormat(nullptr);
    this->setUniqueTagFormat(nullptr);
    this->setIdentifierFormat(nullptr);
}

ConstrainingElement::~ConstrainingElement()
{
    this->clear();

    LOG_DEBUG << "Destructor: ugp3::constraints::ConstrainingElement" << ends;
}

const LabelFormat* ConstrainingElement::getLabelFormat() const
{
    _STACK;

    // get the format: if it is null, get the one of the parent element
    // (if a parent element exists)
    if(this->labelFormat != nullptr)
    {
        return this->labelFormat;
    }
    else if(this->parent != nullptr)
    {
        return this->parent->getLabelFormat();
    }
    else 
    {
        throw Exception("Coluld not find label format for element '" + this->id + "'.", LOCATION);
    }
}

const CommentFormat* ConstrainingElement::getCommentFormat() const
{
    _STACK;

    // get the format: if it is null, get the one of the parent element
    // (if a parent element exists)
    if(this->commentFormat != nullptr)
    {
        return this->commentFormat;
    }
    else if(this->parent != nullptr)
    {
        return this->parent->getCommentFormat();
    }
    else
    {
        throw Exception("Coluld not find comment format", LOCATION);
    }
}


const IdentifierFormat* ConstrainingElement::getIdentifierFormat() const
{
    _STACK;

    // get the format: if it is null, get the one of the parent element
    // (if a parent element exists)
    if(this->identifierFormat != nullptr)
    {
        return this->identifierFormat;
    }
    else if(this->parent != nullptr)
    {
        return this->parent->getIdentifierFormat();
    }
    else
    {
        throw Exception("Coluld not find identifier format", LOCATION);
    }
}

const UniqueTagFormat* ConstrainingElement::getUniqueTagFormat() const
{
    _STACK;

    // get the format: if it is null, get the one of the parent element
    // (if a parent element exists)
    if(this->uniqueTagFormat != nullptr)
    {
        return this->uniqueTagFormat;
    }
    else if(this->parent != nullptr)
    {
        return this->parent->getUniqueTagFormat();
    }
    else
    {
        throw Exception("Coluld not find unique tag format", LOCATION);
    }
}

void ConstrainingElement::setLabelFormat(LabelFormat* labelFormat)
{
    _STACK;

    delete this->labelFormat;
    this->labelFormat = labelFormat;
}

void ConstrainingElement::setCommentFormat(CommentFormat* commentFormat)
{
    _STACK;

    // delete the previous format and set the new one
    delete this->commentFormat;
    this->commentFormat = commentFormat;
}

void ConstrainingElement::setIdentifierFormat(IdentifierFormat* identifierFormat)
{
    _STACK;

    // delete the previous format and set the new one
    delete this->identifierFormat;
    this->identifierFormat = identifierFormat;
}

void ConstrainingElement::setUniqueTagFormat(UniqueTagFormat* uniqueTagFormat)
{
    _STACK;

    // delete the previous format and set the new one
    delete this->uniqueTagFormat;
    this->uniqueTagFormat = uniqueTagFormat;
}

