/***********************************************************************\
|                                                                       |
| DMABData.h                                                            |
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
 * @file DMABData.h
 * Blah
 */

#ifndef HEADER_UGP3_CORE_DMABDATA
#define HEADER_UGP3_CORE_DMABDATA

#include <Data.h>
#include <deque>

namespace ugp3 {
namespace core {

class DMABData : public Data
{
private: // Data used by the DMAB operator selector
    
    // number of times the operator was rewarded for an execution. Updated during the rewarding phase, reset with the PH test.
    unsigned int nExecutions = 0;

    // mean reward of the operator
    double meanReward = 0.0;

    // mean deviation of rewards
    double meanDeviation = 0.0;

    // maximum deviation found in the history until now
    double maxDeviation = 0.0;
    
    // Reward window
    std::deque<double> pastRewards;
    
    friend class DMABOperatorSelector;
    friend class PDMABOperatorSelector;
    friend class KappaRouletteOperatorSelector;
    
protected:
    virtual void clear();
    
    static const std::string XML_CHILD_ELEMENT_DMAB;
    static const std::string XML_ATTRIBUTE_NEXECTUIONS;
    static const std::string XML_ATTRIBUTE_MEAN_REWARD;
    static const std::string XML_ATTRIBUTE_MEAN_DEVIATION;
    static const std::string XML_ATTRIBUTE_MAX_DEVIATION;
    static const std::string XML_ATTRIBUTE_PAST_REWARDS;
    
public:
    DMABData(const std::string& operatorName);
    DMABData(Operator& op);
    DMABData();
    
    /**
     * Perform a reset in the DMAB sense.
     */
    void reset();
    
    /**
     * Return some MAB related indicator of probability of being choosed.
     * @return Current mean reward of the operator.
     */
    double getMABWeight() const;
    
    virtual void dumpStatistics(std::ostream& output) const override;
    virtual void dumpStatisticsHeader(const std::string& name, std::ostream& output) const override;
    
    virtual void writeInnerXml(std::ostream& output) const override;
    virtual void readXml(const xml::Element& element) override;
};

}
}

#endif // HEADER_UGP3_CORE_DMABDATA
