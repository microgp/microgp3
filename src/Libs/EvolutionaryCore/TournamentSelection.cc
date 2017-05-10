/***********************************************************************\
|                                                                       |
| TournamentSelection.cc                                                |
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

#include "TournamentSelection.h"
#include "Population.h"
#include "Statistics.h"
#include "Random.h"
#include <Convert.h>
#include <Distances.h>
#include "Log.h"

using namespace std;
using namespace ugp3;
using namespace ugp3::core;

TournamentSelection::TournamentSelection(/*const Population& population*/)
: CandidateSelection(/*population*/),
  tau(1), tauMin(1), tauMax(1), metaTau(0), metaTauMin(0), metaTauMax(0)
{
}

std::vector< CandidateSolution* > TournamentSelection::infinitePressureSelection(
    const Population& population, unsigned int count) const
{
    std::vector<CandidateSolution*> selectedCandidates;
    
    if (Random::nextDouble() < this->fitnessHole) {
        sortByHole(population, m_selectable.begin(), m_selectable.end());
    } else {
        sortByFitness(population, m_selectable.begin(), m_selectable.end());
    }
    
    // Choose the best
    for (unsigned int i = 0; i < count; i++) {
        selectedCandidates.push_back(m_selectable[i]);
        LOG_VERBOSE << "Candidate " << m_selectable[i] << " chosen" << ends;
    }
    return selectedCandidates;
}

TournamentSelection::CandVecIt TournamentSelection::tournamentSelect(const Population& population, double pressureMultiplier, CandVecIt begin, CandVecIt end, bool useHole) const
{
    // Select an individual using a tournament
    // First, randomly select actualTau candidates for the tournament
    unsigned int actualTau = getActualTau(population, pressureMultiplier);
    std::vector<CandVecIt> tournamentCandidates;
    if (std::distance(begin, end) <= actualTau) {
        LOG_VERBOSE << "Tournament selection: less candidates (" << std::distance(begin, end)
        << ") than actualTau (" << actualTau << "), using all of them." << ends;
        for (auto it = begin; it != end; ++it) {
            tournamentCandidates.push_back(it);
        }
    } else {
        uniqueSelectionsAndDontMessVector(
            actualTau, begin, end,
            [&] (CandVecIt begin, CandVecIt end) {
                CandVecIt selected = begin + Random::nextUInteger(0, std::distance(begin, end) - 1);
                tournamentCandidates.push_back(selected);
                return selected;
            });
    }
    Assert(tournamentCandidates.size() >= 1);
    Assert(tournamentCandidates.size() <= actualTau);
    // choose the best one comparing fitness
    CandVecIt selected;
    if (useHole) {
        selected = *std::max_element(tournamentCandidates.begin(), tournamentCandidates.end(),
                                    [&] (CandVecIt a, CandVecIt b) {
                                        // NOTE if m_reverse is true, we reverse the results
                                        // `!=' is logical XOR for booleans
                                        return m_reverse != !population.compareForFitnessHole(*a, *b);
                                    });
    } else {
        selected = *std::max_element(tournamentCandidates.begin(), tournamentCandidates.end(),
                                    [&] (CandVecIt a, CandVecIt b) {
                                        return m_reverse != !population.compareForSelection(*a, *b);
                                    });
    }
    LOG_VERBOSE << "Candidate " << *selected << " won the tournament" << ends;
    
    return selected;
}

unsigned int TournamentSelection::getActualTau(const Population& population, double pressionMultiplier) const
{
    // before the selection of new individuals, if metaTau(s) are > 0, set the tau(s) accordingly
    // surprise: you can't do that, because this method is const, so no setting of internal variables...
    // trying to adjust it into "tournament"
    Assert(this->getTau() >=1 || this->getMetaTau() > 0);
    
    LOG_DEBUG   << "Tournament selection (tau=" << this->getTau() 
    << ", metaTau=" << this->getMetaTau()
    << ", multiplier=" << pressionMultiplier << ")" << ends;
    double multipliedTau = pressionMultiplier * this->getTau();
    // if metaTau is set, reconfigure tau accordingly
    if( this->metaTau > 0 ) multipliedTau = (double) pressionMultiplier * this->metaTau * population.getCandidateCount();   
    
    LOG_DEBUG << "Actual \"multipliedTau\" used in the tournament will be " << multipliedTau << ends;
    
    unsigned int integerTau = (unsigned int)multipliedTau;
    double decimalTau = multipliedTau - integerTau;
    
    unsigned int actualTau = integerTau + (Random::nextDouble() < decimalTau? 1 : 0);
    Assert(actualTau > 0);
    return actualTau;
}

std::vector< CandidateSolution* > TournamentSelection::customSelection(
    const Population& population, unsigned int count, double pressionMultiplier) const
{
    _STACK;
    
    std::vector<CandidateSolution*> selectedCandidates;
    
    uniqueSelectionsAndDontMessVector(
        count, m_selectable.begin(), m_selectable.end(),
        [&] (CandVecIt begin, CandVecIt end) {
            bool useHole = (Random::nextDouble() < fitnessHole);
            CandVecIt selected = tournamentSelect(population, pressionMultiplier, begin, end, useHole);
            selectedCandidates.push_back(*selected);
            return selected;
        });
    
    return selectedCandidates;
}

void TournamentSelection::setTauMin(double value)
{
    _STACK;
    
    if( value > tauMax || value > tau || value < 1.0 )
    {
        throw ArgumentException(
            "The value (" + Convert::toString(value) + ") for tauMin is not correct (tauMax was " + Convert::toString(tauMax) + ").", LOCATION);
    }
    
    this->tauMin = value;
}

void TournamentSelection::setTauMax(double value)
{
    _STACK;
    
    if( value < tauMin || value < tau )
        throw ArgumentException("", LOCATION);
    
    this->tauMax = value;
}

void TournamentSelection::updateEndogenParameters(Population& population)
{
    unsigned int goodCandidates = 0;
    unsigned int veryGoodCandidates = 0;
    unsigned int badCandidates = 0;
    for( unsigned int i = 0; i < population.getParameters().getActivations().getDataCount(); i++ )
    {
        const Data& data = population.getParameters().getActivations().getData(i);
        veryGoodCandidates += data.getPerformance(Performance::VeryGood);
        goodCandidates += data.getPerformance(Performance::Good);
        badCandidates += data.getPerformance(Performance::Bad) + data.getPerformance(Performance::VeryBad);
    }
    
    Assert(tau >= tauMin);
    Assert(tauMin >= 1);
    Assert(tauMax >= tauMin);
    double newTau = this->tau;
    double newMetaTau = this->metaTau; 
    
    LOG_DEBUG 	<< "Self-adapting tau: tau=" 
    << this->tau << ", tauMin=" 
    << this->tauMin << ", tauMax=" 
    << this->tauMax << ends;
    
    LOG_DEBUG 	<< "Self-adapting tau: metaTau=" << this->metaTau 
    << ", metaTauMin=" << this->metaTauMin 
    << ", metaTauMax=" << this->metaTauMax << ends;
    
    if( veryGoodCandidates > 0 )
    {
        newTau = this->tauMax;
        newMetaTau = this->metaTauMax;
    }
    else if( goodCandidates > 0 )
    {
        newTau = this->tauMin + (this->tau - this->tauMin) / 2.0;
        newMetaTau = this->metaTauMin + (this->metaTau - this->metaTauMin) / 2.0;
    }
    else if( badCandidates > 0 )
    {
        newTau = this->tauMin;
        newMetaTau = this->metaTauMin;
    }
    
    const double inertia = population.getParameters().getInertia();
    this->tau = inertia * this->tau + (1.0 - inertia) * newTau;
    this->metaTau = inertia * this->metaTau + (1.0 - inertia) * newMetaTau;
    
}

void TournamentSelection::setMetaTauMin(double value)
{
    _STACK;
    
    if( value > metaTauMax || value > metaTau || value <= 0 )
    {
        throw ArgumentException(
            "The value (" + Convert::toString(value) + ") for " + XML_ATTRIBUTE_METATAUMIN + " is not correct (it was " + Convert::toString(metaTauMin) + ").", LOCATION);
    }
    
    this->metaTauMin = value;
}

void TournamentSelection::setMetaTauMax(double value)
{
    _STACK;
    
    if( value < metaTauMin || value < metaTau || value > 1.0 )
    {
        throw ArgumentException(
            "The value (" + Convert::toString(value) + ") for " + XML_ATTRIBUTE_METATAUMAX + " is not correct (it was " + Convert::toString(metaTauMax) + ").", LOCATION);
    }
    
    this->metaTauMax = value;
}

std::vector< CandidateSolution* > TwoStepTournamentSelectionWithFitnessHole::customSelection(const Population& population, unsigned int count, double pressionMultiplier) const
{
    _STACK;
    
    if (count == 1) {
        return TournamentSelectionWithFitnessHole::customSelection(population, count, pressionMultiplier);
    }
    
    std::vector<CandidateSolution*> selectedCandidates;
    
    uniqueSelectionsAndDontMessVector(
        count, m_selectable.begin(), m_selectable.end(),
        [&] (CandVecIt begin, CandVecIt end) {
            if (selectedCandidates.empty()) {
                CandVecIt selectedIt = tournamentSelect(population, pressionMultiplier, begin, end, false);
                selectedCandidates.push_back(*selectedIt);
                return selectedIt;
            } else {
                std::vector<CandVecIt> secondStepCandidates;
                size_t secondStepTau = std::min<size_t>(std::distance(begin, end), getActualTau(population, pressionMultiplier));
                uniqueSelectionsAndDontMessVector(
                    secondStepTau, begin, end,
                    [&] (CandVecIt begin, CandVecIt end) {
                        CandVecIt res = tournamentSelect(population, pressionMultiplier, begin, end, false);
                        secondStepCandidates.push_back(res);
                        return res;
                    }
                );
                // Choose the candidate that is most diverse from already selected candidates
                // We use the entropic distance (because it is always available)
                // FIXME maybe use other distances when possible
                double maxDistance = -1;
                CandVecIt maxCandidate = end;
                for (CandVecIt candIt: secondStepCandidates) {
                    double meanDistance = 0;
                    for (CandidateSolution* selected: selectedCandidates) {
                        meanDistance += Distances::entropic(selected->getMessage(), (*candIt)->getMessage());
                    }
                    meanDistance /= selectedCandidates.size();
                    if (meanDistance > maxDistance) {
                        maxDistance = meanDistance;
                        maxCandidate = candIt;
                    }
                }
                Assert(maxCandidate != end);
                selectedCandidates.push_back(*maxCandidate);
                return maxCandidate;
            }
        });
         
    return selectedCandidates;
}

