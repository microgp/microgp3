/***********************************************************************\
|                                                                       |
| ArgumentOutOfRangeException.h	                                        |
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

#ifndef HEADER_UGP3_ARGUMENTOUTOFRANGEEXCEPTION
#define HEADER_UGP3_ARGUMENTOUTOFRANGEEXCEPTION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Exception.h"
#include "Exceptions/ArgumentOutOfRangeException.h"
#include "Exceptions/ArgumentException.h"


namespace ugp3
{
    /** An exception to be thrown when a method parameter is outside the bounds of the possible values. */
	class ArgumentOutOfRangeException : public ArgumentException
	{
	public:
		ArgumentOutOfRangeException(const ArgumentOutOfRangeException & e);
		explicit ArgumentOutOfRangeException(
			const std::string& argumentName, 
			const std::string& file,
			const std::string& method,
			const unsigned int line);
	};
}

namespace ugp3
{
	inline ArgumentOutOfRangeException::ArgumentOutOfRangeException(const ArgumentOutOfRangeException & e)
		: ArgumentException(e.message, e.file, e.method, e.line)
	{ }

	inline ArgumentOutOfRangeException::ArgumentOutOfRangeException(
		const std::string& parameterName, 
		const std::string& file, 
		const std::string& method, 
		const unsigned int line) : ArgumentException("The specified value was outside range of the allowed values for the parameter \"" + parameterName + "\".", file, method, line)
	{ }
}

#endif
