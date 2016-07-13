/***********************************************************************\
|                                                                       |
| EvaluatorLuaWorker.h |
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
 * @file EvaluatorLuaWorker.h
 * Blah
 */

#ifndef HEADER_UGP3_CORE_EVALUATORLUAWORKER
#define HEADER_UGP3_CORE_EVALUATORLUAWORKER

#ifdef UGP3_USE_LUA

#include "EvaluatorLuaDispatcher.h"

#include <thread>

struct lua_State;

namespace ugp3 {
namespace core {
    
template <class T>
class EvaluatorLuaWorker
{
    EvaluatorLuaDispatcher<T>& m_dispatcher;
    
    std::thread m_thread;
    
    bool m_working;
    
    std::mutex m_mutex;
    
    unsigned int m_uniqueId;
    
    /**
     * Each worker must read and write files only in its working directory.
     * Contains the name wihtout the end slash.
     */
    std::string m_workingDirectory;
    
    lua_State* m_L;
    
    int m_environmentRef;
    void createEnvironment();
    void updateGeneration();
    
    typedef typename EvaluatorLuaDispatcher<T>::Wrapper Wrapper;
    void evaluate(Wrapper* w);
    void pushEvaluateFunction();
    void pushIndividualsTable(Wrapper* w);
    int pushFitnessTable();
    void callFunction(int nArgs, int nResults);
    void retrieveGroupFitnessValues(Wrapper* w);
    void checkStopRequest();
    
#ifdef TEST_OPERATOR_SELECTION
    void pushLineages(const Wrapper* w);
    template <class U>
    void pushLineage(const LineageWrapper<U>& lw);
#endif
    
public:
    EvaluatorLuaWorker(EvaluatorLuaDispatcher<T>& dispatcher);
    virtual ~EvaluatorLuaWorker();
    
    /**
     * Start performing evaluations, until popEvaluationRequest() returns nullptr.
     */
    void start();
    
    /**
     * @return true iff the worker is working.
     */
    bool isWorking();
};

}
}
#endif // UGP3_USE_LUA

#endif // HEADER_UGP3_CORE_EVALUATORLUAWORKER
