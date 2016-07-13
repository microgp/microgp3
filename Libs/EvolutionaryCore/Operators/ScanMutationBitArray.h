/***********************************************************************\
|                                                                       |
| ScanMutationBitArrayOperator.h                                                |
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

#ifndef HEADER_UGP3_CORE_SCANMUTATIONBITARRAYOPERATOR
#define HEADER_UGP3_CORE_SCANMUTATIONBITARRAYOPERATOR

#include "Operators/Mutation.h"

namespace ugp3
{
	namespace core
	{
        /** The Scan Mutation Operator. */
		class ScanMutationBitArrayOperator : public MutationOperator
		{
		public:
			ScanMutationBitArrayOperator();

            		/** ScanMutationBitArray 	
				performs a local quasi-exhaustive search in the proximity of the parent individual.
				One parameter is chosen at random in the individual. The behavior then varies according to the
				type of parameter chosen, but generally a considerable number of children is produced. This
				number depends on the value of sigma (the strength of the genetic operators).
				- bitArray
					follows a number of iterations based on sigma. At each iteration, an individual at a certain
					Hamming distance from the parent is created. Once all individuals at a certain Hamming
					distance have been created, the Hamming distance is increased.
				All individuals produced will share the same allopatric tag. This means that (at the most) ONE of
				the offspring will survive in the slaughtering step. If the operator fails, the outChildren vector
				will be empty.

				@param parameters Parameter of the population on which the operator is applicated
				@param outChildren Vector of the new individuals*/
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
 
    virtual Category getCategory() { return DEFAULT_OFF; }

                public: // Xml methods
                        virtual bool hasParameters() const;
                        virtual void writeXml(std::ostream& output) const;
                        virtual void readXml(const xml::Element& element);
		};

                inline bool ScanMutationBitArrayOperator::hasParameters() const
                {
                  return false;
                }

                inline void ScanMutationBitArrayOperator::writeXml(std::ostream& output) const
                {}

                inline void ScanMutationBitArrayOperator::readXml(const xml::Element& element)
                {}
	}
}


#endif

