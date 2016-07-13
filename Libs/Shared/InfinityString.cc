/***********************************************************************\
|                                                                       |
| InfinityString.cc                                                     |
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
 * @file InfinityString.cc
 * Implementation of the InfinityString class.
 * @see InfinityString.h
 */

#include "ugp3_config.h"
#include "InfinityString.h"

using namespace std;

InfinityString::InfinityString()
{
    this->chars.push_back('A');
}

InfinityString::InfinityString(const string& infinityString)
{
    _STACK;

    if(infinityString.size() == 0)
    {
        throw ugp3::ArgumentException("Cannot initialize an InfinityString with an empty string.", LOCATION);
    }

    for(unsigned int i = infinityString.size(); i > 0; i--)
    {
        this->chars.push_back(infinityString[i-1]);
    }
}

InfinityString::~InfinityString()
{ }

InfinityString::InfinityString(const InfinityString& infinityString)
:chars(infinityString.chars)
{
}

InfinityString& InfinityString::operator=( const InfinityString &infinityString )
{
    this->chars = infinityString.chars;
    return *this;
}

int InfinityString::compareTo(const InfinityString& infinityString) const
{
    if(this->chars.size() > infinityString.chars.size())
    {
        return 1;
    }
    else if(this->chars.size() < infinityString.chars.size())
    {
        return -1;
    }

    for(unsigned int i = this->chars.size(); i > 0; i--)
    {
		unsigned int thisValue = this->chars[i-1];
		unsigned int otherValue = infinityString.chars[i-1];

		// a number in base32 is greater than a capitol letter
		// a number in Ascii is less than a capitol letter
		if(thisValue < 'A')
			thisValue += 'Z';
		if(otherValue < 'A')
			otherValue += 'Z';

        if(thisValue > otherValue)
        {
            return 1;
        }
        else if(thisValue < otherValue)
        {
            return -1;
        }
    }

    return 0;
}

const string InfinityString::toString() const
{
	ostringstream stream;

    for(unsigned int i = this->chars.size(); i > 0; i--)
    {
        stream << this->chars[i - 1];
    }

	return stream.str();
}

InfinityString& InfinityString::operator++()
{
    bool carry;
    unsigned int i = 0;

    do
    {
        carry = false;

        if(this->chars.at(i)=='Z')
            this->chars[i] = '2';
        else if(this->chars.at(i)=='7')
        {
            this->chars[i] = 'A';
            carry = true;
            i++;
            if(i >= this->chars.size())
                this->chars.push_back('A'-1);
        }
        else
            this->chars[i]++;
    }
    while(carry);

    return *this;
}

InfinityString InfinityString::operator++(int)
{
    bool carry;
    unsigned int i = 0;

    InfinityString temp(*this);

    do
    {
        carry = false;

        if(this->chars.at(i)=='Z')
            this->chars[i] = '2';
        else if(this->chars.at(i)=='7')
        {
            this->chars[i] = 'A';
            carry = true;
            i++;
            if(i >= this->chars.size())
                this->chars.push_back('A'-1);
        }
        else
            this->chars[i]++;
    }
    while(carry);

    return temp;
}
