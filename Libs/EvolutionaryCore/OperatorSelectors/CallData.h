/***********************************************************************\
|                                                                       |
| CallData.h                                                            |
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
 * @file CallData.h
 * Store statistics about one call to an operator.
 */

#ifndef HEADER_UGP3_CORE_CALLDATA
#define HEADER_UGP3_CORE_CALLDATA

#include "Performance.h"
#include "Data.h"
#include <Debug.h>
#include <vector>
#include <algorithm>

namespace ugp3 {
namespace core {
    
class Data;

class CallData
{
private:
    unsigned int m_validChildrenCount;
    
    unsigned int m_evaluatedChildrenCount;
    
    std::vector<double> m_childRewards;

    std::vector<unsigned int> m_childrenPerformance;

public:
    
    CallData();
    
    /**
     * Set the number of valid children that this call generated.
     * @param count number of children that the call generated.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    void setValidChildrenCount(unsigned int count) {
        m_validChildrenCount = count;
    }
    
    /**
     * Increases the operator's performance level.
     * 
     * This function should be called exactly once per evaluated child.
     * 
     * @param level Performance level to increment.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    void setChildPerformance(const Performance& level) {
        ++m_evaluatedChildrenCount;
        m_childrenPerformance[level.toValue()]++;
    }
    
    /**
     * Reward a good child.
     * 
     * @param double A reward.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    void addChildReward(double reward) {
        m_childRewards.push_back(reward);
    }
    
    /**
     * Returns the number of unique children that achieved a given performance level.
     * 
     * @param level The performance level
     * @returns unsigned int Number of children who reached this level
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    unsigned int getPerformance(const Performance& level) const {
        return m_childrenPerformance[level.toValue()];
    }
    
    /**
     * Return the number of valid children that came out of the operator.
     * 
     * @return unsigned int Number of children.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    unsigned int getValidChildrenCount() const {
        return m_validChildrenCount;
    }
    
    /**
     * Return the number of children that survived the evaluation, i.e. the number
     * of calls to setChildPerformance().
     * 
     * @return unsigned int Number of non-clone children.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    unsigned int getEvaluatedChildrenCount() const {
        return m_evaluatedChildrenCount;
    }
    
    /**
     * Return the number of children that were eliminated during evaluation.
     * 
     * @return unsigned int Number of children killed during evaluation.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    unsigned int getDeadChildrenCount() const {
        Assert(m_validChildrenCount > m_evaluatedChildrenCount);
        return m_validChildrenCount - m_evaluatedChildrenCount;
    }
    
    /**
     * @return double Max child reward obtained for this call.
     */
    double getMaxReward() const {
        auto it = std::max_element(begin(m_childRewards), end(m_childRewards));
        if (it != end(m_childRewards)) {
            return *it;
        }
        return 0;
    }
};

}
}

#endif // HEADER_UGP3_CORE_CALLDATA
