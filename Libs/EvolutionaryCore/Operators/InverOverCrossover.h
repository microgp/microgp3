/***********************************************************************\
|                                                                       |
| InverOverCrossoverOperator.h                                          |
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

#ifndef HEADER_UGP3_CORE_INVEROVERCROSSOVEROPERATOR
#define HEADER_UGP3_CORE_INVEROVERCROSSOVEROPERATOR

#include "Operators/Crossover.h"
#include "CSubGraph.h"

namespace ugp3
{

namespace core
{

/** InverOver is a special crossover for combinatorial parameters */
class InverOverCrossoverOperator : public CrossoverOperator
{

protected:
    int getValidCutPoint(
        const ugp3::ctgraph::CSubGraph& selectedSubGraph1, unsigned int cutPoint1, 
        const ugp3::ctgraph::CSubGraph& selectedSubGraph2) const;
public:
	InverOverCrossoverOperator();

	/** It generates new individuals. They are saved in outChildren. 
	    Parameter outChildren may be empty if operator is failed. Do 
	    not remove attachFloatingEdges function!
		@param outChildren Vector of the new individuals. In this case, 
		       vector size must be 2.*/
    virtual void generate(const std::vector< Individual* >& parents,
                          std::vector< Individual* >& outChildren,
                          IndividualPopulation& population) const;

	/** Gets the name of the operator.
		@return The name of the operator.*/
	virtual const std::string getName() const;

	/** Gets the acronym of the operator.
		@return The acronym of the operator.*/
	virtual const std::string getAcronym() const;

	/** Gets the complete operator description.
		@return Operator description.
	*/
	virtual const std::string getDescription() const;
 
    virtual Category getCategory() { return DEFAULT_ON; }

public: // Xml methods
        virtual bool hasParameters() const;
        virtual void writeXml(std::ostream& output) const;
        virtual void readXml(const xml::Element& element);
};


inline bool InverOverCrossoverOperator::hasParameters() const
{
   return false;
}

inline void InverOverCrossoverOperator::writeXml(std::ostream& output) const
{}

inline void InverOverCrossoverOperator::readXml(const xml::Element& element)
{}

}

}


#endif
