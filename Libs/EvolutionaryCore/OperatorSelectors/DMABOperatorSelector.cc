/***********************************************************************\
|                                                                       |
| DMABOperatorSelector.cc                                               |
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
| $Revision: 655 $
| $Date: 2015-04-14 15:47:04 +0200 (Tue, 14 Apr 2015) $
\***********************************************************************/

/**
 * @file DMABOperatorSelector.cc
 * Implementation of Dynamic Multi Armed Bandit operator selection
 */

#include "DMABOperatorSelector.h"
#include "Statistics.h"
#include "Operator.h"
#include "StackTrace.h"
#include "Log.h"

#include <algorithm>
#include <cmath>

using namespace std;

namespace ugp3 {
namespace core {
    
const std::string DMABOperatorSelector::XML_NAME = "DMAB";

DMABOperatorSelector::DMABOperatorSelector(Statistics& stats)
: SpecificOperatorSelector(stats)
{

}

void DMABOperatorSelector::prepareForSelectionsImpl()
{
}

typename OperatorSelector::Result DMABOperatorSelector::selectImpl()
{
    /* 
     * Find out how many times operators have been executed
     * Must be recomputed before each selection in case some operators are
     * enabled/disabled during the generation.
     */
    m_totalRounds = 0;
    for (unsigned int i = 0; i < getDataCount(); i++) {
        if (getData(i).getSelectable()) {
            m_totalRounds += getData(i).nExecutions;
        }
    }

    LOG_VERBOSE << "Total operator executions so far " << m_totalRounds << ends;
    /*
     * If any of the operators has not been called since the last reset,
     * we select the operator with the highest exploration score.
     */
    for (unsigned int i = 0; i < getDataCount(); i++) {
        auto& data = getData(i);
        if (data.getSelectable() && data.nExecutions == 0) {
            LOG_VERBOSE << "DMAB operator selection: switched to simple exploration because "
            << data.getOperatorName() << " has not been executed yet." << std::ends;
            
            // Select the lowest nExecutions + pending
            Result result;
            result.score = std::numeric_limits<double>::max();
            for (unsigned int i = 0; i < getDataCount(); i++) {
                auto& data = getData(i);
                if (data.getSelectable()) {
                    double score = data.nExecutions + data.pending;
                    if (score < result.score) {
                        result.data = &data;
                        result.op = data.getOperator();
                        result.score = score;
                    }
                }
            }
            Assert(result.data && result.op);
            return result;
        }
    }
    
    return selectWithMAB();    
}

OperatorSelector::Result DMABOperatorSelector::selectWithMAB() const
{
    Result result;
    result.score = -1;
    for (unsigned int i = 0; i < getDataCount(); i++) {
        auto& data = getData(i);
        if (data.getSelectable()) {
            double score = getMABScore(data);
            if (score > result.score) {
                result.data = &data;
                result.op = data.getOperator();
                result.score = score;
            }
        }
    }
    Assert(result.data && result.op);
    return result;
}

// Calculates MAB score according to UCB1 algorithm
double DMABOperatorSelector::getMABScore(const DMABData & data) const
{
    _STACK;
    
    Assert(data.getSelectable());
    Assert(data.nExecutions != 0);
    
    /*
     * Original UCB1 formula, will select the same operator for the whole generation.
     */
    double score = data.meanReward + C * sqrt(std::log(m_totalRounds) / (data.nExecutions));
    
    LOG_VERBOSE << "Operator MAB score: " << score
        << " with stats: (ne: " << data.nExecutions
        << ") (mr: " << data.meanReward << ") (md: " << data.meanDeviation
        << ") (Md: "<< data.maxDeviation << ") - " << data.getOperatorName() << ends;
    
    return score;
}

bool DMABOperatorSelector::rewardAndTestForReset(DMABData & data, double rew)
{
    _STACK;
    
    LOG_DEBUG << "Rewarding operator " << data.getOperatorName() << "with (" << rew << "). "
        "Old stats: (" << data.nExecutions <<")   (" << data.meanReward << ") "
        "(" << data.meanDeviation <<") (" << data.maxDeviation << ")" << std::ends;
    
    LOG_DEBUG << "- meanReward = (" << data.meanReward << " * " << data.nExecutions
        << " + " << rew << " / (" << data.nExecutions << " + 1)" << std::ends;
    
    data.meanReward = (data.meanReward * data.nExecutions + rew) / (data.nExecutions + 1);
    data.nExecutions++;
    
    LOG_DEBUG << "- meanDeviation = " << data.meanDeviation
        << " + (" << data.meanReward << " - " << rew << " + " << PHdelta << ")" << std::ends;
    
    data.meanDeviation = data.meanDeviation + ( data.meanReward - rew + PHdelta );
    data.maxDeviation = std::max(data.maxDeviation, data.meanDeviation);
    
    LOG_VERBOSE << "Rewarded operator " << data.getOperatorName() << "with (" << rew << "). "
        "New stats: (" << data.nExecutions <<")   (" << data.meanReward << ") "
        "(" << data.meanDeviation <<") ("<< data.maxDeviation << ")" << std::ends;
    
    if (data.maxDeviation - data.meanDeviation > PHlambdaLimit) {
        LOG_VERBOSE << "RESET P-H of operator " << data.getOperatorName() << " was triggered" << std::ends;
        return true;
    }
    return false;
}

void DMABOperatorSelector::step(PopulationParameters& params)
{
    _STACK;
    
    if (m_newOperatorActivated) {
        LOG_VERBOSE << "DMAB: new operator activated this generation." << ends;
        LOG_VERBOSE << "Applying P-H reset..." << ends;
        reset();
    }
    
    // For each operator,
    for (unsigned int i = 0; i < getDataCount(); ++i) {
        auto& data = getData(i);
        // For each call to this operator during the last generation,
        for (auto& cd: getData(i).getCallData()) {
            if (cd->getValidChildrenCount() > 0) {
                double instantRew = cd->getMaxReward();
                LOG_VERBOSE << "DMAB instant reward for " << getData(i).getOperatorName()
                << " = " << instantRew << " (" << cd->getValidChildrenCount()
                << " children)" << std::ends;
                /*
                 * Instead of taking the mean or the extreme value, we compute a weighted
                 * mean that gives more weight to extreme values.
                 */
                data.pastRewards.push_back(instantRew);
                if (data.pastRewards.size() > windowSize) {
                    data.pastRewards.pop_front();
                }
                // Copy
                std::vector<double> pastRewards;
                pastRewards.insert(end(pastRewards), begin(data.pastRewards), end(data.pastRewards));
                Assert(!pastRewards.empty());
                // Sort in decreasing order
                std::sort(begin(pastRewards), end(pastRewards), std::greater<double>());
                double DISCOUNT = 0.7; // FIXME random value TODO choose wisely
                double curDiscount = 1;
                double sumDiscounts = 0;
                double weightedSum = 0;
                for (double reward: pastRewards) {
                    weightedSum += reward * curDiscount;
                    sumDiscounts += curDiscount;
                    curDiscount *= DISCOUNT;
                }
                weightedSum /= sumDiscounts;
                
                LOG_VERBOSE << "DMAB reward for " << getData(i).getOperatorName()
                << " = " << weightedSum << " (history";
                for (double reward: data.pastRewards) {
                    LOG_VERBOSE << " " << reward;
                }
                LOG_VERBOSE << ")" << std::ends;
                
                if (rewardAndTestForReset(getData(i), weightedSum)) {
                    LOG_VERBOSE << "Applying P-H reset..." << ends;
                    reset();
                }
            }
        }
    }
}

void DMABOperatorSelector::reset()
{
    for (unsigned int i = 0; i < getDataCount(); ++i) {
        getData(i).reset();
    }
}

}
}
