/***********************************************************************\
|                                                                       |
| ICloneable.h                                                          |
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
 * @file ICloneable.h
 * Definition of the ICloneable class.
 */

#ifndef HEADER_UGP3_ICLONEABLE
/** Defines that this file has been included */
#define HEADER_UGP3_ICLONEABLE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <exception>
#include <memory>

/**
 * ugp3 namespace
 */
namespace ugp3
{
    /**
     * @class ICloneable
     * Template class that allows clonation to the objects
     * @tparam T Object that acquires clonation 
     */
    template<typename T>
    class ICloneable
    {
    public:
        /** 
         * Clones the object.
         * @returns unique_ptr<T> A new T object with the same information of the one that calls this method
         */
        virtual std::unique_ptr<T> clone() const = 0;
        /** 
         * Destructor of the class. It does nothing.
         */
        virtual ~ICloneable();
    };

    template <typename T>
    ICloneable<T>::~ICloneable()
    { }
}

#endif
