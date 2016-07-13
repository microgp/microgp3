/***********************************************************************\
|                                                                       |
| Debug.h                                                               |
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
 * @file Debug.h
 * Definition and implementation of the Debug class.
 */

#ifndef HEADER_UGP3_ASSERT
/** Defines that this file has been included */
#define HEADER_UGP3_ASSERT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "Exceptions/AssertException.h"

#include <stdexcept>
#include <iostream>
#include <sstream>


  /**
   * @def Assert(expr)
   * If in debug mode it checks if the expression is true and throws an AssertException if it is false; if not in debug mode it does nothing. Used to write clean code
   */
  /**
   * @def Negate(expr)
   * If in debug mode it checks if the expression is false and throws an AssertException if it is true; if not in debug mode it does nothing. Used to write clean code
   */
#ifdef	NDEBUG
  #define Assert(expr)		;//(void(*)(int, ...))0
  #define Negate(expr)		;//(void(*)(int, ...))0
#else
  #define Assert(expr) 		Assert::truism(expr, #expr, LOCATION)
  #define Negate(expr) 		Assert::falsism(expr, #expr, LOCATION)
#endif

/** Checks if the expression is true, if not it throws an AssertException and write with the exception the message specified by msg. Used to write clean code */
#define Check(expr, msg)	Assert::checkTrue(expr, #expr, msg, LOCATION)

/**
* @class Assert
* Static class that provides method to check conditions
*/
class Assert {
public:
     /** 
      * Checks if the condition is true. If it is false, throws an exception
      * @param condition Condition to check.
      * @param condition_s String representation of the condition to check. 
      * @param file File that calls this method.
      * @param func Function that calls this method.
      * @param line Line where this method was called.
      * @throws Any exception. AssertException if the condition is false.
      */
     static void truism(bool condition, const char* condition_s, const char* file, const char* func, int line);
     /** 
      * Checks if the condition is false. If it is true, throws an exception
      * @param condition Condition to check.
      * @param condition_s String representation of the condition to check. 
      * @param file File that calls this method.
      * @param func Function that calls this method.
      * @param line Line where this method was called.
      * @throws Any exception. AssertException if the condition is true.
      */
     static void falsism(bool condition, const char* condition_s, const char* file, const char* func, int line);
     /** 
      * Checks if the condition is true. If it is false, throws an exception
      * @param condition Condition to check.
      * @param condition_s String representation of the condition to check.
      * @param description Description of the check to show if the exception is thrown.
      * @param file File that calls this method.
      * @param func Function that calls this method.
      * @param line Line where this method was called.
      * @throws Any exception. AssertException if the condition is false.
      */
     static void checkTrue(bool condition, const char* condition_s, const char* description, const char* file, const char* func, int line);
};

inline void Assert::checkTrue(bool condition, const char* condition_s, const char* description, const char* file, const char* func, int line) {
     if(condition)
	  return;
     
     std::ostringstream text;
     text << description << " (\"" << condition_s << "\" is false)";
     throw ugp3::AssertException(text.str(), file, func, line);
}

inline void Assert::truism(bool condition, const char* condition_s, const char* file, const char* func, int line) {
     if(condition)
	  return;
     
     std::ostringstream text;
     text << "\"" << condition_s << "\" is false";
     throw ugp3::AssertException(text.str(), file, func, line);
}

inline void Assert::falsism(bool condition, const char* condition_s, const char* file, const char* func, int line) {
     if(!condition)
	  return;
     
     std::ostringstream text;
     text << "\"" << condition_s << "\" is true";
     throw ugp3::AssertException(text.str(), file, func, line);

}

#endif
