/***********************************************************************\
|                                                                       |
| Population.cc                                                         |
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
 * @file Parameters.h
 * Abstract class base of PopulationParameters.
 * @see PopulationParameters
 */


#ifndef HEADER_UGP3_CORE_PARAMETERS
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_PARAMETERS

// headers from this module
#include "CandidateSelection.h"

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
 * @class parameters
 * Abstract class that define some getters for the parameter class
 * @see PopulationParameters
 */
class Parameters
{
//public methods
public:
    /** 
     * Returns the value of sigma
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual double          getSigma() const = 0;
    /** 
     * Returns the value of mu
     * @returns unsigned int The value of mu
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual unsigned int 	getMu() const = 0;
    
    /** 
     * Returns the differential evolution crossover probability
     */
    virtual double getDiffEvoCP() const = 0;
    
    /** 
     * Returns the differential evolution cdifferential weight
     */
    virtual double getDiffEvoDW() const = 0;
    
    //virtual destructor
    virtual ~Parameters(){ }
};
}
}

#endif
