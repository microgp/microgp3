/***********************************************************************\
|                                                                       |
| EvaluatorFileDispatcher.cc |
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
| $Revision: 658 $
| $Date: 2015-04-23 11:26:47 +0200 (Thu, 23 Apr 2015) $
\***********************************************************************/

/**
 * @file EvaluatorFileDispatcher.cc
 *
 */

// this flag should be activated for a SPECIFIC PROJECT, only
//#define STARCRAFT_GP

#include "EvaluatorFileDispatcher.h"
#include "Individual.h"
#include "EvaluatorCommon.h"
#include "Population.h"
#include "Group.h"
#include "GEIndividual.h"
#include "GroupPopulation.h"

#include "Log.h"
#include "File.h"
#include "Debug.h"
#include "Environment.h"

using namespace std;

namespace ugp3 {
namespace core {

template <class T>
EvaluatorFileDispatcher<T>::EvaluatorFileDispatcher(EvaluatorCommon< T >& evaluator)
: EvaluatorDispatcher<T>(evaluator), m_requestsSinceFlush(0)
{
}

template <class T>
EvaluatorFileDispatcher<T>::~EvaluatorFileDispatcher()
{
}

template <class T>
void EvaluatorFileDispatcher<T>::evaluate(T& object)
{
    m_requestsSinceFlush++;
    m_pendingEvaluations.push(&object);
}

template <class T>
void EvaluatorFileDispatcher<T>::flush(std::function<void(double)>& showProgress)
{
    unsigned int concurrentEvaluations = EvaluatorDispatcher<T>::getEvaluator().getConcurrentEvaluations();
    std::vector<T*> objects;
    objects.reserve(concurrentEvaluations);

    while (!m_pendingEvaluations.empty()) 
    {
        showProgress((double)(m_requestsSinceFlush - m_pendingEvaluations.size()) / m_requestsSinceFlush);
        while (!m_pendingEvaluations.empty() && objects.size() < concurrentEvaluations) 
	{
            objects.push_back(m_pendingEvaluations.front());
            m_pendingEvaluations.pop();
        }
        runScript(objects);
        objects.clear();
    }
    showProgress(1);
    m_requestsSinceFlush = 0;
}


template <class T>
void EvaluatorFileDispatcher<T>::retrieveEvaluations(const vector<T*>& evaluatedCandidates)
{
    _STACK;
    
    // update the fitness parameters of each individual
    const std::string& outputFile = EvaluatorDispatcher<T>::getEvaluator().getOutputFile();
    
    if (File::exists(outputFile) == false) 
    {
        throw Exception("The evaluator did not create the fitness file \"" + outputFile + "\".", LOCATION);
    }
    
    ifstream fitnessFile;
    fitnessFile.open(outputFile.c_str());
    if (fitnessFile.is_open() == false) 
    {
        throw Exception("The file \"" + outputFile + "\" is not accessible.", LOCATION);
    }
    
    LOG_DEBUG << "Loading fitness evaluation results from file \"" << outputFile << "\"..." << ends;
    for (unsigned int i = 0; i < evaluatedCandidates.size(); i++) 
    {
        if (fitnessFile.eof() == true) 
	{
            throw Exception("Unexpected end-of-file reached.", LOCATION);
        }
        
        // get the line
        string line;
        getline(fitnessFile, line);
        istringstream lineStream(line);
        
        // parse the fitness values
        Assert(evaluatedCandidates[i] != nullptr);
        T& evaluatedCandidate = *evaluatedCandidates[i];
        vector<double> newValues;
        for (unsigned int f = 0; f < evaluatedCandidate.getPopulation().getParameters().getFitnessParametersCount(); f++) 
	{
            double value = 0;
            lineStream >> value;
            if (lineStream.fail() == true) 
	    {
                throw Exception("Bad evaluator output format.", LOCATION);
            }
            
            if(value < 0) throw Exception("The evaluator produced negative fitness values.", LOCATION);
            newValues.push_back(value);
        }
        LOG_DEBUG << "Parsed " << newValues.size() << " fitness parameters" << ends;
        evaluatedCandidate.getRawFitness().setValues(newValues);
        
        // parse the description
        string description;
        lineStream >> description;
        evaluatedCandidate.getRawFitness().setDescription(description);
        
        LOG_VERBOSE << "New fitness for " << TypeName<T>::name << " "
        << evaluatedCandidate << " is "
        << evaluatedCandidate.getRawFitness() << ends;
    }
    
    // check for external stop request
    if (fitnessFile.eof() == false) 
    {
        string line;
        getline(fitnessFile, line);
        
        if(line.substr(0, 5) == "#stop") 
	{
            EvaluatorDispatcher<T>::getEvaluator().setExternalStopRequest(true);
        }
    }
    
    fitnessFile.close();
    
    if (File::remove(outputFile) == false) 
    {
        LOG_WARNING << "Could not delete file \"" << outputFile << "\"" << ends;
    }
    
    for (auto candidate: evaluatedCandidates) 
    {
        EvaluatorDispatcher<T>::getEvaluator().cacheFitness(candidate->getNormalizedPhenotype(), candidate->getRawFitness());
    }
}

template <class T>
void EvaluatorFileDispatcher<T>::runScript(const vector<T*>& evaluatedCandidates)
{
    string scriptCommandline = "";
    vector<string> inputFiles;

    // save the list of files to evaluate
    ofstream individualsListFile("individualsToEvaluate.txt");
    if(individualsListFile.is_open() == false)
    {
        throw Exception("Could not create the individuals list file.", LOCATION);
    }

    // extract the individuals from the queue
    for (T* candidate: evaluatedCandidates) 
    {
        //(!)20111221const string& fileName = File::appendStringToName(this->inputFile, individual->getId());
        const string& fileName = File::formatToName(EvaluatorDispatcher<T>::getEvaluator().getInputFile(), candidate->getId());
        
        // generate the code from the individual
        candidate->toCode(fileName, &inputFiles);
        
        // add the file name as a paremeter of the command line
        scriptCommandline = scriptCommandline + fileName + " ";
        
        // add the file name in the file containing the individuals to evaluate
        individualsListFile << fileName << endl;
    }

    individualsListFile.close();
    scriptCommandline = scriptCommandline.substr(0, scriptCommandline.length() - 1);
    
    // set environment
    SETENV("UGP3_OFFSPRING", scriptCommandline.c_str());
    SETENV("UGP3_FITNESS_FILE", EvaluatorDispatcher<T>::getEvaluator().getOutputFile());
    
    scriptCommandline = EvaluatorDispatcher<T>::getEvaluator().getScriptFile() + " " + scriptCommandline;
    LOG_DEBUG << "Executing command \"" << scriptCommandline << "\" ... "<< ends;
    
    // set the environment variables
    /* 20090825
     f o*r(unsi*gned int i = 0; i < this->environme*ntVariables.size(); i++)
     {
     LOG_WARNING
     << "Setting environment variable " << environmentVariables[i].first << "=\""
     << environmentVariables[i].second << "\"" << ends;
     
     SETENV(environmentVariables[i].first, environmentVariables[i].second);
}
*/
    
    // call the evaluator
    int returnValue = system(scriptCommandline.c_str());

    LOG_DEBUG
    << "The process \"" <<  EvaluatorDispatcher<T>::getEvaluator().getScriptFile()
    << "\" exited with code " << returnValue  << ends;
    
#ifdef STARCRAFT_GP
    // THIS IS JUST FOR A SPECIFIC APPLICATION! REMOVE IT!
    while( returnValue != 0 )
    {
	LOG_DEBUG << "Re-running the evaluation process..." << ends;
	returnValue = system(scriptCommandline.c_str());
    }
#endif

    // retrieve the results
    retrieveEvaluations(evaluatedCandidates);
    
    // clean up the temporary files
    File::remove("individualsToEvaluate.txt");
    if (EvaluatorDispatcher<T>::getEvaluator().getRemoveTemporaryFiles()) {
        for (auto file: inputFiles) {
            File::remove(file);
        }
    }
}

template class EvaluatorFileDispatcher<Group>;
template class EvaluatorFileDispatcher<Individual>;

}
}
