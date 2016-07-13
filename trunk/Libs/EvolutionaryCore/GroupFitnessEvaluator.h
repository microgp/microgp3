/***********************************************************************\
|                                                                       |
| GroupFitnessEvaluator.h                                               |
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

#ifndef HEADER_UGP3_CORE_GROUPFITNESSEVALUATOR
#define HEADER_UGP3_CORE_GROUPFITNESSEVALUATOR

#include "EvaluatorCommon.h"

/**
 * Method for the evaluation of groups
 * ===================================
 * 
 * Change the method number to get the desired behavior.
 * 
 * 0. When we are asked to evaluate an individual, we evaluate it as a group of one.
 * 
 * 1. Same as 0, but we decompose groups into singletons ourselves: the population
 *    will never send us individuals, only groups.
 *    (uses the cache to deduplicate singleton evaluations)
 * 
 * 2. Individual fitness wrt a group = individual contribution to the group
 *    (contribution = coverage(group) - coverage(group without the individual)
 *    Individual fitness = max of individual fitness wrt a group for all groups of this ind. 
 *    Group fitness = coeff * coverage(group) + (1-coeff) * sum(individual contributions)
 *    (quick test on lamps: converges to 4, very high computational load)
 * 
 * 3. TODO reimplement Group fitness = coverage, 1 / sum(individual coverage)
 *    individual fitness = coverage
 *    (quick test on lamps: converges to 7)
 *    
 */
#define GROUP_EVALUATION_METHOD0

#include <unordered_map>
#include <vector>

namespace ugp3
{

namespace core
{
    
class Group;
class GEIndividual;

class GroupFitnessEvaluator : public EvaluatorCommon<Group>
{
private:
#if defined GROUP_EVALUATION_METHOD0
    const unsigned int m_groupFitnessComponentsMultiplier = 1;
    std::vector<Group*> m_pendingInds;
    
#elif defined GROUP_EVALUATION_METHOD1
    const unsigned int m_groupFitnessComponentsMultiplier = 1;
    std::unordered_map< Group*, std::vector<Group*> > m_pendingGroups;
    
#elif defined GROUP_EVALUATION_METHOD2
    const unsigned int m_groupFitnessComponentsMultiplier = 1;
    std::unordered_map< Group*, std::vector< std::pair<Group*, GEIndividual*> > > m_pendingGroups;
    
#endif
    
public:
    GroupFitnessEvaluator() {};
    virtual ~GroupFitnessEvaluator() {};
    
    /**
     * Intercept group evaluations to perform individual evaluations based on the new fitness model.
     */
    virtual void evaluate(CandidateSolution& object) override;
    
    /**
     * Apply computed fitness values to individuals.
     */
    virtual void flush(std::function<void(double)> showProgress) override;
    
    /**
     * Number of fitness components in a group for each fitness component in an individual.
     */
    unsigned int getGroupFitnessComponentMultiplier() const { return m_groupFitnessComponentsMultiplier; }
};

}

}

#endif

