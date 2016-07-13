/***********************************************************************\
|                                                                       |
| EvaluatorLuaDispatcher.cc |
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
 * @file EvaluatorLuaDispatcher.cc
 *
 */

#ifdef UGP3_USE_LUA

#include "EvaluatorLuaDispatcher.h"
#include "EvaluatorLuaWorker.h"
#include "EvaluatorCommon.h"
#include "Group.h"
#include "GEIndividual.h"
#include "GroupPopulation.h"
#include "GroupPopulationParameters.h"

namespace ugp3 {
namespace core {
    
template <class T>
EvaluatorLuaDispatcher<T>::EvaluatorLuaDispatcher(EvaluatorCommon< T >& evaluator)
: EvaluatorDispatcher<T>(evaluator), m_workersToStop(0), m_requestsSinceFlush(0)
{
    setNumberOfWorkers(evaluator.getConcurrentEvaluations());
}

template <class T>
EvaluatorLuaDispatcher<T>::~EvaluatorLuaDispatcher()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_workersToStop = m_workers.size();
        m_newCondition.notify_one();
    }
    for (auto worker: m_workers) {
        delete worker;
    }
    for (auto w: m_inProgress) {
        delete w;
    }
    for (auto w: m_pending) {
        delete w;
    }
}


template <class T>
void EvaluatorLuaDispatcher<T>::setNumberOfWorkers(unsigned int workers)
{
    lock_guard<std::mutex> lock(m_mutex);
    
    unsigned int currentWorkers = 0;
    for (auto worker: m_workers) {
        if (worker->isWorking()) {
            currentWorkers++;
        }
    }
    
    if (currentWorkers > workers) {
        m_workersToStop = currentWorkers - workers;
        m_newCondition.notify_one();
        return;
    }
    // else: new workers are needed
    
    // Restart already created workers
    auto it = m_workers.begin();
    while (it != m_workers.end() && currentWorkers < workers) {
        if (!(*it)->isWorking()) {
            (*it)->start();
            currentWorkers++;
        }
    }
    // Create new workers and start them
    for (; currentWorkers < workers; ++currentWorkers) {
        EvaluatorLuaWorker<T>* newGuy = new EvaluatorLuaWorker<T>(*this);
        newGuy->start();
        m_workers.push_front(newGuy);
    }
}
    
template <class T>
void EvaluatorLuaDispatcher<T>::evaluate(T& object)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_requestsSinceFlush++;
    
    m_pending.insert(new EvaluatorLuaDispatcher<T>::Wrapper(object));
    m_newCondition.notify_one();
}

template <class T>
void EvaluatorLuaDispatcher<T>::done(EvaluatorLuaDispatcher<T>::Wrapper* w)
{
    /* 
     * Get the cache mutex first to avoid deadlocks (since most of the time
     * the evaluator locks the cache first and then calls a synchronized
     * method of the dispatcher).
     */
    std::lock_guard<std::mutex> lock(EvaluatorDispatcher<T>::getEvaluator().getCacheMutex());
    std::lock_guard<std::mutex> lock2(m_mutex);
    
    m_inProgress.erase(w);
    w->writeFitnessToWrappedObject();
    EvaluatorDispatcher<T>::getEvaluator().cacheFitness(w->m_code, w->m_object.getRawFitness());
    delete w;
    
    if (m_pending.size() == 0 && m_inProgress.size() == 0) {
        m_lastCondition.notify_one();
    }
}

template <class T>
void EvaluatorLuaDispatcher<T>::flush(std::function<void(double)>& showProgress)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    
    // Wait until both sets are empty
    while (!m_lastCondition.wait_for(lock, std::chrono::milliseconds(1000), [&] {
        return m_pending.size() == 0 && m_inProgress.size() == 0;
    })) {
        showProgress((double)(m_requestsSinceFlush - m_pending.size() - m_inProgress.size()) / m_requestsSinceFlush);
    }
    showProgress(1);
    m_requestsSinceFlush = 0;
}

template <class T>
typename EvaluatorLuaDispatcher<T>::Wrapper* EvaluatorLuaDispatcher<T>::popPendingRequest()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    
    m_newCondition.wait(lock, [&] {
        return m_pending.size() > 0 || m_workersToStop > 0;
    });
    
    if (m_workersToStop > 0) {
        --m_workersToStop;
        m_newCondition.notify_one();
        return nullptr;
    }
    
    EvaluatorLuaDispatcher<T>::Wrapper* w = *m_pending.begin();
    m_pending.erase(m_pending.begin());
    m_inProgress.insert(w);
    
    return w;
}

template <>
EvaluatorLuaDispatcher<Group>::Wrapper::Wrapper(Group& group)
: m_object(group)
, m_code(group.getNormalizedPhenotype())
#ifdef TEST_OPERATOR_SELECTION
, m_lineage(group, LINEAGE_RECURSION_DEPTH)
#endif
{
    m_fitnessSize = group.getPopulation().getParameters().getIndividualFitnessParametersCount();
    for (auto ind: group.getIndividuals()) {
        m_individualCodes.push_back(ind->getExternalRepresentation());
#ifdef TEST_OPERATOR_SELECTION
        m_individualLineages.emplace_back(*ind, LINEAGE_RECURSION_DEPTH);
#endif
    }
}
    
template <>
EvaluatorLuaDispatcher<Individual>::Wrapper::Wrapper(Individual& ind)
: m_object(ind)
, m_code(ind.getNormalizedPhenotype())
#ifdef TEST_OPERATOR_SELECTION
, m_lineage(ind, LINEAGE_RECURSION_DEPTH)
#endif
{
    m_fitnessSize = ind.getPopulation().getParameters().getFitnessParametersCount();
    m_individualCodes.push_back(ind.getExternalRepresentation());
}
        
template <class T>
void EvaluatorLuaDispatcher<T>::Wrapper::writeFitnessToWrappedObject()
{
    m_object.getRawFitness().setValues(m_fitness);
    m_object.getRawFitness().setDescription(m_fitnessDescription);
}

#ifdef TEST_OPERATOR_SELECTION
template <class T>
LineageWrapper<T>::LineageWrapper(T& object, unsigned int recursion)
{
    m_id = object.getId();
    m_operatorName = object.getLineage().getOperatorName();
    m_parentNames = object.getLineage().getParentNames();
    if (recursion) {
        for (auto parent: object.getLineage().getParents()) {
            m_parents.emplace_back(*parent, recursion - 1);
        }
    }
}
#endif

template class EvaluatorLuaDispatcher<Group>;
template class EvaluatorLuaDispatcher<Individual>;

}
}

// UGP3_USE_LUA
#endif