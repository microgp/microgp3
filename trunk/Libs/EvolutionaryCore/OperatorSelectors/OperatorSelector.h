/***********************************************************************\
|                                                                       |
| OperatorSelector.h |
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
 * @file OperatorSelector.h
 * Base class (interface) for operator selectors.
 */

#ifndef HEADER_UGP3_CORE_OPERATORSELECTOR
#define HEADER_UGP3_CORE_OPERATORSELECTOR

#include <limits>
#include <string>
#include <memory>
#include <algorithm>
#include "Operator.h"
#include <Debug.h>

namespace ugp3 {
namespace core {

class PopulationParameters;
class Operator;
class Data;
class Statistics;

/**
 * Base class for operator selectors. Handles failures and pseudo-deactivation.
 */
class OperatorSelector
{
public:
    /**
     * Result of the select operation.
     */
    struct Result {
        Data * data = nullptr;
        Operator * op = nullptr;
        double score = - std::numeric_limits<double>::infinity();
        
        Result() {}
        Result(Data* d, Operator* o, double s) : data(d), op(o), score(s) {}
    };
    
protected:
    // The statistics object which will be used to perform the selection
    Statistics& m_stats;
    
    // A new operator got out of pseudo-deactivation this generation
    bool m_newOperatorActivated = false;
    
    // Custom selection strategy
    virtual void prepareForSelectionsImpl() = 0;
    virtual Result selectImpl() = 0;
    
public:
    OperatorSelector(Statistics& stats)
    :m_stats(stats)
    {}
    
    virtual ~OperatorSelector() {}
    
    void prepareForSelections();
    
    /**
     * Selects the most suitable operator and returns it.
     * @returns T* the next operator we should apply.
     */
    Result select();
    
    /**
     * Called when the selected operator succeeded.
     */
    void success(const Result& selected);
    
    /**
     * Called when the selected operator succeeded.
     */
    void failure(const Result& selected);
    
    /**
     * Must be called at the end of each generation.
     */
    virtual void step(PopulationParameters& params) = 0;
    
    /**
     * Must be called on each epoch.
     */
    virtual void epoch(PopulationParameters& params) = 0;
    
    /**
     * Returns the Data object of the i-th Operator in the vector of data.
     * @param i Index of the Data in the vector
     * @returns Data& The Data object.
     */
    virtual Data& getData(unsigned int i) const = 0;
    
    /**
     * Returns the number of Data in the vector
     */
    virtual unsigned int getDataCount() const = 0;
    
    /**
     * Returns the Data object of the Operator with the specified name.
     * @param std::string Operator's name.
     * @returns Data* The Data object or null
     */
    virtual Data* getData(const std::string& operatorName) const = 0;
    
    /**
     * Instanciate a Data object of the right type.
     * The created object is not part of the selector.
     * You can make it selectable by giving it to useData().
     */
    virtual std::unique_ptr<Data> instanciateData() const = 0;
    
    /**
     * Use in the selector an existing Data object.
     * The given object must be of the right type.
     */
    virtual void addData(std::unique_ptr<Data> data) = 0;
    
    /**
     * Make the given operator selectable.
     * If it doesn't have any data yet, an object will be created for it.
     * The created/already found data is returned.
     */
    virtual Data& createData(Operator& op) = 0;
    
    /**
     * Remove the named operator from the selector.
     */
    virtual void removeData(const std::string& name) = 0;
    
    /**
     * Instanciate an operator selection policy given its XML name.
     * @returns An instance of OperatorSelector or null if the name is unknown.
     */
    static std::unique_ptr<OperatorSelector> instanciate(
        const std::string& name, Statistics& stats);
    
    /**
     * @returns XML_NAME of the class of this selector.
     */
    virtual const std::string& getXmlName() const = 0;
    
    /**
     * Read/write parameters for this selector.
     */
    virtual void writeInnerXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
};

template <typename T>
class SpecificOperatorSelector: public OperatorSelector
{
private:
    std::vector<std::unique_ptr<T>> m_data;
    
public:
    SpecificOperatorSelector(Statistics& stats)
    : OperatorSelector(stats)
    {}
    
    virtual ~SpecificOperatorSelector() {}
    
    virtual T& getData(unsigned int i) const {
        return *m_data[i];
    }
    
    virtual T* getData(const std::string& operatorName) const {
        for (auto& d: m_data) {
            if (d->getOperatorName() == operatorName) {
                return d.get();
            }
        }
        return nullptr;
    }
    
    virtual unsigned int getDataCount() const {
        return m_data.size();
    }
    
    virtual std::unique_ptr<Data> instanciateData() const {
        return std::unique_ptr<Data>(new T());
    }
    
    virtual void addData(std::unique_ptr<Data> data) {
        Assert(dynamic_cast<T*>(data.get()));
        m_data.emplace_back(static_cast<T*>(data.release()));
    }
    
    virtual Data& createData(Operator& op) {
        T* data = getData(op.getName());
        if (!data) {
            data = new T(op);
            m_data.push_back(std::unique_ptr<T>(data));
        }
        return *data;
    }
    
    virtual void removeData(const std::string& name) {
        m_data.erase(std::remove_if(m_data.begin(), m_data.end(),
            [&] (const std::unique_ptr<T>& data) {
                return data->getOperatorName() == name;
            }));
    }
};

}
}

#endif // HEADER_UGP3_CORE_OPERATORSELECTOR
