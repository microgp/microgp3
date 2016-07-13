/***********************************************************************\
|                                                                       |
| Base.h                                                                |
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
 * @file Base.h
 * Definition of the Base class.
 * @see Base.cc
 */

#ifndef HEADER_UGP3_BASE
/** Defines that this file has been included */
#define HEADER_UGP3_BASE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Enumeration.h"

/**
 * ugp3 namespace
 */
namespace ugp3
{
        /**
         * @class Base
         * Represents the base of a number.
         */
	class Base : public Enumeration
	{
	private:
                // Creates a new base with the value and description specified.
		Base(unsigned int value, const char* description = "");
		
                // Creates a new base with the value 0 and the description "". 
		Base();

	public:
                /** 
                 * Creates a new Base with the same values of the one specified by parameter.
                 * @param base Base to take the values from.
                 */
		Base(const Base& base);

                /** Hexadecimal base */
		static const Base Hexadecimal;
                /** Decimal base */
		static const Base Decimal;
                /** Obtal base */
		static const Base Octal;
                /** Binary base */
		static const Base Binary;
	};
}

#endif
