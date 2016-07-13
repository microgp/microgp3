/***********************************************************************\
|                                                                       |
| Taggable.cc                                                           |
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
#include "Taggable.h"
using namespace std;
using namespace ugp3;

Taggable::Taggable()
{ }

Taggable::~Taggable()
{
    _STACK;


    // cleanup Tag list
    for(map<string, Tag*>::iterator elemIter=tags.begin(); elemIter != tags.end(); ++elemIter)
    {
        delete elemIter->second;
    }

#ifndef NDEBUG
    LOG_DEBUG << "Destructor: ugp3::Taggable" << ends;
#endif
}

bool Taggable::addTag(Tag* tag)
{
    _STACK;

    if(tag == nullptr)
    {
        throw ugp3::ArgumentNullException("tag", LOCATION);
    }

    if(this->containsTag(tag->getName()) == true)
    {
        throw ugp3::ArgumentException( "A tag with the name \"" + tag->getName() + "\" already exists for the object.", LOCATION);
    }

    tags[tag->getName()] = tag;

    LOG_DEBUG << "Added tag " << tag <<  " to Taggable object" << ends;

    return true;
}

bool Taggable::contains(const Tag& tag) const
{
     map<string, Tag*>::const_iterator iterator = this->tags.find(tag.getName());

     if(iterator == this->tags.end()) return false;

     if(iterator->second != & tag) return false;

     return true;
}

bool Taggable::removeTag(const string& name)
{
    map<string, Tag*>::iterator elementIterator = this->tags.find(name);

    if(elementIterator == this->tags.end()) return false;

    delete elementIterator->second;
    elementIterator->second = nullptr;
    this->tags.erase(elementIterator);

    return true;
}

Tag& Taggable::getTag(const string& tagName) const
{
    _STACK;

    map<string, Tag*>::const_iterator searchIterator;

    searchIterator = tags.find(tagName);
    if(searchIterator == tags.end())
    {
        throw ugp3::ArgumentException("The tag " + tagName + " was not found.", LOCATION);
    }

    return *searchIterator->second;
}

void Taggable::readXml(const xml::Element& element)
{
	_STACK;

	this->clear();

    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
    	if(childElement->ValueStr() == "tag")
        {
    		Tag* tag = new Tag();
    		tag->readXml(*childElement);
    		
    		this->addTag(tag);
        }
         
        childElement = childElement->NextSiblingElement();
    }
}

void Taggable::clear()
{
	this->tags.clear();
}

Tag* Taggable::getTag(unsigned int index) const
{
    _STACK;

    if(index >= this->tags.size())
    {
        throw ugp3::IndexOutOfBoundsException("Taggable::tags", LOCATION);
    }

    map<string, Tag*>::const_iterator tagsIterator = tags.begin();
    for(unsigned int i = 0; i < index && tagsIterator != tags.end(); ++tagsIterator, i++); // AAARG! There must be another way

    return tagsIterator->second;
}
void Taggable::addTag(const string& name, const string& value)
{
    _STACK;

	Tag* tag = new Tag(name, value);
    this->addTag(tag);
}

void Taggable::writeXml(ostream& output) const
{
    _STACK;

    for (auto tag: tags) {
        tag.second->writeXml(output);
    }
}
