/***********************************************************************\
|                                                                       |
| MOPopulation.h                                                        |
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

#ifndef MOPOPULATION_H_
#define MOPOPULATION_H_

// headers from this module
#include "IndividualPopulation.h"
#include "MOIndividual.h"
#include "MOPopulationParameters.h"

/* Individuals are divided in levels, such as level n is a Pareto front if level n-1 
 * is deleted.
 * Within each level, individuals are sorted by entropy, and first eliteCardinality 
 * individuals in level 0 are heroes */
   
namespace ugp3 
{
    
namespace core 
{


// Population for Multiobjective evolution
class MOPopulation : public SpecificIndividualPopulation<MOIndividual>
{

private: // fields
    MOPopulationParameters m_parameters;
    
    /** The entropy of the whole population.
        Used for statistics only. */
    int m_maxLevel = 0;	// # of levels = maxLevel + 1 (levels start with 0)	
    bool m_firstRecovery = false;
    bool m_strengthValid = false;

private: // methods
    std::vector<double> computeLevelAverageFitness(int n) const;
    /** Deletes a single individual from every container in the
        population. */
    // Remove a single individual from the population.
    void removeIndividual(unsigned int index);

protected: // virtual methods
    virtual void selectNewZombifiableCandidates();
    /** Removes dead individuals and performs a selection in order to
        keep only the best 'mu' individuals. 
        When removing a dead clone, the fitness of its other 
        clones is rescaled properly.*/
    virtual void slaughtering();
    /** It evaluates all individuals without a fitness value.
        Fitnesses of clone individuals are scaled.*/
    virtual void evaluateAndHandleClones();
	/* This is not really applicable here but is in Population.h. 
	 * Sorts the individuals on each level by entropy  */
    virtual void age();
    
    void computeLevels();		// computes the level for each individual
    void computePerceivedStrength(); // makes all possible comparisons, updates individual strength
    
    virtual void prepareForCommit();
    
    virtual void handleClone(CandidateSolution& master, CandidateSolution& clone, unsigned int number, unsigned int total);
    
    virtual bool checkMaximumFitnessReached();
    virtual void updateSteadyStateGenerations();
    
    /**
     * Equivalent of LevelComparatorWithStrength
     * FIXME is it ok? It will be used for all kinds of selections
     */
    virtual bool compareForSelection(const CandidateSolution* a, const CandidateSolution* b) const;
    
    /**
     * Compare levels and then strength (same as compareForSelection).
     */
    virtual bool compareOperatorPerformance(const CandidateSolution* a, const CandidateSolution* b) const {
        return compareForSelection(a, b);
    }
    
    // TODO define all these functions if the default implemented in Population.h is not satisfying
    // maybe using all the comparators from below
    // virtual bool compareClones(const CandidateSolution* a, const CandidateSolution* b) const;
    // virtual bool compareForFitnessHole(const CandidateSolution* a, const CandidateSolution* b) const;
    // virtual bool compareRawBestWorst(const CandidateSolution* a, const CandidateSolution* b) const;
    // virtual bool compareScaledBestWorst(const CandidateSolution* a, const CandidateSolution* b) const;
    
public: // constructors and destructors
    MOPopulation(
		const EvolutionaryAlgorithm& parent
        );
    
    MOPopulation(
        std::unique_ptr<MOPopulationParameters> parameters,
        const EvolutionaryAlgorithm& parent
        );
    
    virtual ~MOPopulation();
    
public: // static fields

public: // virtual members
    virtual void commit();	
    
    /** Adds a fresh individual to both population and clone map.*/
    virtual void addIndividual(std::unique_ptr<Individual> individual);
    virtual void showStatistics(void) const;
    virtual void dumpStatistics(std::ostream& output) const;
    virtual void dumpStatisticsHeader(std::ostream& output) const;
    
public: // getters and setters
    unsigned int getLevelSize(int n) const; 
    int                    	    getMaxLevel() const;
    void                            setMaxLevel(int n);
    MOPopulationParameters& getParameters() noexcept { return m_parameters; }
    const MOPopulationParameters& getParameters() const noexcept { return m_parameters; }
    void 			    setFirstRecovery(bool);
    
    // In this population, the best and worst individuals are defined with only one measure.
    // We use only the best/worst RAW members
    virtual const MOIndividual* getBestScaledIndividual() const { return m_bestRawIndividual; }
    virtual const MOIndividual* getWorstScaledIndividual() const { return m_worstRawIndividual; }
    const MOIndividual* getBestIndividual() const { return m_bestRawIndividual; }
    const MOIndividual* getWorstIndividual() const { return m_worstRawIndividual; }
    void setBestIndividual(MOIndividual* ind) { m_bestRawIndividual = ind; }
    void setWorstIndividual(MOIndividual* ind) { m_worstRawIndividual = ind; }

public: // Xml interface
    static const std::string XML_SCHEMA_TYPE;
    static const std::string XML_CHILD_ELEMENT_MAXLEVEL;
    static const std::string XML_ATTRIBUTE_VALUE;
    
    virtual const std::string& getType() const { return XML_SCHEMA_TYPE; }
    virtual void writeInnerXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
};
	
/** Compares two individuals considering their entropy and their age.
    If two individuals have the same entropy, the youngest is better. 
*/
struct EntropyComparatorWithBirth 
{	
    bool operator()(const Individual* i1, const Individual* i2)
    {
    	const MOIndividual* ind1 = dynamic_cast<const MOIndividual*>(i1);
    	const MOIndividual* ind2 = dynamic_cast<const MOIndividual*>(i2);

        Assert(ind1 != nullptr);
        Assert(ind2 != nullptr);

	int result = ind1->getDeltaEntropy().compareTo(ind2->getDeltaEntropy());
        if (result == 0)
            return ind1->getBirth() > ind2->getBirth();

        return (result > 0);
    }
};

/** Compares two individuals considering their level and their entropy.
    If two individuals have the same level, the one with higher entropy is the better.
    If they have also the same entropy, then the best is the youngest. 
    N.B.: higher level number = worse level!!!
*/
struct LevelComparatorWithEntropy 
{
	bool operator()(const Individual* i1, const Individual *i2)
	{
		const MOIndividual* ind1 = dynamic_cast<const MOIndividual*>(i1);
		const MOIndividual* ind2 = dynamic_cast<const MOIndividual*>(i2);

		Assert(ind1 != nullptr);
		Assert(ind2 != nullptr);

		if(ind1->getLevel() == ind2->getLevel()) 
		{
    			int result = ind1->getDeltaEntropy().compareTo(ind2->getDeltaEntropy());
    			if (result == 0)
    				return ind1->getBirth() > ind2->getBirth();
			else 
				return (result > 0);
		} 
		else
            		return (ind1->getLevel() < ind2->getLevel());
	}
};

/** Compares two individuals considering their level and their age.
    If two individuals have the same level, then the best is the youngest.
    N.B.: higher level number = worse level!!!
*/
struct LevelComparatorWithBirth {
    bool operator()(const Individual* i1, const Individual *i2)
    {
        const MOIndividual* ind1 = dynamic_cast<const MOIndividual*>(i1);
        const MOIndividual* ind2 = dynamic_cast<const MOIndividual*>(i2);

        Assert(ind1 != nullptr);
        Assert(ind2 != nullptr);

        if(ind1->getLevel() == ind2->getLevel()) 
	{
            return ind1->getBirth() > ind2->getBirth();
        } 
	else
            return (ind1->getLevel() < ind2->getLevel());
    }
};

/** Compares two individuals considering their level.
    N.B.: higher level number = worse level!!!
*/
struct LevelComparator 
{
    bool operator()(const Individual* i1, const Individual *i2)
    {
        const MOIndividual* ind1 = dynamic_cast<const MOIndividual*>(i1);
        const MOIndividual* ind2 = dynamic_cast<const MOIndividual*>(i2);

        Assert(ind1 != nullptr);
        Assert(ind2 != nullptr);

        return (ind1->getLevel() < ind2->getLevel());
    }
};

/** Compares two individuals considering their level and their strength.
    If two individuals have the same level, the one with higher perceived strength is the better.
    If they are equally strong the one with higher entropy is preferred.
    If they have also the same entropy, then the best is the youngest.
    N.B.: higher level number = worse level!!!
*/
struct LevelComparatorWithStrength 
{
    bool operator()(const CandidateSolution* i1, const CandidateSolution *i2)
    {
        const MOIndividual* ind1 = dynamic_cast<const MOIndividual*>(i1);
        const MOIndividual* ind2 = dynamic_cast<const MOIndividual*>(i2);

        Assert(ind1 != nullptr);
        Assert(ind2 != nullptr);

        if(ind1->getLevel() == ind2->getLevel()) 
	{
            if(ind1->getPerceivedStrength() == ind2->getPerceivedStrength())
	    {
                int result = ind1->getDeltaEntropy().compareTo(ind2->getDeltaEntropy());
                if (result == 0)
                    	return ind1->getBirth() > ind2->getBirth();
                else 
			return (result > 0);
            }
	    else
                return (ind1->getPerceivedStrength() > ind2->getPerceivedStrength());
        } 
	else
            return (ind1->getLevel() < ind2->getLevel());
    }
};


inline int MOPopulation::getMaxLevel() const
{
	return m_maxLevel;
}	

inline void MOPopulation::setMaxLevel(int n)
{
	m_maxLevel = n;
}	

inline void MOPopulation::setFirstRecovery(bool value)
{
	this->m_firstRecovery = value;
}

}

}

#endif /*MOPOPULATION_H_*/
