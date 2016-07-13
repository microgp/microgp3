/***********************************************************************\
|                                                                       |
| RankingSelection.cc                                                   |
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

#include "ugp3_config.h"

#include "RankingSelection.h"
#include "Population.h"
#include "Data.h"
#include "Statistics.h"
#include <Convert.h>
#include <limits>
#include "Log.h"

using namespace std;
using namespace ugp3;
using namespace ugp3::core;

RankingSelection::RankingSelection(/*const Population& population*/)
: CandidateSelection(/*population*/),
pressure(1), pressureMin(1), pressureMax(1)
{}

std::vector< CandidateSolution* > RankingSelection::infinitePressureSelection(
    const Population& population, unsigned int count) const
{
    _STACK;
    
    vector<CandidateSolution*> selectedCandidates;
    
    sortByFitness(population, m_selectable.begin(), m_selectable.end());
    // choose the best
    for (unsigned int i = 0; i < count; i++) {
        selectedCandidates.push_back(m_selectable[i]);
        LOG_VERBOSE << "Individual " << m_selectable[i] << " chosen" << ends;
    }
    
    return selectedCandidates;
}

std::vector< CandidateSolution* > RankingSelection::customSelection(const Population& population, unsigned int count, double pressionMultiplier) const
{
    _STACK;

    vector<CandidateSolution*> selectedIndividuals;

    LOG_VERBOSE     << "rankingSelection : performing ranking selection for " << count 
    << " individuals, with pression multiplier " << pressionMultiplier << ends;
    
    sortByFitness(population, m_selectable.begin(), m_selectable.end());
    
    while (selectedIndividuals.size() < count) {
        // Pick an individual from the population
        unsigned int position = this->ranking();
        CandidateSolution* individual = m_selectable[position];
        LOG_VERBOSE << "Individual in position " << position << " selected: " << individual << ends;
        
        // Collect the selected individual
        selectedIndividuals.push_back(individual);
        // Prevent it from being selected again, preserve ordering of the vector
        m_selectable.erase(m_selectable.begin() + position);
    }
    m_selectable.insert(m_selectable.end(), selectedIndividuals.begin(), selectedIndividuals.end());
    
    LOG_DEBUG << "Selection complete. " << selectedIndividuals.size() << " individuals selected" << ends;
    // return the selected individuals
    return selectedIndividuals;
}


void RankingSelection::setPressureMin(double value)
{
    _STACK;
    
    if (value > pressureMax || value > pressure || value < 1.0) {
        throw ArgumentException(
            "The value (" + Convert::toString(value) + ") for pressureMin is not correct (pressureMax was " + Convert::toString(pressureMax) + ").", LOCATION);
    }
    
    this->pressureMin = value;
}

void RankingSelection::setPressureMax(double value)
{
    _STACK;
    
    if (value < pressureMin || value < pressure || value > 2.0)
        throw ArgumentException(
            "The value (" + Convert::toString(value) + ") for pressureMax is not correct (pressureMin was " + Convert::toString(pressureMin) + ").", LOCATION);
        
        this->pressureMax = value;
}

void RankingSelection::updateEndogenParameters(Population& population)
{
    unsigned int goodIndividuals = 0;
    unsigned int veryGoodIndividuals = 0;
    unsigned int badIndividuals = 0;
    for (unsigned int i = 0; i < population.getParameters().getActivations().getDataCount(); i++) {
        const Data& data = population.getParameters().getActivations().getData(i);
        veryGoodIndividuals += data.getPerformance(Performance::VeryGood);
        goodIndividuals += data.getPerformance(Performance::Good);
        badIndividuals += data.getPerformance(Performance::Bad) + data.getPerformance(Performance::VeryBad);
    }
    
    Assert(pressure >= pressureMin);
    Assert(pressureMin >= 1);
    Assert(pressureMax >= pressureMin);
    Assert(pressureMax <= 2);
    double newPressure = this->pressure;
    
    if (veryGoodIndividuals > 0) {
        newPressure = pressureMax;
    } else if (goodIndividuals > 0) {
        newPressure = pressureMin + (pressure - pressureMin) / 2.0;
    } else if (badIndividuals > 0) {
        newPressure = pressureMin;
    }
    
    const double inertia = population.getParameters().getInertia();
    this->pressure = inertia * this->pressure + (1.0 - inertia) * newPressure;
    
}

unsigned int RankingSelection::ranking() const
{
    _STACK;
    
    LOG_VERBOSE << "Ranking selection with pressure " << this->pressure << ends;
    
    Assert(this->pressure >=1);
    Assert(this->pressure <=2);
    
    /* 	individuals are already sorted by fitness, albeit in the opposite order, from best (position 0) to worst (position N)
     *    so the formula is slightly altered, to work in the same way as the original one
     *    original formula:
     *        fitness[POS] = 2 - pressure + 2 * (pressure - 1) * (POS-1) / (N-1)
     *    
     * - so, for pressure == 1, best individual in POS=N has fitness[N] = 1 == fitness[1] (no pressure)
     * - for pressure == 1.5, 
     *    best individual in POS=N has fitness[N] = 0.5 + N-1/N-1 = 1
     *    worst individual in POS=1 has fitness[1] = 0.5
     */
    
    // there is a special case that must be managed: if the number of individuals in the population is 1,
    // rankingSelection can only choose one individual (the formula would be divided by N-1 = 0)
    if ( m_selectable.size() == 1 ) {
        LOG_DEBUG << "rankingSelection : only one individual in population" << ends;
        return 0;
    }
    // so we only have to extract a random double and associate it to the right individual
    double fitnessCumulate = 0;
    vector<double> fitness(m_selectable.size());
    
    for (unsigned int pos = 0; pos < m_selectable.size(); pos++) {
        fitness[pos] = 2 - this->pressure + 2 * (this->pressure - 1 ) * (m_selectable.size() - pos - 1) / (m_selectable.size() - 1);
        LOG_DEBUG << "Individual " << m_selectable[pos] << " goes from " << fitnessCumulate;
        fitnessCumulate += fitness[pos];
        LOG_DEBUG << " to " << fitnessCumulate << ends;
        
    }
    
    // fitnessCumulate is equal to the number of individuals in the population
    // it is stored only for debug purposes 
    double choice = Random::nextDouble(0, fitnessCumulate);
    LOG_DEBUG << "Choice=" << choice << ends;
    
    unsigned int position = 0;
    if ( choice < fitness[0]) {
        position = 0;
    } else {
        fitnessCumulate = fitness[0];
        for (unsigned int pos = 1; pos < m_selectable.size() && choice > fitnessCumulate; pos++) {
            fitnessCumulate += fitness[pos];
            position = pos;
        }   
    }
    
    return position;
}

