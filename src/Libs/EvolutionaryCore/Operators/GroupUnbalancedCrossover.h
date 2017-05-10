#ifndef HEADER_UGP3_CORE_GROUPMULTIPOINTIMPRECISECROSSOVEROPERATOR
#define HEADER_UGP3_CORE_GROUPMULTIPOINTIMPRECISECROSSOVEROPERATOR

#include "Operators/GroupCrossover.h"

namespace ugp3
{
	namespace core
	{
		class GroupUnbalancedCrossoverOperator : public GroupCrossoverOperator
		{
        protected:
    virtual bool doSwaps(unsigned int min, unsigned int max, double sigma,
                         Group* child1, Group* child2,
                         std::vector< GEIndividual* >& swappable1, std::vector< GEIndividual* >& swappable2) const;
            
		public:
			GroupUnbalancedCrossoverOperator();
			~GroupUnbalancedCrossoverOperator();

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
                        virtual void writeXml(ostream& output) const;
                        virtual void readXml(const xml::Element& element);
		};

		inline GroupUnbalancedCrossoverOperator::~GroupUnbalancedCrossoverOperator()
                {}
                
                inline bool GroupUnbalancedCrossoverOperator::hasParameters() const
                {
                    return false;
                }

                inline void GroupUnbalancedCrossoverOperator::writeXml(std::ostream& output) const
                {}

                inline void GroupUnbalancedCrossoverOperator::readXml(const xml::Element& element)
                {}
	}
}


#endif /* HEADER_UGP3_CORE_GROUPMULTIPOINTIMPRECISECROSSOVEROPERATOR */

