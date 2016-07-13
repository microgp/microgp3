/*********************************************************************** \
|                                                                       |
| Individual.h                                                          |
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
| $Revision: 647 $
| $Date: 2015-02-24 22:32:04 +0100 (Tue, 24 Feb 2015) $
\***********************************************************************/

/**
 * @file Individual.h
 * Definition of the Individual class.
 * @see Individual.cc
 * @see Individual.xml.cc
 * @see Individual.instantiate.cc
 */

#ifndef HEADER_UGP3_CORE_INDIVIDUAL
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_INDIVIDUAL

// headers from other modules
#include "ConstrainedTaggedGraph.h"

// headers of this module
#include "CandidateSolution.h"
#include "OperatorToolbox.h"
#include "Lineage.h"

namespace ugp3 {
namespace core {

class IndividualPopulation;

class Population;
class EvolutionaryAlgorithm;

/**
 * @class Individual
 * Class for representing an individual in the population
 * @see EnhancedIndividual
 * @see MOIndividual
 */
class Individual : public CandidateSolution,
    public ICloneable<Individual>
{
private:
    
    // Static count of the individual created. Used to assign the id to each individual.
    static InfinityString idCounter;
    
    // Graph of the Individual
    std::unique_ptr<ugp3::ctgraph::CGraphContainer> m_graphContainer;
    
    // External representation of the individual. When empty, means it must be computed.
    mutable std::string m_externalRepresentation;
    
    SpecificLineage<Individual> m_lineage;
    
protected:
    Individual(const Population& population);
            Individual(const Individual& individual);
    virtual hash_t calculateHashCode(Purpose purpose) const;
    virtual void computeNormalizedPhenotype(std::string& code) const;

public:
    static const std::string XML_NAME;
            static std::string getIndividualCounter();
            static void setIndividualCounter(const std::string& value);

public:
    Individual(
                unsigned long birth, 
                const std::string& geneticOperator, 
                const std::vector<std::string>& parents, 
                const Population& population);

    static std::unique_ptr<Individual> instantiate(
                                                const xml::Element& element, 
                                                const Population& population);

    static std::unique_ptr<Individual> instantiate(
                                                const std::string& type, 
                                                const Population& population);

    virtual ~Individual();

    virtual void step(bool age) override;
    
    virtual void toCode(const std::string& fileName, std::vector<std::string>* outfiles = nullptr) const;
    virtual void outputLineage(std::ostream& lineageFile,
        unsigned int recursion, const std::string& indent = "") const;
    
    const std::string& getExternalRepresentation() const;
    
    virtual bool isGenotypeEqual(const CandidateSolution& other) const;
    
    virtual std::unique_ptr<Individual> clone(const Population& population) const;
    virtual std::unique_ptr<Individual> clone() const = 0;

public:
    
    ugp3::ctgraph::CGraphContainer& getGraphContainer() { return *m_graphContainer; }
    const ugp3::ctgraph::CGraphContainer& getGraphContainer() const { return *m_graphContainer; }
    void setCGraphContainer( std::unique_ptr<ugp3::ctgraph::CGraphContainer>& graphContainer );
    
    virtual SpecificLineage<Individual>& getLineage() { return m_lineage; }
    virtual const SpecificLineage<Individual>& getLineage() const { return m_lineage; }
    
    /// Covariant convenience overload.
    const IndividualPopulation& getPopulation() const;

public:
    virtual const Message& getMessage() const { return m_graphContainer->getMessage(); }
    
    virtual const std::string getDisplayTypeName() const;

public: // IValidable interface
    virtual bool validate() const;

public: // Xml interface
    virtual const std::string& getXmlName() const final { return XML_NAME; }
    virtual const std::string& getType() const = 0;
    virtual void readXml(const xml::Element& element);
    virtual void writeInnerXml(std::ostream& output) const override;
};

template <> struct TypeName<Individual> { static constexpr const char* name = "individual"; };

inline std::unique_ptr<Individual> Individual::clone(
    const Population& parentPopulation) const
{
    _STACK;

    std::unique_ptr<Individual> individual = clone();
    individual->setPopulation(parentPopulation);

    return individual;
}

inline hash_t Individual::calculateHashCode(Purpose purpose) const
{
    return getGraphContainer().getHashCode(purpose);
}

}
}

#endif

