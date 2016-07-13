/***********************************************************************\
|                                                                       |
| DMABOperatorSelector.h                                                |
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
 * @file DMABOperatorSelector.h
 * Implementation of Dynamic Multi Armed Bandit operator selection
 */

#ifndef HEADER_UGP3_CORE_DMABOPERATORSELECTOR
#define HEADER_UGP3_CORE_DMABOPERATORSELECTOR

#include "OperatorSelector.h"
#include "DMABData.h"

namespace ugp3 {
namespace core {

class PopulationParameters;
class Statistics;
class Data;
    
class DMABOperatorSelector : public SpecificOperatorSelector<DMABData>
{
protected:
    // PH reset trigger level (generally from 0.5 to 20)
    // Original value: 0.8
    const double PHlambdaLimit = 1;

    // Enforces PH robustness when dealing with slow changing environments
    const double PHdelta = 0.15;
    
    // Degree of exploration, 7 is a good value according to Maturana et al.
    // in "Extreme Compass and Dynamic Multi-Armed Bandits for Adaptive Operator
    // Selection"
    // However during our experimental tests using the Python simulator, it seems
    // 7 is too much for rewards in [0, 1].
    const double C = 1.4; // ~ sqrt(2)
    
    // Window size for reward assignement ("tau" in the various papers)
    const unsigned int windowSize = 10;
    
    // Scaling data
    unsigned int m_totalRounds = 0;
    
protected:
    // D-MAB reset
    void reset();
    
    // Called when all operator have valid MAB statistics
    // In this class, vanilla DMAB
    virtual OperatorSelector::Result selectWithMAB() const;
    
    // calculates the MAB score used by selectWithMAB()
    virtual double getMABScore(const DMABData& data) const;
    
    /**
     * Reward the given operator with `rew`, and decide if a reset is needed.
     * @return true iff a reset is needed
     */
    virtual bool rewardAndTestForReset(DMABData& data, double rew);
    
    // Select an operator. Runs at least once every op then calls selectWithMAB()
    virtual Result selectImpl() override;
    
    // Compute and cache m_totalRounds
    virtual void prepareForSelectionsImpl() override;

public:
    DMABOperatorSelector(Statistics& stats);
    virtual ~DMABOperatorSelector() {}
    
    /**
     * Update the internal statistics according to call data of this generation.
     */
    virtual void step(PopulationParameters& params);

    /**
     * Does nothing.
     */
    virtual void epoch(PopulationParameters& /* params */) {};
    
    static const std::string XML_NAME;
    
    virtual const std::string& getXmlName() const override { return XML_NAME; }
};

}
}

#endif
