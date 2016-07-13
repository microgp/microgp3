/***********************************************************************\
|                                                                       |
| EvaluatorLuaWorker.cc |
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
 * @file EvaluatorLuaWorker.cc
 *
 */
#ifdef UGP3_USE_LUA

#include "EvaluatorLuaWorker.h"
#include "EvaluatorCommon.h"
#include "Individual.h"
#include "GEIndividual.h"
#include "Group.h"
#include "Info.h"

#include <exception>

#include <lua.hpp>

namespace ugp3 {
namespace core {
    
template <class T>
EvaluatorLuaWorker<T>::EvaluatorLuaWorker(EvaluatorLuaDispatcher< T >& dispatcher)
: m_dispatcher(dispatcher), m_working(false), m_uniqueId(Evaluator::getNewWorkerId())
{
    std::stringstream concat;
    concat << "Worker" << m_uniqueId;
    m_workingDirectory = concat.str();
    File::createDirectory(m_workingDirectory);
    
    m_L = luaL_newstate();
    if (!m_L) {
        // FIXME this happens when using Valgrind (not compatible with LuaJIT)
        // TODO throw something better
        printf("========================================================\n");
        printf("Initialization of LuaJIT failed. Are you using Valgrind?\n");
        printf("To debug ugp3 with Valgrind, rebuild it without LuaJIT:\n");
        printf("either by setting the CMake variable `UGP3_USE_LUA' to `OFF'\n");
        printf("or by linking ugp3 with a standard (non-JIT) version of Lua.\n");
        printf("========================================================\n");
        throw std::exception();
    }
    
    luaL_openlibs(m_L); /* Load Lua libraries */

    int status = luaL_loadfile(m_L, m_dispatcher.getEvaluator().getScriptFile().c_str());
    if (status) {
        /* If something went wrong, error message is at the top of */
        /* the stack */
        // TODO throw something better
        fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(m_L, -1));
        throw 1;
    }
    
    /* Create a global `ugp_environment' table with version information */
    createEnvironment();
    
    /* Evaluate the script a first time */
    int result = lua_pcall(m_L, 0, 0, 0);
    if (result) {
        // TODO throw something better
        fprintf(stderr, "Failed to run script: %s\n", lua_tostring(m_L, -1));
        exit(1);
    }
}

template <class T>
EvaluatorLuaWorker<T>::~EvaluatorLuaWorker()
{
    if (m_thread.joinable()) {
        m_thread.join();
    }
    lua_close(m_L);
}

template <class T>
void EvaluatorLuaWorker<T>::start()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_working) {
        m_working = true;
        if (m_thread.joinable()) {
            m_thread.join();
        }
        m_thread = std::thread([&] {
            while (Wrapper* w = m_dispatcher.popPendingRequest()) {
                updateGeneration();
                evaluate(w);
                m_dispatcher.done(w);
            }
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_working = false;
            }
        });
    }
}

template <class T>
bool EvaluatorLuaWorker<T>::isWorking()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_working;
}

template <class T>
void EvaluatorLuaWorker<T>::evaluate(Wrapper* w)
{
    pushEvaluateFunction();
    pushIndividualsTable(w);
#ifdef TEST_OPERATOR_SELECTION
    pushLineages(w);
    // Two parameters, the second is the lineage table
    callFunction(2, w->m_fitnessSize + 1);
#else
    callFunction(1, w->m_fitnessSize + 1);
#endif
    retrieveGroupFitnessValues(w);
    checkStopRequest();
}

template <class T>
void EvaluatorLuaWorker<T>::pushEvaluateFunction()
{
    lua_getglobal(m_L, "evaluate");
    if (!lua_isfunction(m_L, -1)) {
        lua_pop(m_L, 1);
        std::cout << "The Lua script does not contain an `evaluate' function." <<  std::endl;
        throw 1;
    }
}

template <class T>
void EvaluatorLuaWorker<T>::pushIndividualsTable(Wrapper* w)
{
    lua_newtable(m_L);

    /* Explanation from http://lua-users.org/wiki/SimpleLuaApiExample
     * 
     * To put values into the table, we first push the index, then the
     * value, and then call lua_rawset() with the index of the table in the
     * stack. Let's see why it's -3: In Lua, the value -1 always refers to
     * the top of the stack. When you create the table with lua_newtable(),
     * the table gets pushed into the top of the stack. When you push the
     * index and then the cell value, the stack looks like:
     *
     * <- [stack bottom] -- table, index, value [top]
     *
     * So the -1 will refer to the cell value, thus -3 is used to refer to
     * the table itself. Note that lua_rawset() pops the two last elements
     * of the stack, so that after it has been called, the table is at the
     * top of the stack.
     */
    for (unsigned int i = 0; i < w->m_individualCodes.size(); i++) {
        lua_pushnumber(m_L, i + 1);   /* Push the table index */
        lua_pushstring(m_L, w->m_individualCodes[i].c_str()); /* Push the value */
        lua_rawset(m_L, -3);      /* Stores the pair in the table */
    }
}

template <class T>
int EvaluatorLuaWorker<T>::pushFitnessTable()
{
    lua_newtable(m_L);
    int tableRef = luaL_ref(m_L, LUA_REGISTRYINDEX); // Pops the table and returns a reference to it
    lua_rawgeti(m_L, LUA_REGISTRYINDEX, tableRef); // Pushes back the table onto the stack
    return tableRef;
}
    
template <class T>
void EvaluatorLuaWorker<T>::callFunction(int nArgs, int nResults)
{
    int result = lua_pcall(m_L, nArgs, nResults, 0);
    if (result) {
        // TODO handle errors better, but not with ugp Log, because it is not synchronized
        std::cout << "Failed to run script: " << lua_tostring(m_L, -1) << std::endl;
        exit(1);
    }
}

template <class T>
void EvaluatorLuaWorker<T>::retrieveGroupFitnessValues(Wrapper* w)
{
    /**
     * Stack contents on return:
     * -1: optional fitness description (last returned value)
     * -2: fitness number fitnessSize
     * ...
     * -(fitnessSize + 1): fitness number 1 or "stop" (first returned value)
     */
    const unsigned int firstReturn = - (w->m_fitnessSize + 1);
    
    // Retrieve the group fitness that has been returned by the script
    for (int i = firstReturn; i < -1; ++i) {
        if (!lua_isnumber(m_L, i)) {
            // TODO throw, bad fitness value
            std::cout << "The Lua script set a wrong fitness component for the group." << std::endl;
            throw 1;
        }
        w->m_fitness.push_back(lua_tonumber(m_L, i));
    }
    if (lua_isstring(m_L, -1)) {
        w->m_fitnessDescription = lua_tostring(m_L, -1);
    }
    lua_pop(m_L, w->m_fitnessSize + 1);
}

template <class T>
void EvaluatorLuaWorker<T>::checkStopRequest()
{
    lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_environmentRef); // Pushes the table onto the stack
    lua_getfield(m_L, -1, "stop");
    if (lua_isboolean(m_L, -1)) {
        if (lua_toboolean(m_L, -1)) {
            // TODO synchronize this
            m_dispatcher.getEvaluator().setExternalStopRequest(true);
        }
    }
    lua_pop(m_L, 2);
}

template <class T>
void EvaluatorLuaWorker<T>::createEnvironment()
{
    lua_newtable(m_L);
    m_environmentRef = luaL_ref(m_L, LUA_REGISTRYINDEX); // Pops the table and returns a reference to it
    lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_environmentRef); // Pushes back the table onto the stack
    
    lua_pushstring(m_L, "ugp_version");
    lua_pushstring(m_L, ugp3::frontend::Info::getVersion().c_str());
    lua_rawset(m_L, -3);
    
    lua_pushstring(m_L, "ugp_tagline");
    lua_pushstring(m_L, ugp3::frontend::Info::getTagline().c_str());
    lua_rawset(m_L, -3);
    
    lua_pushstring(m_L, "generation");
    lua_pushnumber(m_L, -1);
    lua_rawset(m_L, -3);
    
    lua_pushstring(m_L, "tempdir");
    lua_pushstring(m_L, m_workingDirectory.c_str());
    lua_rawset(m_L, -3);
    
    lua_setglobal(m_L, "ugp_environment"); // Pops the table and sets the global variable
}

template <class T>
void EvaluatorLuaWorker<T>::updateGeneration()
{
    lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_environmentRef); // Pushes the table onto the stack
    lua_pushstring(m_L, "generation");
    lua_pushnumber(m_L, m_dispatcher.getEvaluator().getCurrentGeneration());
    lua_rawset(m_L, -3);
    lua_pop(m_L, 1);
}

#ifdef TEST_OPERATOR_SELECTION
template <class T>
template <class U>
void EvaluatorLuaWorker<T>::pushLineage(const LineageWrapper<U>& lw)
{
    luaL_checkstack(m_L, 10, "lua stack overflow");
    
    lua_newtable(m_L);
    
    lua_pushstring(m_L, "id");
    lua_pushstring(m_L, lw.m_id.c_str());
    lua_rawset(m_L, -3);
    
    lua_pushstring(m_L, "operatorName");
    lua_pushstring(m_L, lw.m_operatorName.c_str());
    lua_rawset(m_L, -3);
    
    lua_pushstring(m_L, "parentNames");
    lua_newtable(m_L);
    for (unsigned int i = 0; i < lw.m_parentNames.size(); ++i) {
        lua_pushnumber(m_L, i + 1);
        lua_pushstring(m_L, lw.m_parentNames[i].c_str());
        lua_rawset(m_L, -3);
    }
    lua_rawset(m_L, -3);
    
    lua_pushstring(m_L, "parents");
    lua_newtable(m_L);
    for (unsigned int i = 0; i < lw.m_parents.size(); ++i) {
        lua_pushnumber(m_L, i + 1);
        pushLineage(lw.m_parents[i]);
        lua_rawset(m_L, -3);
    }
    lua_rawset(m_L, -3);
}

template <>
void EvaluatorLuaWorker<Individual>::pushLineages(const Wrapper* w)
{
    lua_newtable(m_L);
    lua_pushnumber(m_L, 1);
    pushLineage(w->m_lineage);
    lua_rawset(m_L, -3);
}

template <>
void EvaluatorLuaWorker<Group>::pushLineages(const Wrapper* w)
{
    lua_newtable(m_L);
    
    for (unsigned int i = 0; i < w->m_individualLineages.size(); ++i) {
        lua_pushnumber(m_L, i + 1);
        pushLineage(w->m_individualLineages[i]);
        lua_rawset(m_L, -3);
    }
    
    lua_pushstring(m_L, "group");
    pushLineage(w->m_lineage);
    lua_rawset(m_L, -3);
}
#endif

template class EvaluatorLuaWorker<Group>;
template class EvaluatorLuaWorker<Individual>;

}
}

// UGP3_USE_LUA
#endif
