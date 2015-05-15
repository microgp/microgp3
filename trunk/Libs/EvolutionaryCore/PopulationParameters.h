/**
 * @file PopulationParameters.h
 * Definition of the PopulationParameters class.
 * @see PopulationParameters.cc
 * @see PopulationParameters.xml.cc
 */

#ifndef HEADER_UGP3_CORE_POPULATIONPARAMETERS
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_POPULATIONPARAMETERS

#include "Parameters.h"
#include "Constraints.h"
#include "XMLIFace.h"
#include "Evaluator.h"

// forward declaration
namespace Ui
{
	class MicroGPMainWindow;
}

namespace ugp3 {
namespace core {
    
class Statistics;
class Data;

/**
 * @class PopulationParameters
 * Keeps the parameters of a population
 * @see Population
 */
class PopulationParameters : public Parameters , public xml::XMLIFace
{
private: // static fields
    // A static map to save all the constraints defined in the instances of this class
    static std::map<std::string, const ugp3::constraints::Constraints*> constraintsDefinitions;

protected:
    /** Xml tag to specify the mu(size of the population) parameter */
    static const std::string XML_CHILDELEMENT_MU;
    /** Xml tag to specify the lamda(number of genetic operators to use) parameter */
    static const std::string XML_CHILDELEMENT_LAMBDA;
    /** Xml tag to specify the nu(size of the population at the very beginning of the evolution) parameter */
    static const std::string XML_CHILDELEMENT_NU;
    static const std::string XML_CHILDELEMENT_ENNE;
    /** Xml tag to specify the inertia(weight of the previous values of endogenous parameters in the process of self-adaptation) parameter */
    static const std::string XML_CHILDELEMENT_INERTIA;
    /** Xml tag to specify the number of the fitness used */
    static const std::string XML_CHILDELEMENT_FITNESSPARAMETERS;
    /** Xml tag to specify the differential weight of differential evolution */
    static const std::string XML_CHILDELEMENT_DIFFEVODW;
    /** Xml tag to specify the crossovere probability of differential evolution */
    static const std::string XML_CHILDELEMENT_DIFFEVOCP;
    /** Xml tag to specify the maximum age of the individuals */
    static const std::string XML_CHILDELEMENT_MAXIMUMAGE;
    /** Xml tag to specify the sigma(strength of the mutation operators) parameter */
    static const std::string XML_CHILDELEMENT_SIGMA;
    /** Xml tag to specify the maximum number of generations */
    static const std::string XML_CHILDELEMENT_MAXIMUMGENERATIONS;
    /** Xml tag to specify if the fitnesses have to be invalidated after each generation. If they are invalidated, individuals are re-evaluated each step */
    static const std::string XML_CHILDELEMENT_INVALIDATEFITNESS;
    /** Xml tag to specify the maximum number individuals that can be evaluated */
    static const std::string XML_CHILDELEMENT_MAXIMUMEVALUATIONS;
    static const std::string XML_CHILDELEMENT_ELITE;
    static const std::string XML_CHILDELEMENT_STEADYSTATEGENERATIONS;
    static const std::string XML_CHILDELEMENT_MAXFITNESS;
    static const std::string XML_CHILDELEMENT_MAXTIME;
    static const std::string XML_ATTRIBUTE_HOURS;
    static const std::string XML_ATTRIBUTE_MINUTES;
    static const std::string XML_ATTRIBUTE_SECONDS;
    static const std::string XML_CHILDELEMENT_FITNESSSHARING;
    static const std::string XML_ATTRIBUTE_DISTANCETYPE;
    static const std::string XML_ATTRIBUTE_RADIUS;
    static const std::string XML_CHILDELEMENT_CLONESCALINGFACTOR;
    
    /** Xml tag to specify the evaluator */
    static const std::string XML_CHILDELEMENT_EVALUATOR;

    /** Xml tag to specify the attribute value of a parameter */
    static const std::string XML_ATTRIBUTE_VALUE;

private:
    Population* population;

protected:
    /** Constraints that have to satisfy all the individuals of the population */
    const ugp3::constraints::Constraints* constraints;
    /** Size of the population */
    unsigned int mu;
    /** Number of operators to use in a step */
    unsigned int lambda;
    /** Initial size of the population in the generation 0 */
    unsigned int nu;
    /** differential weight of differential evolution */
    double diffevodw;
    /**crossover probability of differential evolution */
    double diffevocp;
    /** Weight of the previous values of the endogenous parameters in the process of self-adaption */
    double inertia;
    /** Original value of sigma(the strength of the mutation operators) */
	double originalSigma;
    /** Current value of the mutation operator's strength */
    double sigma;
    /** Maximum age that an individual can reach before die */
    unsigned int maximumAge;
    /** Maximum number of generations before stop the evolution */
    unsigned int maximumGenerations;
    /** Attribute to specify if the algorithm has to stop after a PopulationParameters::maximumGenerations number of generations */
    bool maximumGenerationsStop;
    /** Number of fitensses used */
    unsigned int fitnessParametersCardinality;
    /** Object that keeps the actived genetic operators */
    std::unique_ptr<Statistics> activations;
    /** Object to select the individuals in each evolution step */
    std::unique_ptr<CandidateSelection> selector;
    /** Specify if the algorithm dumps the population before the evaluation in each step */
    bool dumpBeforeEvaluation;
    /** Specify if the fitnesses of the individuals are re-calculated each step */
    bool invalidateFitnessAfterGeneration;
    /** Maximum number of individuals to evaluate */
    unsigned long maximumEvaluations;
    /** Attribute to specify if the algorithm has to stop after a PopulationParameters::maximumEvaluations number of evaluations */
    bool maximumEvaluationsStop;
    unsigned int eliteCardinality;
    bool maximumFitnessStop;
    std::vector<double> maximumFitness;
    bool SteadyStateGenerationsStop;
    unsigned int maximumSteadyStateGenerations;
    bool maximumTimeStop;
    std::chrono::seconds maximumTime;
    bool fitnessSharingEnabled;
    std::string fitnessSharingDistance; // type of crowding distance
    double fitnessSharingRadius;     // radius of crowding distance
    double cloneScalingFactor;
    
protected:
    /** 
     * If there is not a set of constraints with the same id that the one specified, it adds the set to the static map of this class. Anyway, it returns the memory pointer where the set is placed in the map.
     * @param constraints Constraints to get the definition from.
     * @returns constraints::Constraints The memory place where the constraints are saved.
     */
    const ugp3::constraints::Constraints& getConstraintsDefinition(const ugp3::constraints::Constraints& constraints);
    /** 
     * Write the information of this object in the output stream with a xml format
     * @param output Stream where the information is going to be written
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual void writeInternalXml(std::ostream& output) const;

public:
    /** 
     * Returns the maximum age that an individual can reach before die
     * @returns unsigned int The maximum age that an individual can reach
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    unsigned int    getMaximumAge() const noexcept { return maximumAge; }
    /** 
     * Returns the number of generations before stop the evolution
     * @returns unsigned int The maximum number of generations before stop the evolution
     * @throws Any exception. Exception if there is not set the maximum generations stop condition
     */
    unsigned int    getMaximumGenerations() const;
    /** 
     * Returns the original value of sigma
     * @returns double The original value of sigma
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    double	    getOriginalSigma() const noexcept { return originalSigma; }
    /** 
     * Returns the current value of sigma, and checks it is in (0,1)
     * @returns double The actual value of sigma
     */
    double          getSigma() const {
        if (sigma <= 0 || sigma >= 1) {
            throw ArgumentException("sigma should be in (0, 1)", LOCATION);
        }
        return sigma;
    }
    /** 
     * Returns the value of mu
     * @returns unsigned int The value of mu
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    unsigned int 	getMu() const noexcept { return mu; }
    /** 
     * Returns the value of differential weight
     * @returns float The value of differential weight
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    double getDiffEvoDW() const noexcept { return diffevodw; }
    /** 
     * Returns the value of crossover probability
     * @returns float The value of crossover probability
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    double getDiffEvoCP() const noexcept { return diffevocp; }
    /** 
     * Returns the value of lambda
     * @returns unsigned int The value of lambda
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    unsigned int 	getLambda() const noexcept { return lambda; }
    /** 
     * Returns the value of nu
     * @returns unsigned int The value of nu
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    unsigned int 	getNu() const noexcept { return nu; }
    
    /** 
     * FIXME different number for groups?
     * Returns the number of fitnesses used
     * @returns unsigned int The number of fitnesses used
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    unsigned int 	getFitnessParametersCount() const noexcept { return fitnessParametersCardinality; }
    /** 
     * FIXME still used?
     * Returns the value of the inertia parameter
     * @returns double The value of the inertia paramter
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    double       	getInertia() const noexcept { return inertia; }
    /** 
     * Returns if the algorithm dumps the population before the evaluation in each step or not
     * @returns bool True if the dump is required, false if not.
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    bool			getDumpBeforeEvaluation() const noexcept { return dumpBeforeEvaluation; }
    /** 
     * Returns if the algorithm has to stop after a PopulationParameters::maximumGenerations number of generations
     * @returns bool True if the algorithm has to stop, false if not.
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    bool            getMaximumGenerationsStop() const noexcept { return maximumGenerationsStop; }
    /** 
     * Returns the selector used to select candidates
     * @returns The selector used
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    CandidateSelection& getSelector() noexcept { return *selector; }
    /** 
     * Returns the selector used to select candidates
     * @returns The selector used
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    const CandidateSelection& getSelector() const noexcept { return *selector; }
    /** 
     * Returns the constraints that has to satisfy the population of this population parameters
     * @returns const constraints::Constraints The constraints to satisfy by the population
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    const ugp3::constraints::Constraints& getConstraints() const noexcept { return *constraints; }
    /** 
     * Returns an object that keeps the actived genetic operators
     * @returns Statistics The object with the actived genetic operators
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    Statistics&     getActivations() const noexcept { return *activations; }
    /** 
     * Returns if the fitnesses of the individuals are re-calculated each step
     * @returns bool True if the are re-calculated, false if not
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    bool            getInvalidateFitnessAfterGeneration() const noexcept { return invalidateFitnessAfterGeneration; }
    

    /** 
     * Returns the evaluator used to evaluate the individuals of the population
     * @returns Evaluator The evaluator used
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual ugp3::core::Evaluator& getEvaluator() = 0;
    virtual const ugp3::core::Evaluator& getEvaluator() const = 0;
    
    /**
     * Returns the population connected to this instance of PopulationParameters
     * @returns Population* Pointer to the population or nullptr
     * @throws nothing. If an exception is thrown, the execution is aborted.
    */
    Population* getPopulation() const { return population; }
    
    unsigned int getEliteCardinality() const noexcept { return eliteCardinality; }
    
    /** 
     * Returns if the algorithm has to stop after a PopulationParameters::maximumEvaluations number of evaluations
     * @returns bool True if the algorithm has to stop, false if not
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    bool            getMaximumEvaluationsStop() const noexcept { return maximumEvaluationsStop; }
    /** 
     * Returns the maximum number of evaluations to do before stop the evolution
     * @returns unsigned long The maximum number of evaluations.
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    unsigned long   getMaximumEvaluations() const noexcept { return maximumEvaluations; }
    
    /**
     * Whether to stop when we reach a given fitness.
     */
    bool                getMaximumFitnessStop() const noexcept { return maximumFitnessStop; }
    std::vector<double> getMaximumFitness()     const noexcept { return maximumFitness; }
    
    /**
     * Whether to stop after the given number of generations with no fitness improvement.
     */
    bool         getSteadyStateGenerationsStop()    const noexcept { return SteadyStateGenerationsStop; }
    unsigned int getMaximumSteadyStateGenerations() const noexcept { return maximumSteadyStateGenerations; }
    
    /**
     * Whether to stop after the given time has passed.
     * FIXME what kind of time? clock? CPU?
     */
    bool getMaximumTimeStop() const { return maximumTimeStop; }
    std::chrono::seconds getMaximumTime() const { return maximumTime; }
    
    /**
     * Whether to use fitness sharing for individuals, and the parameters.
     */
    bool        getFitnessSharingEnabled()  const noexcept { return fitnessSharingEnabled; }
    std::string getFitnessSharingDistance() const noexcept { return fitnessSharingDistance; }
    double       getFitnessSharingRadius()   const noexcept { return fitnessSharingRadius; }
    
    double          getCloneScalingFactor() const noexcept { return cloneScalingFactor; }
    
public: // setters
    /** 
     * Sets the IndividualSelection instance used by the genetic operators to select individuals
     * @param selector The new selector
     * @throws Any exception. ArgumentException if the value specified is 0
     */
    void setSelector(CandidateSelection* selector);
    /** 
     * Sets the value of mu
     * @param mu The value of mu
     * @throws Any exception. ArgumentException if the value specified is 0
     */
    void setMu(unsigned int mu);
    /** 
     * Sets the value of lambda
     * @param lambda The value of lambda
     * @throws Any exception. ArgumentException if the value specified is 0
     */
    void setLambda(unsigned int lambda);
    /** 
     * Sets the value of nu
     * @param nu The value of nu
     * @throws Any exception. ArgumentException if the value specified is 0
     */
    void setNu(unsigned int nu);
    /** 
     * Sets the value of inertia
     * @param inertia The value of inertia
     * @throws Any exception. ArgumentException if the value specified is not netween 0 and 1
     */
    void setInertia(double inertia);
    /** 
     * Sets the value of crossover probability
     * @param cp The value of crossover probability
     * @throws Any exception. ArgumentException if the value specified is not netween 0 and 1
     */
    void setDiffEvoCP(double cp);
    /** 
     * Sets the value of differential weight
     * @param cp The value of differential weight
     * @throws Any exception. ArgumentException if the value specified is not netween 0 and 1
     */
    void setDiffEvoDW(double dw);
    /** 
     * Sets the original value of sigma
     * @param value The original value of sigma
     * @throws Any exception. ArgumentException if the value specified is not netween 0 and 1
     */
	void setOriginalSigma(double value);
    /** 
     * Sets the value of sigma
     * @param value The value of sigma
     * @throws Any exception. ArgumentException if the value specified is not netween 0 and 1
     */
    void setSigma(double value);
    /** 
     * Sets the constraints that has to satisfy the population of this population parameters
     * @param value The constraints to set
     * @throws Any exception.
     */
    void setConstraints(const ugp3::constraints::Constraints& value);
    /** 
     * Sets the maximum age that individuals can reach before die
     * @param value The age to set
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    void setMaximumAge(unsigned int value) noexcept;
    /** 
     * Sets the maximum generations of the evolution
     * @param value The maximum number of generations to set
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    void setMaximumGenerations(unsigned int value) noexcept;
    /** 
     * Sets the number of fitnesses to use
     * @param value The number of fitnesses to use
     * @throws Any exception. Exception if the value specified is 0
     */
    void setFitnessParametersCardinality(unsigned int value);
    /** 
     * Sets the individuals are re-evaluated each step or not
     * @param value True if they have to be re-evaluates, false if not
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    void setInvalidateFitnessAfterGeneration(bool value) noexcept;
    /** 
     * Sets the maximum number of evaluations to do before stop the evolution
     * @param value True if they have to be re-evaluates, false if not
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    void setMaximumEvaluations(unsigned long value) noexcept;
    
    void setEliteCardinality(unsigned int value) noexcept {
        eliteCardinality = value;
    }
    
    void setMaximumFitness(const Fitness& fitness) {
        this->maximumFitnessStop = true;
        this->maximumFitness = fitness.getValues();
    }
    
    void setMaximumSteadyStateGenerations(unsigned int value) noexcept {
        this->SteadyStateGenerationsStop = true;
        this->maximumSteadyStateGenerations = value;
    }
    
    void setMaximumTime(std::chrono::seconds value) {
        this->maximumTimeStop = true;
        this->maximumTime = value;
    }
    
    void setFitnessSharingDistance(const std::string& value) noexcept {
        this->fitnessSharingEnabled = true;
        this->fitnessSharingDistance = value;
    }
    
    void setFitnessSharingRadius(double value) noexcept {
        // TODO check bounds
        this->fitnessSharingRadius = value;
    }
    
    void setCloneScalingFactor(double value) {
        _STACK;
        if (value < 0.0 || value > 1.0) {
            throw Exception("Clone scaling should be in [0, 1].", LOCATION);
        }
        this->cloneScalingFactor = value;
    }
    
    /** 
     * Sets the script that is used as evaluator of individuals
     * @param fileName The name of the script file
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    void setEvaluator(const std::string& fileName) noexcept {
        LOG_DEBUG << "Evaluator for population is " << fileName << " ." << std::ends;
        getEvaluator().setScriptFile(fileName);
    }
    
public:
    /** 
     * Constructor of the class. Instantiates an object of this class with default values
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    PopulationParameters(Population* population);
    /** 
     * Destructor of the class. Frees the memory used by the object
     */
    ~PopulationParameters();
    
    /** 
     * Saves the information of this object in the specified file with a xml format
     * @param fileName File where save the information
     * @throws Any exception. Exception if the file can't be accessed
     */
    void save(const std::string& fileName) const;
    /** 
     * Returns the type of this object
     * @returns const string The type of this object
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual const std::string& getType() const = 0;
        
public:
    /** 
     * Returns an instance of this class with the information loaded from the file specified
     * @param fileName File to read the information from
     * @returns auto_ptr<PopulationParameters> The object instantiated
     */
    static std::unique_ptr<PopulationParameters> fromFile(const std::string& fileName) throw(std::exception);
    /** 
     * Returns an instance of this class with the information loaded from the xml element specified
     * @param element The xml element to read the information from
     * @returns auto_ptr<PopulationParameters> The object instantiate
     * @throws Any exception xml::SchemaException if the element is not a population parameters type
     */
    static std::unique_ptr<PopulationParameters> instantiate(const xml::Element& element) throw(std::exception);
    /** 
     * Returns an object instantiated of the type specified
     * @param type Type of the object to instantiate
     * @returns auto_ptr<PopulationParameters> The object instantiated
     * @throws Any exception Exception if the type is unknowed
     */
    static std::unique_ptr<PopulationParameters> instantiate(const std::string& type) throw(std::exception);
    /** 
     * Clears the map of the constraints of this class
     * @throws Any exception Exception if the type is unknowed
     */
    static void clearConstraintsCache();
    /** Name of this xml element */
    static const std::string XML_NAME;
    /** Xml tag to specify the attribute type of a parameter */
    static const std::string XML_ATTRIBUTE_TYPE;
    
    public: //Xml interface
        virtual void writeXml(std::ostream& output) const;
        virtual void readXml(const xml::Element& element);
        virtual const std::string& getXmlName() const noexcept;
        
        // friend classes, used to access the XML tags inside
        friend class Ui::MicroGPMainWindow;
        
};

inline void PopulationParameters::setMaximumEvaluations(unsigned long value) noexcept
{
    this->maximumEvaluations = value;
    this->maximumEvaluationsStop = true;
}

inline const std::string& PopulationParameters::getXmlName() const noexcept
{
    return XML_NAME;
}

inline void PopulationParameters::setInvalidateFitnessAfterGeneration(bool value) noexcept
{
    this->invalidateFitnessAfterGeneration = value;
}

inline unsigned int PopulationParameters::getMaximumGenerations() const
{
    if(this->maximumGenerationsStop == false)
        throw Exception("maximumGenerationsStop was not set.", LOCATION);
    
    return this->maximumGenerations;
}

inline void PopulationParameters::setMaximumGenerations(unsigned int value) noexcept
{
    this->maximumGenerationsStop = true;
    
    this->maximumGenerations = value;
}

inline void PopulationParameters::setFitnessParametersCardinality(unsigned int value)
{
    _STACK;
    
    if(value == 0)
        throw Exception("Fitness parameters cannot be zero.", LOCATION);
    
    this->fitnessParametersCardinality = value;
}

inline void PopulationParameters::setMaximumAge(unsigned int value) noexcept
{
    this->maximumAge = value;
}

inline const ugp3::constraints::Constraints& PopulationParameters::getConstraintsDefinition(
    const ugp3::constraints::Constraints& constraints)
{
    _STACK;
    
    const ugp3::constraints::Constraints* staticInstance = &constraints;
    
    if(constraintsDefinitions.find(constraints.getId()) != constraintsDefinitions.end())
    {
        staticInstance = constraintsDefinitions.find(constraints.getId())->second;
    }
    else
    {
        constraintsDefinitions[constraints.getId()] = &constraints;
    }
    
    return *staticInstance;
}

inline void PopulationParameters::setConstraints(const ugp3::constraints::Constraints& value)
{
    _STACK;
    
    const ugp3::constraints::Constraints& definition = this->getConstraintsDefinition(value);
    
    this->constraints = &definition;
}


inline void PopulationParameters::setSelector(CandidateSelection* selector)
{
    Assert( selector != nullptr);
    this->selector.reset( selector );
    
    return;
}

inline void PopulationParameters::setInertia(double inertia)
{
    _STACK;
    
    if(inertia < 0.0 || inertia > 1.0)
    {
        throw ArgumentException("The inertia parameter should be in [0, 1].",
                                LOCATION);
    }
    else if(inertia == 1.0)
    {
        LOG_WARNING
        << "Inertia is 1.0: that means self-adaptation will not work." << std::endl;
        
    }
    
    this->inertia = inertia;
}

inline void PopulationParameters::setDiffEvoCP(double cp)
{
    _STACK;
    
    if (cp < 0.0 || cp > 1.0)
    {
        throw ArgumentException("The crossover probability parameter should be in [0, 1].",
                                LOCATION);
    }
    
    this->diffevocp = cp;
}

inline void PopulationParameters::setDiffEvoDW(double dw)
{
    _STACK;
    
    this->diffevodw = dw;
}

inline void PopulationParameters::setLambda(unsigned int value)
{
    _STACK;
    
    if(value == 0)
    {
        throw ArgumentException("The parameter 'lambda' cannot be 0.",
                                LOCATION);
    }
    
    this->lambda = value;
}

inline void PopulationParameters::setMu(unsigned int value)
{
    _STACK;
    
    if(value == 0)
    {
        throw ArgumentException("The parameter 'mu' cannot be 0.", LOCATION);
    }
    
    this->mu = value;
}

inline void PopulationParameters::setNu(unsigned int value)
{
    _STACK;
    
    if(value == 0)
    {
        throw ArgumentException("The parameter 'nu' cannot be 0.", LOCATION);
    }
    
    this->nu = value;
}

inline void PopulationParameters::setSigma(double value)
{
    _STACK;
    
    if (value >= 0 && value < 1) {
        this->sigma = value;
    }
    else throw Exception("Value " + Convert::toString(value)+ " is not valid. Sigma must be in [0, 1[.", LOCATION);
}

inline void PopulationParameters::setOriginalSigma(double value)
{
    _STACK;
    
    if(value >= 0 && value < 1)
    {
        this->originalSigma = value;
    }
    else throw Exception("Value " + Convert::toString(value)+ " is not valid. OriginalSigma must be in [0, 1[.", LOCATION);
}

}

}

#endif
