/***********************************************************************\
|                                                                       |
| InfinityString.h                                                      |
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
 * @file InfinityString.h
 * Definition of the InfinityString class.
 * @see InfinityString.cc
 */

#ifndef HEADER_INFINITYSTRING
/** Defines that this file has been included */
#define HEADER_INFINITYSTRING

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Exceptions/ArgumentException.h"
#include "StackTrace.h"
#include "IString.h"
#include "IComparable.h"

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

/**
 * @class InfinityString
 * This class represents large numbers. It offers simple methods to deal with it like with a string type (e.g. autoincrement and comparison overloaded members).
 * Base32 data encoding is used (RFC 3548).
 * base32EncodeTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"
 * @author Alessandro Aimo, Alessandro Salomone, Luca Motta
 * @version 1
 */
class InfinityString : public ugp3::IString, public ugp3::IComparable<InfinityString>
{
private:
    // Private copy constructor Not implemented and not used
    InfinityString(const InfinityString& copy);
    // Vector with the characters of the infinity string
    std::vector<char> chars;

public: // constructors and destructors
    /**
     * Constructor of the class. Creates a new InfinityString object with the string "A".
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    InfinityString();
    /**
     * Constructor of the class. Creates a new InfinityString object with the string specified
     * @param infinityString The string to create the object
     * @throws std::exception. Argument exception if the string specified is an empty string
     */
    InfinityString(const std::string& str);
    /**
     * Destructor of the class.
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual ~InfinityString();

public: // methods
    /**
     * Returns the string length
     * @returns unsigned int Length of the string
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    unsigned int getLength() const;
    /**
     * Returns the string
     * @returns string The string representation of this object
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual const std::string toString() const;
    /**
     * Compares the value of this infinity string with the value of the one specified and returns 1 if this is bigger, -1 if it is lower or 0 if both are equals.
     * @returns int 1 if this is bigger, 1 if it is lower or 0 if both are e	uals
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual int compareTo(const InfinityString& infinityString) const;
    /**
     * Compares this infinity string with other and returns true if both are equals
     * @returns bool True if the infinity strings are equals, false otherwise
     */
    virtual bool equals(const InfinityString& infinityString) const;

public: // operators
    /**
     * Assignement operator. Copy the value from right operand
     * @param copy The right operand
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    InfinityString& operator=(const InfinityString &infinityString);

    /**
     * Prefix Increment. Increment by one unit the value of InfinityString 
     */
    InfinityString& operator++();

    /**
     * Postfix Increment. Increment by one unit the value of InfinityString 
     */
    InfinityString operator++(int);
};

inline unsigned int InfinityString::getLength() const
{
    return (unsigned int)this->chars.size();
}

inline bool InfinityString::equals(const InfinityString& infinityString) const
{
    return this->compareTo(infinityString) == 0;
}

#endif
