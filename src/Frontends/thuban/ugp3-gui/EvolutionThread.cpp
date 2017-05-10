
// Quest'idea ha un GROSSO problema: come funziona la condivisione di attributi fra thread? qui io sparo le cose dentro LOG_INFO, ma come vengono effettivamente recepite dal thrad principale? E' un casino...
// Al momento, la soluzione è non scrivere nulla sui LOGs dalla GUI mentre il thread gira 

#include "EvolutionThread.h"

// project classes
#include "MicroGPMainWindow.h"

// standard classes
#include <chrono>
#include <sstream>

// ugp3 classes
#include "ConsoleHandler.h"
#include "EvolutionaryAlgorithm.h"
#include "Exception.h"
#include "Level.h"
#include "Population.h"
#include "Program.h"

// Qt classes
#include <QMessageBox>
#include <QString>

using namespace std;
using namespace std::chrono;
using namespace ugp3;
using namespace ugp3::core;
using namespace ugp3::log;
using namespace Ui;

void EvolutionThread::run()
{
	LOG_DEBUG << "Entering thread..." << ends;
	LOG_DEBUG << "Stop condition is " << this->stopConditionReached << "." << ends;

	// check if a stop condition has been reached
	if( this->stopConditionReached == true )
	{
		emit signalEvolutionCannotStart();
		return;
	}
	
	// the thread is running
	this->isRunning = true;	

	// start the evolutionary algorithm, kickstarting all the populations
	for(unsigned int i = 0; i < this->window->evolutionaryAlgorithm->getPopulationCount(); i++)
	{
		// this will be used to temporary store the stream with the statistics
		ostringstream ssStatistics;

		Population& population = this->window->evolutionaryAlgorithm->getPopulation(i);
		
		LOG_INFO 	<< "* Population \"" 
				<< this->window->evolutionaryAlgorithm->getPopulation(i).getName() 
				<< "\" (kickstart)" 
				<< ends;

		if( population.getGeneration() == 0 ) 
		{
		    population.setupPopulation();
		}

		population.evaluateAndHandleClones();

		// keep track of time
		time_point<steady_clock> stopTime = steady_clock::now();
		this->window->evolutionaryAlgorithm->setElapsedTime( duration_cast<seconds>(stopTime - this->window->evolutionaryAlgorithm->getStartTime()) );
	
		population.removeDeadCandidates();
		population.showStatistics();
		
		// update the statistics
		population.dumpStatistics( ssStatistics );
		emit signalUpdateStatistics(QString::fromStdString( ssStatistics.str() ));
	}

	// save "step 0" of the algorithm
	this->window->evolutionaryAlgorithm->saveDumpState();
	// show the statistics
	this->window->evolutionaryAlgorithm->writeStatisticsStream();
	// update the window
	this->window->updateTextEdit();
	emit signalUpdateGraph(); 
	
	// main loop
	this->stopConditionReached = false;
	while( this->isRunning == true && this->stopConditionReached == false)
	{
		this->stopConditionReached = true;

		// perform step for each population, then check
		// if the global stop condition was reached
		for(unsigned int p = 0; p < this->window->evolutionaryAlgorithm->getPopulationCount(); p++)
		{
			// this will be used to temporary store the stream with the statistics
			ostringstream ssStatistics;

			Population& population = this->window->evolutionaryAlgorithm->getPopulation(p);
			
			if( !population.checkStopCondition() )
			{
				LOG_INFO << "* Population \"" << population.getName() << "\" generation " << (population.getGeneration() + 1)  << ends;
				population.step();
				population.showStatistics();

				population.dumpStatistics( ssStatistics );
				//LOG_INFO << "CSV statistics:" << ssStatistics.str() << ends;
				emit signalUpdateStatistics( QString::fromStdString( ssStatistics.str() ) );
				
				this->window->updateTextEdit();
				emit signalUpdateGraph(); 
			}
			
			this->stopConditionReached = this->stopConditionReached && population.checkStopCondition();
		}
		// TODO implement migration and stuff
		
		// keep track of time
		time_point<steady_clock> stopTime = steady_clock::now();
		this->window->evolutionaryAlgorithm->setElapsedTime( duration_cast<seconds>(stopTime - this->window->evolutionaryAlgorithm->getStartTime()) );
	

		this->window->evolutionaryAlgorithm->writeStatisticsStream();
		this->window->evolutionaryAlgorithm->saveDumpState();
	}
	
	if( this->isRunning == true)
	{
		LOG_INFO << "Evolution stopped because a stop condition was reached." << ends;
		emit signalEvolutionTerminated();
	}
	else
	{
		LOG_INFO << "Evolution stopped by the user." << ends;
		emit signalEvolutionPaused(); 
	}

	// in any case, the thread is no longer running
	this->isRunning = false;
	
	// update text edit in main window
	LOG_DEBUG << "Stop condition is " << this->stopConditionReached << "." << ends;
	this->window->updateTextEdit();

	return;

}

void EvolutionThread::setParentWindow(MicroGPMainWindow* window)
{
	this->window = window;
}

void EvolutionThread::pauseEvolution()
{
	this->isRunning = false;
}

void EvolutionThread::resetEvolution()
{
	// TODO actually reset everything inside the population(s)?
	

	LOG_DEBUG << "Resetting evolutionary process..." << ends;
	this->stopConditionReached = false;
	this->run();
}
