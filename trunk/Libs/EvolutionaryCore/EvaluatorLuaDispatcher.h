/***********************************************************************\
|                                                                       |
| EvaluatorLuaDispatcher.h |
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
 * @file EvaluatorLuaDispatcher.h
 * Blah
 */

#ifndef HEADER_UGP3_CORE_EVALUATORLUADISPATCHER
#define HEADER_UGP3_CORE_EVALUATORLUADISPATCHER

#ifdef UGP3_USE_LUA

#include "EvaluatorDispatcher.h"
#include "EvaluatorCommon.h"

#include "Fitness.h"
#include "Hashable.h"

#include <list>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <condition_variable>

namespace ugp3 {
namespace core {

class Individual;    

template <class T> class EvaluatorLuaWorker;

#ifdef TEST_OPERATOR_SELECTION
template <class T>
struct LineageWrapper {
    std::string m_id;
    std::string m_operatorName;
    std::vector<std::string> m_parentNames;
    std::vector<LineageWrapper> m_parents;
    
    LineageWrapper(T& object, unsigned int recursion);
};
#endif
        

template <class T>
class EvaluatorLuaDispatcher: public EvaluatorDispatcher<T>
{
public:
    struct Wrapper {
        T& m_object;
        std::string m_code;
        unsigned int m_fitnessSize; // TODO store this somewhere else, to avoid duplication
        std::vector<double> m_fitness;
        std::string m_fitnessDescription;
        std::vector<std::string> m_individualCodes;
        
#ifdef TEST_OPERATOR_SELECTION
        LineageWrapper<T> m_lineage;
        std::vector<LineageWrapper<Individual>> m_individualLineages;
#endif
        
        Wrapper(T& object);
        void writeFitnessToWrappedObject();
    };
    
private:
    /**
     * A list of active workers that perform evaluations.
     * Each worker should manage its own execution thread.
     */
    std::list<EvaluatorLuaWorker<T>*> m_workers;
    
    /**
     * Number of workers to stop to achieve the goal set
     * by setNumberOfWorkers().
     */
    unsigned int m_workersToStop;
    
    /**
     * Adjust the number of workers.
     */
    void setNumberOfWorkers(unsigned int workers);
    
private: // synchronized data
    /**
     * A set of objects to evaluate. The evaluator pushes evaluation
     * requests to this set, and evaluator workers pop the requests
     * and perform the evaluation.
     */
    std::unordered_set<Wrapper*> m_pending;
    
    /**
     * A set of objects currently being evaluated.
     * When a worker starts the evaluation, it moves the object
     * from the pending set to this one. When the evalution is over,
     * the worker removes the object from this set.
     */
    std::unordered_set<Wrapper*> m_inProgress;
    
    /**
     * The mutex that protects all of the above structures.
     */
    std::mutex m_mutex;
    
    /**
     * A condition used to signal to the workers the availability
     * of new requests.
     */
    std::condition_variable m_newCondition;
    
    /**
     * A condition used to signal that there are no more requests.
     */
    std::condition_variable m_lastCondition;
    
    /**
     * Progress
     */
    unsigned int m_requestsSinceFlush;
public:
    EvaluatorLuaDispatcher(EvaluatorCommon<T>& evaluator);
    virtual ~EvaluatorLuaDispatcher();
    
    virtual void evaluate(T& object);
    virtual void flush(std::function<void(double)>& showProgress);
    
public: // Interface for use by workers
    /**
     * Pops between one and getConcurrentEvaluations() requests from pending,
     * moves them to in progress, returns them in a vector.
     * Will wait for at least one new request to come before returning.
     * @return Wrapper* One object to evaluate or nullptr if the worker should stop.
     */
    Wrapper* popPendingRequest();
    
    /**
     * Removes the objects from the in progress set and, if caching is used,
     * inserts the computed fitnesses in the cache.
     */
    void done(Wrapper* w);
};
}
}

#endif // UGP3_USE_LUA

#endif // HEADER_UGP3_CORE_EVALUATORLUADISPATCHER
