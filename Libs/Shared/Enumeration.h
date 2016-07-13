/***********************************************************************\
|                                                                       |
| Enumeration.h                                                         |
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
 * @file Enumeration.h 
 * Definition and implementation of the Enumeration class
 */

#ifndef HEADER_ENUMERATION
/** Defines that this file has been included */
#define HEADER_ENUMERATION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "IString.h"

/**
 * @class Enumeration
 * Based on the enumeration type, keeps an integer value and a description. Improve the functionality of an enumeration type by allowing equality comparisons between objects.
 */

class Enumeration : ugp3::IString
{
protected: 
        /** Integer value of the enumerated object. */
	unsigned int value;
        /** Description of the enumerated object. */
	const char* description;

	/** 
          * Initializes an instance of the Enumeration class with value 0 and description "". 
          * @throws Nothing. If an exception is thrown, the execution is aborted.
          */
	Enumeration();
	/** 
          * Initializes a new instance of the Enumeration class with the given attributes.
	    @param value The value that identifies an enumeration field. 
	    @param description A text message that describes the enumeration field. 
          * @throws Nothing. If an exception is thrown, the execution is aborted.
          */
	Enumeration(unsigned int value, const char* description = "");
public:
	/** 
          * Initializes a new instance of the Enumeration class with the same values of the enumeration object specified.
	    @param enumeration Enumeration object to take the attributes from.
          */
	Enumeration(const Enumeration& enumeration);
	/** 
          * Returns the description.
            @returns string The description of the enumeration.
          * @throws Nothing. If an exception is thrown, the execution is aborted.
          */
	virtual const std::string toString() const;
	/** 
          * Compares the values of the enumerations.
	    @param enumeration Enumeration object to compare with.
            @returns bool True if the values of the enumerations are equal, false in other case.
          * @throws Nothing. If an exception is thrown, the execution is aborted.
          */
	bool operator==(const Enumeration& enumeration) const;
	/** 
          * Compares the values of the enumerations.
	    @param enumeration Enumeration object to compare with.
            @returns bool False if the values of the enumerations are equal, true in other case.
          * @throws Nothing. If an exception is thrown, the execution is aborted.
          */
	bool operator!=(const Enumeration& enumeration) const;
};



inline Enumeration::Enumeration()
	: value(0),
	description("")
{ }

inline Enumeration::Enumeration(const Enumeration& enumeration)
	: value(enumeration.value),
	description(enumeration.description)
{ }

inline Enumeration::Enumeration(unsigned int value, const char* description)
	: value(value),
	description(description)
{ }

inline const std::string Enumeration::toString() const
{
	return description;
}

inline bool Enumeration::operator==(const Enumeration& enumeration) const
{
	return this->value == enumeration.value;
}

inline bool Enumeration::operator!=(const Enumeration& enumeration) const
{
	return this->value != enumeration.value;
}



#endif
