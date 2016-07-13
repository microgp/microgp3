/***********************************************************************\
|                                                                       |
| Exceptions/ArgumentException.h                                        |
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
 * @file ArgumentException.h
 * Definition and implementation of the ArgumentException class.
 */

#ifndef HEADER_UGP3_ARGUMENTEXCEPTION
/** Defines that this file has been included */
#define HEADER_UGP3_ARGUMENTEXCEPTION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Exception.h"

/**
 * ugp3 namespace
 */
namespace ugp3
{
        /**
         * @class ArgumentException
         * Exception of ugp3. An exception to be thrown when a method parameter is incorrect. 
         * @see Exception
         * @see AssertException
         * @see FileException
         * @see NullReferenceException
         * @see xml::SchemaException
         */
	class ArgumentException : public Exception
	{	
	public:
                /** 
                 * Constructor of the class. Sets the attributes with the same values of the exception specified.
                 * @param e ArgumentException to take the attribute's values from
                 */
		ArgumentException(const ArgumentException & e);
                /** 
                 * Constructor of the class. Sets the attributes with the values specified
                 * @param message Message of the exception
                 * @param file File where the exception is thrown
                 * @param method Method where the exception is thrown
                 * @param line Line where the exception is thrown
                 */
		explicit ArgumentException(
			const std::string& message, 
			const std::string& file,
			const std::string& method,
			const unsigned int line);
	};
}

namespace ugp3
{
	inline ArgumentException::ArgumentException(const ArgumentException & e)
	: Exception(e.message, e.file, e.method, e.line)
	{ }

	inline ArgumentException::ArgumentException(
		const std::string& message, 
		const std::string& file, 
		const std::string& method, 
		const unsigned int line
		)
		: Exception(message, file, method, line)
	{ }
}

#endif
