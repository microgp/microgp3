/***********************************************************************\
|                                                                       |
| CandidateSelection.xml.cc                                            |
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
 * @file CandidateSelection.xml.cc
 * Implementation of the xml related methods of the CandidateSelection class.
 * @see CandidateSelection.h
 */

#include "ugp3_config.h"
#include "CandidateSelection.h"
#include "Population.h"

using namespace std;
using namespace ugp3;
using namespace ugp3::core;


void CandidateSelection::sortByFitness(const Population& population, CandVecIt begin, CandVecIt end) const
{
    std::sort(begin, end, [&] (CandidateSolution* a, CandidateSolution* b) {
        if (m_reverse) {
            return !population.compareForSelection(a, b);
        } else {
            return population.compareForSelection(a, b);
        }
    });
}

void CandidateSelection::sortByHole(const Population& population, CandVecIt begin, CandVecIt end) const 
{
    std::sort(begin, end, [&] (CandidateSolution* a, CandidateSolution* b) {
        if (m_reverse) {
            return !population.compareForFitnessHole(a, b);
        } else {
            return population.compareForFitnessHole(a, b);
        }
    });
}

