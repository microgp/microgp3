/***********************************************************************\
|                                                                       |
| OnePointImpreciseCrossoverOperator.h                                  |
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

#ifndef HEADER_UGP3_CORE_ONEPOINTIMPRECISECROSSOVEROPERATOR
#define HEADER_UGP3_CORE_ONEPOINTIMPRECISECROSSOVEROPERATOR

#include "Operators/Crossover.h"
#include "CSubGraph.h"

namespace ugp3
{

namespace core
{

/** The One-Point Imprecise Crossover Operator creates two
    new individuals from two parent individuals. It tranforms
    the edges in offset parameters, after the generation all the
    offset parameters are replaced by normal edges. It is
    "imprecise" because it doesn't consider outer label. */
class OnePointImpreciseCrossoverOperator : public CrossoverOperator
{

protected:
    int getValidCutPoint(
        const ugp3::ctgraph::CSubGraph& selectedSubGraph1, unsigned int cutPoint1, 
        const ugp3::ctgraph::CSubGraph& selectedSubGraph2) const;
public:
	OnePointImpreciseCrossoverOperator();

	/** It generates new individuals. They are saved in outChildren. 
	    Parameter outChildren may be empty if operator is failed. Do 
	    not remove attachFloatingEdges function!
		A slice can be made of a part of a subGraph. Two or more 
		subGraphs can't belong to a slice.
		Prologue/Epilogue of Constraints and Sections can't belong to 
		a slice. Only the slices are swapped, other subgraphs referred 
		by nodes of the slices are not copied to new individual.
		@param parameters Parameter of the population on which the operator is applicated
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


inline bool OnePointImpreciseCrossoverOperator::hasParameters() const
{
   return false;
}

inline void OnePointImpreciseCrossoverOperator::writeXml(std::ostream& output) const
{}

inline void OnePointImpreciseCrossoverOperator::readXml(const xml::Element& element)
{}

}

}


#endif

