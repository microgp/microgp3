/***********************************************************************\
|                                                                       |
| Data.h                                                                |
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
 * @file Data.h
 * Definition and implementation of the Data class.
 */

#ifndef HEADER_UGP3_CORE_DATA
#define HEADER_UGP3_CORE_DATA

#include "XMLIFace.h"
#include "Log.h"
#include "Performance.h"
#include <vector>


// forward declaration
namespace Ui
{
	class MicroGPMainWindow;
}

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

class SelfAdaptiveOperatorSelector;
class DMABOperatorSelector;
class Operator;
class CallData;
class Performance;

/**
 * @class Data
 * Keeps statistics about an operator.
 * Each population uses some registered operators.
 * The statistics of each of these operators are saved in a Data object.
 * All the Data objects are collected in a Statistics object.
 * Each population has his Statistics object.
 * So a Data object contains statistics of the operators used by a specific population.
 */
class Data : public xml::XMLIFace
{
protected: // fields
    // The operator to which this data refers.
    Operator* _operator = nullptr;

    // The name of the operator. This field is necessary, as the pointer to the operator (Data<T>::_operator) is set to nullptr once the operator's activation is updated.
    std::string operatorName;
    
    // Data about calls of this operator during the current generation.
    std::vector<std::unique_ptr<CallData>> callData;
    
    // Number of calls to this operator during the last generation
    unsigned int nCallsLastGen = 0;
    
    // True iff can be used during evolution
    bool enabled = true;
    
    // True iff the enabling is an explicit user decision
    bool explicitEnabled = false;
   
    // Resets the attributes of this Data object to nullptr operator and default values. 
    virtual void clear();

    // performances are not cumulative: they represent performance of the operator in the last epoch.
    std::vector<unsigned int> performance;

    // failedCalls is not cumulative: it represents failed calls of the operator in the last epoch.
    unsigned int failedCalls = 0;
    
    /*
     * Operator pseudo-deactivation and failure control
     * 
     * This concept deviates from the original DMAB paper. Its goal is to prevent
     * MicroGP from using operators that are not useful on the given problem. For example:
     * in the case of fixed-size groups, all group removal/insertion operators
     * will be useless. Since they will always fail, MicroGP should not use them, even
     * after a reset. To implement this, we add the following fields.
     */
    
    /*
     * At the beginning, all operators are pseudo-deactivated and must "prove"
     * that they can work on the current problem.
     * This field becomes false as soon as this operator produces a valid child. 
     */
    bool isPseudoDeactivated = true;
    
    /* 
     * This field counts the number of chances that the operator still has to
     * prove that it is adapted to the problem. 
     * 
     */
    unsigned int tokens = 3;
    
    /* 
     * In order to prevent MicroGP from discarding completely an operator that might
     * become relevant at a later stage of the evolution, we distribute a new token
     * every <some number> generations.
     */
    unsigned int generationsSinceLastToken = 0;
    
    /*
     * Number of successful calls during the last generation.
     */
    unsigned int pending = 0;
    
    friend class OperatorSelector; // This class is a glorified struct, the logic is in OperatorSelector.
    
protected:
    /** Tag to set the ref attribute in a xml element */
    static const std::string XML_ATTRIBUTE_REF;
    static const std::string XML_ATTRIBUTE_ENABLED;
    static const std::string XML_ATTRIBUTE_EXPLICIT_ENABLED;
    /** Tag to set the performance attribute in a xml element */
    static const std::string XML_CHILD_ELEMENT_PERFORMANCE;
    /** Tag to set the operator parameters in a xml element */
    static const std::string XML_CHILD_ELEMENT_PARAMETERS;
    static const std::string XML_CHILD_ELEMENT_FAILURES;
    static const std::string XML_ATTRIBUTE_IS_PSEUDO_DEACTIVATED;
    static const std::string XML_ATTRIBUTE_TOKENS;
    static const std::string XML_ATTRIBUTE_GENERATIONS_SINCE_LAST_TOKEN;
    
public:            
    /** Tag to set the xml name of this object */
    static const std::string XML_NAME;
    
public: // constructors and destructors
    /**
     * Constructor of the class. It does nothing.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    Data();
    
    /**
     * Constructor of the class. Creates a Data object with the specified operator.
     * @param T element. Operator to set in the Data object.
     */
    Data(ugp3::core::Operator& op);
    
    /**
     * Constructor of the class. Creates a Data object with the specified operator name.
     * @param std::strign operatorName Operator's name.
     * @throws std::exception ArgumentException if an empty string is specified.
     */
    Data(const std::string& operatorName);
    
    /**
     * Destructor of the class. Cleans the object.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    virtual ~Data();

public: // methods
    /**
     * Sets the operator reference to nullptr.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    void cleanOperatorReference();

    /**
     * Cleans the operator's performance data.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    void cleanPerformanceData();
    
    /**
     * Increment the number of failed calls.
     */
    void incrementFailedCalls(unsigned int nb = 1) { failedCalls += nb; }
    
    /**
     * Sets the number of failed calls to 0.
     */
    void resetFailedCalls() { failedCalls = 0; }

public: // getters and setters
    /**
     * Returns the operator which this object makes reference.
     * @returns T The operator referenced.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    Operator* getOperator() const;
    /**
     * Sets the operator which this object makes reference.
     * @param element The operator to reference.
     * @throws std::exception ArgumentNullException if a null pointer is specified, Exception if the operator is already set.
     */
    void setOperator(Operator* element);
    /**
     * Returns the operator's name which this object makes reference.
     * @returns std::string The operator referenced's name.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    const std::string& getOperatorName() const;
    
    /**
     * Returns true iff this operator can be used during evolution.
     */
    bool getEnabled() const { return enabled; }
    
    /**
     * Return true iff the `enabled` value is an explicit user decision.
     */
    bool getExplicitEnabled() const { return explicitEnabled; }
    
    /**
     * Returns true iff this operator can be selected
     * (enabled, has been successful at least once, still has tokens).
     */
    bool getSelectable() const { return enabled && !isPseudoDeactivated && tokens > 0; }
    
    /**
     * Returns the number of times that the operator had the performance level specified 
     * @param level The performance level to increment.
     * @returns unsigned int. The times the operator had this performance level.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    unsigned int getPerformance(const Performance& level) const;
    /**
     * Sets the number of times that the operator had the performance level specified 
     * @param level The performance level to increment.
     * @param performance The value to set.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    void incrementPerformance(const Performance& level, unsigned int performance);
    /**
     * Returns the number of failed calls.
     * @returns unsigned int The number of failed calls.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    unsigned int getFailedCalls() const;
    
    /**
     * Register new data about this operator being called in this generation.
     * The produced call data should be filled by the called. It will be used by
     * the Statistics::step() function to update the overall statistics, and
     * automatically deleted.
     * 
     * @return The data to be filled about the call
     */
    CallData* newCallData();
    
    const std::vector<std::unique_ptr<CallData>>& getCallData() const;
    
    /**
     * Delete the CallData objects and clean the list.
     */
    void deleteCallData();
    
    unsigned int getNumberCallsLastGeneration() const { return nCallsLastGen; }
    
    /**
     * Print statistics about this operator to a CSV stream.
     */
    virtual void dumpStatisticsHeader(const std::string& name, std::ostream& output) const;
    virtual void dumpStatistics(std::ostream& output) const;
    
public: // Xml interface
    virtual void writeXml(std::ostream& output) const override final;
    virtual void readXml(const xml::Element& element) override;
    virtual const std::string& getXmlName() const override final;
    
    virtual void writeInnerXml(std::ostream& output) const;

public:
	// friend class, used for the gui
	friend class Ui::MicroGPMainWindow;
};

}

}

#endif
