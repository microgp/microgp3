/***********************************************************************\
|                                                                       |
| Statistic.cc                                                          |
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
 * @file Statistics.cc
 * Implementation and instanciations of the Statistics template class.
 */

// headers from this module
#include "Statistics.h"

#include "Performance.h"
#include "GeneticOperator.h"
#include "GroupOperator.h"
#include "PopulationParameters.h"
#include "IndividualPopulation.h"
#include "GroupPopulation.h"
#include "Individual.h"
#include "Group.h"
#include "EvolutionaryAlgorithm.h"
#include <Debug.h>
#include <Convert.h>
#include <ArgumentNullException.h>
#include <SchemaException.h>
#include <Random.h>
#include <Log.h>

#include "KappaRouletteOperatorSelector.h"
#include "make_unique.h"

/**
 * ugp3 namespace
 */
namespace ugp3
{
    
/**
 * ugp3::core namespace
 */
namespace core
{

//Implementation of the template //////////////////////////////////////////////
//In C++ the implementation of a template class has to be in the same file.

/**
 * @def MY_PATCH_WEIGHT
 * If defined, the sum of the activation weights in the Statistics::getRandomElement is showed in LOG_VERBOSE
 */
#define MY_PATCH_WEIGHT

using namespace std;

Statistics::Statistics(const Population* population)
:population(population)
{
    selector = make_unique<KappaRouletteOperatorSelector>(*this);
    clear();
}
void Statistics::clear()
{
    defaultOperators = XML_CONSTANT_SMART;
}

const std::string Statistics::toString() const
{
	unsigned int histogram[5];
	unsigned int failures = 0;
	for(unsigned int i = 0; i < 5; i++)
	{
		histogram[i] = 0;
	}

	for(unsigned int i = 0; i < getDataCount(); i++)
	{
        Data& data = getData(i);
		histogram[0] += data.getPerformance(Performance::VeryBad);
		histogram[1] += data.getPerformance(Performance::Bad);
		histogram[2] += data.getPerformance(Performance::Normal);
		histogram[3] += data.getPerformance(Performance::Good);
		histogram[4] += data.getPerformance(Performance::VeryGood);
		failures += data.getFailedCalls();
	}

	return
		"VBad=" + Convert::toString(histogram[0]) + "; " +
		"Bad=" + Convert::toString(histogram[1]) + "; " +
		"Norm=" + Convert::toString(histogram[2]) + "; " +
		"Good=" + Convert::toString(histogram[3]) + "; " +
		"VGood=" + Convert::toString(histogram[4]) + "; " +
		"FAIL=" + Convert::toString(failures);
}

Statistics::~Statistics()
{
	_STACK;
}

OperatorSelector& Statistics::getOperatorSelector()
{
    return *selector;
}

void Statistics::epoch(PopulationParameters& params)
{
    _STACK;
    
    for(unsigned int i = 0; i < getDataCount(); i++)
    {
        Data& data = getData(i);
        data.resetFailedCalls();
        data.cleanPerformanceData();
    }
    
    selector->epoch(params);
}

void Statistics::step(PopulationParameters& params)
{
    _STACK;
    
    for (unsigned int i = 0; i < getDataCount(); i++)
    {
        // get the statistics for the i-th operator
        Data& data = getData(i);
        for (unsigned int i = 0; i < data.getCallData().size(); ++i) {
            CallData & cd = *data.getCallData()[i];
            if (cd.getValidChildrenCount() == 0) {
                data.incrementFailedCalls();
            } else {
                for (unsigned int j = 0; j < Performance::PerformanceLevelCount; ++j) {
                    auto& perf = *Performance::values[j];
                    data.incrementPerformance(perf, cd.getPerformance(perf));
                }
            }
        }
    }
    
    /*
     * Allow operator selectors to update their internal state using
     * the CallData gathered during this generation.
     */
    selector->step(params);
    
    for (unsigned int i = 0; i < getDataCount(); ++i) {
        getData(i).deleteCallData();
    }
}

std::vector< Individual* > Statistics::getRandomLiveSampleFromPopulation(unsigned int size, Individual* ind)
{
    // NOTE selective pressure of 0 to get random individuals
    return ind->getPopulation().getParameters().individualSelector(size, 0.0);
}

std::vector< Group* > Statistics::getRandomLiveSampleFromPopulation(unsigned int size, Group* group)
{
    return group->getPopulation().getParameters().groupSelection(size, 0.0);
}


template <class T>
void Statistics::updateOperatorsStatistics(
    std::vector<T *> everyone,
    const std::vector<T *>& newGeneration,
    const T* best,
    const T* worst)
{
    _STACK;
    if (best && (!best->getFitness().getIsValid() || !best->getRawFitness().getIsValid()))
        best = nullptr;
    if (worst && (!worst->getFitness().getIsValid() || !worst->getRawFitness().getIsValid()))
        worst = nullptr;
    
    Assert(!best || best->getFitness().getIsValid());
    Assert(!worst || worst->getFitness().getIsValid());
    
    // Step one: from the fitness to the histogram
    // (Genetic.c, ugp2.7, line 1003).
    // For each child, compare it with its parents and update the
    // performance statistics of the operator that generated the child
    for (auto child: newGeneration) {
        const Population& population = child->getPopulation();
        const std::string operatorName = child->getLineage().getOperatorName();
        CallData* callData = child->getLineage().getCallData();
        
        // Dead case: the child may have been killed during evaluation
        if (child->isDead()) {
            LOG_DEBUG << "The " << child->getDisplayTypeName() << " child " << child 
            << " was killed during evaluation." << std::ends;
            continue;
        }
        
        /*
         * This should not happen anymore: individuals are always produced by a genetic
         * operator and groups are either produced by group operators or by genetic
         * operators via generateSonsGroups().
         */
        Assert(callData);
        if (!callData) {
            LOG_WARNING << "The " << child->getDisplayTypeName() << " child " << child 
            << " with operator (" << operatorName << ") has no statistics!" << std::ends;
            continue;
        }
    
        if (best && population.compareOperatorPerformance(child, best))
        {
            callData->setChildPerformance(Performance::VeryGood);
            LOG_VERBOSE << "Woah! " << child->getDisplayTypeName() << " " << child << " is better than " << best << std::ends;
        }
        else if (worst && !population.compareOperatorPerformance(child, worst))
        {
            callData->setChildPerformance(Performance::VeryBad);
            LOG_VERBOSE << "Yeuch! " << child->getDisplayTypeName() << " " << child << " is worse than " << worst << std::ends;
        }
        else
        {
            unsigned int numberOfParents = 0;
            unsigned int improvement = 0;
            for (auto parent: child->getLineage().getParents()) {
                if (!parent->isDead()) {
                    ++numberOfParents;
                    if (population.compareOperatorPerformance(child, parent)) {
                        ++improvement;
                    }
                }
            }
            if (numberOfParents == 0) {
                // Evaluate against a random sample of the population
                std::vector<T*> randomSample = getRandomLiveSampleFromPopulation(5, child);
                for (auto candidate: randomSample) {
                    ++numberOfParents;
                    if (population.compareOperatorPerformance(child, candidate)) {
                        ++improvement;
                    }
                }
            }
            if (improvement == numberOfParents) {
                // it's better than all its parents
                callData->setChildPerformance(Performance::Good);
            } else if (improvement == 0) {
                // it's worse than all its parents
                callData->setChildPerformance(Performance::Bad);
            } else {
                callData->setChildPerformance(Performance::Normal);
            }
        }
    }
    
    /*
     * To get a finer measure of operator performance, we compare this child
     * against the whole old and new generations, and we reward the operator
     * according to the rank of its offspring.
     */
    if (!newGeneration.empty()) {
        auto& population = newGeneration.front()->getPopulation();
        // Sort everyone
        everyone.erase(std::remove_if(begin(everyone), end(everyone),
            [] (const T* a) { return a->isDead(); }), end(everyone));
        std::sort(begin(everyone), end(everyone), [&] (const T* a, const T* b) {
            return population.compareOperatorPerformance(a, b);
        });
        // Determine the equivalence classes (= ranks) for all new live candidates
        std::unordered_map<T*, unsigned int> classes;
        for (T* c: newGeneration) {
            if (!c->isDead()) {
                classes[c] = 0;
            }
        }
        unsigned int nbClasses = 1;
        T* classRep = everyone.front();
        for (T* c: everyone) {
            if (population.compareOperatorPerformance(classRep, c)) {
                // Next equivalence class
                classRep = c;
                ++nbClasses;
            }
            auto it = classes.find(c);
            if (it != end(classes)) {
                it->second = nbClasses;
            }
        }
        if (nbClasses > 1) {
            // Distribute rewards based on rank
            for (auto& it: classes) {
                T* c = it.first;
                CallData* callData = c->getLineage().getCallData();
                callData->addChildReward((double)(nbClasses - it.second) / (double)(nbClasses - 1));
            }
        }
    }
}

// Explicit instanciations
template
void Statistics::updateOperatorsStatistics<Group>(
    std::vector<Group *> everyone,
    const std::vector<Group *>& newGeneration,
    const Group* best,
    const Group* worst);
template
void Statistics::updateOperatorsStatistics<Individual>(
    std::vector<Individual *> everyone,
    const std::vector<Individual *>& newGeneration,
    const Individual* best,
    const Individual* worst);

void Statistics::forceDefaultEnabling(bool defaultEnabled)
{
    if (defaultEnabled) {
        defaultOperators = XML_CONSTANT_ALL;
    } else {
        defaultOperators = XML_CONSTANT_NONE;
    }
    applyDefaultEnabling();
}

void Statistics::applyDefaultEnabling()
{
    // Compute the default enabling rules
    std::bitset<Operator::CATEGORY_COUNT> enabledOperatorCategories;
    if (defaultOperators == XML_CONSTANT_NONE) {
        enabledOperatorCategories.reset(); // All false
    } else if (defaultOperators == XML_CONSTANT_SMART) {
        enabledOperatorCategories.set(Operator::DEFAULT_ON);
    } else if (defaultOperators == XML_CONSTANT_ALL) {
        enabledOperatorCategories.set(); // All to true
    }
    
    // Categorize and clean current operators
    // and disable all operators that where enabled implicitly and are now disabled by default
    std::unordered_set<std::string> blacklist, enabled;
    std::vector<std::string> toDelete;
    for (unsigned int i = 0; i < getDataCount(); ++i) {
        Data& d = getData(i);
        
        if (d.getExplicitEnabled()) {
            if (d.getEnabled()) {
                enabled.insert(d.getOperatorName());
            } else {
                blacklist.insert(d.getOperatorName());
            }
        } else {
            if (d.getEnabled()) {
                if (enabledOperatorCategories.test(d.getOperator()->getCategory())) {
                    enabled.insert(d.getOperatorName());
                } else {
                    // An operator was implicitly enabled and should now be disabled
                    toDelete.push_back(d.getOperatorName());
                }
            } else {
                // An operator that is disabled and not blacklisted should not be stored
                toDelete.push_back(d.getOperatorName());
            }
        }
    }
    
    for (const std::string& name: toDelete) {
        selector->removeData(name);
    }
    
    // Add all operators that are not yet enabled, not blacklisted and enabled by default
    for (unsigned int i = 0; i < Operator::getOperatorCount(); ++i) {
        Operator* op = Operator::getOperator(i);
        if (blacklist.find(op->getName()) == blacklist.end()
            && enabled.find(op->getName()) == enabled.end()
            && op->isApplicable(population)
            && enabledOperatorCategories.test(op->getCategory())) {
            LOG_VERBOSE << "Operator " << op->getName() << " enabled by default." << std::ends;
            selector->createData(*op);
            enabled.insert(op->getName());
        }
    }
    
    if (enabled.empty()) {
        throw Exception("No enabled operators.", LOCATION);
    }
}

Data& Statistics::getData(unsigned int i) const
{
    return selector->getData(i);
}

unsigned int Statistics::getDataCount() const
{
    return selector->getDataCount();
}

// Xml related implementation //////////

const std::string Statistics::XML_NAME = "operators";
const std::string Statistics::XML_ATTRIBUTE_DEFAULT_OPERATORS = "default";
const std::string Statistics::XML_CONSTANT_ALL = "all";
const std::string Statistics::XML_CONSTANT_NONE = "none";
const std::string Statistics::XML_CONSTANT_SMART = "smart";
const std::string Statistics::XML_ATTRIBUTE_SELECTOR = "selector";

void Statistics::readXml(const xml::Element& element)
{
    _STACK;

    this->clear();

    LOG_DEBUG << "Population Operators: parsing ..." << ends;

    // get the name of the element
    string elementName = element.ValueStr();
    if(elementName != this->getXmlName())
    {
        throw xml::SchemaException("expected element './" + this->getXmlName() + "', found '" + elementName +  "'", LOCATION);
    };
    
    /*
     * Instanciate the right operator selector
     */
    if (xml::Utility::hasAttribute(element, XML_ATTRIBUTE_SELECTOR)) {
        const std::string& name = xml::Utility::attributeValueToString(
            element, XML_ATTRIBUTE_SELECTOR);
        auto maybe_selector = OperatorSelector::instanciate(name, *this);
        if (!maybe_selector) {
            LOG_WARNING << "Operator selection policy \"" << name
            << "\" not found. Using \"" << selector->getXmlName() << "\" instead."
            << std::ends;
        } else {
            selector = std::move(maybe_selector);
            LOG_VERBOSE << "Using operator selector \"" << selector->getXmlName()
            << "\"." << std::ends;
            selector->readXml(element);
        }
    }

    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        elementName = childElement->ValueStr();
        if(elementName == Data::XML_NAME)
        {
            unique_ptr<Data> d = selector->instanciateData();

            d->readXml(*childElement);
            
            if (d->getOperator()) {
                if (!d->getOperator()->isApplicable(population)) {
                    LOG_WARNING << "Operator " << d->getOperatorName() << " has been disabled "
                    "because it is not applicable to the current type of population." << std::ends;
                } else {
                    if (d->getEnabled() || d->getExplicitEnabled()) {
                        bool alreadyThere = false;
                        if (selector->getData(d->getOperatorName())) {
                            LOG_ERROR << "Operator " << d->getOperatorName()
                            << " appears more than once in your configuration file." << std::ends;
                            alreadyThere = true;
                            break;
                        }
                        if (!alreadyThere) {
                            selector->addData(std::move(d));
                        }
                    }
                }
            }
        }
        
        childElement = childElement->NextSiblingElement();
    }
    
    /*
     * Get preferences for default operator activations
     */
    if (xml::Utility::hasAttribute(element, XML_ATTRIBUTE_DEFAULT_OPERATORS)) {
        defaultOperators = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_DEFAULT_OPERATORS);
        if (defaultOperators != XML_CONSTANT_SMART
            && defaultOperators != XML_CONSTANT_NONE
            && defaultOperators != XML_CONSTANT_ALL) {
            throw Exception("The only acceptable values for attribute \""
            + XML_ATTRIBUTE_DEFAULT_OPERATORS + "\" are \""
            + XML_CONSTANT_ALL + "\", \"" 
            + XML_CONSTANT_NONE + "\" or \"" 
            + XML_CONSTANT_SMART + "\". Found \"" 
            + defaultOperators + "\".", LOCATION);
            }
    }
    applyDefaultEnabling();
}

void Statistics::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object ugp3::core::Statistics" << ends;

    output << "<" << this->getXmlName()
    << " " << XML_ATTRIBUTE_DEFAULT_OPERATORS << "=\"" << defaultOperators << "\""
    << " " << XML_ATTRIBUTE_SELECTOR << "=\"" << selector->getXmlName() << "\""
    << ">" << endl;
    
    selector->writeInnerXml(output);

    for (unsigned int i = 0; i < getDataCount(); i++) {
        getData(i).writeXml(output);
    }

    output << "</" << this->getXmlName() << ">" << endl;
}

const std::string& Statistics::getXmlName() const
{
    return XML_NAME;
}

}
}
