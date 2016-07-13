/***********************************************************************\
|                                                                       |
| Statistic.h                                                           |
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
 * @file Statistics.h
 * Definition and implementation of the Statistics template class.
 */

#ifndef HEADER_UGP3_CORE_STATISTICS
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_STATISTICS

#include "DMABOperatorSelector.h"
#include "SelfAdaptiveOperatorSelector.h"
#include "Data.h"
#include "CallData.h"
#include "Population.h"
#include "Operator.h"
#include "XMLIFace.h"
#include "IString.h"
#include <cmath>
#include <bitset>

// forward declaration
namespace Ui
{
	class MicroGPMainWindow;
}

namespace ugp3 {
namespace core {
    
class PopulationParameters;

/**
 * @class Statistics
 * Keeps information about the operators performance and provides access to operator selectors.
 */
class Statistics : public xml::XMLIFace, public IString
{
private:
    // Population for which we manage operators
    const Population* population;
    
    // Default operators: "none", "all" or "smart"
    std::string defaultOperators;
    
    // Clears the vector of data.
    void clear();
    
    // Operator selector
    std::unique_ptr<OperatorSelector> selector;
    
    // Helper for updateOperatorsStatistics
    std::vector<Group*> getRandomLiveSampleFromPopulation(unsigned int size, Group* group);
    std::vector<Individual*> getRandomLiveSampleFromPopulation(unsigned int size, Individual* ind);
    
public:
    /** Name of this xml element for all operators */
    static const std::string XML_NAME;
    static const std::string XML_ATTRIBUTE_DEFAULT_OPERATORS;
    static const std::string XML_CONSTANT_NONE;
    static const std::string XML_CONSTANT_ALL;
    static const std::string XML_CONSTANT_SMART;
    static const std::string XML_ATTRIBUTE_SELECTOR;
    
public:
    /**
     * Constructor of the class. It does nothing
     */
    Statistics(const Population* population);
    /**
     * Destructor of the class. Cleans memory occupied by the object.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    virtual ~Statistics();
    
    /**
     * Should be called when a command-line option overrides the default
     * operator settings. By default we use the settings from the XML.
     * @param defaultEnabled True if we should enable all operators by default
     *                       False if -------- disable -----------------------
     */
    void forceDefaultEnabling(bool defaultEnabled);
    
    /**
     * Will add all operators that are enabled by default and not blacklisted,
     * and remove all operators that are disabled by default and were not added
     * explicitly by the user.
     */
    void applyDefaultEnabling();
    
    /**
     * Returns the Data object of the i-th Operator in the vector of data.
     * @param i Index of the Data in the vector
     * @returns Data& The Data object.
     * @throws std::exception
     */
    Data& getData(unsigned int i) const;
    
    /**
     * Returns the number of Data in the vector
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    unsigned int getDataCount() const;
    
    /**
     * Return the an operator selector based on these statistics.
     */
    OperatorSelector & getOperatorSelector();
    
    /**
     * Should be called at the end of every generation, for each kind of newly created individuals and groups.
     * Updates operator statistics.
     * This function handles dead candidates.
     * The best and worst candidates must either be nullptr or have a valid fitness.
     * 
     * @param newGeneration New children generated and evaluated during this generation.
     * @param best Best individual/group from previous generation.
     * @param worst Worst individual/group from previous generation.
     * @return void
     */
    template <class T>
    void updateOperatorsStatistics(
        std::vector<T*> everyone,
        const std::vector<T*>& newGeneration,
        const T* best,
        const T* worst
    );
    
    /**
     * Should be called at the end of each generation.
     * Performs a final update of operator statistics, and deletes call data from this generation.
     */
    void step(PopulationParameters& params);
    
    /**
     * Should be called on every epoch.
     */
    void epoch(PopulationParameters& params);
    
    const Population* getPopulation() const { return population; }
    
public: // IString interface
    virtual const std::string toString() const;
    
public: // Xml interface
    virtual void writeXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
    virtual const std::string& getXmlName() const;
            
public: // friend class, used for the gui
	friend class Ui::MicroGPMainWindow;
};

}
}

#endif

