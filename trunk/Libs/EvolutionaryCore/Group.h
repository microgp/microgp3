/***********************************************************************\
|                                                                       |
| Group.h                                                               |
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
 * @file Group.h
 * Definition of the Group class.
 * @see Group.cc
 * @see Group.xml.cc
 */

#ifndef HEADER_UGP3_CORE_GROUP
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_GROUP

#include "XMLIFace.h"
#include "IComparable.h"
#include "InfinityString.h"
#include "Lineage.h"
#include "Fitness.h"
#include "EvolutionaryAlgorithm.h"
#include "CandidateSolution.h"
#include "ScaledFitness.h"

using namespace std;

namespace ugp3 {
namespace core {

class GroupPopulation;
class GEIndividual;

/** 
 * @class Group
 * Represents a group of individuals.
 */
class Group : public CandidateSolution,
public ICloneable<Group>,
public MessageHolder
{
    
private:
    static InfinityString idCounter;
    static InfinityString temporaryIdCounter;
    
    /** Individuals in this group */
    std::vector<GEIndividual*> m_individuals;
    
    SpecificLineage<Group> m_lineage;
    
    /// Check that the list of groups does not contain twice the same individual.
    bool checkNoDuplicateIndividual() const;
    
    // TODO Enhanced fitness for fitness sharing
    ScaledFitness m_fitness;

public:
    /** 
     * Sets the idCounter for the groups
     * @param value The id to set
     */
    static void setGroupCounter(const std::string& value);

    /** 
     * Returns the actual id for the groups
     * @return string The current id of the groups
     */
    static std::string getGroupCounter();

public: 
    /** 
     * Using this constructor, this group will get an id out of the normal
     * group id pool and is expected to be destroyed before the
     * end of the generation. Used to wrap individuals in groups
     * just for evaluation.
     */
    Group(unsigned long birth, const GroupPopulation &population, bool temporary);
    
    /** 
     * Constructor of the class. Creates a new group with the specified parameters.
     */
    Group(unsigned long birth, const GroupPopulation &population);

    /** 
     * Constructor of the class. Creates a new group with the specified parameters.
     */
    Group(const xml::Element& element, const GroupPopulation& population);

    /** 
     * Constructor of the class. Creates a copy of the specified group but replacing the old individuals for the new ones.
     */
    Group(const Group* g, const std::vector< GEIndividual* >& oldIndividuals, const std::vector< GEIndividual* >& newIndividuals);

    /** 
     * Destructor of the class. Frees the memory by removing all individuals in the group.
     */
    virtual ~Group();

    /** 
     * Checks if the group contains an individual
     * @returns bool True if the individual is in the group, false otherwise
     * @throws Any exception. ugp3::ArgumentNullException if a null pointer is passed by argument
     */
    bool contains(const GEIndividual* individual) const;


    /** 
     * Adds an individual to the group. Invalidates fitness and hash codes.
     * @param individual Individual to add
     * @returns bool True if the individual has been added successfully, false otherwise
     * @throws Any exception. ugp3::ArgumentNullException if a null pointer is passed by argument
     */
    bool addIndividual(GEIndividual* individual);

    /** 
     * Removes an individual from the group. Invalidates fitness and hash codes.
     * @param individual Individual to remove
     * @returns bool True iff the individual was in the group.
     * @throws Any exception. ugp3::ArgumentNullException if a null pointer is passed by argument.
     */
    // TODO harmonize with individual.removeGroup / safeRemoveGroup
    bool removeIndividual(GEIndividual* individual);
    
    /**
     * Removes the given clone from the group and substitutes it with the given master.
     * Invalidates fitness and hash codes only if the master was already present in the group.
     */
    void substituteClonedIndividual(GEIndividual* clone, GEIndividual* master);

    /** 
     * Writes the code of the individuals in differents files and writes a file named filename with the names of these files
     * @param filename Name of the file to write the code
     * @throws Any exception. ugp3::Exception if the file can't be opened
     */
    virtual void toCode(const string& fileName, std::vector<std::string>* outfiles = nullptr) const;
    virtual void outputLineage(std::ostream& lineageFile,
                       unsigned int recursion, const std::string& indent = "") const;

    /** 
     * Removes the individuals code files created with the method toCode
     */
    void removeIndividualsCodeFiles(std::string fileName) const;

    /**
     * Returns an individual of the group randomly
     * @returns GEIndividual* An individual of the group.
     */
    GEIndividual* getRandomIndividual();

    /** 
     * Returns the individuals of the group
     * @returns std::vecotr<GEIndividual*> Individuals of the group
     */
    const std::vector<GEIndividual*>& getIndividuals() const { return m_individuals; }

    /** 
     * Returns the individuals of the group
     * @returns std::vecotr<GEIndividual*> Individuals of the group
     */
    std::vector<GEIndividual*>& getIndividuals() { return m_individuals; }

    /** 
     * Returns the number of individuals in the group
     * @returns unsigned int Number of individuals
     */
    std::size_t getIndividualCount() const { return m_individuals.size(); }
    
    /**
     * Computes a string that describes the group.
     * It contains the concatenated name of group members in alphabetical
     * order separated by spaces.
     */
    virtual void computeNormalizedPhenotype(string& code) const;
    
    const GroupPopulation& getPopulation() const;
    
    virtual const Message& getMessage() const { return MessageHolder::getMessage(); };
    virtual void computeMessage(Message& message) const;
    
    virtual unique_ptr<Group> clone() const;
    virtual bool isGenotypeEqual(const CandidateSolution& other) const {
        // FIXME does noncoding DNA make sense for groups?
        // For groups, noncoding DNA does not make sense
        // Hence phenotype-equality and genotype-equality are equivalent
        return isNormalizedPhenotypeEqual(other);
    }
    
    virtual const string getDisplayTypeName() const;
    
    virtual const SpecificLineage<Group>& getLineage() const { return m_lineage; }
    virtual SpecificLineage<Group>& getLineage() { return m_lineage; }
    
    // TODO fitness sharing?
    virtual ScaledFitness& getFitness() { return m_fitness; }
    virtual const ScaledFitness& getFitness() const { return m_fitness; }
    
    virtual void setState(State state);
    
public: // IValidable interface
    virtual bool validate() const;
        
public: // Hashable interface
    virtual hash_t calculateHashCode(Purpose purpose) const;
    
   

public: // Xml interface
    static const std::string XML_NAME;
    static const std::string XML_TYPE;
    static const std::string XML_CHILD_INDIVIDUALS;
    static const std::string XML_CHILD_INDIVIDUAL;
    static const std::string XML_ATTRIBUTE_REF;
    
    virtual const string& getXmlName() const { return XML_NAME; }
    virtual const string& getType() const { return XML_TYPE; }
    virtual void writeInnerXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
};

template <> struct TypeName<Group> { static constexpr const char* name = "group"; };

}
}

#endif