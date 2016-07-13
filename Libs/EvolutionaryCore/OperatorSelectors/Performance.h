/***********************************************************************\
|                                                                       |
| Performance.h                                                         |
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
 * @file Performance.h
 * Definition of the Performance class.
 * @see Performance.cc
 */

#ifndef HEADER_UGP3_CORE_PERFORMANCE
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_PERFORMANCE

// headers from shared module
#include "Enumeration.h"
#include <map>

/**
 * ugp3 namespace
 */
namespace ugp3
{
    
/**
 * ugp3::core namespace
 */
namespace core
{

    /**
     * @class Performance
     * The performance histogram classes used to update the operator weight.
     */
    class Performance : public Enumeration
    {
    private:
        Performance(unsigned int value, const char* description = "");
        Performance(const Performance& other); // Prevent copy
        
    public:
        static const unsigned int PerformanceLevelCount = 5;
        static const Performance* values[];

        /** Indicates that the performance of an operator is VeryGood. */
        static const Performance VeryGood;
        /** Indicates that the performance of an operator is Good. */
        static const Performance Good;
        /** Indicates that the performance of an operator is Normal. */
        static const Performance Normal;
        /** Indicates that the performance of an operator is Bad. */
        static const Performance Bad;
        /** Indicates that the performance of an operator is VeryBad. */
        static const Performance VeryBad;
        
        /** 
         * Returns the integer value of this enumerated type.
         * @returns unsigned int The integer value of this object.
         */
        unsigned int toValue() const;
    };

inline unsigned int Performance::toValue() const
{
    return this->value;
}

}

}

#endif
