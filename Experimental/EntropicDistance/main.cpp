// Simple tool to compute the entropic distance and compare it agains the deltaFitness, for each couple of individuals in a population
// by Alberto Tonda, 2012 <alberto.tonda@gmail.com>

#include <iostream>
#include <sstream>

#include "ConsoleHandler.h"
#include "EvolutionaryCore.h"
#include "File.h"
#include "Log.h"
#include "OtherMetric.h"
#include "Program.h"

#define OUTPUT_FILE "analysis.csv"

#define AVG_ENTROPIC 0
#define STDEV_ENTROPIC 1
#define AVG_OTHER 2
#define STDEV_OTHER 3

// functions
void computeAllDistances(unique_ptr<ugp3::core::EvolutionaryAlgorithm> evolutionaryAlgorithm, OtherMetric* otherMetric, ostringstream* output, vector<double>& statistics);
double computeEntropicDistance(	const ugp3::core::EnhancedIndividual* startingPoint, const ugp3::core::EnhancedIndividual* endPoint);

int main(int argc, char* argv[])
{
	// check on the number of arguments
	if( argc == 1 )
	{
		cout 	<< "Program that will save all entropic distances and deltaFitnesses for the individuals in several population files. "
			<< "It will consider only the FIRST population inside every status file."
			<< endl << "Usage: " << endl << " " << argv[0] << " <status_file1.xml status_file2.xml ... status_fileN.xml>" << endl;
		return 0;
	}

	// open debug log
	/*
	cout << "Opening debug log..." << endl;
        ugp3::log::ConsoleHandler* debugHandler = new ugp3::log::ConsoleHandler();
        ::log_.addHandler(*debugHandler); //::log_ is a global instance of a Log class
        debugHandler->setLevel(ugp3::log::Level::Debug);
	*/
	
	// vector(s) and other stuff to hold the statistics
	vector<double> statistics(4, 0.0);
	vector<double> avgEntropicVector;
	vector<double> stdDevEntropicVector;
	vector<double> avgOtherVector;
	vector<double> stdDevOtherVector;
	double avgEntropic = 0.0, avgOther = 0.0, stdDevEntropic = 0.0, stdDevOther = 0.0;
	unsigned int files = 0;

	// register operators (used later)
	ugp3::frontend::Program::registerOperators();
	
	// instantiate OtherMetric, or leave it to nullptr to avoid computations
	OtherMetric* otherMetric = new OtherMetric();
	
	// the input arguments are a set of "status.xml" files
	cout << "Now reading all the files..." << endl;
	
	// stringstream
	ostringstream output;
	
	for(int a = 1; a < argc; a++)
	{
		cout << "Now reading file \"" << argv[a] << "\"..." << endl;
		// recover the algorithm
		unique_ptr<ugp3::core::EvolutionaryAlgorithm> evolutionaryAlgorithm = 
						ugp3::core::EvolutionaryAlgorithm::fromFile(argv[a], false);
		
		// compute the distances and save them in a buffer
		computeAllDistances(evolutionaryAlgorithm, otherMetric, &output, statistics);
		
		// store data
		avgEntropicVector.push_back( statistics[AVG_ENTROPIC] );
		avgEntropic += statistics[AVG_ENTROPIC];

		stdDevEntropicVector.push_back( statistics[STDEV_ENTROPIC] );
		stdDevEntropic += statistics[STDEV_ENTROPIC];

		avgOtherVector.push_back( statistics[AVG_OTHER] );
		avgOther += statistics[AVG_OTHER];

		stdDevOtherVector.push_back( statistics[STDEV_OTHER] );
		stdDevOther += statistics[STDEV_OTHER];

		// delete the algorithm
		evolutionaryAlgorithm.reset();
		
		// increase the number of files evaluated
		files++;
	}
	
	cout << "The CSV output will be written in \""OUTPUT_FILE"\"..." << endl;
	ofstream outputStream(OUTPUT_FILE);

	if(outputStream.good()) 
	{
		// header
		outputStream << "Generation,Individual1,Individual2,EntropicDistance,DeltaFitness";
		if( otherMetric != nullptr) outputStream << "," << otherMetric->name;
		outputStream << endl;
		outputStream << output.str();
	}
	outputStream.close();
	
	// compute some final statistics
	avgEntropic /= files;
	avgOther /= files;
	stdDevEntropic /= files;
	stdDevOther /= files;
	
	double sdAvgEntropic = 0.0, sdAvgOther = 0.0, sdStdDevEntropic = 0.0, sdStdDevOther = 0.0;
	
	cout << "AvgEntropic,StdDevEntropic,AvgOther,StdDevOther" << endl;

	for(unsigned int f = 0; f < files; f++)
	{
		sdAvgEntropic += abs( avgEntropicVector[f] - avgEntropic );
		sdAvgOther += abs( avgOtherVector[f] - avgOther );
		sdStdDevEntropic += abs( stdDevEntropicVector[f] - stdDevEntropic );
		sdStdDevOther += abs( stdDevOtherVector[f] - stdDevOther );

		cout 	<< avgEntropicVector[f] << "," << stdDevEntropicVector[f] << "," 
			<< avgOtherVector[f] << "," << stdDevOtherVector[f] << endl;
	}
	
	sdAvgEntropic /= files;
	sdAvgOther /= files;
	sdStdDevEntropic /= files;
	sdStdDevOther /= files;

	cout 	<< "Final statistics:" << endl
		<< "- Entropic Distance Average avg=" << avgEntropic << " ; stddev=" << sdAvgEntropic << endl
		<< "- Entropic Distance StdDev avg=" << stdDevEntropic << " ; stddev=" << sdStdDevEntropic << endl
		<< "- Other Distance Average avg=" << avgOther << " ; stddev=" << stdDevOther << endl
		<< "- Other Distance StdDev avg=" << stdDevOther << " ; stddev=" << sdStdDevOther << endl;
	
	cout << "Done." << endl;
	return 0;
}

// function that manages the computation of all distances and other details
void computeAllDistances(unique_ptr<ugp3::core::EvolutionaryAlgorithm> evolutionaryAlgorithm, OtherMetric* otherMetric, ostringstream* output, vector<double>& statistics)
{
	// the only population considered is the first one
	ugp3::core::Population& population = evolutionaryAlgorithm->getPopulation(0);
	
	// first of all, get the generation
	unsigned int generation = population.getGeneration();
	
	// arrays to store information and compute some statistics
	vector<double> entropicDistances;
	vector<double> otherDistances;
	double entropicDistanceAverage = 0.0;
	double otherDistanceAverage = 0.0;
	unsigned int numberOfEvaluations = 0;
	
	// now, for each couple of individuals
	for(int i = 0; i < population.getIndividualCount(); i++)
	{

		const ugp3::core::EnhancedIndividual* individual1;
		if( (individual1 = dynamic_cast<const ugp3::core::EnhancedIndividual*>( &population.getIndividual(i)) ) == nullptr)
		{
			cerr << "Error! Population file does not contain EnhancedIndividuals. Aborting..." << endl;
			exit(0);
		}
		
		for(int j = 0; j < population.getIndividualCount(); j++)
		if(i != j)
		{
			const ugp3::core::EnhancedIndividual* individual2 = 
					dynamic_cast<const ugp3::core::EnhancedIndividual*>( &population.getIndividual(j));
			
			// compute EntropicDistance
			double entropicDistance = computeEntropicDistance(individual1,individual2);
			entropicDistances.push_back( entropicDistance );
			entropicDistanceAverage += entropicDistance;

			// compute DeltaFitness (only on the first fitness function?)
			double deltaFitness = individual2->getFitness().getValues()[0] - individual1->getFitness().getValues()[0];
		
			// write everything in the buffer
			*output << generation << "," << individual1->getId() << "," << individual2->getId() << ","
				<< entropicDistance << "," << deltaFitness;
			
			// eventually compute OtherMetric
			if( otherMetric != nullptr)
			{
				double otherDistance = otherMetric->compute(individual1,individual2);
				*output << "," << otherDistance;
				otherDistances.push_back( otherDistance );
				otherDistanceAverage += otherDistance;
			}

			*output << endl;
			
			// increase number of evaluations
			numberOfEvaluations++;
		}
	}
	
	// here, store something about the average distance and (more importantly) the standard deviation
	// first, compute the average
	entropicDistanceAverage /= numberOfEvaluations;
	otherDistanceAverage /= numberOfEvaluations;
	
	// then, compute the standard deviation
	double stdDevEntropicDistance = 0.0;
	double stdDevOtherDistance = 0.0;
	for(unsigned int i = 0; i < numberOfEvaluations; i++)
	{
		stdDevEntropicDistance += abs( entropicDistances[i] - entropicDistanceAverage );
		stdDevOtherDistance += abs( otherDistances[i] - otherDistanceAverage );
	}
	stdDevEntropicDistance /= numberOfEvaluations;
	stdDevOtherDistance /= numberOfEvaluations;
	
	cout 	<< "avg(entropic) = " << entropicDistanceAverage << " ; stddev(entropic) = " << stdDevEntropicDistance << endl
		<< "avg(other) = " << otherDistanceAverage << " ; stddev(other) = " << stdDevOtherDistance << endl;
	
	statistics[AVG_ENTROPIC] = entropicDistanceAverage;
	statistics[STDEV_ENTROPIC] = stdDevEntropicDistance;
	statistics[AVG_OTHER] = otherDistanceAverage;
	statistics[STDEV_OTHER] = stdDevOtherDistance;

	return;
}

// function that computes the entropic distance
double computeEntropicDistance(	const ugp3::core::EnhancedIndividual* individual1, const ugp3::core::EnhancedIndividual* individual2)
{
	// to avoid problem with getMessageInformation...
	ugp3::core::EnhancedIndividual* startingPoint = const_cast<ugp3::core::EnhancedIndividual*>(individual1);
	ugp3::core::EnhancedIndividual* endPoint = const_cast<ugp3::core::EnhancedIndividual*>(individual2);

	// first, compute the entropy of both individuals
	ugp3::Message totalMessage;
	totalMessage += startingPoint->getMessageInformation();
	totalMessage += endPoint->getMessageInformation();
	
	// then, entropy of starting point alone
	ugp3::Message startingPointMessage;
	startingPointMessage += startingPoint->getMessageInformation();
	
	// finally, entropy of the end point alone
	ugp3::Message endPointMessage;
	endPointMessage += endPoint->getMessageInformation();
	
	// entropic distance is the the difference between the two
	return 2*totalMessage.getSize() - startingPointMessage.getSize() - endPointMessage.getSize();
}

