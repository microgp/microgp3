/***********************************************************************\
|                                                                       |
| EvaluatorFileDispatcher.h |
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
 * @file EvaluatorFileDispatcher.h
 * Blah
 */

#ifndef HEADER_UGP3_CORE_EVALUATORFILEDISPATCHER
#define HEADER_UGP3_CORE_EVALUATORFILEDISPATCHER

#include <EvaluatorDispatcher.h>

#include <queue>
#include <string>

namespace ugp3 {
namespace core {

template <class T>
class EvaluatorFileDispatcher : public EvaluatorDispatcher<T>
{
protected:
    std::queue<T*> m_pendingEvaluations;
    
    void runScript(const std::vector<T*>& objects);
    void retrieveEvaluations(const std::vector<T*>& objects);
    
    void setEnvironmentVariable(const std::string& name, const std::string& value);
    
    /**
     * Progress
     */
    unsigned int m_requestsSinceFlush;
    
public:
    EvaluatorFileDispatcher(EvaluatorCommon< T >& evaluator);
    virtual ~EvaluatorFileDispatcher();
    
    virtual void evaluate(T& object);
    virtual void flush(std::function<void(double)>& showProgress);
};

}
}

#endif // HEADER_UGP3_CORE_EVALUATORFILEDISPATCHER
