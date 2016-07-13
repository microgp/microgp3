/***********************************************************************\
|                                                                       |
| Exception.cc                                                          |
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
 * @file Exception.cc
 * Implementation of the Exception class.
 * @see Exception.h
 */

#include "ugp3_config.h"
#include "Log.h"
#include "Exception.h"
#include "Convert.h"
using namespace ugp3;

// Libs/Shared/Exception.cc
#define REAL_PATH_LEN 24

Exception::Exception(const Exception & e)
	: message(e.message),
	file(e.file),
	method(e.method),
	stackTrace(e.stackTrace),
	line(e.line)
{ }

Exception::Exception(
	const std::string& message, 
	const std::string& file, 
	const std::string& method, 
	const unsigned int line
	)
	: message(message + " [" + method + "@" + &file[strlen(UGP3_BASE_DIR)-REAL_PATH_LEN] + ":" + Convert::toString(line) + "]"),
	file(file),
	method(method),
	stackTrace(""),
	line(line)
{ 
    LOG_DEBUG << "Throwing exception: " << message;

#ifdef USE_STACK_TRACE
	this->stackTrace = ugp3::StackTrace::getTrace();
	LOG_DEBUG << std::endl << this->stackTrace;
#endif

	LOG_DEBUG << std::ends;

#ifdef _MSC_VER
    // break execution and start debugging in Microsoft Visual Studio
    _CrtDbgBreak();
#endif
}

Exception::~Exception() noexcept
{ }
