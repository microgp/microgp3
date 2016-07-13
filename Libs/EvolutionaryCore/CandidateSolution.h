/***********************************************************************\
|                                                                       |
| CandidateSolution.h                                                   |
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
 * @file CandidateSolution.h
 * 
 */

#ifndef HEADER_UGP3_CORE_CANDIDATESOLUTION
#define HEADER_UGP3_CORE_CANDIDATESOLUTION

#include <XMLIFace.h>
#include <IComparable.h>
#include <ICloneable.h>
#include <Hashable.h>
#include <IString.h>
#include <IValidable.h>
#include <Fitness.h>
#include <DeltaEntropy.h>
#include <InfinityString.h>
#include "TypeName.h"
#include "Entropy.h"
#include "ScaledFitness.h"

#include <map>

namespace ugp3 {
namespace core {
    
class Population;
class Lineage;

class CandidateSolution :
    public xml::XMLIFace,
    public Hashable,
    public IString,
    public IValidable
{
public:
    typedef enum {
        DEAD   = 0,
        ZOMBIE = 1,
        ALIVE  = 2,
        HERO   = 3,
        STATE_COUNT,
    } State;

private:
    // Individual id
    std::string m_id;
    /** Allopatric tag for this individual */
    std::string m_allopatricTag;
    
    // State of the candidate. Never set it directly,
    // always use setState() so that subclasses can intercept the events
    State m_state;
    // Generation in which the individual has been created
    unsigned long m_birthGen;
    // Generation in which the individual dies
    unsigned long m_deathGen;
    // Actual age of the individual
    unsigned long m_age;

    // Normalized phenotype of the individual. If empty, means it should be computed.
    mutable std::string m_normalizedPhenotype;
    
    // Population where the individual exists
    const Population* m_population;
    // Fitness of the individual according to the evaluator
    Fitness m_rawFitness;
    
    // Contribution of this individual to global entropy
    DeltaEntropy m_deltaEntropy;
   
protected:
    
    static const std::string XML_ATTRIBUTE_ID;
    static const std::string XML_ATTRIBUTE_TYPE;
    static const std::string XML_ATTRIBUTE_ALLOPATRIC_TAG;
    static const std::string XML_ATTRIBUTE_DELTAENTROPY;
    static const std::string XML_CHILD_ELEMENT_LIFE;
    static const std::string XML_ATTRIBUTE_STATE;
    static const std::string XML_ATTRIBUTE_BIRTH;
    static const std::string XML_ATTRIBUTE_DEATH;
    static const std::string XML_ATTRIBUTE_AGE;
    
    /** 
     * Constructor of the class. Instantiates an individual with default paramters(next id, birth=0, dead=maximum, age=0, fitnesses=0, no lineage) and assigns it to the population specified.
     * @param population where the individual exists
     */
    CandidateSolution(unsigned int birth, const Population& population, const std::string& id);
    CandidateSolution(const Population& population, const std::string& id);
    CandidateSolution(const CandidateSolution& other, const std::string& id);
    
    /**
     * Write the normalized phenotype of the candidate solution into the given string.
     * Will be used for evaluation caching.
     */
    virtual void computeNormalizedPhenotype(std::string& code) const = 0;
    
    /**
     * Subclasses must call this function when the change their DNA.
     */
    void invalidateNormalizedPhenotype() { m_normalizedPhenotype.clear(); }
    
public:
    virtual ~CandidateSolution();

    /** Static count of the allopatric tag. */
    static InfinityString allopatricTagCounter;
    static void setAllopatricTagCounter(const std::string& value);
    
    const std::string& getId() const { return m_id; }
    
    const std::string& getAllopatricTag() const { return m_allopatricTag; }
    void setAllopatricTag(const std::string& tag) { m_allopatricTag = tag; }
    
    /**
     * Semantics: an individual is either dead, zombie or alive.
     * Hero is a subset of alive.
     */
    bool isDead()   const { return m_state == DEAD; }
    bool isZombie() const { return m_state == ZOMBIE; }
    bool isAlive()  const { return m_state == ALIVE || m_state == HERO; }
    bool isHero()   const { return m_state == HERO; }

    /**
     * Raw setting of the state.
     * Subclasses can override this method to process the event,
     * but they should respect the decision.
     * @see setDeath().
     */
    virtual void setState(State state);
    State getState() const { return m_state; }
    
    unsigned long getBirth() const { return m_birthGen; }
    unsigned long getDeath() const { return m_deathGen; }
    unsigned long getAge() const { return m_age; }
    
    /**
     * Smart killing of candidates.
     * This method can zombify candidates instead of killing them.
     * To kill unconditionnally use setState(DEAD)
     * @see setState
     */
    void setDeath(unsigned int deathDate);
    
    const Population& getPopulation() const { return *m_population; }
    void setPopulation(const Population& population) { m_population = &population; }
    
    /**
     * This fitness is exactly what the evaluator returns.
     */
    Fitness& getRawFitness() { return m_rawFitness; }
    const Fitness& getRawFitness() const { return m_rawFitness; }
    
    /**
     * This fitness value is used for comparisons, selections, tournaments,
     * slaughtering... it should be some kind of scaled fitness.
     */
    virtual ScaledFitness& getFitness() = 0;
    virtual const ScaledFitness& getFitness() const = 0;
    
    const DeltaEntropy& getDeltaEntropy() const { return m_deltaEntropy; }
    DeltaEntropy& getDeltaEntropy() { return m_deltaEntropy; }
    
    virtual const Lineage& getLineage() const = 0;
    virtual Lineage& getLineage() = 0;

    virtual const Message& getMessage() const = 0;
    
    virtual void setFitnessStructure(Fitness fitness);
    
    /** 
     * TODO redocument this and change name/parameters
     * Called when the candidate moves to the next generation.
     * @param age If true, increments the age of the candidate
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual void step(bool age);
    
    virtual const std::string toString() const;
    
    /** 
     * Writes the code of this candidate in differents files and append their
     * names to the outfiles vector.
     * @param filename Name of the file to write the code
     * @param outfiles Names of the written files
     */
    virtual void toCode(const std::string& fileName, std::vector<std::string>* outfiles = nullptr) const;
    
    /** 
     * Writes the lineage of this candidate in differents files and append their
     * names to the outfiles vector.
     * @param filename Name of the file to write the lineage
     * @param outfiles Names of the written files
     * @param recursion Levels of lineage to recurse through (0 = none, 1 = parents...)
     * @param indent Formatting help for recursion, do not use.
     */
    virtual void outputLineage(std::ostream& lineageFile,
        unsigned int recursion, const std::string& indent = "") const;
    
    /**
     * Individual: returns the external representation of the individual with relabelled ids,
     * so that two individuals with the same structure have the same normalized
     * phenotype.
     * 
     * Group: returns the list of individual names.
     * 
     * This is used to perform caching and deduplication in the evaluator,
     * since the only thing that matters to the evaluator is the external representation.
     * We can't use the genotype/hash values for that purpose because in the presence of
     * noncoding DNA (i.e. parts of the genotype that have no impact on the phenotype),
     * two individuals with different genotypes can have the same phenotype (so their
     * evaluation should be deduplicated).
     */
    const std::string& getNormalizedPhenotype() const;
    
    /**
     * Check whether two candidates have the same normalized phenotype.
     * @see getNormalizedPhenotype()
     */
    bool isNormalizedPhenotypeEqual(const CandidateSolution& other) const {
        return getNormalizedPhenotype() == other.getNormalizedPhenotype();
    }
    
    /**
     * Compares the genotype of two candidates. This is used for clone detection.
     * This function should take noncoding DNA into account.
     * @see getNormalizedPhenotype()
     */
    virtual bool isGenotypeEqual(const CandidateSolution& other) const = 0;
    
    /**
     * Return a displayable string that identifies the type
     * of candidate being considered. Used for debug/information
     * messages.
     */
    virtual const std::string getDisplayTypeName() const = 0;
    
public:
    virtual const std::string& getType() const = 0;
    virtual const std::string& getXmlName() const = 0;
    virtual void writeInnerXml(std::ostream& output) const = 0;
    virtual void readXml(const xml::Element& element);
    virtual void writeXml(std::ostream& output) const final;
    
public:
    /**
     * Hash function object usable as template parameter for STL containers.
     */
    struct CloneHash {
        std::size_t operator()(CandidateSolution* a) const {
            return a->getHashCode(GENOTYPE);
        }
    };
    
    /**
     * Equality function object usable as template parameter for STL containers.
     */
    struct CloneEquality {
        bool operator()(CandidateSolution* a, CandidateSolution* b) const {
            return a->isGenotypeEqual(*b);
        }
    };
    
    /**
     * Order-by-id functor for use with STL sort.
     * This is needed in order to get deterministic results for algorithms
     * that work on lists of candidates.
     */
    struct OrderById {
        bool operator()(CandidateSolution* a, CandidateSolution* b) {
            return a->getId() < b->getId();
        }
    };
};

template <> struct TypeName<CandidateSolution> { static constexpr const char* name = "candidate solution"; };

}
}

#endif // HEADER_UGP3_CORE_CANDIDATESOLUTION
