/***********************************************************************\
|                                                                       |
| IEquatable.h                                                          |
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
 * @file IEquatable.h
 * Definition of the IEquatable class.
 */

#ifndef HEADER_UGP3_IEQUATABLE
/** Defines that this file has been included */
#define HEADER_UGP3_IEQUATABLE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/**
 * ugp3 namespace
 */
namespace ugp3
{
    /**
     * @class IEquatable
     * Template class that allows the object to be compared with another in order to know if they are equals
     * @tparam T Object to allow to be compared
     */
    template<typename T>
    class IEquatable
    {
    public:
        /**
         * Compares the object with another specified and returns if they are in the same position in the way that the objects are ordered
         * @param instance Object to compare with
         * @returns bool True if the objects are in the same position, false otherwise
         */
        virtual bool equals(const T& instance) const = 0;

        /**
         * Compares the object with another specified and returns if they are in the same position in the way that the objects are ordered
         * @param instance Object to compare with
         * @returns bool True if the objects are in the same position, false otherwise
         */
        bool operator==(const T& instance) const;
        /**
         * Compares the object with another specified and returns if they are in the same position in the way that the objects are ordered
         * @param instance Object to compare with
         * @returns bool False if the objects are in the same position, True otherwise
         */
        bool operator!=(const T& instance) const;

        /**
         * Destructor of the class. It does nothing
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
        virtual ~IEquatable();
    };

    template<typename T>
    IEquatable<T>::~IEquatable()
    { }

    template<typename T>
    bool IEquatable<T>::operator==(const T& instance) const
    {
        return this->equals(instance) == true;
    }

    template<typename T>
    bool IEquatable<T>::operator!=(const T& instance) const
    {
        return this->equals(instance) == false;
    }
}

#endif
