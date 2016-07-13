/***********************************************************************\
|                                                                       |
| Expression.cc                                                         |
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


Expression::Expression(const GenericMacro *parent, const vector<ExpressionElement*> *elements)
	: parent(parent)
{ 
	_STACK;

	if(parent == nullptr)
	{
		throw ArgumentNullException("parent", LOCATION);
	}

	this->elements = *elements;
}

void Expression::clear()
{
	this->elements.clear();
}


Expression::Expression(const GenericMacro *parent)
	: parent(parent)
{ }

Expression::~Expression()
{
	_STACK;

	for(unsigned int i = 0 ; i < this->elements.size() ; i++)
	{
		delete this->elements[i];
		this->elements[i] = nullptr;
	}

	LOG_DEBUG << "Destructor: ugp3::constraints::Expression" << ends;
}

const string Expression::getRegex() const
{
	// adding [\s]* before and after the regex SEEMS like a good idea,
	// but it actually creates problems when checking for empty regexes.
	// it's better to manage this directly at Population::assimilate() level
	//string regex = "[\\s]*";
	
	string regex = "";
	
	// build the regex, by iterating over every element
	for(unsigned int i = 0; i < this->elements.size(); i++)
	{
		regex += this->elements[i]->getRegex();
	}
	//regex += "[\\s]*";

	// however, it is a good idea to replace compulsory spaces with optional spaces at the beginning of a line 
	string toReplace = "[\\s]+";
	
	if( regex.compare(0, toReplace.length(), toReplace ) == 0 )
		// replace with [\\s]*
		regex[4] = '*';
	//else /* for some reason, this creates problems */
	//	regex = toReplace + regex;

	return regex;
}
