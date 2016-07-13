/***********************************************************************\
|                                                                       |
| SimpleDifferentialOperator.h                                          |
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

#ifndef HEADER_UGP3_CORE_ALLOPATRICDIFFERENTIALOPERATOR
#define HEADER_UGP3_CORE_ALLOPATRICDIFFERENTIALOPERATOR

#include "Operators/DifferentialEvolutionOperator.h"
namespace ugp3
{

namespace core
{


class AllopatricDifferentialOperator : public DifferentialEvolutionOperator
{


public:
	AllopatricDifferentialOperator();

    virtual void apply(Population& population,
                       std::vector< CandidateSolution* >& newGeneration) const;

	/** Gets the name of the operator.
		@return The name of the operator.*/
	virtual const std::string getName() const;

	/** Gets the description of the operator.
		@return Operator description.*/
	virtual const std::string getDescription() const;

	/** Gets the acronym of the operator.
		@return The acronym of the operator.*/
	virtual const std::string getAcronym() const;
    
    virtual Category getCategory() { return DEFAULT_OFF; }

    inline bool hasParameters() const
    {
        return false;
    }
    
    inline void writeXml(std::ostream& /* output */) const
    {}
    
    inline void readXml(const xml::Element& /* element */)
    {}
};

}

}


#endif
