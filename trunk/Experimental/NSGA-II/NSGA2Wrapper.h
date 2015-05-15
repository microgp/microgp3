// wrapper for the NSGA2 implementation provided by Shark
// the main convenience is the detached fitness function

#ifndef NSGA2WRAPPER
#define NSGA2WRAPPER

#include <string>
#include <vector>

#define INDIVIDUAL_NAME "individual.in"
#define NSGA2_POPULATION_FILE "nsga2-population.csv"
#define STOP_REQUESTED_FILE "NSGA2_STOP_REQUESTED"

// forward declarations
class ArchiveMOO;
class IndividualMOO;
class PopulationMOO;

class NSGA2Wrapper
{

public:
	unsigned seed;		// random seed
	unsigned dimension;	// problem dimension
	unsigned mu;		// population size
	unsigned lambda;	// offspring size
	unsigned maxEvaluations;// maximum number of evaluations
	unsigned maxIterations;	// maximum number of generations
	unsigned archiveSize;	// how many solutions in external archive
	unsigned numberOfObjectives;
	unsigned iterations;	// current iterations

	// boundaries, for each variable
	std::vector<double> variableMin;
	std::vector<double> variableMax;
	
	// operators' parameters
	double   nm;
	double   nc;
	double   pc;	// crossover probability
	double   pm;	// mutation probability

	std::string fitnessFunction; // external fitness function to call
	std::string fitnessFile; // file where the fitness values will be stored
	
	// populations
	PopulationMOO* parents;
	PopulationMOO* offspring;
	
	// population archive
	ArchiveMOO* archive;
	
	// utility variable, to store the current evaluations
	unsigned currentEvaluations;
	// save the whole population?
	std::string savePopulationFile;
	
public:
	// constructors
	NSGA2Wrapper();
	NSGA2Wrapper(std::string xmlFileName);

	// run algorithm
	int run( bool minimizeFitnessFunction = true );
	// single step of the algorithm
	int step( unsigned iteration );
	// write population to CSV file
	void populationToCSV( std::string fileName, bool paretoFrontOnly = true );
	// call fitness function and compute value of an individual
	void computeFitness(IndividualMOO &i);
	// check if a stop condition is reached
	bool checkStopCondition(unsigned iterations);

};

#endif
