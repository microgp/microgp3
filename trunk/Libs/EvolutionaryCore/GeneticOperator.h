/***********************************************************************\
|                                                                       |
| GeneticOperator.h                                                     |
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
 * @file GeneticOperator.h
 * Definition of the GeneticOperator class.
 * @see GeneticOperator.cc
 */

#ifndef HEADER_UGP3_CORE_GENETICOPERATOR
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_GENETICOPERATOR

#include "Operator.h"
#include "Individual.h"

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
    
class IndividualPopulationParameters;
    
/** 
 * @class GeneticOperator
 * Represents a genetic operator. It is used by the evolutionary algorithm to generate new individuals. 
 * @author Alessandro Aimo.
 * @author Alessandro Salomone.
 */
class GeneticOperator : public Operator
{
protected:
    /** 
     * Deletes all the individuals of the vector.
     * @param individuals Vector of individuals to delete
     * @throws Any exception
     */
    void release(std::vector<Individual*>& individuals);

public:
    /** Type of this element */
    static const std::string TYPE;

    /**
     * This operator applies to individuals
     */
    typedef Individual OperandType;
    
    virtual bool isApplicable (const Population* population);
    
    /**
     * Adapter funtion that simply does some casting and calls the other `generate()' function.
     * TODO maybe merge these two functions, and factorize some code
     */
    virtual void apply(Population& population,
                       std::vector< CandidateSolution* >& newGeneration) const;

    /**
     * Generates new individuals.
     * @param parents Individuals to use to generate the new ones. They are
     *                selected with a pressure of 1.0 and there are exactly
     *                `getParentsCardinality()' of them.
     * @param outChildren A containter that is filled with the new individuals.
     *                    It may be left empty if the operator fails.
     * @param population Population of the individuals
     */
    virtual void generate(const std::vector<Individual*>& parents,
                          std::vector<Individual*>& outChildren,
                          IndividualPopulation& population) const = 0;

public: // Xml methods
    virtual bool hasParameters() const = 0;
    virtual void writeXml(std::ostream& output) const = 0;
    virtual void readXml(const xml::Element& element) = 0;
};

}

}

#endif


