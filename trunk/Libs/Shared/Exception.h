/***********************************************************************\
|                                                                       |
| Exception.h                                                           |
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
 * @file Exception.h
 * Definition of the Exception class.
 * @see Exception.cc
 */

#ifndef HEADER_UGP3_EXCEPTION
/** Defines that this file has been included */
#define HEADER_UGP3_EXCEPTION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/** Actuall location in the code execution. */
#define LOCATION __FILE__,__FUNCTION__,__LINE__

#include <stdexcept>
#include <string>

#include "StackTrace.h"

/**
 * ugp3 namespace
 */
namespace ugp3
{
        /**
         * @class Exception
         * Exception of ugp3. Keeps the information of the exception and provides methods to read them
         * @see ArgumentException
         * @see AssertException
         * @see FileException
         * @see NullReferenceException
         * @see xml::SchemaException
         */
	class Exception : public std::exception
	{
	protected:
                /** Message of the exception */
		std::string message;
                /** File where the exception was thrown */
		std::string file;
                /** Method where the exception was thrown */
		std::string method;
                /** Trace of the exception */
		std::string stackTrace;
                /** Line where the exception was thrown */
		unsigned int line;

	public:
                /** 
                 * Constructor of the class. Sets the attributes with the same values of the exception specified.
                 * @param e Exception to take the attribute's values from
                 */
		Exception(const Exception & e);
                /** 
                 * Constructor of the class. Sets the attributes with the values specified
                 * @param message Message of the exception
                 * @param file File where the exception is thrown
                 * @param method Method where the exception is thrown
                 * @param line Line where the exception is thrown
                 */
		explicit Exception(
			const std::string& message, 
			const std::string& file,
			const std::string& method,
			const unsigned int line
		     );

                /** 
                 * Destructor of the class. Used to clean memory if necessary.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		virtual ~Exception() noexcept;

                /** 
                 * Returns the message of the exception
                 * @returns char * The message of the exception
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		virtual const char * what() const noexcept;
                /** 
                 * Returns the method name of the exception
                 * @returns string The method name of the exception
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		const std::string& getMethod() const;
                /** 
                 * Returns the file name of the exception
                 * @returns string The file name of the exception
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		const std::string& getFile() const;
                /** 
                 * Returns the line number of the exception
                 * @returns int Line number of the exception
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		unsigned int getLine() const;
                /** 
                 * Returns the trace of the exception
                 * @returns string The trace of the exception
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		const std::string getStackTrace() const;
	};
}

namespace ugp3
{
	inline const char * Exception::what() const noexcept
	{
		return this->message.c_str();
	}

	inline const std::string& Exception::getMethod() const
	{
		return this->method;
	}

	inline const std::string& Exception::getFile() const
	{
		return this->file;
	}

	inline unsigned int Exception::getLine() const
	{
		return this->line;
	}

	inline const std::string Exception::getStackTrace() const
	{
		return this->stackTrace;
	}
}

#endif
