/***********************************************************************\
|                                                                       |
| IndividualPopulation.h |
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
 * @file IndividualPopulation.h
 * Base mechanisms for populations of individuals
 */

#ifndef HEADER_UGP3_CORE_INDIVIDUALPOPULATION
#define HEADER_UGP3_CORE_INDIVIDUALPOPULATION

#include "Operator.h"
#include "OperatorSelector.h"
#include "IndividualPopulationParameters.h"
#include "CGraphContainer.h"
#include "Population.h"

namespace ugp3 {
namespace core {

class IndividualPopulation: public Population
{
public:
    IndividualPopulation(std::unique_ptr< PopulationParameters > parameters,
                         unsigned int generation, const EvolutionaryAlgorithm& parent)
    : Population(std::move(parameters), generation, parent)
    {}
    IndividualPopulation(const EvolutionaryAlgorithm& parent)
    : Population(parent)
    {}
    virtual ~IndividualPopulation() {};
    
    // virtual const Individual* getBestRawIndividual() const = 0;
    // virtual const Individual* getWorstRawIndividual() const = 0;
    
    virtual std::size_t getIndividualCount() const = 0;
    virtual Individual& getIndividual(unsigned int i) const = 0;
    
    virtual std::unique_ptr<Individual> newRandomIndividual() const = 0;
    
    virtual IndividualPopulationParameters& getParameters() = 0;
    virtual const IndividualPopulationParameters& getParameters() const = 0;
};
    
template <class IndividualType>
class SpecificIndividualPopulation: public IndividualPopulation
{
protected:
    std::vector<IndividualType*> m_individuals;
    IndividualType* m_bestRawIndividual = nullptr;
    IndividualType* m_worstRawIndividual = nullptr;
    IndividualType* m_bestScaledIndividual = nullptr;
    IndividualType* m_worstScaledIndividual = nullptr;
    
public:
    SpecificIndividualPopulation(std::unique_ptr< PopulationParameters > parameters,
                         unsigned int generation, const EvolutionaryAlgorithm& parent)
    : IndividualPopulation(std::move(parameters), generation, parent)
    {}
    SpecificIndividualPopulation(const EvolutionaryAlgorithm& parent)
    : IndividualPopulation(parent)
    {}
    virtual ~SpecificIndividualPopulation();
    
    virtual const CandidateSolution* getBestCandidate() const { return m_bestRawIndividual; }
    virtual const CandidateSolution* getWorstCandidate() const { return m_worstRawIndividual; }
    virtual const IndividualType* getBestRawIndividual() const { return m_bestRawIndividual; }
    virtual const IndividualType* getWorstRawIndividual() const { return m_worstRawIndividual; }
    virtual const IndividualType* getBestScaledIndividual() const { return m_bestScaledIndividual; }
    virtual const IndividualType* getWorstScaledIndividual() const { return m_worstScaledIndividual; }
    
    virtual IndividualType& getIndividual(unsigned int i) const {
        return *m_individuals[i];
    }
    
    IndividualType* getIndividualById(std::string id) const
    {
        auto it = std::find_if(m_individuals.begin(), m_individuals.end(), [&] (IndividualType* ind) {
            return ind->getId() == id;
        });
        if (it != m_individuals.end()) {
            return *it;
        }
        return nullptr;
    }
    
    
    typename std::vector<IndividualType*>::const_iterator getIndividualsConstBegin() { return m_individuals.cbegin(); }
    typename std::vector<IndividualType*>::const_iterator getIndividualsConstEnd() { return m_individuals.cend(); }
    
    /** 
     * Adds a new individual to the population.
     * @param individual Individual to add
     * @throws Any exception. ugp3::ArgumentNullException if a null pointer is passed by argument, ugp3::ArgumentException if the individual already exists or there is another individual with the same id.
     */
    virtual void addIndividual(std::unique_ptr<Individual> individual);
    
    /** 
     * Adds the individuals of a vector to the population
     * @param individuals Vector with the individuals to add
     * @throws Any exception. ugp3::ArgumentNullException if a null pointer is in an element of the vector, ugp3::ArgumentException if an individual already exists or there is another individual with the same id.
     */
    void addIndividuals(const std::vector<Individual*>& individuals) {
        for (auto individual: individuals) {
            addIndividual(std::unique_ptr<Individual>(individual));
        }
    }
    
    /** 
     * Returns the number of individuals in the population
     * @returns unsigned int Number of individuals
     */
    virtual std::size_t getIndividualCount() const { return m_individuals.size(); }
    
    virtual std::size_t getCandidateCount() const { return m_individuals.size(); }
    
    virtual std::size_t getLiveCandidateCount() const {
        return std::count_if(m_individuals.begin(), m_individuals.end(), [] (Individual* i) {
            return i->isAlive();
        });
    }
    
    
    /** 
     * Creates a new random instance of an Individual.
     * @returns Individual* A pointer to the new individual
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    std::unique_ptr<IndividualType> newRandomSpecificIndividual() const 
    {
        return std::unique_ptr<IndividualType>(
            new IndividualType(getGeneration(), Operator::NO_OPERATOR, std::vector<std::string>(), *this)
        );
    }
    virtual unique_ptr<Individual> newRandomIndividual() const 
    {
        return std::unique_ptr<Individual>(newRandomSpecificIndividual().release());
    }
    
    virtual void setupPopulation();
    
    virtual void mergeNewGeneration(const std::vector< CandidateSolution* >& newGeneration);
    
    /*virtual*/ void discardFitnessValues(); 
    /*
    {
        for (auto ind: m_individuals) 
        {
            ind->getRawFitness().invalidate();
            ind->getFitness().invalidate();
        }
    }
    */
    
    virtual void evaluateAndHandleClones();
    
    virtual void checkFitnessValidity();
    
    virtual void updateOperatorStatistics(const std::vector< CandidateSolution* >& newGeneration);
    
    virtual void selectNewZombifiableCandidates();

    virtual void age() 
    {
        promoteHeroesAndAge(m_individuals.begin(), m_individuals.end());
    }
    
    virtual void handleZombies() 
    {
        for (auto ind: m_individuals) 
	{
            if (ind->isZombie() && !isZombifiable(ind)) 
	    {
                // Zombie not found in candidates for zombification
                ind->setState(CandidateSolution::DEAD);
            }
        }
    }
    
    virtual void removeDeadCandidates() 
    {
        removeCorpses(m_individuals);
    }
    
    virtual void slaughtering();
    
    virtual void prepareForCommit();
    
    virtual void commit();
    
    virtual void showStatistics(void) const;
    virtual void dumpStatisticsHeader(std::ostream& output) const;
    virtual void dumpStatistics(std::ostream& output) const;
    
    virtual double getAverageAge() const 
    {
        return computeAverageAge(m_individuals.begin(), m_individuals.end());
    }

    virtual std::vector< double > getAverageRawFitness() const 
    {
        return computeAverageRawFitness(m_individuals.begin(), m_individuals.end());
    }
    
    /** 
     * Returns the average size of the population measured in nodes. 
     * @returns double The average size of the population
     */
    double computeAverageIndividualSize() const;

    // seed the population with individuals
    void seeding(std::string fileName);
    
    virtual void dumpAllCandidates();
    
    /** Xml tag to specify the individuals of the population */
    static const std::string XML_CHILD_ELEMENT_INDIVIDUALS;
    virtual void readXml(const xml::Element& element);
    virtual void writeInnerXml(std::ostream& output) const;
    void parseIndividuals(const xml::Element& element);
};

}
}

#endif // HEADER_UGP3_CORE_INDIVIDUALPOPULATION
