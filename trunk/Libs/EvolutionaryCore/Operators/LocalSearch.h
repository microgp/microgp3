// The idea of this operator is to create offspring following a certain user-defined logic.
// The operator will thus call an external program, using the individual (wrote to file) as an argument

// what should the program do
// - read individual(s) in input
// - produce individual file(s) in output

// Assimilate
// - will be a static class, probably ugp3::core::Assimilate
// - method Assimilate takes as arguments
// 	-- constraints
//	-- file to assimilate


#ifndef HEADER_UGP3_CORE_LOCALSEARCHOPERATOR
#define HEADER_UGP3_CORE_LOCALSEARCHOPERATOR

// the number of parents might vary, so it cannot inherit from MutationOperator or CrossoverOperator
#include "GeneticOperator.h"

namespace ugp3
{
	namespace core
	{
		class LocalSearchOperator : public GeneticOperator
		{
		private:
			// XML tags
			static const std::string XML_ATTRIBUTE_NUMBEROFPARENTS;
			static const std::string XML_ATTRIBUTE_EXECUTABLE;
			static const std::string XML_ATTRIBUTE_OUTPUT;
			
			// name of the executable
			std::string executable;
			// output pattern
			std::string outputPattern;
			// number of parents
			unsigned int parentsCardinality;
			// parents' and offspring naming pattern
			static const std::string PARENT_FILENAME;
			static const std::string OFFSPRING_FILENAME;

		public:
			// XML tag, needs to be accessible from PopulationParameters.xml.cc
			static const std::string XML_NAME;

			LocalSearchOperator();
			~LocalSearchOperator();

            		/** LocalSearch 	
				@param parameters Parameter of the population on which the operator is applicated
				@param outChildren Vector of the new individuals
			*/
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
			
			/** Gets the number of parents required by the operator. In this case, the number is read directly from the XML.
				@return The number of parents required by the operator, in this case read directly from the XML.
			*/
			unsigned int getParentsCardinality() const;
 
    virtual Category getCategory() { return DEFAULT_OFF; }

                public: // Xml methods
                        virtual bool hasParameters() const;
                        virtual void writeXml(std::ostream& output) const;
                        virtual void readXml(const xml::Element& element);
		};

                inline bool LocalSearchOperator::hasParameters() const
                {
                  return true;
                }

	}
}


#endif

