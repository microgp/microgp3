/***********************************************************************\
|                                                                       |
| TwoPointCrossoverOperator.h                                           |
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

#ifndef HEADER_UGP3_CORE_TWOPOINTCROSSOVEROPERATOR
#define HEADER_UGP3_CORE_TWOPOINTCROSSOVEROPERATOR

#include "Operators/OnePointCrossover.h"

namespace ugp3
{
	namespace core
	{
        /** The Two-Point Crossover Operator creates two new
            individuals from two parent individuals. It tranforms the
            edges in offset before swapping, and after the generation
            all the offset parameters are replaced by normal edges.
            If the swapped nodes include outer references, all linked
            subgraphs  are copied into the new individual. */
		class TwoPointCrossoverOperator : public OnePointCrossoverOperator
		{
		public:
			TwoPointCrossoverOperator();

			/** It generates new individuals. They are saved in outChildren. outChildren may be empty if
				operator is failed. Do not remove attachFloatingEdges function!
				A slice can be made of a part of a subGraph. Two or more subGraphs can't belong to a slice.
				Prologue/Epilogue of Constraints and Sections can't belong to a slice. Subgraphs referred by
				nodes of the slices are copied to new individual. If a "copied" subgraph is not referred anymore,
				it is removed from parent individual.
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
	}
}

namespace ugp3
{
    namespace core
    {
        inline const std::string TwoPointCrossoverOperator::getName() const
        {
            return "twoPointCrossover";
        }
        
        inline const std::string TwoPointCrossoverOperator::getAcronym() const
        {
            return "2P.X";
        }
        
        inline bool TwoPointCrossoverOperator::hasParameters() const
        {
            return false;
        }
        
        inline void TwoPointCrossoverOperator::writeXml(std::ostream& /* output */) const
        {}
        
        inline void TwoPointCrossoverOperator::readXml(const xml::Element& /* element */)
        {}
    }
}


#endif
