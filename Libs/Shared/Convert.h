/***********************************************************************\
|                                                                       |
| Convert.h                                                             |
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
 * @file Convert.h
 * Definition of the Convert class.
 * @see Convert.cc
 * @see Base.h
 */

#ifndef HEADER_UGP3_CONVERT
/** Defines that this file has been included */
#define HEADER_UGP3_CONVERT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <vector>

#include "Base.h"

/**
 * ugp3 namespace
 */
namespace ugp3
{
        /**
         * @class Convert
         * Static class that provides methods to convert between types.
         * @see Base
         */
	class Convert
	{
	public:
                /** 
                 * Returns the unsigned int value of the string parameter in the base specified. 
                 * @param string String to convert
                 * @param Base Base of the number
                 * @returns unsigned int 
                 * @throws Any exception
                 */
		static unsigned int toUInt(const std::string&, Base base = Base::Decimal);
                /** 
                 * Returns the long value of the string parameter in the base specified. 
                 * @param string String to convert
                 * @param Base Base of the number
                 * @returns long 
                 * @throws Any exception
                 */
		static long int toLong(const std::string&, Base base = Base::Decimal);
                /** 
                 * Returns the unsigned long value of the string parameter
                 * @param string String to convert
                 * @returns unsigned long 
                 * @throws Any exception
                 */
		static unsigned long toULong(const std::string&);
                /** 
                 * Returns the int value of the string parameter 
                 * @param string String to convert
                 * @returns int 
                 * @throws Any exception
                 */
		static int toInt(const std::string&);
                /** 
                 * Returns the vector<int> value of the string parameter 
                 * @param string String to convert
                 * @returns vector<int> 
                 * @throws Any exception
                 */
		static std::vector<int> toIntVector(const std::string&, const std::string& delimiters);
                /** 
                 * Returns a tokenization of the string in a vector<string>, given a separator
                 * @param string String to convert
                 * @returns vector<string> 
                 * @throws Any exception
                 */
		static std::vector<std::string> toStringVector(const std::string&, const std::string& delimiters);
                /** 
                 * Returns the double value of the string parameter 
                 * @param string String to convert
                 * @returns double 
                 * @throws Any exception
                 */
		static double toDouble(const std::string&);
                /** 
                 * Returns the float value of the string parameter 
                 * @param string String to convert
                 * @returns float 
                 * @throws Any exception
                 */
		static float toFloat(const std::string&);
                /** 
                 * Returns the bool value of the string parameter 
                 * @param string String to convert
                 * @returns bool 
                 * @throws Any exception
                 */
		static bool toBool(const std::string&);

                /** 
                 * Returns the string representation of the vector<int> parameter
                 * @param vector<int> Vector to convert
                 * @returns string
                 * @throws nothing. if an exception is thrown, the execution is aborted.
                 */
		static const std::string toString(std::vector<int> value);
                /** 
                 * Returns the string representation of the vector<string> parameter
                 * @param vector<string> Vector to convert
                 * @returns string
                 * @throws nothing. if an exception is thrown, the execution is aborted.
                 */
		static const std::string toString(std::vector<std::string> value);
                /** 
                 * Returns the string representation of the bool parameter
                 * @param bool Bool to convert
                 * @returns string
                 * @throws nothing. if an exception is thrown, the execution is aborted.
                 */
		static const std::string toString(bool value);
                /** 
                 * Returns the string representation of the double parameter
                 * @param double Double to convert
                 * @returns string
                 * @throws nothing. if an exception is thrown, the execution is aborted.
                 */
		static const std::string toString(double);
                /** 
                 * Returns the string representation of the int parameter
                 * @param int Integer to convert
                 * @returns string
                 * @throws nothing. if an exception is thrown, the execution is aborted.
                 */
		static const std::string toString(int);
                /** 
                 * Returns the string representation of the long int parameter
                 * @param long int Long integer to convert
                 * @returns string
                 * @throws nothing. if an exception is thrown, the execution is aborted.
                 */
		static const std::string toString(long int);
                /** 
                 * Returns the string representation of the unsigned int parameter in the base specified
                 * @param unsigned int Unsigned int to convert
                 * @param Base of the number
                 * @returns string
                 * @throws Any exception
                 */
		static const std::string toString(unsigned int, Base base = Base::Decimal);
                /** 
                 * Returns the string representation of the unsigned long int parameter
                 * @param unsigned long int to convert
                 * @param Base of the number
                 * @returns string
                 * @throws Any exception
                 */
		static const std::string toString(unsigned long int, Base base = Base::Decimal);
                /** 
                 * Returns the string representation of the float parameter
                 * @param float to convert
                 * @returns string
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		static const std::string toString(float);
                /** 
                 * Returns the string representation of a binary number, starting from the string representation of a number in any base
                 * @param string representation of number to convert, base
                 * @returns string
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		static const std::string toBitString(std::string, Base base = Base::Binary);
                /** 
                 * Returns the string representation of a number in a given base, starting from its string binary representation
                 * @param string representing binary number, base
                 * @returns string
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		static const std::string fromBitString(std::string, Base base = Base::Binary);
	};
}
#endif
