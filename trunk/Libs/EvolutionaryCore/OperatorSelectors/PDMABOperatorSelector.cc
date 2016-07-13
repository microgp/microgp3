/***********************************************************************\
|                                                                       |
| PDMABOperatorSelector.cc |
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
 * @file PDMABOperatorSelector.cc
 *
 */

#include "PDMABOperatorSelector.h"

#include <cmath>

namespace ugp3 {
namespace core {
    
const std::string PDMABOperatorSelector::XML_NAME = "PDMAB";

PDMABOperatorSelector::PDMABOperatorSelector(Statistics& stats)
: DMABOperatorSelector(stats)
{
}

double PDMABOperatorSelector::getMABScore(const DMABData& data) const
{
    _STACK;
    
    Assert(data.getSelectable());
    Assert(data.nExecutions != 0);
    
    unsigned int totalPending = 0;
    for (unsigned int i = 0; i < getDataCount(); ++i) {
        if (getData(i).getSelectable()) {
            totalPending += getData(i).pending;
        }
    }
    
    /*
     * Modified UCB1 formula, will change its mind during the generation.
     */
    double score = data.meanReward
        + C * sqrt(std::log(m_totalRounds + totalPending)
            / (data.nExecutions + data.pending));
    
    LOG_VERBOSE << "Operator MAB score: " << score
        << " with stats: (ne: " << data.nExecutions
        << ") (mr: " << data.meanReward << ") (md: " << data.meanDeviation
        << ") (Md: "<< data.maxDeviation << ") - " << data.getOperatorName()
        << std::ends;
    
    return score;
}

}
}