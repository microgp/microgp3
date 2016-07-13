/***********************************************************************\
|                                                                       |
| Evaluator.h                                                           |
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

#ifndef HEADER_UGP3_CORE_EVALUATOR
#define HEADER_UGP3_CORE_EVALUATOR


#include <string>
#include <mutex>
#include <atomic>
#include <chrono>

#include "XMLIFace.h"

// forward declaration
namespace Ui
{
	class MicroGPMainWindow;
}

// class
namespace ugp3
{

namespace core
{

class CandidateSolution;

class Evaluator: public xml::XMLIFace
{
private:
    static unsigned int workerCounter;
    
private:
    // Configuration
    std::string m_scriptFile;
    std::string m_inputFile;
    std::string m_outputFile;
    bool m_removeTemporaryFiles;
    std::size_t m_cacheSize;
    unsigned int m_concurrentEvaluations;
    // External stop request
#ifdef UGP3_USE_LUA
    std::atomic<bool> m_externalStopRequest;
#else
    bool m_externalStopRequest;
#endif
    // Measure evaluation time (from first evaluate() to end of flush())
    bool m_flushed = true;
    std::chrono::time_point<std::chrono::steady_clock> m_startNewEvaluations;
    std::chrono::milliseconds m_totalMilliSeconds;
    
    static const std::string XML_CHILDELEMENT_EVALUATORPATHNAME;
    static const std::string XML_CHILDELEMENT_EVALUATORINPUTPATHNAME; 
    static const std::string XML_CHILDELEMENT_EVALUATOROUTPUTPATHNAME;
    static const std::string XML_CHILDELEMENT_REMOVETEMPFILES;
    static const std::string XML_CHILDELEMENT_TOTALMILLISECONDS;
    static const std::string XML_CHILDELEMENT_CONCURRENTEVALUATIONS;
    static const std::string XML_CHILDELEMENT_CACHESIZE;

public:
    Evaluator();
    virtual ~Evaluator();

    /**
     * Unsynchronized functions, do not call while evaluations are in progress!
     */
    const std::string& getScriptFile() const { return m_scriptFile; }
    void setScriptFile(const std::string& fileName);
    
    const std::string& getInputFile() const { return m_inputFile; }
    void setInputFile(const std::string& fileName);
    
    const std::string& getOutputFile() const { return m_outputFile; }
    void setOutputFile(const std::string& fileName);

    bool getRemoveTemporaryFiles() const { return m_removeTemporaryFiles; }
    void setRemoveTemporaryFiles(bool value) { m_removeTemporaryFiles = value; }
    
    std::chrono::seconds getTotalTime() const {
        return std::chrono::duration_cast<std::chrono::seconds>(m_totalMilliSeconds);
    }
    void setTotalTime(std::chrono::seconds value) {
        m_totalMilliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(value);
    }
    
    /**
     * Returns the number of actual evaluations performed since the 
     * begining of this population.
     */
    virtual unsigned int getTotalEvaluations() const = 0;
    
    /**
     * Maximum size of the evaluation cache.
     * Set to zero to disable caching.
     */
    std::size_t getCacheSize() const { return m_cacheSize; }
    void setCacheSize(unsigned int size) { m_cacheSize = size; }
    
    /**
     * The number of concurrent evaluations is the number of individuals
     * that are given to every call of an external evaluator, or is the number
     * of Lua threads when using the Lua evaluator.
     */
    unsigned int getConcurrentEvaluations() const { return m_concurrentEvaluations; }
    void setConcurrentEvaluations(unsigned int value) { m_concurrentEvaluations = value; }
    
    bool getExternalStopRequest() { return m_externalStopRequest; }
    void setExternalStopRequest(bool value) { m_externalStopRequest = value; }
    
    /**
     * Request an evaluation of the given object.
     * 
     * If caching is used and the object's hash is in the cache, the fitness is not 
     * computed again.
     * 
     * @param object Object to be evaluated.
     */
    virtual void evaluate(CandidateSolution& object);
    
    /**
     * Perform all requested evaluations and return.
     */
    virtual void flush(std::function<void(double)> showProgress);
    
    /**
     * Must be called at the end of each generation. Performs cache management.
     * @param generation Number of the generation that is about to begin.
     */
    virtual void step(unsigned int nextGeneration) = 0;

    /**
     * Reset all values before reading from XML.
     */
    virtual void clear();
    
    /**
     * Display some informations about the evaluator's work.
     */
    virtual void showStatistics() const = 0;
    
    /**
     * Print statistics about the evaluator and the cache to a CSV stream.
     */
    virtual void dumpStatisticsHeader(const std::string& name, std::ostream& output) const = 0;
    virtual void dumpStatistics(std::ostream& output) const = 0;
        
public:
    /**
     * Each worker should get a unique id on creation (used only by Lua workers)
     */
    static void resetWorkerCounter(unsigned int counter) { workerCounter = counter; }
    static unsigned int getNewWorkerId() { return workerCounter++; }

public: // Xml interface
	virtual void writeXml(std::ostream& output) const;
	virtual void writeInnerXml(std::ostream& output) const {};
	virtual void readXml(const xml::Element& element);
	virtual const std::string& getXmlName() const { return XML_NAME; }
	
    static const std::string XML_NAME;
    static const std::string XML_ATTRIBUTE_TYPE;
    
    void parseEnvironmentVariables(const xml::Element& element);

public: // friend class, used for gui
	friend class Ui::MicroGPMainWindow;
};


}

}

#endif

