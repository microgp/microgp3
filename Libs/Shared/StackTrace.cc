/***********************************************************************\
|                                                                       |
| StackTrace.cc                                                         |
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
 * @file StackTrace.cc
 * Implementation of the StackTrace class.
 * @see StackTrace.h
 */

#include "ugp3_config.h"
#include <sstream>
#include <stack>

#include "StackTrace.h"
#include "Log.h"
using namespace std;

// Libs/Shared/StackTrace.cc
#define REAL_PATH_LEN 25

#ifdef USE_STACK_TRACE
namespace ugp3
{
    stack<string> StackTrace::stackTrace;

    StackTrace::StackTrace() { }

    StackTrace::~StackTrace()
    {
        StackTrace::stackTrace.pop();
    }

    StackTrace::StackTrace(const char* fileName, const char* methodName, int line)
    {
        // TODO use constexpr to do this concatenation at compile time
        const string& trace = string(methodName) + "@" + &fileName[strlen(UGP3_BASE_DIR)-REAL_PATH_LEN];

        // save the method name on the top of the stack
        StackTrace::stackTrace.push(trace);
    }

    const string StackTrace::getTrace()
    {
        // get a copy of the stack
        stack<string> sTrace = StackTrace::stackTrace;

        // print the calls stack on a stream
        ostringstream trace;
        trace << "\nSTACK TRACE:" << endl;
        while(sTrace.empty() == false)
        {
            trace << "    " << sTrace.top() << endl;
            sTrace.pop();
        }

        return trace.str();
    }
}
#endif
