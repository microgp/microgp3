/***********************************************************************\
|                                                                       |
| GEIndividual.h                                                        |
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
 * @file GEIndividual.h
 * Definition of the GEIndividual class.
 * @see GEIndividual.cc
 * @see GEIndividual.xml.cc
 */

#ifndef HEADER_UGP3_CORE_GEINDIVIDUAL
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_GEINDIVIDUAL

#include "Individual.h"

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

class Group;
class GroupPopulation;

/**
 * @class GEIndividual
 * Represents an Individual in the group evolution
 */
class GEIndividual : public Individual
{
  private:
    /** Groups that contains this individual */
    std::vector<Group*> m_groups;
    /** Contribution to each of this groups */
    std::vector< std::vector<double> > m_groupContributions;
    
    ScaledFitness m_fitness;
    
  public: //Constructors and destructors
    /** 
     * Constructor of the class. Instantiates a GEIndividual
     * @param population where the individual exists
     * @throws Any exception
     */
    GEIndividual(const Population& population);

    /** 
     * Constructor of the class. Instantiates a GEIndividual
     * @param birth Generation in which the individual has been created
     * @param geneticOperator Genetic operator that generated this individual
     * @param parents Parents used to get the individual
     * @param population where the individual exists
     * @throws Any exception
     */
    GEIndividual(
        unsigned long birth,
        const std::string& geneticOperator,
        const std::vector<std::string>& parents,
        const Population& population);

    /** 
     * Clone constructor. Creates a new GEIndividual with the same information that the one specified.
     * @param individual Individual to take the information from
     */
    GEIndividual(const GEIndividual& individual);

    /** 
     * Destructor of the class
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual ~GEIndividual();

public:
    const GroupPopulation& getPopulation() const;
      
    /** 
     * Sets a new group where the individual is contained
     * @param group Group to add
     * @throws Any exception. ugp3::ArgumentNullException if a null pointer is passed by argument, ugp3::ArgumentException if the group already exists or there is another group with the same id.
     */
    void addGroup(Group* group);

    /** 
     * Removes the group of the list of groups in the individual
     * @param group Group to remove
     * @throws Any exception. ugp3::ArgumentNullException if a null pointer is passed by argument, ugp3::Exception if the group is not found
     */
    void removeGroup(Group* group);
    
    /** 
     * Adds the group to the list of groups in the individual, if it is not present. If it is, does nothing.
     * @param group Group to add
     */
    bool safeAddGroup(Group* group) noexcept;
    
    /** 
     * Removes the group of the list of groups in the individual, if it is found. If not, does nothing.
     * @param group Group to remove
     */
    bool safeRemoveGroup(Group* group) noexcept;

    /** 
     * Returns the number of the groups where the individual exists
     * @returns unsigned int The number of groups where the individual exists
     */
    std::size_t getGroupsCount() const { return m_groups.size(); }
    
    /**
     * Returns true iff the individual belongs to the given group.
     */
    bool belongsTo(Group* group) {
        return std::find(m_groups.begin(), m_groups.end(), group) != m_groups.end();
    }

    /** 
     * Returns the groups where the individual exists
     * @returns const vector<Group*>& The groups where the individual exists
     */
    const std::vector<Group*>& getGroups() const { return m_groups; }
    const std::vector<Group*> getGroupsCopy() const { return m_groups; }
    
    /**
     * Returns the best group to which this individual belongs,
     * as defined by GroupPopulation::compareForSelection().
     * FIXME maybe use GroupPopulation::compareBestWorst() (raw fitness) ?
     * 
     * This function must only consider groups with a valid fitness
     * (i.e. groups that were not created during the current generation)
     * because the function generateSonsGroups will include individuals
     * from the current generation into newly created groups.
     * FIXME specify somewhere that during the generation, an individual can
     * belong to invalid groups.
     * 
     * This function might return nullptr.
     */
    Group* getBestGroup() const;
    
    /**
     * Sets the contribution to the given group and recomputes the fitness based on
     * the best of all contributions.
     */
    void setGroupContribution(Group* group, std::vector<double>&& contribution);

    using Individual::clone;
    virtual std::unique_ptr<Individual> clone() const;
    
    /**
     * Intercept death events to remove the individual from groups.
     */
    virtual void setState(State state);
    
#if 0
    // NOTE no need to zombify now that individuals can live without a group
    /**
     * Should be called on every change in the groups of this individual
     * (group added, group removed or group state change). Will check
     * that the individual belongs to at least one live group. If not,
     * the individual will be zombified.
     */
    void maybeZombify();
    
    /**
     * Same as above. Should be called after the merging of the new generation,
     * to unzombify this individual if it has been added to new live groups.
     */
    void maybeUnzombify();
#endif
    
    virtual const std::string getDisplayTypeName() const;
    
    virtual const ScaledFitness& getFitness() const noexcept { return m_fitness; }
    virtual ScaledFitness& getFitness() noexcept { return m_fitness; }

  public:
    /** Type of this xml element */
    static const std::string XML_TYPE;
    
    virtual const std::string& getType() const { return XML_TYPE; }
    virtual void writeInnerXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
};

template <> struct TypeName<GEIndividual> { static constexpr const char* name = "GE individual"; };

}

}

#endif
