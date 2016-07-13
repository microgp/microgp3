/***********************************************************************\
|                                                                       |
| Exceptions/SchemaException.h                                          |
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
 * @file SchemaException.h
 * Definition and implementation of the SchemaException class.
 */

#ifndef HEADER_UGP3_SCHEMAEXCEPTION
/** Defines that this file has been included */
#define HEADER_UGP3_SCHEMAEXCEPTION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Exception.h"

/**
 * xml namespace
 */
namespace xml
{
        /**
         * @class SchemaException
         * Exception of ugp3 used when a xml schema is not well formed
         * @see ugp3::Exception
         * @see ugp3::ArgumentException
         * @see ugp3::AssertException
         * @see ugp3::FileException
         * @see ugp3::NullReferenceException
         * @see DuplicateElementSchemaException
         * @see MissingElementSchemaException
         */
	class SchemaException : public ugp3::Exception
	{	
	public:
                /** 
                 * Constructor of the class. Sets the attributes with the same values of the exception specified.
                 * @param e SchemaException to take the attribute's values from
                 */
		SchemaException(const SchemaException& e);
                /** 
                 * Constructor of the class. Sets the attributes with the values specified
                 * @param message Message of the exception
                 * @param file File where the exception is thrown
                 * @param method Method where the exception is thrown
                 * @param line Line where the exception is thrown
                 */
		explicit SchemaException(
			const std::string& message, 
			const std::string& file,
			const std::string& method,
			const unsigned int line);
	};

        /**
         * @class MissingElementSchemaException
         * Exception of ugp3 used when a xml element is missing
         * @see SchemaException
         * @see DuplicateElementSchemaException
         */
	class MissingElementSchemaException : public SchemaException
	{	
	public:
                /** 
                 * Constructor of the class. Sets the attributes with the same values of the exception specified.
                 * @param e MissingElementSchemaException to take the attribute's values from
                 */
		MissingElementSchemaException(const MissingElementSchemaException& e);
                /** 
                 * Constructor of the class. Sets the attributes with the values specified
                 * @param message Message of the exception
                 * @param file File where the exception is thrown
                 * @param method Method where the exception is thrown
                 * @param line Line where the exception is thrown
                 */
		explicit MissingElementSchemaException(
			const std::string& message, 
			const std::string& file,
			const std::string& method,
			const unsigned int line);
	};

        /**
         * @class DuplicateElementSchemaException
         * Exception of ugp3 used when a xml element is duplicated
         * @see SchemaException
         * @see MissingElementSchemaException
         */
	class DuplicateElementSchemaException : public SchemaException
	{	
	public:
                /** 
                 * Constructor of the class. Sets the attributes with the same values of the exception specified.
                 * @param e DuplicateElementSchemaException to take the attribute's values from
                 */
		DuplicateElementSchemaException(const DuplicateElementSchemaException& e);
                /** 
                 * Constructor of the class. Sets the attributes with the values specified
                 * @param message Message of the exception
                 * @param file File where the exception is thrown
                 * @param method Method where the exception is thrown
                 * @param line Line where the exception is thrown
                 */
		explicit DuplicateElementSchemaException(
			const std::string& message, 
			const std::string& file,
			const std::string& method,
			const unsigned int line);
	};
}

namespace xml
{
	inline SchemaException::SchemaException(const SchemaException& e)
		: ugp3::Exception(e.message, e.file, e.method, e.line)
	{ }

	inline SchemaException::SchemaException(
		const std::string& message, 
		const std::string& file, 
		const std::string& method, 
		const unsigned int line)
		: ugp3::Exception("xml schema error: " + message, file, method, line)
	{ }

	inline MissingElementSchemaException::MissingElementSchemaException(
		const std::string& elementName, 
		const std::string& file, 
		const std::string& method, 
		const unsigned int line)
		: SchemaException("mandatory element \"" + elementName + "\" not found.", file, method, line)
	{ }

	inline MissingElementSchemaException::MissingElementSchemaException(const MissingElementSchemaException& e)
		: SchemaException(e.message, e.file, e.method, e.line)
	{ }

	inline DuplicateElementSchemaException::DuplicateElementSchemaException(
		const std::string& elementName, 
		const std::string& file, 
		const std::string& method, 
		const unsigned int line)
		: SchemaException("element \"" + elementName + "\" must appear only once.", file, method, line)
	{ }

	inline DuplicateElementSchemaException::DuplicateElementSchemaException(const DuplicateElementSchemaException& e)
		: SchemaException(e.message, e.file, e.method, e.line)
	{ }
}

#endif
