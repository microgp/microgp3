/***********************************************************************\
|                                                                       |
| ExpressionElement.cc                                                  |
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
#include "RegexMatch.h"

using namespace ugp3::constraints;


ExpressionElement::ExpressionElement(const Parameter& parameter)
    : parameter(&parameter),
    characters(nullptr)
{ }

ExpressionElement::~ExpressionElement()
{ 
    this->parameter = nullptr;
    delete this->characters;
}

ExpressionElement::ExpressionElement(const std::string& characters)
    : parameter(nullptr),
characters(new std::string(characters))
{ }

const std::string ExpressionElement::getRegex() const
{
	if( this->parameter != nullptr )
	{
		// this ExpressionElement represents a parameter
		return this->parameter->getRegex();
	}
	else if( this->characters != nullptr )
	{
		// it's just a string of characters
		// replace all special characters with escaped versions 
		std::string regex = RegexMatch::stringToRegex( *this->characters);
		
		return regex;
		
	}
	else
	{
		LOG_ERROR << "There is an ExpressionElement instance that does not represent neither a parameter, nor a string of characters..." << std::ends;
		return "";
	}
}

/* // old getRegex() function
const std::string ExpressionElement::getRegex() const
{
	if( this->parameter != nullptr )
	{
		// this ExpressionElement represents a parameter
		return this->parameter->getRegex();
	}
	else if( this->characters != nullptr )
	{
		// it's just a string of characters
		std::string regex = "";
		
		// replace all spaces with \s+
		for(unsigned int i = 0; i < this->characters->length(); i++)
		{
			// lots of characters should be replaced by the '\s+' matching
			if( this->characters->at(i) == ' ' || this->characters->at(i) == '\t')
			{
				regex += "[\\s]+";
				while( this->characters->at(i+1) == ' ' || this->characters->at(i+1) == '\t' ) i++;

			}
			// now, for some special characters
			// that must be escaped, either in C++ strings, or in ECMAScript regex syntax
			else if( this->characters->at(i) == '%' )
			{
				regex += "%";
			}
			else if( this->characters->at(i) == '.' )
			{
				regex += "\\.";
			}
			else if( this->characters->at(i) == '$' )
			{
				regex += "\\$";
			}
			else if( this->characters->at(i) == '*' )
			{
				regex += "\\*";
			}
			else if( this->characters->at(i) == '+' )
			{
				regex += "\\+";
			}
			else if( this->characters->at(i) == '?' )
			{
				regex += "\\?";
			}
			// also add some spaces before and after the new lines
			else if( this->characters->at(i) == '\n' )
			{
				regex += "[\\s]*\\n[\\s]*";
			}
			else
			{
				regex += this->characters->at(i);
			}
		} 
		
		return regex;
		
	}
	else
	{
		LOG_ERROR << "There is an ExpressionElement instance that does not represent neither a parameter, nor a string of characters..." << std::ends;
		return "";
	}
}
*/
