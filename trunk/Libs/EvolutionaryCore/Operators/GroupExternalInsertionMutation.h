/***********************************************************************\
|                                                                       |
| GroupExternalInsertionMutationOperator.h                              |
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

#ifndef HEADER_UGP3_CORE_GROUPEXTERNALINSERTIONMUTATIONOPERATOR
#define HEADER_UGP3_CORE_GROUPEXTERNALINSERTIONMUTATIONOPERATOR

#include "Operators/GroupMutation.h"

namespace ugp3
{
	namespace core
	{
        /** The Group External Insertion Mutation Operator. It adds one or more individuals to the group. */
		class GroupExternalInsertionMutationOperator : public GroupMutationOperator
		{
		public:
			GroupExternalInsertionMutationOperator();
			~GroupExternalInsertionMutationOperator();

                        virtual void generate( const vector<Group*> &parents, std::vector<Group*>& outChildren, GroupPopulation* population) const;

			/** Gets the name of the operator.
				@return The name of the operator.*/
			virtual const std::string getName() const;

			/** Gets the complete operator description.
				@return Operator description.
			*/
			virtual const std::string getDescription() const;
			/** Gets the acronym of the operator.
				@return The acronym of the operator.*/
			virtual const std::string getAcronym() const;
 
    virtual Category getCategory() { return DEFAULT_OFF; }

                public: // Xml methods
                        virtual bool hasParameters() const;
                        virtual void writeXml(ostream& output) const;
                        virtual void readXml(const xml::Element& element);

                private: // static fields
                        /** Xml tag to specify value of an xml parameter attribute */
                        static const std::string XML_ATTRIBUTE_VALUE;
                        /** Xml tag to specify the selector path */
                        static const std::string XML_PARAMETER_SELECTOR;
                        /** Xml tag to specify the number of individuals used to make the selection */
                        static const std::string XML_PARAMETER_INDIVIDUALS;
                        /** Xml tag to specify the fitness hole */
                        static const std::string XML_PARAMETER_FITNESSHOLE;

                private:
                        /** File name of the selector */
                        std::string selectorFileName;
                        /** Number of individuals used to make the selection */
                        unsigned int numberOfIndividuals;
                        /** Fitness hole to use in the selection process */
                        double fitnessHole;

			Individual* callExternalSelector(Group *g, std::vector<Individual*> inds) const;
		};

		inline GroupExternalInsertionMutationOperator::~GroupExternalInsertionMutationOperator()
                {}
	}
}


#endif
