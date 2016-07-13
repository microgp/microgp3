/***********************************************************************\
|                                                                       |
| GroupPopulation.h                                                     |
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
 * @file GroupPopulation.h
 * Definition of the GroupPopulation class.
 * @see GroupPopulation.cc
 * @see GroupPopulation.xml.cc
 */

#ifndef HEADER_UGP3_CORE_GROUPPOPULATION
#define HEADER_UGP3_CORE_GROUPPOPULATION

// headers from this module
#include "IndividualPopulation.h"
#include "Group.h"
#include "GEIndividual.h"
#include "GroupPopulationParameters.h"

#include <vector>

/**
 * ugp3 namespace
 */
namespace ugp3
{

/**
 * ugp3::core namespace
 */
namespace core
{

/**
 * @class GroupPopulation
 * Represents a population of groups of individuals
 * @see GroupPopulationParameters
 */
class GroupPopulation : public SpecificIndividualPopulation<GEIndividual>
{
private: // fields
    GroupPopulationParameters m_parameters;
    /** The best group in the population */
    Group* m_bestRawGroup = nullptr;
    /** The worst group in the population */
    Group* m_worstRawGroup = nullptr;
    /** The best group in the population */
    Group* m_bestScaledGroup = nullptr;
    /** The worst group in the population */
    Group* m_worstScaledGroup = nullptr;
    /** Previous best group fitness for steady-state detection */
    Fitness m_previousGroupMaxFitness;
    /** Groups in this population */
    std::vector<Group*> m_groups;

private: // methods

    /**
     * Kill the groups that have a wrong number of individuals.
     * Those groups are not zombified, since they are invalid.
     */
    void killInvalidGroups(vector<Group*>& groups);

    /**
     * Generates the groups for the new individuals.
     * These groups are clones of the parents ones but replacing
     * the parents by the sons. The sons have to be from the same parents.
     * The produced groups are guaranteed to be valid.
     * The produced groups have the genetic operator and their group parents as a lineage
     */
    std::vector<Group*> generateSonsGroups(
        const OperatorSelector::Result& selected,
        const std::vector<GEIndividual*>& sons);
    
    /** Helper function for the above algorithm. */
    std::vector<Group*> selectParentsGroupsFrom(const vector<Group*>& parentsGroups, unsigned int numberToSelect);

    /** 
     * Returns the average size of the groups in the population measured in individuals. 
     * @returns double The average size of the groups in the population
     */
    double computeGroupsAverageSize() const;
    
protected: // static fields
    /** Xml tag to specify the groups of the population */
    static const std::string XML_CHILD_ELEMENT_GROUPS;

protected:
    virtual void checkFitnessValidity();
    
    virtual void updateOperatorStatistics(const std::vector< CandidateSolution* >& newGeneration);
  
    virtual void selectNewZombifiableCandidates();
    
    virtual void slaughtering();
    
    virtual void handleZombies();
    
    virtual void removeDeadCandidates();
    
    virtual void prepareForCommit();

    /**
     * Sets the best and wort group in the population
     * @throws std::exception
     */
    virtual void commit();
public: // constructors and destructors
  /**
   * Constructor of the class. Creates a GroupPopulation with the given algorithm.
   * @param parent Evolutionary algorithm
   * @throws std::exception
   */
  GroupPopulation(const EvolutionaryAlgorithm& parent);

  /** 
   * Destructor of the class. Frees the memory by removing all the goups in the population.
   */
  ~GroupPopulation();

public: 

    /** 
     * Returns the number of groups in the population
     * @returns unsigned int Number of groups
     */
    std::size_t getGroupCount() const { return m_groups.size(); }
    
    virtual std::size_t getLiveCandidateCount() const {
        return std::count_if(m_groups.begin(), m_groups.end(), [] (Group* g) {
            return g->isAlive();
        });
    }
    
    virtual std::size_t getCandidateCount() const { return m_groups.size(); }

    /** 
     * Returns the ith group in the population
     * @returns Group The ith group in the vector
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    const Group& getGroup(unsigned int i) const;
    
    std::vector<Group*>::const_iterator getGroupsConstBegin() { return m_groups.cbegin(); }
    std::vector<Group*>::const_iterator getGroupsConstEnd() { return m_groups.cend(); }

    /**
     * Returns a group of the population randomly
     * @returns Group* A group of the population.
     * DEPRECATED use getParameters().groupSelection(1, 0.0) instead.
     */
    //Group* getRandomGroup();

    /**
     * Returns the best group of the population
     * @returns const Group* The best group of the population
     */
    const Group* getBestRawGroup() const { return m_bestRawGroup; }
    const Group* getBestScaledGroup() const { return m_bestScaledGroup; }
    virtual const Group* getBestCandidate() const { return m_bestRawGroup; }

    /**
     * Returns the worst group of the population
     * @returns const Group* The worst group of the population
     */
    const Group* getWorstRawGroup() const { return m_worstRawGroup; }
    const Group* getWorstScaledGroup() const { return m_worstScaledGroup; }
    virtual const Group* getWorstCandidate() const { return m_worstRawGroup; }

    /**
     * Individuals: we use the group fitness and the delta entropy
     * Groups: we use delta entropy.
     */
    virtual bool compareForFitnessHole(const CandidateSolution* a, const CandidateSolution* b) const;

    /**
     * Creates a new random instance of a Group.
     * @returns Group* A pointer to the new group
     */
    virtual std::unique_ptr<Group> newRandomGroup();

    /** 
     * Adds a new group to the population.
     * @param group Group to add
     * @throws Any exception. ugp3::ArgumentNullException if a null pointer is passed by argument, ugp3::ArgumentException if the group already exists or there is another group with the same id.
     */
    virtual void addGroup(std::unique_ptr<Group> group);

    /** 
     * Adds the groups to the population
     * @param groups Vector with the groups to add
     * @throws Any exception. ugp3::ArgumentNullException if a null pointer is in an element of the vector, ugp3::ArgumentException if a group already exists or there is another group with the same id.
     */
    void addGroups(std::vector<Group*> groups);

    /** 
     * Writes the groups and individuals composition in LOG_INFO
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    void writeGroupsAndIndividualsComposition() const;
    
    /**
     * Individuals:
     * When we detect individual clones, we remove them and replace them with their
     * master in the groups.
     *
     * Groups:
     * We mark group clones as dead.
     * Copy the fitness from raw to enhanced
     */
    virtual void handleClone(CandidateSolution& master, CandidateSolution& clone,
                             unsigned int number, unsigned int total);

    virtual std::vector< CandidateSolution* > applyOperator(
        CallData* callData, const OperatorSelector::Result& selected);
    
    virtual void mergeNewGeneration(const std::vector< CandidateSolution* >& newGeneration) override;
    
    virtual double getAverageAge() const;
    
    virtual vector< double > getAverageRawFitness() const;
    
    /**
     * Scales the fitness of all individuals with at least one group
     * according to the fitness of the best group it belongs to.
     */
    void scaleIndividualContribution();

public: // Population methods
  /**
   * Creates the individuals of the population and the groups of them
   */
  virtual void setupPopulation() override;

  /**
   * Evaluates all individuals without a fitness value. Fitnesses of clone individuals are scaled. It also evaluates the fitness of the groups
   * @throws std::exception
   */
  virtual void evaluateAndHandleClones();

  /**
   * Increments the ages of individuals and groups
   * @throws std::exception
   */
  virtual void age();

  virtual void showStatistics(void) const;
  virtual void dumpStatistics(std::ostream& output) const;
  virtual void dumpStatisticsHeader(std::ostream& output) const;

  virtual GroupPopulationParameters& getParameters() noexcept { return m_parameters; }
  virtual const GroupPopulationParameters& getParameters() const noexcept { return m_parameters; }
  
    virtual void discardFitnessValues();
    
    virtual void dumpAllCandidates();

public: // static fields
  /** Type of this xml element */
  static const std::string XML_SCHEMA_TYPE;

public: // Xml interface

  virtual const std::string& getType() const { return XML_SCHEMA_TYPE; }
    /**
     * Parses the groups on element and adds them to the population
     */
    void parseGroups(const xml::Element& element);
    virtual void writeInnerXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);

};

}

}

#endif
