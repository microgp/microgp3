/***********************************************************************\
|                                                                       |
| Population.h                                                          |
|                                                                       |
| This file is part of MicroGP v3 (ugp3)                                |
| http://ugp3.sourceforge.net/                                          |
|                                                                       |
| Copyright (c) 2006-2016 Giovanni Squillero                            |
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
 * @file Population.h
 * Definition of the Population class.
 * @see Population.cc
 * @see Population.xml.cc
 * @see Population.instantiate.cc
 */

#ifndef HEADER_UGP3_CORE_POPULATION
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_POPULATION

/** Old threshold, used in tentative experiments to reset sigma */
#define SIGMA_THRESHOLD 0.0001
/** The self-adapting parameters will be changed each EPOCH generations */
#define EPOCH 1

#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <forward_list>

// headers from shared module
#include "XMLIFace.h"
#include "Progress.h"
#include "Log.h"

// headers from this module
#include "CandidateSolution.h"
#include "PopulationParameters.h"
#include "Entropy.h"
#include "OperatorSelector.h"
#include "EnhancedIndividual.h"
#include "MOIndividual.h"
#include "GEIndividual.h"
#include "Group.h"

/**
 * ugp3 namespace
 */
namespace ugp3
{

// forward declaration
namespace constraints
{
	class Constraints;
}

/**
 * ugp3::core namespace
 */
namespace core
{

// forward declaration
class EvolutionaryAlgorithm;
class Individual;
class PopulationParameters;
class GeneticOperator;

/**
 * @class Population
 * Static class that provides methods to instantiate and handle a population of ugp.
 * @see PopulationParameters
 * @see EnhancedPopulation
 * @see MOPopulation
 */
class Population : public xml::XMLIFace
{
private:
    //Algorithm that makes the evolution
    const EvolutionaryAlgorithm& algorithm;
    
    // Best fitness from the previous generation, for steady-state detection.
    Fitness m_previousMaxFitness;
    
protected:
    /** Actual generation of the population */
    unsigned int generation;
    /** Name of the population */
    std::string name;
    
    unsigned int m_steadyStateGenerations;
    
    // The entropy of the whole population. Used for statistics only.
    double m_entropy;
    
    // A list of candidates that must be kept in the population, dead or alive.
    std::vector<const CandidateSolution*> m_bloodMagicWaitingList;

protected: // constructors
    /** 
     * Constructor of the class. Instantiates a population with the specified algorithm, generation=0 and default parameters.
     * @param parent Algorithm to use in the evolution
     */
    Population(
        const EvolutionaryAlgorithm& parent);

    /** 
     * Constructor of the class. Instantiates a population with the specified algorithm, generation and parameters.
     * @param parameters Parameters of the population
     * @param generation Actual generation of the population
     * @param parent Algorithm to use in the evolution
     */
    Population(
        std::unique_ptr<PopulationParameters> parameters,
        unsigned int generation,
        const EvolutionaryAlgorithm& parent);
    
public: // constructors and destructors
    /** 
     * Destructor of the class. Does nothing. Subclasses must free the memory
     * by deleting all candidates themselves.
     */
    virtual ~Population() {}

public: // static methods
    /** 
     * Instantiates a Population object by reading the data in the element and sets the parent parameter as the evolutionary algorithm to use
     * @param element Element for read the population information
     * @param parent The evolutionary algorithm to use
     * @throws Any exception. xml::SchemaException if the element specified is not 'population'
     */
    static std::unique_ptr<Population> instantiate(
        const xml::Element& element,
        const EvolutionaryAlgorithm& parent
        );

    /** 
     * Instantiates a Population object of the specified type and sets the parent parameter as the evolutionary algorithm to use
     * @param parent The evolutionary algorithm to use
     * @param type Type of the population. Allowed: EnhancedPopulation::XML_SCHEMA_TYPE, MOPopulation::XML_SCHEMA_TYPE
     * @throws Any exception. Exception if the type specified is unknown
     */
    static std::unique_ptr<Population> instantiate(
        const EvolutionaryAlgorithm& parent,
        const std::string& type
        );
    
    /** 
     * Instantiates a Population object from the specified parameter file
     * @param evolutionaryAlgorithm The evolutionary algorithm to use
     * @param fileName Parameter file of the population
     * @throws Any exception. Exception if the type specified in the parameters is unknown
     */
    static std::unique_ptr<Population> fromParametersFile(
        const EvolutionaryAlgorithm& algorithm,
        const std::string& fileName);

    /** 
     * Instantiates a Population by reading the xml file specified and sets the evolutionaryAlgorithm as the evolutionary algorithm to use
     * @param xmlFileName Xml file with the information of the population
     * @param evolutionaryAlgorithm The evolutionary algorithm to use
     * @throws Any exception. xml::SchemaException if the element specified is not 'population'
     */
    static std::unique_ptr<Population> fromFile(
        const std::string& xmlFileName,
        const EvolutionaryAlgorithm& evolutionaryAlgorithm
        );

private: // methods
    // Population& operator=(const Population& population);
    
protected: // step() internals
    /**
     * Apply the given operator to the population and return any valid children
     * in a vector. If the returned vector is empty, the operator has failed.
     */
    virtual std::vector<CandidateSolution*> applyOperator(
        CallData* callData, const OperatorSelector::Result& selected);
    
    /**
     * Merge the candidates of the given vector into the population.
     * The vector can contain individuals and groups, for group evolution.
     */
    virtual void mergeNewGeneration(const std::vector<CandidateSolution*>& newGeneration) = 0;
    
public: // step() internals
    /**
     * Invalidate all fitness values.
     */
    virtual void discardFitnessValues() = 0;
    
    /** 
     * Evaluates the individuals of the population.
     * Subclasses must use the detectClones() functions themselves at that point.
     * @throws Any exception
     */
    virtual void evaluateAndHandleClones() = 0;
    
    /**
     * Check that all fitness values of all non dead candidates of the
     * population are valid, using assertions.
     * Will be called only in debug mode.
     */
    virtual void checkFitnessValidity() = 0;
    
protected: // step() internals
    /**
     * Handle clones.
     * Subclasses should use this function to implement fitness scaling.
     * The master and/or its clones might be zombies.
     * @param master Master clone as defined by the function compareClones()
     * @param clone The master again if number == 0 or one of its clones
     * @param number 0 for master, 1 for the first clone to (total - 1) for the last,
     *               in the order defined by the function compareClones()
     * @param total number of clones, master included
     */
    virtual void handleClone(CandidateSolution& master, CandidateSolution& clone,
                             unsigned int number, unsigned int total) = 0;
    
    /** 
     * Updates the operators statistics with the newGeneration and
     * updates the self-adapting parameters of the population and
     * the genetic operators in base to these statistics.
     * The vector can contain groups and individuals, in case of group evolution.
     * WARNING the vector will contain some dead individuals, with invalid fitness.
     * @param newGeneration Generation of individuals for updates the statistics
     * @throws Any exception
     */
    virtual void updateOperatorStatistics(const std::vector< CandidateSolution* >& newGeneration) = 0;
    
    /** 
     * Updates the sigma value in base to the genetic operators performance in the evolution
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual void updateSigma();
    
    /**
     * Fill the internal vector `m_bloodMagicWaitingList' with individuals that must be saved
     * at all costs. A default implementation cand select the candidate with the best raw fitness.
     * MOPopulation can save the whole Pareto front if it makes sense.
     * WARNING the population will contain some dead individuals, with invalid fitness.
     */
    virtual void selectNewZombifiableCandidates() = 0;
    
    /** 
     * Updates the age of each individual in the population, and kill old individuals.
     * Select heroes.
     * WARNING the population will contain some dead individuals, with invalid fitness.
     * @throws Any exception
     */
    virtual void age() = 0;
    
    /** 
     * Kills and removes the worst individuals and the older.
     * Should also perform allopatric selection.
     * The slaughtering should be done only by calling `setDeath()' on individuals
     * and finally `removeCorpses()', because `setDeath()' zombifies the candidates
     * that we should keep in the population.
     * @throws Any exception
     */
    virtual void slaughtering() = 0;
    
    /**
     * Deal with zombies from the previous generation.
     * A default implementation should only keeps zombies selected by
     * `selectNewZombifiableCandidates()'.
     */
    virtual void handleZombies() = 0;
    
public: // step() internals
    /**
     * Removes dead candidates from the population.
     */
    virtual void removeDeadCandidates() = 0;
    
    /** 
     * Updates fitness sharing, delta entropy and any other metrics
     * that depend on the population as a whole, taking into account
     * the deaths/zombifications caused by the selection process that
     * just happened.
     * @throws Any exception
     */
    virtual void prepareForCommit() = 0;
    
    /** 
     * Saves the best an worst candidates in the population.
     * @throws Any exception
     */
    virtual void commit() = 0;
    
protected: // Population toolbox. Use these templates instead of duplicating code
    /**
     * Partition the range by putting all dead candidates at the begining.
     * The partition is stable.
     * @return Iterator past the end of dead candidates.
     */
    template <typename RandomAccessIterator>
    RandomAccessIterator regroupAndSkipDeadCandidates(RandomAccessIterator begin, RandomAccessIterator end);
    
    /**
     * Partition the range by putting all dead and zombie candidates at the begining.
     * The partition is stable.
     * @return Iterator past the end of dead and zombie candidates.
     */
    template <typename RandomAccessIterator>
    RandomAccessIterator regroupAndSkipNotAliveCandidates(RandomAccessIterator begin, RandomAccessIterator end);
    
    /**
     * Sorts the given range of candidates according to the compareHeroes() method,
     * resets the heroes and ages all other individuals.
     */
    template <typename RandomAccessIterator>
    void promoteHeroesAndAge(RandomAccessIterator begin, RandomAccessIterator end);
    
    /** 
     * Makes the allopatric selection in the given range: in each group
     * of candidates that share the same allopatric tag, we keep the one with
     * the best fitness (according to compareForSelection) and mark the others as dead.
     */
    template <typename RandomAccessIterator>
    void allopatricSelection(RandomAccessIterator begin, RandomAccessIterator end);
    
    /** 
     * Selects mu candidates in the given range: we keep the ones with the
     * the best fitnesses (according to compareForSelection) and mark the others as dead.
     */
    template <typename RandomAccessIterator>
    void simpleSelection(unsigned int mu, RandomAccessIterator begin, RandomAccessIterator end);
    
    /** 
     * Returns the average delta entropy of the given range.
     * @returns double The average delta entropy of the range.
     */
    template <typename ForwardIterator>
    double computeAverageDeltaEntropy(ForwardIterator begin, ForwardIterator end) const;
    
    /** 
     * Returns the average age of the given range.
     * @returns double The average age of the range.
     */
    template <typename ForwardIterator>
    double computeAverageAge(ForwardIterator begin, ForwardIterator end) const;
            
    /** 
     * Returns a vector with the average raw fitness of all candidates in the range
     * @returns vector<double> The average raw fitness of the range.
     */
    template <typename ForwardIterator>
    std::vector<double> computeAverageRawFitness(ForwardIterator begin, ForwardIterator end) const;
    
    /**
     * Requests evaluation of all not dead candidates in the given range which
     * have an invalid raw fitness, and prints nice log messages.
     * Also copies the raw fitness to the scaled fitness.
     * @return Number of candidates that were really sent to be evaluated.
     */
    template <typename RandomAccessIterator>
    std::size_t runEvaluator(RandomAccessIterator begin, RandomAccessIterator end);
    
    /**
     * Detects clones in the given range and calls the handleClones() method with a list of
     * clone individuals ordered according to the function compareClones().
     * Clones are detected using getHashCode(purpose) and isGenotypeEqual().
     * Complexity: amortized O(n) genotype comparisons with n length of range.
     */
    template <typename RandomAccessIterator>
    void detectAndHandleClones(RandomAccessIterator begin, RandomAccessIterator end);
    
    /**
     * Calls the function handleClone() for each pair (master, clone) in the given range,
     * including the pair (master, master).
     */
    template <typename ForwardIterator>
    void handleClones(ForwardIterator begin, ForwardIterator end);
    
    /**
     * Computes the entropy of the given range and the delta entropy of
     * all elements of the range. 
     * Entropy is gathered using getMessageInformation().
     * FIXME should also work for groups, maybe later
     * @return the entropy of the range.
     */
    template <typename RandomAccessIterator>
    double updateDeltaEntropy(RandomAccessIterator begin, RandomAccessIterator end);
    
    /**
     * Apply fitness sharing to the given range.
     * The SHARING_SCALING coeff of the candidates of the range will be updated.
     * Will use the two following function to compute distances
     * depending on the type of the candidates.
     */
    template <typename RandomAccessIterator>
    void shareFitness(RandomAccessIterator begin, RandomAccessIterator end);
    /**
     * Compute first the distance between the two individuals.
     * If this distance is less or equal than the appropriate radius,
     * compute the apport to sharing (1 - distance/radius), else 
     * return a negative apport.
     */
    std::tuple<double, double> computeSharingApport(Individual* a, Individual* b);
    std::tuple<double, double> computeSharingApport(Group* a, Group* b);
    
    /**
     * Removes dead candidates from the given vector of candidates.
     */
    template <typename VectorOfCandidates>
    void removeCorpses(VectorOfCandidates& candidates);
    
    /**
     * Print on LOG_VERBOSE a description of the population:
     * size, number of live/heroic/undead/dead candidates.
     */
    template <typename ForwardIterator>
    void describePopulation(const std::string& context, ForwardIterator begin, ForwardIterator end);
    
    /**
     * Checks that the hash code and normalized phenotypes are coherent
     * with the genotype equality for all couples of candidates in the range,
     * and computes the number of hash collisions in the range.
     * @return True iff everything ok
     */
    template <class RandomAccessIterator>
    bool checkHashGenoPhenoEqualitiesAndHashQuality(
        RandomAccessIterator begin, RandomAccessIterator end) const;
    
    /** 
     * Calculates a new value using the inertia.
     * @param oldValue Old value of the parameter
     * @param newValue Desired value to reach
     * @returns double The result of apply inertia.
     */
    double useInertia(double oldValue, double newValue);

public:
    /** 
     * Fills the population with random individuals to start the evolution
     */
    virtual void setupPopulation() = 0;
    
    /** 
     * Generates a new generation of the population by applying the operators to the actual population
     * NOTE: subclasses are not allowed to override this function anymore. To customize the behavior
     * they should override some of the virtuals methods defined in the section `step() internals' of
     * this header.
     */
    void step();
        
    /**
     * Return true if the evolution should stop. It checks:
     *   1. the number of generations
     *   2. the number of evaluations
     *   3. the time spent working
     *   4. external stop requests from the evaluator
     *   5. the fitness reached, @see checkMaximumFitnessReached()
     *   6. the number of steady-state generations, @see updateSteadyStateGenerations()
     */
    bool checkStopCondition();
    
    /** 
     * Should return true if the population has reached a maximum fitness.
     * The default behavior checks whether the best individual has a
     * fitness higher than getParameters().getMaximumFitness().
     * 
     * This function is only called when the user gave a maximum fitness
     * value, i.e. getParameters().getMaximumFitnessStop() is true.
     */
    virtual bool checkMaximumFitnessReached();
    
    /**
     * Should update the protected variable m_nbSteadyStateGenerations.
     * The default behaviour watches the evolution of the raw fitness of the
     * best individual.
     * 
     * This function is only called when the user gave a maximum 
     * steady-state generation number, i.e.
     * getParameters().getSteadyStateGenerationsStop() is true.
     */
    virtual void updateSteadyStateGenerations();
    
    /** 
     * NOT IMPLEMENTED 
     * @param element
     * @throws Any exception.
     */
    void reloadStatistics(const xml::Element* element);
    
    /** 
     * Writes a summary of the current population state in the ugp3::log::LOG_INFO and in the specified stream (in the stream it writes the information in xml)
     * @param output Stream to write the summary
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    virtual void showStatistics(void) const;
    virtual void dumpStatistics(std::ostream& output) const;
    virtual void dumpStatisticsHeader(std::ostream& output) const;
    
    /**
     * Return the average age of the candidates.
     */
    virtual double getAverageAge() const = 0;
    
    /**
     * Return the average raw fitness of the population.
     */
    virtual std::vector<double> getAverageRawFitness() const = 0;
    
    /** 
     * Saves this population in a xml file.
     * @param fileName Name of the xml file
     * @throws Any exception. Exception if the file can't be accessed
     */
    void save(const std::string& fileName) const;
    
    //Writes a file called "individualsInPopulation.txt" with all the information about the individuals in the population
    virtual void dumpAllCandidates() = 0;
        
public: // Comparison functions to tune some selection mechanisms
    /**
     * WARNING ABOUT THE SEMANTICS OF THE FOLLOWING FUNCTIONS
     * ======================================================
     * 
     * The comparison functions presented thereafter can be used:
     *  1.  as-is with the std::sort function to order candidates
     *      in **decreasing** order.
     *      Example:
     *          std::sort(begin, end, [&] (CandidateSolution* a, CandidateSolution* b) {
     *                  return compareWithSomeCriterion(a, b);
     *              });
     *          // Now `begin' points to the best element.
     * 
     *  2.  **inverted** with the std::min_element or std::max_element
     *      funtions.
     *      Example:
     *          std::max_element(begin, end, [&] (CandidateSolution* a, CandidateSolution* b) {
     *                  return compareWithSomeCriterion(b, a);
     *                  //                              ^ NOTE THE INVERSION
     *              });
     * 
     * The following functions and their overrides must define strict
     * *total* orderings. The standard library requires for the std::sort()
     * a strict weak order, but for the sake of determistic reproductibility
     * we enforce that the orders should be total, leaving no margin of
     * interpretation to the standard library.
     */
    
    /**
     * The following function can be used to get strict total orderings
     * using the result of an IComparable comparison.
     */
    bool useResultOrTakeOldestOrTakeFirstId(int result, const CandidateSolution* a, const CandidateSolution* b) const;
    
    /**
     * Order two clones. When this function is called with two candidates, they
     * are guaranteed to be phenotype-equal. The default implementation separates
     * live and zombie candidates (live candidates go first) and orders each subgroup 
     * by decreasing birth dates (youngest first).
     * Possible states of the arguments: alive, hero or zombie
     * @return True if clone `a' should go before clone `b' in the list of clones.
     *         The master will be the first in the resulting list.
     */
    virtual bool compareClones(const CandidateSolution* a, const CandidateSolution* b) const;
    
    /**
     * Choose heroes. The default implementation prefers candidates with a high scaled fitness.
     * FIXME might also make sense to prefer a high RAW fitness?
     * Possible states of the arguments: alive or hero
     * @return True if `a' would make a better hero than `b'.
     */
    virtual bool compareHeroes(const CandidateSolution* a, const CandidateSolution* b) const;
    
    /**
     * Choose the mu individuals to save while slaughtering, or the best during tournaments.
     * Default implementation compares the scaled fitness and then the birth
     * (equivalent to IndividualComparatorWithBirth).
     * Possible states of the arguments: alive or hero
     * @return True if `a' should be part of the mu individuals more than `b'.
     */
    virtual bool compareForSelection(const CandidateSolution* a, const CandidateSolution* b) const;
    
    /**
     * Choose best individuals in tournaments based on an alternative criterion.
     * Default implementation compares the deltaEntropy of candidates.
     * FIXME also compare the size to reduce bloat?
     * Possible states of the arguments: alive or hero
     * @return True if `a' is better `b' wrt the alternative measure
     */
    virtual bool compareForFitnessHole(const CandidateSolution* a, const CandidateSolution* b) const;
    
    /**
     * Choose the best and worst individuals for the commit() function.
     * The fitness used for comparison depends on the function name
     * Possible states of the arguments: alive, hero or zombie
     * @return True if `a' has a better raw fitness than `b'.
     */
    virtual bool compareRawBestWorst(const CandidateSolution* a, const CandidateSolution* b) const;
    virtual bool compareScaledBestWorst(const CandidateSolution* a, const CandidateSolution* b) const;
    
    /**
     * Compare the (newly produced) candidate `a' to a reference candidate `b'
     * (e.g. parent or best candidate of the population) in order to evaluate 
     * the performance of the operator that produced `a'.
     * Default implementation compares their scaled fitness and then their entropy.
     * Possible states of the arguments: alive, hero or zombie
     * @return True if `a' is an improvement over `b'.
     */
    virtual bool compareOperatorPerformance(const CandidateSolution* a, const CandidateSolution* b) const;
    
public: // setters, getters
    /** 
     * Sets the name of this population
     * @param value Name of the population
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    void setName(const std::string& value) noexcept { name = value; }
    
    /** 
     * Returns the current generation of the population
     * @returns unsigned int Number of generation
     */
    unsigned int getGeneration() const { return generation; }
    
    /** 
     * Returns the evolutionary algorithm used
     * @returns EvolutionaryAlgorithm The algorithm used
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    const EvolutionaryAlgorithm& getAlgorithm() const { return algorithm; }
    
    /** 
     * Returns the parameters of the population
     * @returns PopulationParameters Parameters of the population
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    virtual PopulationParameters& getParameters() = 0;
    virtual const PopulationParameters& getParameters() const = 0;
    
    /** 
     * Returns the name of the population
     * @returns string Name of the population
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    const std::string& getName() const noexcept { return name; }
    
    /** 
     * Returns the boolean true if the size of the population is zero
     * @returns boolean
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    bool extincted() { return getCandidateCount() == 0; }
    
    double getEntropy() const noexcept { return m_entropy; }
    
    /**
     * Number of candidates that will be included in a dump.
     * Includes zombies.
     */
    virtual std::size_t getCandidateCount() const = 0;
    
    /**
     * Number of live candidates.
     */
    virtual std::size_t getLiveCandidateCount() const = 0;
    
    virtual const CandidateSolution* getBestCandidate() const = 0;
    virtual const CandidateSolution* getWorstCandidate() const = 0;
    
    /**
     * @return True if the given candidate should be kept as a zombie.
     */
    bool isZombifiable(CandidateSolution* candidate) const {
        return std::find(m_bloodMagicWaitingList.begin(), m_bloodMagicWaitingList.end(),
                         candidate) != m_bloodMagicWaitingList.end();
    }
    
public: // Stuff to rework
    
    // Removes a single individual from the population. Declared public to be used inside the graphical user interface
    // TODO remove this function as soon as the GUI does not use it anymore
    // void removeIndividual(unsigned int index);
    
    /** 
     * TODO rework this function and the underlying mechanisms
     * Merge this population with the one specified
     * @param population to merge with
     * @throws Any exception. Exception if the populations types are different
     */
    void merge(std::unique_ptr<Population> population);
    
    /**
     * Assimilates an individual, trying to open an individual file and parse it, building an internal representation.
     * @param std::string fileName name of the file to be assimilated.
     * @param ugp3::constraints::Constraints* modifiedConstraints constraints might need to be modified in order to take into account the structure of the individual. If the pointer is nullptr, the original constraints have not been modified.
     * @returns Individual* pointer to Individual's internal representation. 
     */
    CandidateSolution* assimilate(std::string fileName, ugp3::constraints::Constraints* modifiedConstraints);

/**
     * Assimilates several candidate solutions, reading them from file. Virtual, since each population has different types of candidate solutions (groups, individuals).
     * @param std::string fileName name of the file that contains the individuals' filenames.
*/
    virtual void seeding(std::string fileName) = 0;

protected: // static fields
    /** Xml tag to specify the generation of the population */
    static const std::string XML_ATTRIBUTE_GENERATION;
    /** Xml tag to specify the type of the population */
    static const std::string XML_ATTRIBUTE_TYPE;
    /** Xml tag to specify the name of the population */
    static const std::string XML_ATTRIBUTE_NAME;
    /** Xml tag to specify the actual number of steady generations */
    static const std::string XML_CHILD_ELEMENT_STEADYSTATE;
    static const std::string XML_ATTRIBUTE_RAW_BEST;
    static const std::string XML_ATTRIBUTE_SCALED_BEST;
    static const std::string XML_ATTRIBUTE_RAW_WORST;
    static const std::string XML_ATTRIBUTE_SCALED_WORST;
    static const std::string XML_CHILDELEMENT_ENTROPY;
    
public: // static fields
    /** Name of this xml element */
    static const std::string XML_NAME;
    
public: //Xml interface
    /** 
     * Returns the type of the population
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual const std::string& getType() const = 0;
    
    virtual void writeXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
    virtual const std::string& getXmlName() const { return XML_NAME; }
    
protected:
    /**
     * Subclasses can implement this method to add fields into the <population> tag.
     * In particular, we use this function to write information about
     * the steady state. If a subclass overrides steady-state detection, it
     * should use this method to write its own state.
     */
    virtual void writeInnerXml(std::ostream& output) const;
};

using namespace std;
using namespace ugp3::log;

template <typename RandomAccessIterator>
void Population::promoteHeroesAndAge(RandomAccessIterator begin, RandomAccessIterator end)
{
    _STACK;
    
    begin = regroupAndSkipDeadCandidates(begin, end);
    
    // NOTE/DET This sort guarantees deterministic results.
    std::sort(begin, end, [&] (CandidateSolution* a, CandidateSolution* b) {
        return compareHeroes(a, b);
    });
        
    unsigned int heroesToPromote = getParameters().getEliteCardinality();
    for (auto it = begin; it != end; ++it) {
        CandidateSolution* candidate = *it;
        if (candidate->isAlive()) {
            if (heroesToPromote) {
                --heroesToPromote;
                candidate->setState(CandidateSolution::HERO);
            } else {
                // He may have been a hero before, but he ain't no more
                candidate->setState(CandidateSolution::ALIVE);
            }
        }
        // TODO rename this function back to age?
        // TODO or remove the boolean parameter? Why did I add it already?
        // TODO find where it is called with "false"
        candidate->step(true);
    }
}

template <typename RandomAccessIterator>
RandomAccessIterator Population::regroupAndSkipDeadCandidates(RandomAccessIterator begin, RandomAccessIterator end)
{
    LOG_VERBOSE << "Regroup dead " << TypeName<decltype(*begin)>::name << "s at the begining of the range..." << ends;
    
    auto notDeadBegin =  std::stable_partition(begin, end, [] (CandidateSolution* a) {
        return a->isDead();
    });
    
    LOG_VERBOSE << "Regrouped " << std::distance(begin, notDeadBegin) << " dead of "
    << std::distance(begin, end) << " " << TypeName<decltype(*begin)>::name
    << "s at the begining of the range." << ends;
    
    return notDeadBegin;
}

template <typename RandomAccessIterator>
RandomAccessIterator Population::regroupAndSkipNotAliveCandidates(RandomAccessIterator begin, RandomAccessIterator end)
{
    LOG_VERBOSE << "Regroup dead and zombie " << TypeName<decltype(*begin)>::name << "s at the begining of the range..." << ends;
    
    auto liveBegin =  std::stable_partition(begin, end, [] (CandidateSolution* a) {
        return !a->isAlive();
    });
    
    LOG_VERBOSE << "Regrouped " << std::distance(begin, liveBegin) << " dead and zombie of "
    << std::distance(begin, end) << " " << TypeName<decltype(*begin)>::name
    << "s at the begining of the range." << ends;
    
    return liveBegin;
}

template <typename RandomAccessIterator>
void Population::simpleSelection(unsigned int mu, RandomAccessIterator begin, RandomAccessIterator end)
{
    begin = regroupAndSkipNotAliveCandidates(begin, end);
    
    if (std::distance(begin, end) <= (int)mu) {
        LOG_VERBOSE << "Skipping simple selection (mu = " << mu << "): only " << std::distance(begin, end)
        << TypeName<decltype(*begin)>::name << " in the range." << ends;
        return;
    }
    
    LOG_VERBOSE << "Performing simple selection (mu = " << mu << ") among "
    << std::distance(begin, end) << " " << TypeName<decltype(*begin)>::name << "s..." << ends;
    
    // sort the candidates by fitness
    // NOTE/DET guarantees deterministic results
    std::sort(begin, end, [this] (CandidateSolution* a, CandidateSolution* b) {
        return compareForSelection(a, b);
    });
    
    size_t kills = 0;
    for (auto it = std::next(begin, mu); it != end; ++it) {
        (*it)->setDeath(getGeneration());
        ++kills;
    }
    
    LOG_VERBOSE << "Performing simple selection (mu = " << mu << ") among "
    << std::distance(begin, end) << " " << TypeName<decltype(*begin)>::name << "s: "
    "killed " << kills << " of them." << ends;
}

template <typename RandomAccessIterator>
void Population::allopatricSelection(RandomAccessIterator begin, RandomAccessIterator end)
{
    begin = regroupAndSkipDeadCandidates(begin, end);
    
    if (std::distance(begin, end) < 2) {
        LOG_VERBOSE << "Skipping allopatric selection: only " << std::distance(begin, end)
        << TypeName<decltype(*begin)>::name << " in the range." << ends;
        return;
    }
    
    LOG_VERBOSE << "Performing allopatric selection among " << std::distance(begin, end)
    << " " << TypeName<decltype(*begin)>::name << "s..." << ends;
    
    // Makes the allopatric selection
    // Group individuals by allopatric tag, and in each group by decreasing fitness.
    // NOTE/DET guarantees deterministic results
    std::sort(begin, end, [this] (CandidateSolution* a, CandidateSolution* b) {
        unsigned int result = a->getAllopatricTag().compare(b->getAllopatricTag());
        if (result != 0)
            return  result < 0;
        return compareForSelection(a, b);
    });
    // For each group, keep the first and kill the others.
    unsigned int kills = 0;
    auto groupBegin = begin;
    auto it = begin;
    do {
        ++it;
        if (it == end || (*groupBegin)->getAllopatricTag() != (*it)->getAllopatricTag()) {
            // 'it' is past the end of the current group
            LOG_DEBUG << "Allopatric selection for tag " << (*groupBegin)->getAllopatricTag()
            << ": we keep the " << (*groupBegin)->getDisplayTypeName() << " " << *groupBegin
            << " and kill the others." << ends;
            for (auto groupIt = next(groupBegin); groupIt != it; ++groupIt) {
                (*groupIt)->setDeath(getGeneration());
                ++kills;
            }
            groupBegin = it;
        }
    } while (it != end);
    
    LOG_VERBOSE << "Performing allopatric selection among " << std::distance(begin, end)
    << " " << TypeName<decltype(*begin)>::name << "s: killed " << kills << " of them." << ends;
}

template <typename ForwardIterator>
vector<double> Population::computeAverageRawFitness(ForwardIterator begin, ForwardIterator end) const
{
    // FIXME/DET Does determinism matters for this function?
    // NOTE/DET if it matters we should order by id befor the computation because addition on floats is not associative.
    
    // initialize the fitness vector
    vector<double> fitness(getParameters().getFitnessParametersCount(), 0.0);
    
    for (auto it = begin; it != end; ++it) {
        const vector<double>& indFitness = (*it)->getRawFitness().getValues();
        Assert(fitness.size() == indFitness.size());
        for (unsigned int i = 0; i < fitness.size(); ++i) {
            fitness[i] += indFitness[i];
        }
    }
    for (double& coeff: fitness) {
        coeff /= std::distance(begin, end);
    }
    return fitness;
}

template <typename ForwardIterator>
double Population::computeAverageAge(ForwardIterator begin, ForwardIterator end) const
{
    // FIXME/DET Does determinism matters for this function?
    // NOTE/DET if it matters we should order by id befor the computation because addition on floats is not associative.
    
    double age = 0;
    for (auto it = begin; it != end; ++it) {
        age += (*it)->getAge();
    }
    age /= std::distance(begin, end);
    return age;
}

template <typename ForwardIterator>
double Population::computeAverageDeltaEntropy(ForwardIterator begin, ForwardIterator end) const
{
    // FIXME/DET Does determinism matters for this function?
    // NOTE/DET if it matters we should order by id befor the computation because addition on floats is not associative.
    
    double deltaEntropy = 0;
    for (auto it = begin; it != end; ++it) {
        deltaEntropy += (*it)->getDeltaEntropy().getValue();
    }
    deltaEntropy /= std::distance(begin, end);
    return deltaEntropy;
}

template <typename RandomAccessIterator>
std::size_t Population::runEvaluator(RandomAccessIterator begin, RandomAccessIterator end)
{
    // NOTE/DET We sort by id to get deterministic results.
    std::sort(begin, end, CandidateSolution::OrderById());
    
    Evaluator& evaluator = getParameters().getEvaluator();
    std::size_t evaluatedCandidates = 0;
    
    LOG_INFO << "Evaluating " << TypeName<decltype(*begin)>::name << "s" << Progress(0) << ends;
        
    for (auto it = begin; it != end; ++it) {
        // We also evaluate zombies, hence the !isDead()
        if (!(*it)->isDead() && !(*it)->getRawFitness().getIsValid()) {
            evaluator.evaluate(**it);
            evaluatedCandidates++;
        }
    }

    // force the evaluation of the candidates
    evaluator.flush([&] (double progress) {
        LOG_INFO << "Evaluating " << evaluatedCandidates << " " << TypeName<decltype(*begin)>::name << "s" << Progress(progress) << ends;
    });
    
    evaluator.step(generation + 1);
    
    LOG_VERBOSE << "Copying raw fitness to scaled fitness" << std::ends;
    for (auto it = begin; it != end; ++it) {
        if (!(*it)->isDead()) {
            Assert((*it)->getRawFitness().getIsValid());
            (*it)->getFitness().setDescription((*it)->getRawFitness().getDescription());
            (*it)->getFitness().setValues((*it)->getRawFitness().getValues());
        }
    }

    if (evaluatedCandidates == 0) {
        LOG_VERBOSE << "No " << TypeName<decltype(*begin)>::name << " evaluated in population generation " << this->getGeneration() << ends;
    }
    
    return evaluatedCandidates;
}


template <typename ForwardIterator>
void Population::handleClones(ForwardIterator begin, ForwardIterator end)
{
    for (auto it = begin; it != end; ++it) {
        handleClone(**begin, **it, distance(begin, it), distance(begin, end));
    }
}

template <typename RandomAccessIterator>
void Population::detectAndHandleClones(RandomAccessIterator begin, RandomAccessIterator end)
{
    begin = regroupAndSkipDeadCandidates(begin, end);
    
    string operation = string("Detecting ") + TypeName<decltype(*begin)>::name + " clones";
    LOG_INFO << operation << Progress::START << ends;
    
    /*
     * NOTE/DET Instead of using a std::unordered_map with our custom hash
     * and key equality functions (which would work great and fast) we make do
     * without it to ensure deterministic (STL implementation-independent)
     * call order of the hash and equality functions.
     */
    std::unordered_multimap<hash_t, std::vector<CandidateSolution*>> cloneMap;
    
    // NOTE/DET We sort by id in order to call the genotype equality/hash function in a deterministic order.
    // Not that it should matter for actual computation results, but it changes the order of debug log messages.
    std::sort(begin, end, CandidateSolution::OrderById());
    
    // Step 1: build groups of clones
    for (auto it = begin; it != end; ++it) {
        bool foundClones = false;
        hash_t hash = (*it)->getHashCode(Hashable::GENOTYPE);
        auto range = cloneMap.equal_range(hash);
        for (auto jt = range.first; jt != range.second; ++jt) {
            std::vector<CandidateSolution*>& clones = jt->second;
            if (clones.front()->isGenotypeEqual(**it)) {
                foundClones = true;
                clones.push_back(*it);
            }
        }
        if (!foundClones) {
            cloneMap.insert({hash, {*it}});
        }
    }
    
    // Step two: order each group and move it out of the map
    std::vector<std::vector<CandidateSolution*>> groupsOfClones;
    for (auto& record: cloneMap) {
        std::vector<CandidateSolution*>& clones = record.second;
        std::sort(clones.begin(), clones.end(), [this] (CandidateSolution* a, CandidateSolution* b) {
            return compareClones(a, b);
        });
        groupsOfClones.push_back(std::move(clones));
    }
    
    // Step three: order groups of clones by id of their master and process them in order
    std::sort(groupsOfClones.begin(), groupsOfClones.end(),
              [this] (const std::vector<CandidateSolution*>& a, const std::vector<CandidateSolution*>& b) {
                  return a.front()->getId() < b.front()->getId();
              });
    std::size_t todo = groupsOfClones.size(), done = 0;
    for (auto& clones: groupsOfClones) {
        LOG_DEBUG << "Handling the " << TypeName<decltype(*begin)>::name << " clones of " << clones.front() << " = { ";
        for (auto it = std::next(clones.begin()); it != clones.end(); ++it) {
            LOG_VERBOSE << (*it)->toString() << " ";
        }
        LOG_VERBOSE << "}" << ends;
        handleClones(clones.begin(), clones.end());
        ++done;
        LOG_INFO << operation << Progress((double)done / todo) << ends;
    }
}

template <typename RandomAccessIterator>
double Population::updateDeltaEntropy(RandomAccessIterator begin, RandomAccessIterator end)
{
    // Only consider live individuals in the entropy
    begin = regroupAndSkipNotAliveCandidates(begin, end);
    
    // NOTE/DET We sort by id to get deterministic results.
    std::sort(begin, end, CandidateSolution::OrderById());
    
    string operation = string("Evaluating entropy of ") + TypeName<decltype(*begin)>::name + "s";
    Message totalMessage;
    
    size_t count = 0;
    for (auto it = begin; it != end; ++it) {
        totalMessage += (*it)->getMessage();
        ++count;
    }
    
    double totalEntropy = totalMessage.getEntropy();
    double progress = 0;
    for (auto it = begin; it != end; ++it, ++progress) {
        totalMessage -= (*it)->getMessage();
        (*it)->getDeltaEntropy().setValue(totalEntropy - totalMessage.getEntropy());
        totalMessage += (*it)->getMessage();
        
        LOG_INFO << operation << Progress(progress / count) << ends;
    }
    LOG_INFO << operation << Progress::END << ends;
    
    return totalEntropy;
}

template <typename VectorOfCandidates>
void Population::removeCorpses(VectorOfCandidates& candidates)
{
    LOG_VERBOSE << "Remove corpses of dead " << TypeName<decltype(candidates.front())>::name << "s" << ends;
    
    // NOTE/DET We sort by id to get deterministic results.
    std::sort(candidates.begin(), candidates.end(), CandidateSolution::OrderById());
    
    unsigned int totCorpses = 0;
    for (auto& candidate: candidates) {
        if (candidate->isDead()) {
            ++totCorpses;
            delete candidate;
            candidate = nullptr;
        }
    }
    candidates.erase(std::remove(candidates.begin(), candidates.end(), nullptr), candidates.end());
    LOG_VERBOSE << "Removed " << totCorpses << " corpses." << ends;
}

template <typename ForwardIterator>
void Population::describePopulation(const std::string& context, ForwardIterator begin, ForwardIterator end)
{
    // NOTE/DET No need for determinism here (unless the getState() function has side-effects)
    
    std::size_t total = 0;
    std::size_t totalPerState[CandidateSolution::STATE_COUNT] = {0};
    for (auto it = begin; it != end; ++it) {
        ++total;
        ++totalPerState[(*it)->getState()];
    }
    LOG_VERBOSE << context << ": " << TypeName<decltype(*begin)>::name << "s, "
    << totalPerState[CandidateSolution::ALIVE] + totalPerState[CandidateSolution::HERO] << " alive "
    << "(including " << totalPerState[CandidateSolution::HERO] << " heroes), "
    << totalPerState[CandidateSolution::ZOMBIE] << " zombies, "
    << totalPerState[CandidateSolution::DEAD] << " dead."
    << std::ends;
}

template <typename RandomAccessIterator>
void Population::shareFitness(RandomAccessIterator begin, RandomAccessIterator end)
{
    begin = regroupAndSkipDeadCandidates(begin, end);
    
    // NOTE/DET We sort by id to get deterministic results.
    std::sort(begin, end, CandidateSolution::OrderById());
    
    // Always reset the fitness sharing (even for zombies)
    for (auto it = begin; it != end; ++it) {
        (*it)->getFitness().setScalingFactor(ScaledFitness::SHARING_SCALING, 1);
        (*it)->getFitness().setAdditionalDescription(ScaledFitness::SHARING_SCALING, "");
    }
        
    // Skip zombies
    begin = std::partition(begin, end, [] (CandidateSolution* c) { return c->isZombie(); });
    
    // Sort for selection to get the output of the next loop right
    std::sort(begin, end, [this] (CandidateSolution* a, CandidateSolution* b) {
        return compareForSelection(a, b);
    });
    
    // TODO: insert crowding/fitness sharing here
    // this is the CLASSICAL fitness sharing; given the sharingFitnessRadius and a considered individual,
    // we count all individuals inside the sharingFitnessRadius from the considered individual, and
    // the scaled fitness of the considered individual is set to f' = f/m , where m is a sum over the neighbours
    // m = sum_j_neighbours( sh(distance(i,j) )
    // sh = (1 - distance(i,j) ) / radius
    // we have some suspects that a better way to operate would be to find the best individual in a radius and decrease
    // only the fitness of the remaining individuals in the radius...
    // TODO make function for the following description?
    LOG_VERBOSE << "Population before fitness sharing:" << ends;
    for (auto it = begin; it != end; ++it) {
        LOG_VERBOSE << TypeName<decltype(*begin)>::name << " " << *it
        << ", raw fitness = " << (*it)->getRawFitness() 
        << ends;
        
        // FIXME should use group mu when working on groups
        if (std::distance(begin, it) == this->getParameters().getMu())
            LOG_VERBOSE << "-- The population will be cut here --" << ends;
    }
    
    std::string operation = string("Sharing ") + TypeName<decltype(*begin)>::name + " fitness";
    
    /*
     * FIXME the next comment is not true anymore, the code below is classical sharing
     * Since we want to prevent the best individuals from being killed during
     * selection, we use the same comparison function as for selections.
     * 
     * NOTE/DET This sort guarantees deterministic results.
     */
    std::sort(begin, end, CandidateSolution::OrderById());
    
    for (auto it = begin; it != end; ++it) {
        double progress = (double)std::distance(begin, it) / std::distance(begin, end);
        LOG_INFO << operation << Progress(progress*progress) << std::ends;
        double m = 0;
        unsigned int neighbours = 0;
        auto currentIndividual = *it;
        
        // ok, let's introduce an exception for individuals with fitness == 0 (which are actually outside the population)
        if (currentIndividual->getFitness().getValues()[0] <= 0)
            continue;
        
        for (auto jt = begin; jt != end; ++jt) {
            auto jIndividual = *jt;
            
            // ok, let's introduce an exception for individuals with fitness == 0 (which are actually outside the population)
            if (jIndividual->getFitness().getValues()[0] <= 0)
                continue;
            
            // Skip clones
            if (jIndividual->isGenotypeEqual(*currentIndividual))
                continue;
            
            double distance, apport;
            std::tie(distance, apport) = computeSharingApport(currentIndividual, jIndividual);
            if (apport > 0) {
                LOG_VERBOSE << jIndividual << " is a neighbour of " << *currentIndividual
                << " with distance " << distance << " and an apport of " << apport << " on m." << ends;
                m += apport;
                ++neighbours;
            }
        }
        
        // if there is at least one neighbour in the radius, divide all fitness values by m
        // note that for clones (scaledFitness == 0), m is not computed
        /*
         * FIXME since when do clones have a scaled fitness of 0? anyway clones
         * are already excluded from the range that this function receives, so maybe
         * there are some useless checks in this function.
         * FIXME clones are not excluded from the range that this function receives
         */
        if (m > 1)
        {
            LOG_VERBOSE << "Before: " << *currentIndividual << " fitness is " << currentIndividual->getFitness() << ends;
            
            currentIndividual->getFitness().setScalingFactor(ScaledFitness::SHARING_SCALING, 1/m);
            currentIndividual->getFitness().setAdditionalDescription(
                ScaledFitness::SHARING_SCALING,
                " [" + ugp3::Convert::toString(1/m) + " sharing, " + ugp3::Convert::toString(neighbours) + " neighbours]");
            
            LOG_VERBOSE << "After: " << *currentIndividual << " fitness is " << currentIndividual->getFitness() << ends;
        }
    }
    
    LOG_INFO << operation << Progress(1) << std::ends;
    
    
    // Sort for selection to get the output of the next loop right
    std::sort(begin, end, [this] (CandidateSolution* a, CandidateSolution* b) {
        return compareForSelection(a, b);
    });
    // TODO make function (cf beginning of this function)
    LOG_VERBOSE << "Population after fitness sharing:" << ends;
    for (auto it = begin; it != end; ++it) {
        CandidateSolution* currentIndividual = *it;
        LOG_VERBOSE << TypeName<decltype(*begin)>::name << " " << *currentIndividual
        << ", scaled fitness = " << currentIndividual->getFitness()
        << ends;
        
        if (distance(begin, it) == this->getParameters().getMu())
            LOG_VERBOSE << "-- The population will be cut here --" << ends;
    }
}

template <class RandomAccessIterator>
bool Population::checkHashGenoPhenoEqualitiesAndHashQuality(
    RandomAccessIterator begin, RandomAccessIterator end
) const
{
    if (std::distance(begin, end) <= 1)
        return true;
    
    constexpr auto typeName = TypeName<decltype(*begin)>::name;
    bool success = true;
#if 0 // This loop is really slow for big populations
    LOG_INFO << "(debug only) Checking " << typeName << " hash-geno-pheno relations" << Progress(0) << ends;
    bool genoPhenoEquivalent = true;
    for (auto it1 = begin; it1 != std::prev(end); ++it1) {
        for (auto it2 = std::next(it1); it2 != end; ++ it2) {
            LOG_INFO << "(debug only) Checking " << typeName
            << " hash-geno-pheno relations" << Progress(
                (double) std::distance(begin, it1) / std::distance(begin, end)
            ) << ends;
            CandidateSolution* ind1 = *it1;
            CandidateSolution* ind2 = *it2;
            
            bool geno = ind1->isGenotypeEqual(*ind2);
            bool normPheno = ind1->isNormalizedPhenotypeEqual(*ind2);
            /*
             * We verify that having the same genotype implies
             * having the same normalized genotype and the same hash.
             * 
             * NOTE In general (possible presence of non-coding DNA),
             * phenotype-equal does not imply hash-equal, as hashes
             * are based on genotype.
             */
            if (geno) {
                bool hash = ind1->getHashCode(purpose) == ind2->getHashCode(purpose);
                success = success && normPheno && hash;
                if (!normPheno)
                    LOG_WARNING << typeName << "s " << ind1 << " and " << ind2
                    << " have the same genotype but different normalized phenotypes!" << ends;
                if (!hash)
                    LOG_WARNING << typeName << "s " << ind1 << " and " << ind2
                    << " have the same genotype but different hash values!" << ends;
            }
            // We try to detect the presence of non-coding DNA.
            genoPhenoEquivalent = genoPhenoEquivalent && (geno == normPheno);
        }
    }
    LOG_INFO << "(debug only) Checking " << typeName << " hash-geno-pheno relations" << Progress(1) << ends;
    LOG_INFO << "Report on population of " << std::distance(begin, end) << " "
    << typeName << "s:" << ends;
    if (success) {
        LOG_INFO << "All relationships between genotypes, phenotypes and hash values okay." << ends;
    } else {
        LOG_INFO << "Error(s) detected, wrong normalized phenotype or hash function!" << ends;
    }
    if (genoPhenoEquivalent) {
        LOG_INFO << "Genotypes and normalized phenotypes are equivalent in this population." << ends;
    } else {
        LOG_INFO << "This population has non-coding DNA: genotypes and normalized phenotypes differ." << ends;
    }
#endif
    // Count collisions
    std::unordered_set<CandidateSolution*, CandidateSolution::CloneHash, CandidateSolution::CloneEquality>
        uniqueIndividuals(begin, end);
    std::unordered_set<hash_t> uniqueHashValues;
    for (auto it = begin; it != end; ++it) {
        uniqueHashValues.insert((*it)->getHashCode(Hashable::GENOTYPE));
    }
    LOG_INFO << uniqueHashValues.size() << " unique hash values for "
    << uniqueIndividuals.size() << " unique " << typeName << "s (collision rate "
    << (double)(uniqueIndividuals.size() - uniqueHashValues.size()) / uniqueIndividuals.size() << ")" << ends;
    return success;
}

}
}

#endif

