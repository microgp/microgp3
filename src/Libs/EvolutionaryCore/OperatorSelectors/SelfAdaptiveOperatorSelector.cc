/***********************************************************************\
|                                                                       |
| SelfAdaptiveOperatorSelector.cc |
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
 * @file SelfAdaptiveOperatorSelector.cc
 *
 */

#include "SelfAdaptiveOperatorSelector.h"
#include "GeneticOperator.h"
#include "Data.h"
#include "Statistics.h"
#include "GEIndividual.h"
#include "PopulationParameters.h"

namespace ugp3 {
namespace core {
    
const std::string SelfAdaptiveOperatorSelector::XML_NAME = "selfAdaptive";
    
SelfAdaptiveOperatorSelector::SelfAdaptiveOperatorSelector(Statistics & stats)
: SpecificOperatorSelector(stats)
{
}
    
void SelfAdaptiveOperatorSelector::prepareForSelectionsImpl()
{
    // Before there was a normalization step here. It was useless.
}

OperatorSelector::Result SelfAdaptiveOperatorSelector::selectImpl()
{
    _STACK;

    Assert(getDataCount() > 0);
    
    OperatorSelector::Result result;

    double totalWeight = 0;
    for(unsigned int i = 0; i < getDataCount(); i++)
    {
        if (getData(i).getSelectable()) {
            totalWeight += getData(i).getWeight();
        }
    }
    
    if(totalWeight == 0)
    {
        const unsigned int uniformIndex = Random::nextUInteger(0, getDataCount() - 1);
        result.data = &getData(uniformIndex);
        result.op = getData(uniformIndex).getOperator();
        result.score = 0;
        return result;
    }
    
    // choose an uniform random value in the range of the sum of the weights
    // since now the weight values are normalized, the sum of the weights is 1
    double randomSamplePosition = Random::nextDouble(0, totalWeight);

    double cumulative = 0.0;
    for(unsigned int i = 0; i < getDataCount(); i++)
    {
        if (getData(i).getSelectable()) {
            cumulative += getData(i).getWeight();

            if(cumulative >= randomSamplePosition)
            {
                //printf("ho applicato %u!\n", i);
                result.data = &getData(i);
                result.op = getData(i).getOperator();
                result.score = 0;
                return result;
            }
        }
    }
    
    // should never happen
    throw Exception("Russian roulette sampling failed.", LOCATION);
}

void SelfAdaptiveOperatorSelector::step(PopulationParameters& params)
{
    _STACK;
    
    // Step two: from the histogram to the weight (Genetic.c, ugp2.7, line 724).
    // Use the statistics to update the activation probabilities of the operators.
    for (unsigned int i = 0; i < getDataCount(); i++)
    {
        // get the statistics for the i-th operator
        SAData& data = getData(i);
        
        // compute the new weight
        double aimedValue = getWeightFromHistogram(data);

        // blend the new weight with the old one considering the inertia
        updateWeight(data, aimedValue, params.getInertia());
    }
}

double SelfAdaptiveOperatorSelector::getWeightFromHistogram(SAData & data)
{
    _STACK;
    
    double newWeight;
    unsigned int totalCalls = 0;
    
    /*
     * We compute here the histogram only for this generation because
     * Data stores histograms for the whole epoch.
     */
    std::vector<unsigned int> performance;
    performance.resize(Performance::PerformanceLevelCount, 0);
    for (unsigned int i = 0; i < data.getCallData().size(); ++i) {
        CallData & cd = *data.getCallData()[i];
        // Only consider successful calls, failures are handled by the base class
        if (cd.getValidChildrenCount() > 0) {
            for (unsigned int j = 0; j < Performance::PerformanceLevelCount; ++j) {
                auto& perf = *Performance::values[j];
                performance[j] += cd.getPerformance(perf);
            }
            ++totalCalls;
        }
    }

    // If Operator was not called in the last generation, his weight remains
    // the same.
    if (totalCalls == 0) {
        return data.getWeight();
    }  
    
    // the new weight is set
    if(performance[Performance::VeryGood.toValue()] > 0
        || performance[Performance::Good.toValue()] > 0)
    {
        //newWeight = this->getWeight() + delta;
        newWeight = 1;
    }
    else if(performance[Performance::Normal.toValue()] > 0 )
    {
        newWeight = data.getWeight();
    }
    else if(performance[Performance::Bad.toValue()] > 0 ||
        performance[Performance::VeryBad.toValue()] > 0)
    {
        // newWeight = this->getWeight() - delta;
        newWeight = 0;
    }
    else
    {
        //
        newWeight = data.getWeight();
    }
    
    return newWeight;
    
}

void SelfAdaptiveOperatorSelector::updateWeight(SAData & data, double aimedValue, double inertia)
{
    _STACK;
    
    data.setWeight(inertia * data.getWeight() + (1.0 - inertia) *  aimedValue);
    LOG_DEBUG
    << "New value for" << data.getOperatorName() << " is " << data.getWeight()
    << std::ends;
}

void SelfAdaptiveOperatorSelector::epoch(PopulationParameters& params)
{
}


}
}