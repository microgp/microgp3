/***********************************************************************\
|                                                                       |
| IComparable.h                                                         |
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
 * @file IComparable.h
 * Definition of the IComparable class.
 */

#ifndef HEADER_UGP3_ICOMPARABLE
/** Defines that this file has been included */
#define HEADER_UGP3_ICOMPARABLE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "IEquatable.h"

/**
 * ugp3 namespace
 */
namespace ugp3
{
    /**
     * @class IComparable
     * Template that allows comparisons between two objects using operators
     * @tparam T Object to acquire comparisons properties
     */
    template<typename T>
    class IComparable : public IEquatable<T>
    {
    public:
        /**
         * Compares the object with another one 
         * @param instance Object to compare with
         * @returns int 1 if this object goes before the one specified in the way that these objects are ordered, -1 if goes after and 0 if they go in the same position
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
        virtual int compareTo(const T& instance) const = 0;
 
        /**
         * Compares the object with another one 
         * @param instance Object to compare with
         * @returns bool True if this object goes before the one specified in the way that these objects are ordered, false otherwise
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
        bool operator>(const T& instance) const;
        /**
         * Compares the object with another one 
         * @param instance Object to compare with
         * @returns bool True if this object goes after the one specified in the way that these objects are ordered, false otherwise
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
        bool operator<(const T& instance) const;
        /**
         * Compares the object with another one 
         * @param instance Object to compare with
         * @returns bool True if this object goes before or in the same position that the one specified in the way that these objects are ordered, false otherwise
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
        bool operator>=(const T& instance) const;
        /**
         * Compares the object with another one 
         * @param instance Object to compare with
         * @returns bool True if this object goes after or in the same position that the one specified in the way that these objects are ordered, false otherwise
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
        bool operator<=(const T& instance) const;

        /**
         * Destructor of the class. It does nothing.
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
        virtual ~IComparable();
    };

    template<typename T>
    IComparable<T>::~IComparable()
    { }

    template<typename T>
    bool IComparable<T>::operator>(const T& instance) const
    {
        return this->compareTo(instance) > 0;
    }

    template<typename T>
    bool IComparable<T>::operator<(const T& instance) const
    {
        return this->compareTo(instance) < 0;
    }

     template<typename T>
    bool IComparable<T>::operator>=(const T& instance) const
    {
        return this->compareTo(instance) >= 0;
    }

    template<typename T>
    bool IComparable<T>::operator<=(const T& instance) const
    {
        return this->compareTo(instance) <= 0;
    }
}

#endif
