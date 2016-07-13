/***********************************************************************\
|                                                                       |
| StackTrace.h                                                          |
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
 * @file StackTrace.h
 * Definition of the StackTrace class.
 * @see StackTrace.cc
 */

#ifndef HEADER_UGP3_STACKTRACE
/** Defines that this file has been included */
#define HEADER_UGP3_STACKTRACE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <stack>

/**
 * @def USE_STACK_TRACE
 * If in debug mode, defined as 1, else not defined
 */
#ifndef NDEBUG
#define USE_STACK_TRACE 1
#endif

/**
 * @def _STACK
 * If USE_STACK_TRACE is defined, traces the actual location, else it does nothing
 */
#ifdef USE_STACK_TRACE
#  define _STACK		ugp3::StackTrace trace(LOCATION)
#else
#  define _STACK		0
#endif

#ifdef USE_STACK_TRACE
/**
 * ugp3 namespace
 */
namespace ugp3
{
        /**
         * @class StackTrace
         * Provides methods to save traces of the execution of the application
         */
	class StackTrace
	{
	private:
                // Stack to save the traces
		static std::stack<std::string> stackTrace;

                // Constructor of the class. Not implemented.
		StackTrace(const StackTrace &);
                // Constructor of the class. It does nothing.
		StackTrace();
	public:
                /** 
                 * Constructor of the class. Pushs a new trace with the parameters specified in the static stack of the class.
                 * @param fileName Name of the file of the trace
                 * @param methodName Method name of the trace
                 * @param line Number line of the trace
                 */
		StackTrace(const char* fileName, const char* methodName, int line);
                /** 
                 * Destructor of the class. Pops the trace of the static stack of the class.
                 */
		~StackTrace();

                /** 
                 * Returns a string with all the traces of the static stack of the class. At the end the stack is empty.
                 * @returns string The traces in the stack
                 */
		static const std::string getTrace();
	};
}
#endif


#endif
