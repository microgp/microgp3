/***********************************************************************\
|                                                                       |
| EvaluatorCommon.h                                                     |
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
| $Revision: 654 $
| $Date: 2015-04-14 15:39:32 +0200 (Tue, 14 Apr 2015) $
\***********************************************************************/

/**
 * @file EvaluatorCommon.h
 * Class that provide evaluation caching and work queueing and dispatching using a thread pool.
 * This file also contains the support class "CacheEntry"
 */

#ifndef HEADER_UGP3_CORE_EVALUATORCOMMON
#define HEADER_UGP3_CORE_EVALUATORCOMMON

#include "Evaluator.h"

#include "Fitness.h"
#include "Hashable.h"
#include <unordered_map>
#include <mutex>

namespace ugp3 {
namespace core {
    
class CacheEntry
{
private:
    Fitness m_fitness;
    unsigned int m_generationStored;
    unsigned int m_generationRead;
    std::vector<CandidateSolution*> m_waiters;
    
    static const std::string XML_ELEMENT_HISTORY;
    static const std::string XML_ATTRIBUTE_GENERATIONSTORED;
    static const std::string XML_ATTRIBUTE_GENERATIONREAD;
    
public:
    /**
     * Create a cache entry with an invalid fitness, that will accept
     * waiting objects with the same hash. When the original object is
     * evaluated, the waiters will be updated with the new fitness.
     * The given code is used to match objects to this cache entry.
     */
    CacheEntry(unsigned int generationStored);
    
    /**
     * If the fitness is valid, update the object with the current fitness.
     * Else, add the object to the "waiters" and it will be updated when
     * the store() function is called.
     * @return True iff the cache entry has been accessed on this same generation.
     */
    bool read(CandidateSolution& object, unsigned int generationRead);
    
    /**
     * Update the cache entry with the given fitness.
     */
    void store(const Fitness& fitness);
    
    /**
     * Return the generation number used for LRU.
     */
    unsigned int getGenerationLastUsed() const { return m_generationRead; }
    
    /**
     * Return the generation at which the entry was created.
     */
    unsigned int getGenerationStored() const { return m_generationStored; }
    
    /**
     * Serialize the cache entry
     */
    void readXml(const xml::Element& element);
    void writeInnerXml(std::ostream& output) const;
};
    
template <class T> class EvaluatorDispatcher;

template <class T>
class EvaluatorCommon : public Evaluator
{
private:
    static const std::string XML_CHILDELEMENT_TOTALEVALUATIONS;
    static const std::string XML_CHILDELEMENT_CACHESAVED;
    static const std::string XML_CHILDELEMENT_CACHE;
    static const std::string XML_CHILDELEMENT_CACHEENTRY;
    static const std::string XML_ATTRIBUTE_VALUE;
    static const std::string XML_ATTRIBUTE_DUPLICATE;
    static const std::string XML_ATTRIBUTE_CACHE;
    static const std::string XML_ATTRIBUTE_PHENOTYPE;
    
    /**
     * Evaluation cache.
     */
    std::unordered_map<std::string, CacheEntry> m_cache;
    
#ifdef UGP3_USE_LUA
    /**
     * Mutex to protect cache access and synchronize workers with the main thread.
     */
    mutable std::mutex m_cacheMutex;
#endif
    
    /**
     * The class that will actually perform evaluations.
     */
    EvaluatorDispatcher<T>* m_dispatcher;
    
    /**
     * Current generation as reported to us using step();
     */
    unsigned int m_generation;
    
    /**
     * Statistics for the current generation.
     * TODO erase after each generation?
     * Each evaluation request falls into one of these three categories.
     */
    unsigned int m_actualEvaluationCount = 0;
    unsigned int m_duplicateRequestCount = 0;
    unsigned int m_cacheResolvedCount = 0;

    /** An internal flag, that is used to decide whether the cache will be saved
    */
    bool m_cacheSaved;
    
    /**
     * Finds the entry of any clone of the given object.
     * Before calling this function, the class must own the cache mutex.
     * @return The entry of a clone of nullptr.
     */
    CacheEntry* findCacheEntry(const std::string& normalizedPhenotype);
    
    /**
     * Creates a new cache entry for the given candidate solution.
     * Before calling this function, the class must own the cache mutex.
     */
    void createCacheEntry(const std::string& normalizedPhenotype);
    
    
public:
    /**
     * Create the evaluator.
     */
    EvaluatorCommon();
    virtual ~EvaluatorCommon();
    
    virtual void evaluate(CandidateSolution& object);
    virtual void flush(std::function<void(double)> showProgress);
    virtual void step(unsigned int generation);
    virtual void clear();
    virtual unsigned int getTotalEvaluations() const { return m_actualEvaluationCount; }
    virtual void showStatistics() const;
    virtual void dumpStatistics(std::ostream& output) const;
    virtual void dumpStatisticsHeader(const std::string& name, std::ostream& output) const;
    
public: // API for dispatchers
    /**
     * @return the current generation.
     */
    unsigned int getCurrentGeneration() const { return m_generation; }
    
    /**
     * Put the current fitness of the given objet in the cache.
     * 
     * In the concurrent case, this function expects that the cache
     * mutex (returned by the following function) is already locked 
     * by the caller.
     */
    void cacheFitness(const std::string& code, const Fitness& fitness);

    /**
	Clears all values currently in the cache.
    */
    void clearCache();
    
#ifdef UGP3_USE_LUA
    std::mutex& getCacheMutex() const { return m_cacheMutex; }
#endif
    
public:
    /**
     * Used to initialize the dispatcher after we read its configuration.
     */
    virtual void readXml(const xml::Element& element);
    virtual void writeInnerXml(std::ostream& output) const;
};

}
}

#endif // HEADER_UGP3_CORE_EVALUATORCOMMON
