/***********************************************************************\
|                                                                       |
| GroupOperator.h                                                       |
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
 * @file GroupOperator.h
 * Definition of the GroupOperator class.
 * @see GroupOperator.cc
 */

#ifndef HEADER_UGP3_CORE_GROUPOPERATOR
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_GROUPOPERATOR

#include "Operator.h"

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
    
class Group;
class GroupPopulation;
class GEIndividual;

/** 
 * @class GroupOperator
 * Represents a group operator. It is used by the evolutionary algorithm to generate new groups. 
 */
class GroupOperator : public Operator
{
protected:
    /** 
     * Deletes all the groups of the vector.
     * @param groups Vector of groups to delete
     * @throws Any exception
     */
    void release(std::vector<Group*>& groups);
    
    /**
     * Remove the individual from the group and the group from the individual in a transaction.
     * If the function returns true, the individual was in the group and the group was in the 
     * individual and the function succeeded in removing those links. If it returns false, 
     * the function did not find the said links and did not modify anything.
     * @return True iff the individual was in the group and it was removed.
     */
    bool removeIndividual(Group* child, GEIndividual* individual) const;
    
    /**
     * Add the individual to the group and reciprocally in a transaction.
     * @see removeIndividual
     */
    bool addIndividual(Group* child, GEIndividual* individual) const;
    
    /**
     * Move an individual from child1 to child2 in a transaction.
     * @see removeIndividual
     */
    bool moveIndividual(Group* child1, GEIndividual* individual, Group* child2) const;
    
    /**
     * Swap the individuals between the groups and reciprocally in a transaction.
     * @see removeIndividual
     */
    bool swapIndividuals(
        Group* child1, GEIndividual* ind1,
        Group* child2, GEIndividual* ind2
    ) const;
    
public:
    virtual ~GroupOperator() {};
    
    /** Type of this element */
    static const std::string TYPE;
    
    /**
     * This operator applies to groups
     */
    typedef Group OperandType;
    
    virtual bool isApplicable(const Population* population);
    
    /**
     * Adapter function that does some casts, selects parents and calls the other
     * version of `generate()'.
     */
    virtual void apply(Population& population, std::vector<CandidateSolution*>& outChildren) const;

    /**
     * Generates new groups. 
     * @param parents Groups to use to generate the new ones
     * @param outChildren A containter that is filled with the new groups. It may be empty if the operator fails.
     * @param population Population of the groups
     * @throws Any exception.
     */
    virtual void generate(const std::vector<Group*> &parents, std::vector<Group*>& outChildren, GroupPopulation* population) const = 0;

public: // Xml methods
    virtual bool hasParameters() const = 0;
    virtual void writeXml(std::ostream& output) const = 0;
    virtual void readXml(const xml::Element& element) = 0;
};

}

}

#endif

