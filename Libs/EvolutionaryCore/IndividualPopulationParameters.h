/***********************************************************************\
|                                                                       |
| IndividualPopulationParameters.h |
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
 * @file IndividualPopulationParameters.h
 * Blah
 */

#ifndef HEADER_UGP3_CORE_INDIVIDUALPOPULATIONPARAMETERS
#define HEADER_UGP3_CORE_INDIVIDUALPOPULATIONPARAMETERS

#include "PopulationParameters.h"

namespace ugp3 {
namespace core {
    
class Individual;
class IndividualPopulation;
template <class IndividualType> class SpecificIndividualPopulation;

class IndividualPopulationParameters : public PopulationParameters
{
public:
    IndividualPopulationParameters(IndividualPopulation* population);
    
    /**
     * Convenience function for genetic operators.
     */
    IndividualPopulation* getPopulation() const;
    
    /** 
     * Select individuals.
     * If there are not enough individuals to select from, this function will return
     * a vector with less than the required number of individuals.
     * @param cardinality Number of individuals to return
     * @param pressureMoltiplier multiplier for the selection pressure
     * @returns vector<Individual*> vector of selected individuals
     */
    virtual std::vector<Individual*> individualSelector(unsigned int cardinality, double pressureMultiplier) const = 0;
};

template <class IndividualType>
class SpecificIndividualPopulationParameters : public IndividualPopulationParameters
{
public:
    SpecificIndividualPopulationParameters(SpecificIndividualPopulation<IndividualType>* population);
    
    virtual std::vector<Individual*> individualSelector(unsigned int cardinality, double pressureMultiplier) const;
};

}
}

#endif // HEADER_UGP3_CORE_INDIVIDUALPOPULATIONPARAMETERS
