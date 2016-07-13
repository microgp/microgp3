/***********************************************************************\
|                                                                       |
| IndividualPopulationParameters.cc |
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
 * @file IndividualPopulationParameters.cc
 *
 */

#include "IndividualPopulationParameters.h"
#include "Individual.h"
#include "IndividualPopulation.h"
#include "MOIndividual.h"
#include "GEIndividual.h"
#include "EnhancedIndividual.h"
#include "CandidateSelection.h"

namespace ugp3 {
namespace core {
    
    
IndividualPopulationParameters::IndividualPopulationParameters(IndividualPopulation* population)
: PopulationParameters(population)
{
}

IndividualPopulation* IndividualPopulationParameters::getPopulation() const
{
    return static_cast<IndividualPopulation*>(PopulationParameters::getPopulation());
}

template <class IndividualType>
SpecificIndividualPopulationParameters<IndividualType>::SpecificIndividualPopulationParameters(SpecificIndividualPopulation< IndividualType >* population)
: IndividualPopulationParameters(population)
{
}

template <class IndividualType>
std::vector<Individual*> SpecificIndividualPopulationParameters<IndividualType>::individualSelector(
    unsigned int cardinality, double pressureMultiplier) const
{
    SpecificIndividualPopulation<IndividualType>* population =
    dynamic_cast<SpecificIndividualPopulation<IndividualType>*>(getPopulation());
    Assert(population);
    std::vector<CandidateSolution*> selected = getSelector().selectCandidates(
        population->getIndividualsConstBegin(), population->getIndividualsConstEnd(),
        *getPopulation(), cardinality, pressureMultiplier);
    std::vector<Individual*> selectedIndividuals;
    for (auto candidate: selected) {
        IndividualType* ind = dynamic_cast<IndividualType*>(candidate);
        Assert(ind);
        selectedIndividuals.push_back(ind);
    }
    return selectedIndividuals;
}

template class SpecificIndividualPopulationParameters<MOIndividual>;
template class SpecificIndividualPopulationParameters<GEIndividual>;
template class SpecificIndividualPopulationParameters<EnhancedIndividual>;

}
}