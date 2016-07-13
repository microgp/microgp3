/***********************************************************************\
|                                                                       |
| EvaluatorCommon.cc                                                    |
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
| $Revision: 656 $
| $Date: 2015-04-15 14:16:06 +0200 (Wed, 15 Apr 2015) $
\***********************************************************************/

/**
 * @file EvaluatorCommon.cc
 *
 */

#include "EvaluatorCommon.h"

#include "Group.h"
#include "Individual.h"
#include "EvaluatorDispatcher.h"
#include "EvaluatorLuaDispatcher.h"
#include "EvaluatorFileDispatcher.h"

#include <algorithm>
#include <unordered_set>

namespace ugp3 {
namespace core {
    
template <class T>
const std::string EvaluatorCommon<T>::XML_CHILDELEMENT_TOTALEVALUATIONS = "totalEvaluations";
template <class T>
const std::string EvaluatorCommon<T>::XML_CHILDELEMENT_CACHE = "cache";
template <class T>
const std::string EvaluatorCommon<T>::XML_CHILDELEMENT_CACHESAVED = "cacheSaved";
template <class T>
const std::string EvaluatorCommon<T>::XML_CHILDELEMENT_CACHEENTRY = "cacheEntry";
template <class T>
const std::string EvaluatorCommon<T>::XML_ATTRIBUTE_VALUE = "value";
template <class T>
const std::string EvaluatorCommon<T>::XML_ATTRIBUTE_DUPLICATE = "duplicate";
template <class T>
const std::string EvaluatorCommon<T>::XML_ATTRIBUTE_CACHE = "cache";
template <class T>
const std::string EvaluatorCommon<T>::XML_ATTRIBUTE_PHENOTYPE = "phenotype";

template <class T>
EvaluatorCommon<T>::EvaluatorCommon()
: m_dispatcher(nullptr), m_generation(0), m_cacheSaved(false)
{
}

template <class T>
EvaluatorCommon<T>::~EvaluatorCommon()
{
    if (m_dispatcher) 
    {
        delete m_dispatcher;
    }
}

template <class T>
void EvaluatorCommon<T>::clear()
{
#ifdef UGP3_USE_LUA
    std::lock_guard<std::mutex> lock(m_cacheMutex);
#endif
    
    ugp3::core::Evaluator::clear();
    
    m_actualEvaluationCount = 0;
    m_duplicateRequestCount = 0;
    m_cacheResolvedCount = 0;
    
    m_cache.clear();
    if (m_dispatcher) 
    {
        delete m_dispatcher;
        m_dispatcher = nullptr;
    }
}

template <class T>
void EvaluatorCommon<T>::evaluate(CandidateSolution& object)
{
    Evaluator::evaluate(object);
    
#ifdef UGP3_USE_LUA
    std::lock_guard<std::mutex> lock(m_cacheMutex);
#endif
    
    LOG_DEBUG << "Eval: req. for " << object << " = " << object.getNormalizedPhenotype() << std::ends;
    CacheEntry* entry = findCacheEntry(object.getNormalizedPhenotype());

    if (entry) 
    {
        bool immediate = entry->read(object, m_generation);
        if (immediate) 
	{
            LOG_DEBUG << "Eval: cache returned an entry from this generation (duplicate)." << std::ends;
            ++m_duplicateRequestCount;
        } 
	else 
	{
            LOG_DEBUG << "Eval: cache returned an entry from generation " << entry->getGenerationStored() << std::ends;
            ++m_cacheResolvedCount;
        }

    }
    else 
    {
        // The entry will be updated when the object gets evaluated.
        LOG_DEBUG << "Eval: creating cache entry." << std::ends;
        createCacheEntry(object.getNormalizedPhenotype());
        ++m_actualEvaluationCount;
        Assert(m_dispatcher);
        m_dispatcher->evaluate(static_cast<T&>(object));
    }
}

template <class T>
CacheEntry* EvaluatorCommon<T>::findCacheEntry(const std::string& code)
{
    auto it = m_cache.find(code);
    if (it != m_cache.end()) 
    {
        return &it->second;
    }
    return nullptr;
}

template <class T>
void EvaluatorCommon<T>::createCacheEntry(const std::string& code)
{
    m_cache.insert(std::make_pair(code, CacheEntry(m_generation)));
}
        
    
template <class T>
void EvaluatorCommon<T>::flush(std::function<void(double)> showProgress)
{
    Assert(m_dispatcher);
    m_dispatcher->flush(showProgress);
    
    Evaluator::flush(showProgress);
}

template <class T>
void EvaluatorCommon<T>::readXml(const xml::Element& element)
{
    ugp3::core::Evaluator::readXml(element);
    
    const xml::Element* childElement = element.FirstChildElement();
    while (childElement) 
    {
        const string& elementName = childElement->ValueStr();
        if (elementName == XML_CHILDELEMENT_TOTALEVALUATIONS) 
	{
            m_actualEvaluationCount = xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE);

            if (xml::Utility::hasAttribute(*childElement, XML_ATTRIBUTE_DUPLICATE)) 
	    {
                m_duplicateRequestCount = xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_DUPLICATE);
            }

            if (xml::Utility::hasAttribute(*childElement, XML_ATTRIBUTE_CACHE)) 
	    {
                m_cacheResolvedCount = xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_CACHE);
            }

        } 
	else if (elementName == XML_CHILDELEMENT_CACHE) 
	{
#ifdef UGP3_USE_LUA
            std::lock_guard<std::mutex> lock(m_cacheMutex);
#endif
            auto entryElement = childElement->FirstChildElement();
            while (entryElement) 
	    {
                const auto& code = xml::Utility::attributeValueToString(*entryElement, XML_ATTRIBUTE_PHENOTYPE);
                const auto& entry = m_cache.emplace(code, 0);
                entry.first->second.readXml(*entryElement);
                entryElement = entryElement->NextSiblingElement();
            }
        }
	else if (elementName == XML_CHILDELEMENT_CACHESAVED )
	{
		m_cacheSaved = xml::Utility::attributeValueToBool(*childElement, XML_ATTRIBUTE_VALUE);
	}
        childElement = childElement->NextSiblingElement();
    }
    
#ifdef UGP3_USE_LUA
    const std::string& file = getScriptFile();
    if (file.substr(file.length() - 4, file.length()) == ".lua") 
    {
        m_dispatcher = new EvaluatorLuaDispatcher<T>(*this);
    } 
    else 
    {
        m_dispatcher = new EvaluatorFileDispatcher<T>(*this);
    }
#else
    m_dispatcher = new EvaluatorFileDispatcher<T>(*this);
#endif
}

template <class T>
void EvaluatorCommon<T>::writeInnerXml(ostream& output) const
{
    output << "<" << XML_CHILDELEMENT_TOTALEVALUATIONS
    << " " << XML_ATTRIBUTE_VALUE << "='" << m_actualEvaluationCount << "'"
    << " " << XML_ATTRIBUTE_DUPLICATE << "='" << m_duplicateRequestCount << "'"
    << " " << XML_ATTRIBUTE_CACHE << "='" << m_cacheResolvedCount << "'"
    << " />" << std::endl;
    
#ifdef UGP3_USE_LUA
    std::lock_guard<std::mutex> lock(m_cacheMutex);
#endif
    // we save the cache to file ONLY if the appropriate flag is set
    if( m_cacheSaved == true )
    {
	output << "<" << XML_CHILDELEMENT_CACHE << ">" << std::endl;
	for (auto& entry: m_cache) 
	{
		output << "<" << XML_CHILDELEMENT_CACHEENTRY
		<< " " << XML_ATTRIBUTE_PHENOTYPE << "='" << xml::Utility::transformXmlEscChar(entry.first) << "'"
		<< ">" << std::endl;
		entry.second.writeInnerXml(output);
		output << "</" << XML_CHILDELEMENT_CACHEENTRY << ">" << std::endl;
	}
	output << "</" << XML_CHILDELEMENT_CACHE << ">" << std::endl;
    }
}

template <class T>
void EvaluatorCommon<T>::cacheFitness(const std::string& code, const Fitness& fitness)
{
    CacheEntry* entry = findCacheEntry(code);
    Assert(entry);
    entry->store(fitness);
}

template <class T>
void EvaluatorCommon<T>::step(unsigned int generation)
{
#ifdef UGP3_USE_LUA
    std::lock_guard<std::mutex> lock(m_cacheMutex);
#endif
    
    m_generation = generation;
    if (getCacheSize() == 0) 
    {
        m_cache.clear();
    } 
    else if (m_cache.size() > getCacheSize()) 
    {
        LOG_VERBOSE << "Resizing the evaluator cache using LRU. Current size: " 
        << m_cache.size() << ", target: " << getCacheSize() << ends;

        // Generate histogram of cache entry ages
        std::vector<std::size_t> generationHistogram(m_generation + 1, 0);
        for (auto& entry : m_cache) 
	{
            ++generationHistogram[entry.second.getGenerationLastUsed()];
        } 

        // Erase the tail of the histogram
        std::vector<std::size_t> toDeleteHistogram(m_generation + 1, 0);
        std::size_t toDelete = m_cache.size() - getCacheSize();
        for (unsigned int i = 0; i < m_generation + 1; ++i) 
	{
            std::size_t toDeleteThisGen = std::min(toDelete, generationHistogram[i]);
            toDeleteHistogram[i] = toDeleteThisGen;
            toDelete -= toDeleteThisGen;
        }

        LOG_VERBOSE << "toDelete: " << toDelete << ", toDeleteHistogram: ";
        for (size_t td: toDeleteHistogram) 
	{
            LOG_VERBOSE << td << " ";
        }
        LOG_VERBOSE << ends;

        // Apply the deletions to the map
        auto it = m_cache.begin();
        while (it != m_cache.end()) 
	{
            auto gen = it->second.getGenerationLastUsed();
            if (toDeleteHistogram[gen] > 0) 
	    {
                --toDeleteHistogram[gen];
                it = m_cache.erase(it);
            } 
	    else 
	    {
                ++it;
            }
        }
        Assert(m_cache.size() == getCacheSize());
    }
}

template <class T>
void EvaluatorCommon<T>::clearCache()
{
	this->m_cache.clear();
}

template <class T>
void EvaluatorCommon<T>::dumpStatistics(ostream& output) const
{
    output << "," << m_actualEvaluationCount;
    output << "," << m_duplicateRequestCount;
    output << "," << m_cacheResolvedCount;
}

template <class T>
void EvaluatorCommon<T>::dumpStatisticsHeader(const string& name, ostream& output) const
{
    output << "," << name << "_EvalCount";
    output << "," << name << "_DuplicateCount";
    output << "," << name << "_CacheCount";
}

template <class T>
void EvaluatorCommon<T>::showStatistics() const
{
#ifdef UGP3_USE_LUA
    std::lock_guard<std::mutex> lock(m_cacheMutex);
#endif
    
    unsigned int requestedEvaluationCount = m_actualEvaluationCount + m_duplicateRequestCount + m_cacheResolvedCount;
    LOG_INFO << "Evaluator: "
    << requestedEvaluationCount << " requests, "
    << m_actualEvaluationCount << " actually performed, "
    << m_duplicateRequestCount << " duplicates and "
    << m_cacheResolvedCount << " found in cache (currently "
    << m_cache.size() << " elements are stored in the cache)."
    << ends;
    
    LOG_INFO << "Evaluator cache: ";
    if (getCacheSize() > 0) 
    {
        LOG_INFO	<< m_cache.size() << " entries (max " << getCacheSize() << ")";
	
	if( m_cache.size() != 0 )
		LOG_INFO 	<< ", LRU from generation "
				<< std::min_element(m_cache.begin(), m_cache.end(),
                            		[] (const std::pair<std::string, CacheEntry>& a, const std::pair<std::string, CacheEntry>& b) {
                                	return a.second.getGenerationLastUsed() < b.second.getGenerationLastUsed();
                            		})->second.getGenerationLastUsed();
	LOG_INFO << ends;
    } 
    else 
    {
        LOG_INFO << "disabled (maximum size set to 0)" << ends;
    }

}

const std::string CacheEntry::XML_ELEMENT_HISTORY = "history";
const std::string CacheEntry::XML_ATTRIBUTE_GENERATIONSTORED = "generationStored";
const std::string CacheEntry::XML_ATTRIBUTE_GENERATIONREAD = "generationRead";

CacheEntry::CacheEntry(unsigned int generationStored)
: m_generationStored(generationStored), m_generationRead(generationStored)
{
    m_fitness.invalidate();
}

bool CacheEntry::read(CandidateSolution& object, unsigned int generationRead)
{
    if (m_fitness.getIsValid())
    {
        object.getRawFitness() = m_fitness;
    } 
    else 
    {
        m_waiters.push_back(&object);
    }

    if (m_generationRead == generationRead) 
    {
        return true;
    } 
    else 
    {
        m_generationRead = generationRead;
        return false;
    }

}

void CacheEntry::store(const Fitness& fitness)
{
    Assert(fitness.getIsValid());
    m_fitness = fitness;
    for (CandidateSolution* object: m_waiters) 
    {
        object->getRawFitness() = m_fitness;
    }
    m_waiters.clear();
}

void CacheEntry::readXml(const xml::Element& element)
{
    const xml::Element* childElement = element.FirstChildElement();
    while (childElement) 
    {
        const string& elementName = childElement->ValueStr();
        if (elementName == XML_ELEMENT_HISTORY) 
	{
            m_generationRead = xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_GENERATIONREAD);
            m_generationStored = xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_GENERATIONSTORED);
        } 
	else if (elementName == Fitness::XML_NAME) 
	{
            m_fitness.readXml(*childElement);
        }
        childElement = childElement->NextSiblingElement();
    }
}

void CacheEntry::writeInnerXml(ostream& output) const
{
    output << "<" << XML_ELEMENT_HISTORY
    << " " << XML_ATTRIBUTE_GENERATIONREAD << "='" << m_generationRead << "'"
    << " " << XML_ATTRIBUTE_GENERATIONSTORED << "='" << m_generationStored << "'"
    << " />" << std::endl;
    
    m_fitness.writeXml(output);
}

// Explicit instanciations
template class EvaluatorCommon<Individual>;
template class EvaluatorCommon<Group>;

}
}
