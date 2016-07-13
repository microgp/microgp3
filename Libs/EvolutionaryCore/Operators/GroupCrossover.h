/***********************************************************************\
|                                                                       |
| GroupCrossoverOperator.h                                              |
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
 * @file GroupCrossoverOperator.h
 * Definition of the GroupCrossoverOperator class.
 * @see GroupCrossoverOperator.cc
 */

#ifndef HEADER_UGP3_CORE_GROUPCROSSOVEROPERATOR
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_GROUPCROSSOVEROPERATOR

#include "Group.h"
#include "GroupOperator.h"

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
    
    class GroupCrossoverOperator : public GroupOperator
    {
    protected:
        
        /**
         * The concrete crossover operators must implement this function.
         * The resulting groups must be valid.
         */
        virtual bool doSwaps(unsigned int min, unsigned int max, double sigma,
                             Group* child1, Group* child2,
                             vector<GEIndividual*>& swappable1, vector<GEIndividual*>& swappable2) const = 0;
        
    public:
        GroupCrossoverOperator();
        
        virtual void generate( const vector<Group*> &parents, std::vector<Group*>& outChildren, GroupPopulation* population) const;
        
        /** Gets the number of parents required by the operator.
         * @return The number of parents required by the operator.*/
        virtual unsigned int getParentsCardinality() const { return 2; }
        
        /** Gets the name of the operator.
         *          @return The name of the operator.*/
        virtual const std::string getName() const { return "groupCrossover"; }
        
        /** Gets the acronym of the operator.
         *          @return The acronym of the operator.*/
        virtual const std::string getAcronym() const { return "G.X"; }
        
        /** Gets the complete operator description.
         *          @return Operator description.
         */
        virtual const std::string getDescription() const {
            return "Swap individuals between two groups. Only operates when group evolution is active.";
        }
        
        public: // Xml methods
            virtual bool hasParameters() const = 0;
            virtual void writeXml(std::ostream& output) const = 0;
            virtual void readXml(const xml::Element& element) = 0;
    };
}
}

#endif
