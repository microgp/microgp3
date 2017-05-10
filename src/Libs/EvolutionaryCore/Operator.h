/***********************************************************************\
|                                                                       |
| Operator.h                                                            |
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
 * @file Operator.h
 * Definition and implementation of the Operator template class.
 */

#ifndef HEADER_UGP3_CORE_OPERATOR
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_OPERATOR

#include "IString.h"
#include "XMLIFace.h"
#include "StackTrace.h"
#include "Exception.h"
#include "Log.h"
#include <vector>

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
    
class Population;
class CandidateSolution;
    
/** 
 * @class Operator
 * This class is the parent of every operator in ugp.
 * @see GeneticOperator
 * @see GroupOperator
 */
class Operator : public IString
{	
private:
    /** A container that mantains the references to the registered 
        operators. Use the Operator::registration method to 
        insert new operators. 
        @remarks Operator registration is required before loading a 
                 population from a file in order to restore the binding 
                 of the operator to the population by knowing only the 
                 operator's name.*/
    static std::vector<Operator*> operators;

protected: // constructors
    /** 
     * Constructor of the class. It does nothing.
     * @throws Any exception
     */
    Operator();

public: // static fields
    /** Constant to indicate NO_OPERATOR, usefull to specify that an operator is not applied */
    static const std::string NO_OPERATOR;

    /** Category of the operator. */
    typedef enum {
        DEFAULT_ON, ///< Stable and general-purpose operator, enabled by default
        DEFAULT_OFF, ///< Scan, randomizer, experimental
        CATEGORY_COUNT
    } Category;
public:
    /**
     * Destructor of the class used for cleaning if necessary.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    virtual ~Operator();

    /**
     * Gets the number of registered operators.
     * @returns unsigned int The number of operators registered
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    static unsigned int getOperatorCount();

    /**
     * Gets an operator by index.
     * @param index The index of the operator in the vector
     * @throws Any exception
     */
    static Operator* getOperator(unsigned int index);

    /**
     * Gets an operator by name.
     * @param name The name of the operator
     * @throws Any exception. ArgumentException if the name is empty.
     */
    static Operator* getOperator(const std::string& name);

    /**
     * Insert an operator in the vector of registered operators.
     * @param Operator The operator to insert
     * @throws Any exception. ArgumentNullException if the operator is null, Exception if the operator already exists in the vector
     */
    static void registration(Operator* op);

    /**
     * Deletes all the operators in the vector of registered operators.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    static void unregisterAll();

    /**
     * Gets the name of the operator.
     * @returns The name of the operator.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    virtual const std::string getName() const = 0;

    /**
     * Gets the acronym of the operator.
     * @return The acronym of the operator.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    virtual const std::string getAcronym() const = 0;

    /**
     * Gets the lengthy description of the operator.
     * @return Operator description, in string form.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    virtual const std::string getDescription() const = 0;

    /**
     * Gets the number of parents required by the operator.
     * @return The number of parents required by the operator.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    virtual unsigned int getParentsCardinality() const = 0;
    
    /**
     * Return the category of this operator.
     * Experimental and scan operators are disabled by default, others
     * (stable and cheap) are enabled by default.
     */
    virtual Category getCategory() = 0;
    
    /**
     * Returns true iff this operator can be applied to the given population.
     */
    virtual bool isApplicable(const Population* population) = 0;
    
    /**
     * Generate new candidates and push them into the given vector.
     * The operator must read parameters and pick parents from the
     * given population.
     */
    virtual void apply(Population& population, std::vector<CandidateSolution*>& newGeneration) const = 0;

    /**
     * Returns true if the operator has parameters
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    virtual bool hasParameters() const = 0;

    /**
     * Writes the information of this operator in the stream
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    virtual void writeXml(std::ostream& output) const = 0;

    /**
     * Reads the information of this operator from the stream
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    virtual void readXml(const xml::Element& element) = 0;
    
public: // IString interface
    virtual const std::string toString() const;
};

}

}


#endif

