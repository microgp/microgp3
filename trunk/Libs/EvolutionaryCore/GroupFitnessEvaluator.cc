/***********************************************************************\
|                                                                       |
| GroupFitnessEvaluator.cc                                              |
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

#include "GroupFitnessEvaluator.h"
#include "Group.h"
#include "GEIndividual.h"

using namespace std;
using namespace ugp3::core;

void GroupFitnessEvaluator::evaluate(CandidateSolution& object)
{
#if defined GROUP_EVALUATION_METHOD0
    if (GEIndividual* ind = dynamic_cast<GEIndividual*>(&object)) {
        // NOTE Use the temporary group constructor to avoid "stealing" ids from the main program
        Group* indGroup = new Group(ind->getBirth(), ind->getPopulation(), true);
        indGroup->addIndividual(ind);
        m_pendingInds.push_back(indGroup);
        LOG_DEBUG << "GroupFitnessEvaluator: requesting evaluation of subgroup " << indGroup << ends;
        EvaluatorCommon::evaluate(*indGroup);
    } else
#endif
        
    if (Group* group = dynamic_cast<Group*>(&object)) {
        
#if defined GROUP_EVALUATION_METHOD1
        // Create one group per individual and ask for its evaluation
        LOG_VERBOSE << "GroupFitnessEvaluator: intercepted evaluation of group " << group << ", queuing subgroups..." << ends;
        std::vector<Group*> individualGroups;
        individualGroups.reserve(group->getIndividualCount());
        for (auto ind: group.getIndividuals()) {
            // NOTE Use the temporary group constructor to avoid "stealing" ids from the main program
            Group* indGroup = new Group(group->getBirth(), group->getPopulation(), true);
            indGroup->addIndividual(ind);
            individualGroups.push_back(indGroup);
            LOG_DEBUG << "GroupFitnessEvaluator: requesting evaluation of subgroup " << indGroup << ends;
            EvaluatorCommon::evaluate(*indGroup);
        }
        m_pendingGroups.insert(std::make_pair(group, std::move(individualGroups)));
        
#elif defined GROUP_EVALUATION_METHOD2
        // Create one group per individual and ask for its evaluation
        std::vector<std::pair<Group*, GEIndividual*> > individualGroups;
        individualGroups.reserve(group->getIndividualCount());
        for (auto ind: group.getIndividuals()) {
            // NOTE Use the temporary group constructor to avoid "stealing" ids from the main program
            Group* indGroup = new Group(group, std::vector<Individual*>({ind}), std::vector<Individual*>(), true);
            individualGroups.push_back(std::make_pair(indGroup, ind));
            EvaluatorCommon<Group>::evaluate(*indGroup);
        }
        m_pendingGroups.insert(std::make_pair(group, std::move(individualGroups)));
#endif
    
        EvaluatorCommon::evaluate(*group);
    }
}

void GroupFitnessEvaluator::flush(std::function<void(double)> showProgress)
{
    EvaluatorCommon::flush(showProgress);
    
#if defined GROUP_EVALUATION_METHOD0
    for (Group* group: m_pendingInds) {
        group->getIndividuals()[0]->getRawFitness() = group->getRawFitness();
        delete group;
    }
    m_pendingInds.clear();
    
#elif defined GROUP_EVALUATION_METHOD1
    for (auto record: m_pendingGroups) {
        for (auto indGroup: record.second) {
            // Report the fitness to the individual
            indGroup->getIndividuals()[0]->getRawFitness() = indGroup->getRawFitness();
            delete indGroup;
        }
    }
    m_pendingGroups.clear();
    
#elif defined GROUP_EVALUATION_METHOD2
    for (auto record: m_pendingGroups) {
        Group *baseGroup = record.first;
        size_t fitnessSize = baseGroup->getPopulation().getParameters().getIndividualFitnessParametersCount();
        
        vector<double> sumContributions(fitnessSize, 0);
        for (auto pair: record.second) {
            Group* groupWithout = pair.first;
            GEIndividual* ind = pair.second;
            std::vector<double> contribution(fitnessSize, 0);
            for (size_t i = 0; i < fitnessSize; ++i) {
                contribution[i] = baseGroup->getRawFitness().getValues()[i] - groupWithout->getRawFitness().getValues()[i];
                sumContributions[i] += contribution[i];
            }
            ind->setGroupContribution(baseGroup, std::move(contribution));
        }
        for (size_t i = 0; i < fitnessSize; ++i) {
            sumContributions[i] = 0.5 * sumContributions[i] + 0.5 * baseGroup->getRawFitness().getValues()[i];
        }
        baseGroup->getRawFitness().setValues(sumContributions);
    }
    m_pendingGroups.clear();
    
#elif defined GROUP_EVALUATION_METHOD3
    for (auto record: m_pendingGroups) {
        Group *base = record.first;
        size_t fitnessSize = base->getPopulation().getParameters().getIndividualFitnessParametersCount();
        std::vector<double> sumIndividualFitnesses(fitnessSize, 0.0);
        for (auto indGroup: record.second) {
            for (size_t i = 0; i < fitnessSize; ++i) {
                sumIndividualFitnesses[i] += 1 + indGroup->getRawFitness().getValues()[i];
            }
            // Report the fitness to the individual
            indGroup->getIndividuals()[0]->getRawFitness() = indGroup->getRawFitness();
            delete indGroup;
        }
        // Compute the second fitness values for the group
        for (double& component: sumIndividualFitnesses) {
            if (component != 0) {
                component = 1 / component;
            } else {
                component = HUGE;
            }
        }
        /*
         * Here we alternate fitness components of the group and fitness 
         * components of the inverse-individual-fitness-sum, to reach a
         * total of twice more fitness components than what individuals have.
         */
        std::vector<double> values = base->getRawFitness().getValues();
        std::vector<double> interleaved;
        interleaved.reserve(fitnessSize * 2);
        for (size_t i = 0; i < fitnessSize; ++i) {
            interleaved.push_back(values[i]);
            interleaved.push_back(sumIndividualFitnesses[i]);
        }
        base->getRawFitness().setValues(interleaved);
    }
    m_pendingGroups.clear();

#endif
}
