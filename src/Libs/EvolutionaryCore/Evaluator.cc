/*********************************************************************** \
|                                                                       |
| Evaluator.cc                                                          |
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

#include "ugp3_config.h"

#include "Evaluator.h"
#include "ArgumentException.h"

using namespace std;
using namespace ugp3;
using namespace core;

unsigned int Evaluator::workerCounter;

Evaluator::Evaluator()
{
    Evaluator::clear();
}

void Evaluator::clear()
{
    Evaluator::workerCounter = 0;
    m_scriptFile = "fitness.script";
    m_inputFile = "fitness.input";
    m_outputFile = "fitness.output";
    m_removeTemporaryFiles = false;
    m_concurrentEvaluations = 1;
    m_totalMilliSeconds = chrono::milliseconds(0);
    m_externalStopRequest = false;
    m_cacheSize = 10000; // FIXME Completely arbitrary
}

Evaluator::~Evaluator()
{

}

void Evaluator::evaluate(CandidateSolution& object)
{
    if (m_flushed) 
    {
        m_flushed = false;
        m_startNewEvaluations = std::chrono::steady_clock::now();
    }
}

void Evaluator::flush(std::function<void(double)> showProgress)
{
    using namespace std::chrono;
    
    m_flushed = true;
    auto stop = steady_clock::now();
    m_totalMilliSeconds += duration_cast<milliseconds>(stop - m_startNewEvaluations);
}

/* (!)20090825
void Evaluator::setEnvironmentVariable(const string& name, const string& value)
{
    _STACK;

    if(name.empty())
    {
	throw ArgumentException("The parameter \"name\" cannot be an empty string", LOCATION);
    }

    bool found = false;
    for(unsigned int i = 0; i < this->environmentVariables.size(); i++)
    {
	if(this->environmentVariables[i].first == name)
	{
	    this->environmentVariables[i].second = value;
	    found = true;
	    break;
	}
    }

    if(found == false)
    {
	this->environmentVariables.push_back(EnvironmentVariable(name, value));
    }
}
*/

void Evaluator::setScriptFile(const string& fileName)
{
    _STACK;

    if(fileName.empty()) {
        throw ArgumentException("The parameter 'fileName' cannot be an empty string.", LOCATION);
    }
    
    m_scriptFile = fileName;
}

void Evaluator::setInputFile(const string& fileName)
{
    _STACK;
    
    if (fileName.empty()) {
        throw ArgumentException("The parameter 'fileName' cannot be an empty string.", LOCATION);
    }
    
    m_inputFile = fileName;
}

void Evaluator::setOutputFile(const string& fileName)
{
    _STACK;
    
    if (fileName.empty()) {
        throw ArgumentException("The parameter 'fileName' cannot be an empty string.", LOCATION);
    }
    
    m_outputFile = fileName;
}

