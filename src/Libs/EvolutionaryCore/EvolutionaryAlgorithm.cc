/***********************************************************************\
|                                                                       |
| EvolutionaryAlgorithm.cc                                              |
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
| $Revision: 656 $
| $Date: 2015-04-15 14:16:06 +0200 (Wed, 15 Apr 2015) $
\***********************************************************************/

#include "ugp3_config.h"
#include "SignalHandling.h"
#include "EvolutionaryCore.h"
using namespace std;
using namespace std::chrono;
using namespace ugp3::core;



EvolutionaryAlgorithm::EvolutionaryAlgorithm()
  : outputPathName("statusDump.xml"),
  overwriteOutput(true),
  statisticsPathName(""),
  algorithmStep(0),
  migrator(nullptr),
  m_elapsedTimeThisRun(seconds(0)),
  m_elapsedTimePreviousRuns(seconds(0))
{
    // save real start time (the one below is just to count the time spent running exclusively ugp3, not regarding the evaluator)
    m_startTime = steady_clock::now();
}

EvolutionaryAlgorithm::~EvolutionaryAlgorithm()  
{
    _STACK;

    LOG_DEBUG << "Destructor: ugp3::core::EvolutionaryAlgorithm" << ends;
    
    if (migrator) {
        delete migrator;
    }

    for (unsigned int i = 0; i < this->populations.size(); i++)
    {
        delete this->populations[i];
        this->populations[i] = nullptr;
    }
}

void EvolutionaryAlgorithm::addPopulation(unique_ptr<Population> population)
{
    _STACK;

    if (population.get() == nullptr)
    {
        throw ArgumentNullException("population", LOCATION);
    }

    for (unsigned int i = 0; i < this->populations.size(); i++)
    {
        if (this->populations[i] == population.get())
        {
            LOG_WARNING << "The population already belongs to the algorithm" << ends;
            return;
        }
    }

    this->populations.push_back(population.release());

    LOG_VERBOSE << "Added a population to the evolutionary algorithm" << ends;
}

bool EvolutionaryAlgorithm::step()
{
    _STACK;

    if (this->populations.empty() == true)
    {
        throw Exception("At least one population should be specified before the evolutionary algorithm can be started.", LOCATION);
    }

    bool globalStopReached = false;
    bool populationExtincted = false;
    this->algorithmStep++;
    LOG_VERBOSE << "Performing algorithm step number " << this->algorithmStep << "..." << ends;


    // iterate through each population
    // the choice taken, at the moment, is to terminate the execution if even one of the populations reaches the stop condition
    for (unsigned int p = 0; p < this->populations.size(); p++)
    {
        if (!populations[p]->checkStopCondition())
        {
            LOG_INFO << "* Population \"" << this->populations[p]->getName() << "\" generation " << this->populations[p]->getGeneration() + 1  << ends;
            this->populations[p]->step();
            this->populations[p]->showStatistics();
            if (this->populations[p]->extincted() == true )
                populationExtincted = true;
        }
	else
	{
	    LOG_INFO << "* Population \"" << this->populations[p]->getName() << "\" reached a stop condition. The global evolution will now stop..." << ends;
	    globalStopReached = true;
	}
     }

    if (globalStopReached == false && populationExtincted == false)
    {
        // migrate individuals between the populations
        if (this->migrator != nullptr && this->populations.size() > 1)
        {
            this->migrator->migrate(&this->populations);
        }
    }
    else
    {
        LOG_INFO << "Evolution terminated" << ends;
    }

    return globalStopReached;
}

void EvolutionaryAlgorithm::save(const string& xmlFile) const
{
    _STACK;

    LOG_VERBOSE << "Saving xml file \"" << xmlFile << "\"..." << ends;

    ofstream output;
    output.open(xmlFile.c_str());
    if (output.is_open() == false)
    {
        throw Exception("Cannot access file \"" + xmlFile + "\"", LOCATION);
    }

    this->writeXml(output);
    output.close();

    LOG_VERBOSE << "Evolutionary Algorithm successfully saved" << ends;
}

Population& EvolutionaryAlgorithm::getPopulation(unsigned int index) const
{
    _STACK;

    if (index >= this->populations.size())
    {
        throw IndexOutOfBoundsException("populations", LOCATION);
    }

    return *this->populations[index];
}

void EvolutionaryAlgorithm::run()
{
    // check if the statistics should be saved
    // const bool saveStatistics = this->statisticsPathName != "";
    bool extincted = false;
    
    // open a stream for the statistics; it now became a class attribute
    //std::ofstream statisticsStream;

    // avoid being interrupted while executing the algorithm
    ugp3::SigIntRequested = false;
    deferSignals = true;
    ugp3::SigIntMessage = "SIGINT Caught... Stopping at the end of current generation.";
    signal(SIGINT, signalHandler);

#ifdef NDEBUG
    try
    {
#endif
	/** moved to its own method
        // is the CSV statistics file defined?
        if (saveStatistics == true)
        {
	    // what happens if the file already exists? it means that the algorithm restarted
	    // and so the header should not be printed, and the statisticsStream should be opened in append mode;
	    // in other cases, the filename is unique, because it has already been checked and renamed (if a file from
	    // previous unrecovered executions exists)
	    if( File::exists( statisticsPathName ) )
	    {
		statisticsStream.open(statisticsPathName.c_str(), std::ofstream::app); 
	    }
	    else
	    {
		statisticsStream.open(statisticsPathName.c_str());
		statisticsStream << "GEN,CPU";
		for (unsigned int i = 0; i < this->populations.size(); i++)
		{
		    populations[i]->dumpStatisticsHeader(statisticsStream);
		}
		statisticsStream << endl;
	    }
        }
	*/
	// the first call to openStatisticsStream has been relocated to Program.Run.cc
	//this->openStatisticsStream();

        bool checkStopCondition = false;
        while (checkStopCondition == false)
        {

            // perform the generation step
            checkStopCondition = this->step();
            
            time_point<steady_clock> stopTime = steady_clock::now();
                
            m_elapsedTimeThisRun = duration_cast<seconds>(stopTime - m_startTime);
            
#ifdef NDEBUG
            // Check that we spent more than half our time evaluating candidates
            // In debug mode this is not true because the debug asserts are slow
            seconds evaluationTimeAllPops(0);
            for (Population* pop: populations) 
	    {
                evaluationTimeAllPops += pop->getParameters().getEvaluator().getTotalTime();
            }
            if (evaluationTimeAllPops < getElapsedTime() / 2) 
	    {
                LOG_WARNING << "We are spending more time in the evolutionary "
                "algorithm than in the evaluator." << std::ends;
            }
#endif

            // save statistics to CSV file
	    this->writeStatisticsStream();

	    /*
            if (this->statisticsPathName != "")
            {
                statisticsStream << this->algorithmStep << "," << m_elapsedTimeThisRun.count();
                for (unsigned int i = 0; i < this->populations.size(); i++)
                {
                    Assert(this->populations[i] != nullptr);
                    Population& population = *this->populations[i];
                    if (population.extincted() == false)
                    {
                        population.dumpStatistics(statisticsStream);
                    }
                    else
                    {
                        extincted = true;
                    }
                }
                statisticsStream << std::endl;
            }
	    */
	
	    // the commented code above has been replaced by this check; I don't know
	    // why there was an if...else... to avoid dumping stats if a population is extinct
            for (unsigned int i = 0; i < this->populations.size(); i++)
            {
		if(populations[i]->extincted() == true) extincted = true;
	    }
	    

            /** 
             * LOOKS HORRIBLE !
             *  */ 
            if (extincted == true)
            {
                LOG_INFO << "Stopping because the population is extinct!" << ends;
                return;
            }
	    // I agree, WTF is this code above??? RETURN???

            this->saveDumpState();

            ugp3::SigIntMessage = "SIGINT Caught... Stopping at the end of current generation.";
            if (ugp3::SigIntRequested)
            {
                LOG_INFO << "Stopping as requested..." << ends;
                /** 
                statisticsStream << "<!-- incomplete data: " << "algorithm
                  interrupted by SIGINT -->"  << std::endl;
                statisticsStream << "</" << XML_STEPS << ">"  << std::endl;
                statisticsStream.close();
                LOG_INFO << "Finished saving the algorithm state. Exiting..." << ends;
                **/ 
                return;
            }

            // if a file with a given name is present in ugp3 running directory, then the evolution is stopped
            struct stat stFileInfo;
            int fileNotExists;

            // attempt to get the file attributes
            string stopFileName = "UGP3_STOP_REQUESTED";
            fileNotExists = stat(stopFileName.c_str(), &stFileInfo);

            if (fileNotExists == 0)
            {
                LOG_INFO << "Special file \"" << stopFileName << "\" found in ugp3 running directory. Stop requested" << ends;
                checkStopCondition = true;
            }


        } // end while checkStopCondition == false

        if ( this->statisticsPathName != "")
        {
            statisticsStream.close();
        }
#ifdef NDEBUG
    }
    catch (exception& e)
    {
        if ( this->statisticsPathName != "" )
        {
            // YEUCH!!!!!!!!!!
            /***
            statisticsStream
              << "<!-- incomplete data: "
              << "algorithm interrupted due to exception -->"  << std::endl;
            statisticsStream
              << "<!-- exception details:"
              << xml::Utility::transformXmlEscChar(e.what())
            << " -->"  << std::endl;
            statisticsStream << "</" << XML_STEPS << ">"  << std::endl;
            */
            statisticsStream.close();
        }

        throw;
    }
#endif
}

void EvolutionaryAlgorithm::saveDumpState() const
{
    if (this->outputPathName != "")
    {
        LOG_VERBOSE << "Saving algorithm dump state..." << ends;

        // avoid being interrupted while writing the file
        ugp3::SigIntMessage = "Please be patient,"
                              " the algorithm is being saved ...";

        if (this->overwriteOutput == true)
        {
            // dump the algorithm state before moving to the next generation
            if (File::exists(this->outputPathName) == true)
            {
                // rename the old dump file to avoid overwriting it
                File::rename(this->outputPathName, "old." + this->outputPathName);
            }

            // save the file
            this->save(this->outputPathName);

            // remove the old file dump since the new one has been successfully saved
            File::remove("old." + this->outputPathName);
        }
        else
        {
            // save the file
            const std::string& newFileName = File::appendTimeToName(
                                                                   File::appendStringToName(
                                                                                           this->outputPathName, "." +
                                                                                           Convert::toString(this->algorithmStep) )
                                                                   );

            this->save(newFileName);
        }
    }
    else
	LOG_VERBOSE << "Output path name not defined, cannot save state..." << ends;
}

// these two new functions, along with their respective attributes, have been added to store information on "Generation 0"
// of each population; since the "Generation 0" is handled outside EvolutionaryAlgorithm (in Program.Run.cc),
// it is necessary to expose a couple of methods to manipulate the statistics ofstream
void EvolutionaryAlgorithm::openStatisticsStream()
{
        // is the CSV statistics file defined?
        if (this->statisticsPathName != "")
        {
	    // what happens if the file already exists? it means that the algorithm restarted
	    // and so the header should not be printed, and the statisticsStream should be opened in append mode;
	    // in other cases, the filename is unique, because it has already been checked and renamed (if a file from
	    // previous unrecovered executions exists)
	    if( File::exists( statisticsPathName ) )
	    {
		this->statisticsStream.open(statisticsPathName.c_str(), std::ofstream::app); 
	    }
	    else
	    {
		this->statisticsStream.open(statisticsPathName.c_str());
		this->statisticsStream << "GEN,CPU";
		for (unsigned int i = 0; i < this->populations.size(); i++)
		{
		    this->populations[i]->dumpStatisticsHeader(statisticsStream);
		}
		this->statisticsStream << endl;
	    }
        }

}

void EvolutionaryAlgorithm::writeStatisticsStream()
{
            if (this->statisticsPathName != "")
            {
                statisticsStream << this->algorithmStep << "," << m_elapsedTimeThisRun.count();
                for (unsigned int i = 0; i < this->populations.size(); i++)
                {
                    Assert(this->populations[i] != nullptr);
                    Population& population = *this->populations[i];
                    if (population.extincted() == false)
                    {
                        population.dumpStatistics(statisticsStream);
                    }
                }
                statisticsStream << std::endl;
            }
}
