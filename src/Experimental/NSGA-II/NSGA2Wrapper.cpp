// TODO all try-catch constructs, everywhere

#include "NSGA2Wrapper.h"

// ugp3 includes
#include "Log.h"
#include "ConsoleHandler.h"
#include "FileHandler.h"
#include "BriefFormatter.h"
#include "ExtendedFormatter.h"
#include "PlainFormatter.h"

#include "EvolutionaryCore.h"
#include "GeneticOperator.h"
#include "Operators/AllopatricDifferentialOperator.h"
#include "Operators/AlterationMutation.h"
#include "Operators/InsertionMutation.h"
#include "Operators/OnePointCrossover.h"
#include "Operators/OnePointImpreciseCrossover.h"
#include "Operators/RandomizerOperator.h"
#include "Operators/RemovalMutation.h"
#include "Operators/ReplacementMutation.h"
#include "Operators/ScanMutationBitArray.h"
#include "Operators/ScanMutationConstant.h"
#include "Operators/ScanMutationFloat.h"
#include "Operators/ScanMutationInnerLabel.h"
#include "Operators/ScanMutationInteger.h"
#include "Operators/SimpleDifferentialOperator.h"
#include "Operators/SingleParameterAlterationMutation.h"
#include "Operators/SubGraphInsertionMutation.h"
#include "Operators/SubGraphRemovalMutation.h"
#include "Operators/SubGraphReplacementMutation.h"
#include "Operators/SwapMutation.h"
#include "Operators/TwoPointCrossover.h"
#include "Operators/TwoPointImpreciseCrossover.h"

// SHARK includes
#include <Array/Array.h>
#include <FileUtil/Params.h>
#include <MOO-EALib/PopulationMOO.h>
#include <MOO-EALib/ArchiveMOO.h>

using namespace std;

// void constructor
NSGA2Wrapper::NSGA2Wrapper()
{}

// constructor from XML file
NSGA2Wrapper::NSGA2Wrapper(string xmlFileName)
{
	LOG_INFO << "Reading population's settings file \"" << xmlFileName << "\"..." << ends;

	// unfortunately, we need to register ugp3's genetic operators, or it will create exceptions
	// when the algorithm tries to read them in the population file
	LOG_VERBOSE << "Registering genetic operators..." << ends;
	ugp3::core::GeneticOperator::registration(new ugp3::core::AllopatricDifferentialOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::AlterationMutationOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::InsertionMutationOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::InverOverCrossoverOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::OnePointCrossoverOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::OnePointImpreciseCrossoverOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::RandomizerOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::RemovalMutationOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::ReplacementMutationOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::ScanMutationBitArrayOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::ScanMutationConstantOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::ScanMutationFloatOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::ScanMutationInnerLabelOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::ScanMutationIntegerOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::SimpleDifferentialOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::SingleParameterAlterationMutationOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::SubGraphInsertionMutationOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::SubGraphRemovalMutationOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::SubGraphReplacementMutationOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::SwapMutationOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::TwoPointCrossoverOperator());
	ugp3::core::GeneticOperator::registration(new ugp3::core::TwoPointImpreciseCrossoverOperator());
	LOG_VERBOSE << "Registration terminated." << ends;

	// read the population file
	// this has changed in the most recent version of ugp3
	// unique_ptr<ugp3::core::PopulationParameters> populationParameters = ugp3::core::PopulationParameters::fromFile(xmlFileName);
	// so, first we need to instantiate an algorithm
	ugp3::core::EvolutionaryAlgorithm algorithm;
	// then, given the algorithm, we read the parameters file
	unique_ptr<ugp3::core::Population> population = ugp3::core::Population::fromParametersFile(algorithm, xmlFileName);
	// finally, we get the population paremeters
	ugp3::core::PopulationParameters& populationParameters = population->getParameters();

	// set the corresponding parameters
	this->mu = populationParameters.getMu();
	this->lambda = populationParameters.getLambda();
	this->archiveSize = mu;
	LOG_INFO 	<< "Population size is " << this->mu 
			<< ", offspring size is " << this->lambda
			<< ends;

	this->numberOfObjectives = populationParameters.getFitnessParametersCount();
	LOG_INFO << "Number of objectives is " << this->numberOfObjectives << ends;

	this->fitnessFunction = populationParameters.getEvaluator().getScriptFile();
	this->fitnessFile = populationParameters.getEvaluator().getOutputFile();
	
	// the two stop conditions are optional
	if( populationParameters.getMaximumGenerationsStop() )
	{
		this->maxIterations = populationParameters.getMaximumGenerations();
	}
	else
	{
		this->maxIterations = 0;
	}

	if( populationParameters.getMaximumEvaluationsStop() )
	{
		this->maxEvaluations = populationParameters.getMaximumEvaluations();
	}
	else
	{
		this->maxEvaluations = 0;
	}

	// check: if all stop conditions are void, there is something wrong, the algorithm will never stop...
	if( this->maxEvaluations == 0 && this->maxIterations == 0)
	{
		LOG_WARNING << "No stop condition set. The algorithm will only stop by pressing CTRL+C...and then probably crash ^_^;" << ends;
	}
	
	// now, the hard part: from the constraints file, try to understand dimension and limits of the problem
	const ugp3::constraints::Constraints& constraints = populationParameters.getConstraints();
	bool isConstraintsGood = true;
	this->dimension = 0;

	// TODO theoretically, real values could also appear inside the global prologue, the global epilogue and shit
	//	but I really really hope everyone is using stuff inside the subsection's macros
	// for each section in the constraints (hopefully only one!)
	if( constraints.getSectionCount() > 1 )
	{
		LOG_ERROR << "There should be only ONE section inside the individual." << ends;
		isConstraintsGood = false;
	}
	
	const ugp3::constraints::Section& section = constraints.getSection(0);
	
	if( section.getSubSectionCount() > 1 )
	{
		LOG_ERROR << "There should be only ONE subsection inside the section." << ends;
		isConstraintsGood = false;
	}

	const ugp3::constraints::SubSection& subsection = section.getSubSection(0);

	// check: is the number of instances fixed?
	if( subsection.getMaximumMacroInstances() != subsection.getMinimumMacroInstances() )
	{
		LOG_ERROR << "The genome is of variable length. NSGA-II cannot manage non-fixed length problems." << ends;
		isConstraintsGood = false;
	}
			
	// for each macro definition inside the subsection (there should be only ONE)
	if( subsection.getMacroCount() > 1 )
	{
		LOG_ERROR << "There should be only ONE macro inside the section." << ends;
		isConstraintsGood = false;
	}

	ugp3::constraints::Macro& macro = subsection.getMacro(0);

	for(unsigned p = 0; p < macro.getParameterCount(); p++)
	{
		ugp3::constraints::Parameter& parameter = macro.getParameter(p);
		
		// so, if the parameter is NOT a FloatParameter, we can just forget about everything
		if( dynamic_cast<ugp3::constraints::FloatParameter*>( &parameter ) != nullptr )
		{
			// increase problem dimension
			this->dimension++;
			
			// set boundaries for the parameter
			ugp3::constraints::FloatParameter* floatParameter = 
				dynamic_cast<ugp3::constraints::FloatParameter*>( &parameter );
			
			this->variableMin.push_back( floatParameter->getMinimum() );
			this->variableMax.push_back( floatParameter->getMaximum() );
		}
		else
		{
			LOG_ERROR 	<< "Parameter \"" << parameter.getName() 
					<< "\" of macro \"" << macro.getId()
					<< "\" is not a floating point parameter." << ends;
			isConstraintsGood = false;
		}
	}
	
	// if the constraints are not good, too bad!
	if( isConstraintsGood == false )
	{
		LOG_ERROR << "Constraints not compliant with NSGA-II. Constraints should contain floating point values ONLY. Aborting..." << ends;
		// TODO throw an exception here
	}
	
	// and now, the REAL problem dimension; all macros *should* have the same number of parameters (hopefully)
	// so the number is the number of macro instances * number of parameters in a macro
	this->dimension *= subsection.getMaximumMacroInstances();
	
	// also, adjust the boundaries
	while( this->variableMin.size() < this->dimension && this->variableMax.size() < this->dimension )
	{
		for(unsigned p = 0; p < macro.getParameterCount(); p++)
		{
			this->variableMin.push_back( this->variableMin[p] );
			this->variableMax.push_back( this->variableMax[p] );
		} 
	}
	
	LOG_INFO << "Dimension of the problem is " << this->dimension << ends;
	if( this->dimension == 0 )
	{
		LOG_ERROR 	<< "Problem dimension cannot be 0. Aborting..." << ends;
		// TODO throw exception
	}

	// release the pointer
	//populationParameters.release();
	
	// there are some default values for the parameters tied to operators, but they can be overridden by the user somewhere else
	this->nm = 20;
	this->nc = 20;
	this->pc = 0.9;
	this->pm = 1.0 / this->dimension;
	
	// other default value: the seed is 0, if not specified elsewhere
	this->seed = 0;
	
	// current iterations start at 0
	this->iterations = 0;
}

// run the algorithm
int NSGA2Wrapper::run( bool minimizeFitnessFunction )
{
	// TODO make all checks on the attributes before starting

	LOG_INFO << "Initializing Shark NSGA-II..." << ends;

	// initialize random number generator
	if( this->seed == 0 )
	{
		this->seed = std::time(nullptr);
	}
	Rng::seed( this->seed );
	LOG_INFO << "Random seed is " << this->seed << ends;

	// define populations
	this->parents = new PopulationMOO(this->mu, ChromosomeT< double >(this->dimension));
	this->offspring = new PopulationMOO(this->lambda, ChromosomeT< double >(this->dimension));
	
	// initialize archive
	unsigned archiveSize = this->mu;
	archive = new ArchiveMOO(archiveSize);
	archive->setMaxArchive(archiveSize);

	// set minimization task
	if( minimizeFitnessFunction)
	{
		LOG_INFO << "NSGA-II will try to minimize the objectives..." << ends;
		parents->setMinimize();
		offspring->setMinimize();
		archive->minimize();
	}
	else
	{
		LOG_INFO << "NSGA-II will try to maximize the objectives..." << ends;
		parents->setMaximize();
		offspring->setMaximize();
		archive->maximize();
	}

	// set # objective funstions
	parents->setNoOfObj( this->numberOfObjectives );
	offspring->setNoOfObj( this->numberOfObjectives );

	// bounds used by operators
	if( variableMin.size() != 0 && variableMin.size() != dimension )
	{
		LOG_ERROR << "Warning: size of the problem and size of the boundaries differ. This will probably crash the program..." << ends;
	}

	vector<double> lower(dimension), upper(dimension);
	for (unsigned i = 0; i < lower.size() && i < this->variableMin.size(); ++i)
	{
		lower[i] = this->variableMin[i];
		upper[i] = this->variableMax[i];
	}

	// initialize all chromosomes of parent population
	LOG_INFO << "Generation 0: setting up the evolutionary process..." << ends;
	LOG_INFO << "NSGA-II will run for"; 
	if( this->maxIterations > 0 )
		LOG_INFO << " " << this->maxIterations  << " generations or"; 
	if( this->maxEvaluations > 0 )
		LOG_INFO << " " << this->maxEvaluations << " evaluations or";
	if( this->maxIterations == 0 && this->maxEvaluations == 0 )
		LOG_INFO << "ever";
	
	LOG_INFO << " until you create a file called \"" STOP_REQUESTED_FILE "\" in the current working directory." << ends;
	
	LOG_INFO << "All individuals will be saved inside file \"" << this->savePopulationFile << "\"." << ends;

	for (unsigned i = 0; i < parents->size(); ++i)
		((ChromosomeT< double >*)&(*this->parents)[ i ][ 0 ])->initialize(this->variableMin, this->variableMax);

	// reset evaluations and evaluate parents
	this->currentEvaluations = 0;
	LOG_INFO << "Generation 0: evaluating initial population..." << ends;
	for (unsigned i = 0; i < parents->size(); ++i)
		( this->computeFitness( (*this->parents)[i] ) );

	parents->crowdedDistance();

	// until a stop condition is met
	this->iterations = 0;
	while( !this->checkStopCondition(iterations) )
	{
		this->step(iterations);
		iterations++;
	}

	// prepare archive for data output
	archive->cleanArchive();
	for(unsigned ii = 0; ii < this->archiveSize; ii++)
		archive->addArchive( (*parents)[ ii ]);

	archive->nonDominatedSolutions();
	
	LOG_INFO << "The final Pareto front contains " << archive->size() << " individuals." << ends;
	
	return 0;
}

// single step of the algorithm
int NSGA2Wrapper::step(unsigned iteration)
{
	LOG_INFO << "Generation " << (iterations+1) << ": generating and evaluating offspring..." << ends;

	// copy parents to offsprings
	offspring->selectBinaryTournamentMOO(*parents);

	// recombine by crossing over two parents
	for(unsigned i = 0; i < offspring->size(); i += 2)
	{
		if (Rng::coinToss(pc))
		{
			dynamic_cast< ChromosomeT< double >& >((*offspring)[i][0]).SBX(dynamic_cast< ChromosomeT< double >& >((*offspring)[i+1][0]), variableMin, variableMax, nc, .5);
		}
	}

	// mutate by flipping bits and evaluate objective function
	for(unsigned i = 0; i < offspring->size(); ++i)
	{
		// modify
		dynamic_cast< ChromosomeT< double >& >((*offspring)[i][0]).mutatePolynomial(variableMin, variableMax, nm, pm);

		// evaluate objective function
		this->computeFitness( (*offspring)[i] );
	}

	// selection
	parents->selectCrowdedMuPlusLambda(*offspring);

	// every 10 generations the dominated solutions are removed from the archive
	if(!(iteration % 10)) 
	{
		archive->cleanArchive();
		for(unsigned i = 0; i < parents->size(); i++)
		{
			archive->addArchive((*parents)[i]);
		}
		archive->nonDominatedSolutions();
	}
	
	LOG_INFO << "There are currently " << this->archive->size() << " individuals in the Pareto front." << ends;
	
	return 0;
}

// write population to specified file
void NSGA2Wrapper::populationToCSV( string fileName, bool paretoFrontOnly )
{
	if( paretoFrontOnly )
	{
		// save only the Pareto front
		LOG_INFO << "Writing Pareto front to CSV file \"" << fileName << "\"..." << ends; 
	}
	else
	{
		// save the whole population
		LOG_INFO << "Writing whole population to CSV file \"" << fileName << "\"..." << ends; 
	}
	
	// horrible patch because the saveArchiveGPT uses char* instead of const char*
	char filenameChar[256];
	sprintf( filenameChar, "%s", fileName.c_str() );
	archive->saveArchiveGPT( filenameChar );
}

// check if a stop condition is reached
bool NSGA2Wrapper::checkStopCondition(unsigned iterations)
{
	if( this->maxIterations > 0 && iterations >= this->maxIterations )
	{
		LOG_INFO 	<< "Maximum number of generations (" << iterations << "/"
				<< this->maxIterations << ") reached. Stopping." << ends;
		return true;
	}
	else if( this->maxEvaluations > 0 && this->currentEvaluations >= this->maxEvaluations )
	{
		LOG_INFO 	<< "Maximum number of evaluations (" << this->currentEvaluations 
				<< "/" << this->maxEvaluations << ") reached. Stopping." << ends;
		return true;
	}
	else if( ugp3::File::exists( STOP_REQUESTED_FILE ) )
	{
		LOG_INFO 	<< "Found special file \"" STOP_REQUESTED_FILE "\". Stopping." << ends; 
		return true;
	}

	return false;
}

// call fitness function and compute value of an individual
void NSGA2Wrapper::computeFitness(IndividualMOO &individual)
{
	int returnValue = 0; //TODO the function could actually return something, to describe errors that could possibly arise?
	string commandLine = this->fitnessFunction + INDIVIDUAL_NAME;

	// temporary chromosomes for decoding
	ChromosomeT< double >& dblchrom = dynamic_cast< ChromosomeT< double >& >( individual[ 0 ] );
	
	// write individual to file
	ofstream output(INDIVIDUAL_NAME);
	
	if( !output.good() )
	{
		LOG_ERROR << "Cannot write on file \"" << INDIVIDUAL_NAME << "\"! Aborting..." << ends;
		// TODO throw exception
	}
	
	// write each chromosome of the individual to file
	// TODO change output format, depending on the constraints file...it's rather hard to do, unfortunately...
	for(unsigned c = 0; c < dblchrom.size(); c++)
	{
		output << dblchrom[c] << " "; // TODO specific format for this experiment
	}

	output.close();
	
	// call function
	LOG_DEBUG << "Calling function \"" << commandLine << "\"" << ends;
	returnValue = system( commandLine.c_str() );

	// read fitness file
	if( !ugp3::File::exists( this->fitnessFile ) )
	{
		LOG_ERROR << "Fitness file \"" << this->fitnessFile << "\" not found!" << ends;
		// TODO throw exception
	}
	
	// try to open the file
	ifstream input(this->fitnessFile.c_str());
	if( !input.good() )
	{
		LOG_ERROR << "Cannot open fitness file \"" << this->fitnessFile << "\"!" << ends;
		// TODO throw exception
	}
	
	LOG_DEBUG << "Fitness for individual is: "; 
	vector<double> fitnessValues;
	for(unsigned f = 0; f < this->numberOfObjectives; f++)
	{
		double temp;
		input >> temp;
		fitnessValues.push_back(temp);
		
		LOG_DEBUG << temp << " ";
	}
	individual.setMOOFitnessValues(fitnessValues);
	LOG_DEBUG << ends;

	input.close();

	// delete temporary files
	commandLine.clear();
	commandLine = "rm " INDIVIDUAL_NAME " ";
	commandLine += this->fitnessFile;
	returnValue = system( commandLine.c_str() );
	
	// if the population must be saved, do it like that (very slow, but I have no better idea...)
	if( this->savePopulationFile.length() > 0 )
	{
		LOG_DEBUG << "I am going to save everything on file \"" << this->savePopulationFile << "\"..." << ends;

		ofstream outputPopulation( this->savePopulationFile.c_str(), std::fstream::app );
		if( !outputPopulation.good() )
		{
			LOG_ERROR << "Cannot open population file \"" << this->savePopulationFile << "\"!" << ends;
			// TODO throw exception
		}
		
		// if the number of evaluations is 0, set the header
		if( this->currentEvaluations == 0 )
		{
			outputPopulation << "Generation";
			for(unsigned int i = 0; i < fitnessValues.size(); i++)
				outputPopulation << ",Fitness" << i;
			for(unsigned int i = 0; i < dblchrom.size(); i++)
				outputPopulation << ",Chromosome" << i;
			outputPopulation << endl;
		}

		// generation,fitness1,...,fitnessN,chromosome1,...,chromosomeN
		outputPopulation << this->iterations;
		for(unsigned int i = 0; i < fitnessValues.size(); i++)
			outputPopulation << "," << fitnessValues[i];
		for(unsigned int i = 0; i < dblchrom.size(); i++)
			outputPopulation << "," << dblchrom[i];
		outputPopulation << endl;

		// close file
		outputPopulation.close();
	}

	// increase evaluations
	this->currentEvaluations++;
}

