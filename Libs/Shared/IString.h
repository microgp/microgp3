/***********************************************************************\
|                                                                       |
| IString.h                                                             |
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
 * @file IString.h
 * Definition of the IString class.
 * @see IString.cc
 */

#ifndef HEADER_UGP3_ISTRING
/** Defines that this file has been included */
#define HEADER_UGP3_ISTRING

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <ostream>

/**
 * ugp3 namespace
 */
namespace ugp3
{

    /**
     * @class IString
     * Abstract class; interface of an extended string type.
     */
    class IString
    {
    public:
        /** 
         * Returns a string representation of the object.
         * @returns string Representation of the object.
         * @throws Nothing. If an exception is thrown, the execution is aborted.
         */
        virtual const std::string toString() const = 0;
        /** 
         * Returns a string representation of the object.
         * @returns string Representation of the object.
         * @throws Nothing. If an exception is thrown, the execution is aborted.
         */
        operator const std::string() const; 

        /** 
         * Destructor of the class. It does nothing.
         * @throws Nothing. If an exception is thrown, the execution is aborted.
         */
        virtual ~IString();
        
        /** 
         * Adds the iString stream to the os stream and returns the last one.
         * @param os Pointer to ostream. Stream to add to.
         * @param iString Pointer to iString. Stream to add.
         * @returns ostream Stream resulting of adding os and iString
         */
        friend std::ostream& operator << (std::ostream &os, const IString& iString);
    };

    inline IString::operator const std::string() const
    { 
        return this->toString(); 
    } 
    
    inline std::ostream& operator << (std::ostream &os, const IString& iString) 
    {
		os << iString.toString();
		return os;
	}
}

#endif
