/***********************************************************************\
|                                                                       |
| IValidable.h                                                          |
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
 * @file IValidable.h
 * Definition of the IValidable class.
 */

#ifndef HEADER_UGP3_IVALIDABLE
/** Defines that this file has been included */
#define HEADER_UGP3_IVALIDABLE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/**
 * ugp3 namespace
 */
namespace ugp3
{
        /**
         * @class IValidable
         * Provides an interface for debug-time self-validation of an instance. 
         */
	class IValidable
	{
	public:
	
                /**
                 * Validates the instance. 
                 * @returns True if the instance is valid, false otherwise.
                 * @throws nothing. if an exception is thrown, the execution is aborted.
                 */
		virtual bool validate() const = 0;

                /**
                 * Destructor of the class for clean memory if necessary
                 * @throws nothing. if an exception is thrown, the execution is aborted.
                 */
		virtual ~IValidable();
	};

	inline IValidable::~IValidable()
	{ }
}

#endif
