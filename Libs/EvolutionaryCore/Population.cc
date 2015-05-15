/***********************************************************************\
|                                                                       |
| Population.cc                                                         |
|                                                                       |
| This file is part of MicroGP v3 (ugp3)                                |
| http://ugp3.sourceforge.net/                                          |
|                                                                       |
| Copyright (c) 2002-2015 Giovanni Squillero                            |
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
| $Revision: 661 $
| $Date: 2015-04-23 11:37:49 +0200 (Thu, 23 Apr 2015) $
\***********************************************************************/

/**
 * @file Population.cc
 * Implementation of the Population class.
 * @see Population.h
 * @see Population.xml.cc
 * @see Population.instantiate.cc
 */

#include "ugp3_config.h"
#include "Population.h"
#include "LineInformation.h"
#include "Parameters.h"
#include "OperatorSelector.h"
#include "Statistics.h"
#include "EvolutionaryAlgorithm.h"
#include "TournamentSelection.h"
#include "RankingSelection.h"
#include "Operator.h"
#include "GroupPopulationParameters.h"
#include "RegexMatch.h"
#include "Environment.h"
#include "Distances.h"

using namespace std;
using namespace ugp3;
using namespace ugp3::log;
using namespace ugp3::core;
using namespace ugp3::ctgraph;

Population::Population(
                      unique_ptr< PopulationParameters > parameters, unsigned int generation, const EvolutionaryAlgorithm& parent)
:   algorithm(parent),
  generation(generation),
  m_steadyStateGenerations(0),
  m_entropy(0)
{
    _STACK;
}

Population::Population(const EvolutionaryAlgorithm& algorithm)
: algorithm(algorithm),
  generation(0),
  m_steadyStateGenerations(0),
  m_entropy(0)
{

}

void Population::save(const string& fileName) const
{
    ofstream output;
    output.open(fileName.c_str());
    if( output.is_open() == false )
    {
        throw Exception("Cannot access file \"" + fileName + "\"", LOCATION);
    }

    output << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl;
    this->writeXml(output);
    output.close();
}

bool Population::checkStopCondition()
{
    if (this->getParameters().getMaximumGenerationsStop() == true)
    {
        Assert(this->getParameters().getMaximumGenerations() >= this->generation);

        if (this->getParameters().getMaximumGenerations() == this->generation)
        {
            LOG_INFO << "Reached maximum number of generations" << ends;
            return true;
        }
    }

    if( this->getParameters().getMaximumEvaluationsStop() == true )
    {
        if( this->getParameters().getMaximumEvaluations() <= this->getParameters().getEvaluator().getTotalEvaluations() )
        {
            LOG_INFO << "Reached maximum number of total evaluations" << ends;
            return true;
        }
    }
    
    /* FIXME strange to compare here the global time from the algorithm, maybe
     * this test should be moved to EvolutionaryAlgorithm. */
    if (this->getParameters().getMaximumTimeStop() == true && 
        getAlgorithm().getElapsedTime() >= this->getParameters().getMaximumTime())
    {
        LOG_INFO << "Maximum time elapsed" << ends;
        return true;
    }

    if (getParameters().getEvaluator().getExternalStopRequest()) {
        LOG_INFO << "External stop request received." << ends;
        return true;
    }
    
    if (this->getParameters().getMaximumFitnessStop() == true)
    {
        if (checkMaximumFitnessReached())
            return true;
    }
    
    if (getParameters().getSteadyStateGenerationsStop()) {
        updateSteadyStateGenerations();
        if (m_steadyStateGenerations >= getParameters().getMaximumSteadyStateGenerations()) {
            LOG_INFO << "Reached maximum number of steady-state generations." << ends;
            return true;
        }
    }
    
    return false;
}

double Population::useInertia(double oldValue, double newValue)
{
    return getParameters().getInertia() * oldValue + (1.0 - getParameters().getInertia()) * newValue;
}



void Population::step()
{
    _STACK;
    
    // start the next generation
    this->generation++;
    SETENV("UGP3_GENERATION", Convert::toString(this->generation));
    LOG_VERBOSE << "Performing generation step " << this->generation << ends;

    if (getCandidateCount() == 0) {
        LOG_WARNING << "The population is empty" << ends;
        return;
    }

    // All the individuals (and groups) created in this generation are stored into this
    // temporary container.
    // The generation will be merged together with its ancestors at the end of
    // the population's evolutionary step.
    vector<CandidateSolution*> newGeneration;

    LOG_VERBOSE << "Applying operators... " << ends;

    LOG_INFO << "Generating offspring" << Progress(Progress::START) << ends;

    PopulationParameters &params = this->getParameters();
    OperatorSelector& selector = params.getActivations().getOperatorSelector();
    selector.prepareForSelections();
    
    // The lambda parameter specifies the number of operators that should be
    // applied to the population at each generation
    unsigned int l = 0;
    while (l < getParameters().getLambda()) {
        // Select the best operator according to MAB algorithm
        OperatorSelector::Result result = selector.select();
        CallData* callData = result.data->newCallData();
        vector<CandidateSolution*> generated = applyOperator(callData, result);
        if (!generated.empty()) {
            selector.success(result);
            newGeneration.insert(newGeneration.end(), generated.begin(), generated.end());
            ++l;
        } else {
            selector.failure(result);
        }
        LOG_INFO << "Generating offspring" << Progress(l / (double) getParameters().getLambda()) << ends;
    }
    LOG_INFO << "Generating offspring" << Progress::END << ends;
    LOG_DEBUG << "Merging the new generation with its ancestors..." << ends;
    
    // NOTE/DET Sort the new generation by id.
    std::sort(newGeneration.begin(), newGeneration.end(), CandidateSolution::OrderById());

    // Merge the new generation with its ancestors
    mergeNewGeneration(newGeneration);

    // Dump all the population before starting evaluation, including zombies (if required)
    if (getParameters().getDumpBeforeEvaluation()) {
        this->dumpAllCandidates();
    }

    // Invalidate the fitness of all the individuals, including zombies (if required)
    if (getParameters().getInvalidateFitnessAfterGeneration()) {
        discardFitnessValues();
    }
    
    /*
     * NOTE Up to this point in the generation, there should be no dead candidate.
     * The following `evaluate()' function is allowed to kill individuals, but
     * it MUST NOT remove/delete any candidate from the population, because all
     * of them are needed to update operator statistics.
     */

    /*
     * Evaluate the fitness of the candidates that do not have a valid fitness.
     * This function is allowed to kill candidates (e.g. clones).
     * NOTE It is (almost) useless to kill clones before the evaluations since
     * the evaluator already has caching.
     */
    evaluateAndHandleClones();
    
    /*
     * NOTE At this point, all fitness values (raw and scaled) of all not-dead
     * candidates (including zombies) must be valid.
     * 
     * The fitness of dead candidates can be either valid or invalid. 
     * It can be used but MUST be checked for validity before use.
     */
#ifndef NDEBUG
    checkFitnessValidity();
#endif
    
    // Now collect the statistics and update the endogen parameters
    updateOperatorStatistics(newGeneration);
    this->getParameters().getActivations().step(getParameters());
    
    // Build a list of the best raw candidates and save them from future death
    selectNewZombifiableCandidates();
    
    /*
     * NOTE From now on, it is OK to remove/delete dead candidates.
     * WARNING The pointers returned by getBest/Worst.. might be dangling,
     * DO NOT dereference them. TODO nullify them
     * However, the killing of candidates should be performed using
     * exclusively the `setDeath()' function, which will perform required
     * zombifications automatically.
     */

    // Make the candidates older
    age();

    // Perform allopatric selection and resize the population
    // discarding the individuals with low fitness or advanced age
    slaughtering();
    
    // Deal with zombies from previous generations
    handleZombies();
    
    removeDeadCandidates();
    
    if (extincted())
        return;
    
    /*
     * Update scaled fitness and entropy for the next generation,
     * based on the real state of the population.
     */
    prepareForCommit();
    
    // Save a reference to the best and worst individuals
    commit();
    
    // only once every EPOCH generations
    if (this->getGeneration() % EPOCH == 0) {
        LOG_INFO << "Generation: " << this->getGeneration() << " -- Now changing the self-adapting parameters..." << ends;
        // Update the mutation strength of the operators
        // (uses operator statistics of the current generation)
        this->updateSigma();
        
        // Update the endogen parameters of the selection procedure (e.g. tau)
        // (uses operator statistics of the current generation)
        getParameters().getSelector().updateEndogenParameters(*this);
        
        // Reset the statistics
        getParameters().getActivations().epoch(getParameters());
    }
    
    // No more than mu individuals should survive
    Assert(getLiveCandidateCount() <= getParameters().getMu());
}

std::vector<CandidateSolution*> Population::applyOperator(
    CallData* callData,
    const OperatorSelector::Result& selected)
{
    LOG_VERBOSE << "Applying operator \"" << selected.op << "\" to population" << ends;
    
    std::vector<CandidateSolution*> newCandidates;
    
    // Apply the genetic operator on the set of selected individuals.
    selected.op->apply(*this, newCandidates);
    
    // All the new candidates must be valid!
    for (CandidateSolution*& candidate: newCandidates) {
        Assert(candidate->validate());
        if (!candidate->validate()) {
            // Do not crash MicroGP, just discard it
            LOG_WARNING << "The new candidate " << candidate
            << " generated by the operator " << selected.op
            << " is not valid. It has been discarded." << ends;
            delete candidate;
            candidate = nullptr;
        }
    }
    newCandidates.erase(std::remove(newCandidates.begin(), newCandidates.end(),
                                    nullptr), newCandidates.end());
    
    // Increase operator usage
    callData->setValidChildrenCount(newCandidates.size());
    // Bind new candidates to this operator call
    for (CandidateSolution* candidate: newCandidates) {
        candidate->getLineage().setCallData(callData);
    }
    
    if (newCandidates.empty()) {
        // no individuals were generated by the operator
        // complete operator failure!
        LOG_VERBOSE << "The operator \"" << selected.op << "\" failed." << std::ends;
    } else {
        LOG_VERBOSE
        << "The operator " << selected.op << " generated "
        << newCandidates.size() << " new valid candidates." << ends;
    }
    
    return newCandidates;
}

void Population::updateSigma()
{
    double goodIndividuals = 0;
    double veryGoodIndividuals = 0;
    double badIndividuals = 0;
    double newSigma;
    double thresholdGoodIndividuals = 0;

    // Debug
    LOG_VERBOSE << getParameters().getActivations().toString() << ends;


    //counting good and bad individuals
    for( unsigned int i = 0; i < getParameters().getActivations().getDataCount(); i++ )
    {
        Data& data = getParameters().getActivations().getData(i);

        veryGoodIndividuals += data.getPerformance(Performance::VeryGood);
        goodIndividuals += data.getPerformance(Performance::Good);
        badIndividuals += data.getPerformance(Performance::Bad) + data.getPerformance(Performance::VeryBad);
    }

    // parameters are evaluated only once each EPOCH generations
    veryGoodIndividuals /= EPOCH;
    goodIndividuals /= EPOCH;
    badIndividuals /= EPOCH;

    // if the performance of the genetic operators is good, sigma increases;
    // else, it decreases
    thresholdGoodIndividuals = 0.1 * getParameters().getLambda();

    // if there are at least *2* veryGoodIndividuals, we increase sigma
    if( veryGoodIndividuals >= 1 )
    {
        // newSigma = 1 - numeric_limits<double>::epsilon();
        newSigma = 0.99; // Avoid extreme values of sigma, it really slows down the offspring generation
    }
    // else, if there are less goodIndividuals than a certain threshold value,
    // sigma decreases
    else if( goodIndividuals <= thresholdGoodIndividuals )
    {
        //newSigma = getParameters().getSigma() + delta;
        newSigma = 0.01; // Avoid extremely low sigma, it makes most operators useless
    }
    else
    {
        //newSigma = getParameters().getSigma() - delta;
        newSigma = getParameters().getSigma();
    }

    // Debug
    LOG_VERBOSE << "Since there are: " << veryGoodIndividuals
      << " veryGoodIndividuals; " << goodIndividuals
      << " goodIndividuals; " << badIndividuals << " badIndividuals; Sigma is shifting towards "
      << newSigma <<". (Threshold is " << thresholdGoodIndividuals << " goodIndividuals)" << ends;

    // new value of sigma
    getParameters().setSigma( getParameters().getSigma() *
                                getParameters().getInertia() +
                                newSigma * (1.0 - getParameters().getInertia()) );

    return;

}

void Population::showStatistics(void) const
{
    _STACK;
    
    // write the summary in the log
    LOG_INFO << "Current global entropy: " <<  this->getEntropy() << ends;
    // TODO add a showStatistics() method to the selector class
    if (dynamic_cast<const TournamentSelection*>(&getParameters().getSelector()) != nullptr)
    {
        const TournamentSelection& selection = (const TournamentSelection&)getParameters().getSelector();
        
        LOG_INFO    << "Sigma: " <<  getParameters().getSigma(); 
        
        if( selection.getMetaTau() > 0 )
            LOG_INFO    << "; Tau: " << ( selection.getMetaTau() * this->getCandidateCount() )
            << " (" << (selection.getMetaTau() * 100) << "% of the population)" << ends;
        else
            LOG_INFO    << "; Tau: " << selection.getTau() 
            << " (" << (selection.getTau() / this->getCandidateCount() * 100) << "% of the population)" 
            << ends;
    }
    else if (dynamic_cast<const RankingSelection*>(&getParameters().getSelector()) != nullptr)
    {
        
        const RankingSelection& selection = (const RankingSelection&)getParameters().getSelector();
        
        LOG_INFO << "Sigma: " <<  getParameters().getSigma() << "; Pressure: " << selection.getPressure() << ends;
    }
    else
    {
        LOG_INFO << "Sigma: " <<  getParameters().getSigma() << ends;
    }
    
    // TODO: add information about other types of selection before "else"
    
    // write information about number of evaluations and time elapsed
    getParameters().getEvaluator().showStatistics();
    
    chrono::hours hours = chrono::duration_cast<chrono::hours>(getAlgorithm().getElapsedTime());
    chrono::minutes minutes = chrono::duration_cast<chrono::minutes>(getAlgorithm().getElapsedTime() - hours);
    chrono::seconds seconds = chrono::duration_cast<chrono::seconds>(getAlgorithm().getElapsedTime() - minutes);

    LOG_INFO 	<< "Elapsed time: " << (hours.count() > 9 ? "" : "0") << hours.count() << ":" 
		<< (minutes.count() > 9 ? "" : "0") << minutes.count() << ":" 
		<< (seconds.count() > 9 ? "" : "0") << seconds.count() 
		<< ends;
}

// ambitious function, aiming at recreating BORG v3
CandidateSolution* Population::assimilate(std::string fileName, ugp3::constraints::Constraints* modifiedConstraints)
{
    /*
     LOG_DEBUG << "Borg v3.0 :: file \"" << fileName << "\", prepare to be assimilated..." << ends;
     
     LOG_DEBUG << "This is just a series of trials..." << ends;
     
     string text = "This is text containing text! Text! Teeeeext!";
     string regex = "([T|t][e]+xt)";
     //string text = "bingo bongo   Bingo    Bongo bingo         Bongo";
     //string regex = "\\s*([B|b]ingo)\\s+([B|b]ongo)\\s*";
     vector<string> matches;	
     
     LOG_DEBUG << "Now calling RegexMatch..." << ends;
     RegexMatch::regexMatch(text, regex, matches);
     
     LOG_DEBUG << "Results from RegexMatch (" << matches.size() << "):" << ends;
     for(unsigned int i = 0; i < matches.size(); i++)
     {
     LOG_DEBUG << "Match[" << i << "]: \"" << matches[i] << "\"" << ends;
}
*/
    
    // first of all, get the reference constraints; they will be later altered and returned in the pointer modifiedConstraints
    const ugp3::constraints::Constraints& constraints = this->getParameters().getConstraints();
    
    // load the file to assimilate, storing it in an array with one string per line
    vector<LineInformation> textToAssimilate;
    ifstream input( fileName.c_str() );
    
    if( input.is_open() == false )
    {
        LOG_ERROR << "Cannot open file to assimilate \"" << fileName << "\". Aborting assimilation process..." << ends;
        return nullptr;
    }
    
    // here, a new class is used to store information about
    // each line: LineInformation
    unsigned int lines = 0;
    while( input.good() )
    {
        string buffer;
        getline(input, buffer);
        
        LineInformation tempLineInformation(buffer);
        textToAssimilate.push_back(tempLineInformation);
        lines++;
    }
    input.close();
    
    LOG_DEBUG << "File \"" << fileName << "\": " << lines << " lines read." << ends;
    
    // first of all, we must remove all the labels and update the structure of the file
    // get the regular expression for the label
    const string& identifier = constraints.getIdentifierFormat()->get("[A-Z0-9]+");
    const string& labelFormat = constraints.getLabelFormat()->get( "(" + identifier + ")" );
    string labelFormatForRegexp;
    
    // now, replace all spaces with the corresponding regular expression
    for(unsigned int i = 0; i < labelFormat.length(); i++)
    {
        if( labelFormat[i] == ' ' )
        {
            labelFormatForRegexp += "\\s*";
        }
        else
        {
            labelFormatForRegexp += labelFormat[i];
        }
    }
    
    // I just add another regexp to match everything that comes after the label
    labelFormatForRegexp += "(.*)";
    
    LOG_DEBUG << "Regular expression for labels is \"" << labelFormatForRegexp << "\": looking for labels..." << ends;	
    
    // look for labels in every line and match them LIKE A BOSS
    for(unsigned int l = 0; l < textToAssimilate.size(); l++)
    {
        vector<string> matches;
        
        // every time a label is found
        if( RegexMatch::regexMatch( textToAssimilate[l].text, labelFormatForRegexp, matches ) > 0 )
        {
            LOG_DEBUG 	<< "Found label \"" << matches[0] << "\" on line #" << l 
            << ": \"" << textToAssimilate[l].text << "\"." << ends;
            
            // note the label, put the rest of the expression in the original
            textToAssimilate[l].label = matches[0];
            textToAssimilate[l].text = matches[1];
            
            LOG_DEBUG 	<< "Label is \"" << textToAssimilate[l].label << "\", rest of the text is \""
            << textToAssimilate[l].text << "\"." << ends;
            
            // search for the labels and note the corresponding lines?
            for(unsigned int l2 = 0; l2 < textToAssimilate.size(); l2++)
            {
                vector<string> matches2;
                
                if( l2 != l && 
                    RegexMatch::regexMatch( textToAssimilate[l2].text, textToAssimilate[l].label, matches2 ) > 0 )
                {
                    textToAssimilate[l2].referenceTo = l;
                    LOG_DEBUG	<< "Line #" << l2 << " : \"" << textToAssimilate[l2].text 
                    << "\" contains a reference to line #" << l << ends;
                }
		
		// TODO: send a warning if a label is not referenced anywhere?
            }
        } // if the string matches
    }// for every line to assimilate
    
    // first, some debug: the new individual, with labels removed
    LOG_DEBUG << "Individual without labels:" << ends;
    for(unsigned int i = 0; i < textToAssimilate.size(); i++) LOG_DEBUG << textToAssimilate[i].text << ends;
	
    // and now, let the MATCHING BEGIN!
    // two variables, to keep track of the last lines matched
    unsigned int topLineNotMached = 0;
    unsigned int bottomLineNotMatched = textToAssimilate.size() - 1;

    // build a regex for global prologue and epilogue, match them top-down and bottom-up, respectively
    // here we have to get the regular expression from an instance of the Expression class;
    // the best way to do this, it to write a method "getRegex()" inside the Expression class,
    // that, depending on the type of parameter, creates a [] regex bracket; for example,
    // [0-9]+ would match any integer, but not a floating point representation!
    string globalPrologueRegex = constraints.getPrologue().getExpression().getRegex();
    string globalEpilogueRegex = constraints.getEpilogue().getExpression().getRegex();

    // IMPORTANT NOTE: having a regex that ends with '\n' might be a huge issue for our matching algorithm; so, for the moment, the '\n' is replaced by '\0'
    LOG_DEBUG << "Regular expression for the global prologue is: \"" << globalPrologueRegex << "\"" << ends;
    LOG_DEBUG << "Regular expression for the global epilogue is: \"" << globalEpilogueRegex << "\"" << ends;

    // if the globalPrologue or the globalEpilogue are empty, skip the corresponding matching
    if( globalPrologueRegex.length() > 0 )
    {
	// avoid issues with '\n'
	if( globalPrologueRegex.back() == '\n' ) globalPrologueRegex.back() = '\0';

	// TODO matching, starting from the top
    }
    else
	LOG_DEBUG << "Global prologue is empty, skipping matching..." << ends;

    if( globalEpilogueRegex.length() > 0 )
    {
	// TODO matching, starting from the bottom
	if( globalEpilogueRegex.back() == '\n' ) globalEpilogueRegex.back() = '\0';
    }
    else
	LOG_DEBUG << "Global epilogue is empty, skipping matching..." << ends;

    // for each Section!
    for(unsigned int s = 0; s < constraints.getSectionCount(); s++)
    {
	// get a pointer to the section
	const ugp3::constraints::Section& section = constraints.getSection(s);
	
	unsigned int sectionBeginning = topLineNotMached;
	unsigned int sectionEnd = bottomLineNotMatched; // TODO this could be modified, to look for the epilogue top-down
		
	string sectionPrologueRegex = section.getPrologue().getExpression().getRegex();
	string sectionEpilogueRegex = section.getEpilogue().getExpression().getRegex();
	
	LOG_DEBUG << "Regex for section \"" << section.getId() << "\" prologue is: \"" << sectionPrologueRegex << "\"" << ends; 
	LOG_DEBUG << "Regex for section \"" << section.getId() << "\" epilogue is: \"" << sectionEpilogueRegex << "\"" << ends; 
	
	if( sectionPrologueRegex.length() > 0 )
	{
		// first, try to match the prologue
		if( RegexMatch::incrementalRollbackMatch( sectionPrologueRegex, textToAssimilate, sectionBeginning, sectionEnd, RegexMatch::topDown) == 0 )
		{
			LOG_DEBUG << "Section prologue not found." << ends;
		}
		else
		{
			LOG_DEBUG << "Section prologue found!" << ends;
		}
	}

    }
    
    return nullptr;
}

bool Population::checkMaximumFitnessReached()
{
    if (getBestCandidate() && getBestCandidate()->getRawFitness().getValues() >= getParameters().getMaximumFitness()) {
        LOG_INFO << "Reached maximum fitness: " << getBestCandidate()->getRawFitness() << ends;
        return true;
    }
    return false;
}

void Population::updateSteadyStateGenerations()
{
    if (!m_previousMaxFitness.getIsValid()) {
        if (getBestCandidate()) {
            LOG_VERBOSE << "Steady state: previous max fitness is invalid, initializing." << ends;
            m_previousMaxFitness.setValues(getBestCandidate()->getRawFitness().getValues());
            m_steadyStateGenerations = 0;
        } else {
            LOG_WARNING << "Steady state: previous max fitness is invalid and no best candidate to initialize." << ends;
        }
        return;
    }
    if (getBestCandidate() && getBestCandidate()->getRawFitness() > m_previousMaxFitness) {
        m_previousMaxFitness.setValues(getBestCandidate()->getRawFitness().getValues());
        m_steadyStateGenerations = 0;
        LOG_VERBOSE << "Steady state: new maximum fitness!" << ends;
    } else {
        ++m_steadyStateGenerations;
        LOG_INFO << "Steady state: the maximum fitness did not change during the last "
        << m_steadyStateGenerations << " generations, max "
        << getParameters().getMaximumSteadyStateGenerations() << "." << ends;
    }
}

void Population::dumpStatistics(ostream& output) const
{
    _STACK;
    
    const PopulationParameters& params = getParameters();
    
    output << "," << this->generation;
    output << "," << this->getEntropy();
    output << "," << params.getMu();
    output << "," << params.getLambda();
    output << "," << params.getSelector().getCsvVal();
    output << "," << params.getSigma();
    
    getParameters().getEvaluator().dumpStatistics(output);
    
    // OPERATORS' ACTIVATION PROBABILITIES
    for (unsigned int i = 0; i < params.getActivations().getDataCount(); i++) 
    {
        params.getActivations().getData(i).dumpStatistics(output);
    }
}


void Population::dumpStatisticsHeader(ostream& output) const
{
    _STACK;
    
    const PopulationParameters& params = getParameters();
    
    output << "," << this->name << "_Generation";
    output << "," << this->name << "_E";
    output << "," << this->name << "_Mu";
    output << "," << this->name << "_Lambda";
    output << "," << this->name << "_" << params.getSelector().getCsvText();
    output << "," << this->name << "_Sigma";
    
    getParameters().getEvaluator().dumpStatisticsHeader(name, output);
    
    // OPERATORS' ACTIVATION PROBABILITIES
    for (unsigned int i = 0; i < params.getActivations().getDataCount(); i++) 
    {
        params.getActivations().getData(i).dumpStatisticsHeader(name, output);
    }
}


bool Population::useResultOrTakeOldestOrTakeFirstId(int result, const CandidateSolution* a, const CandidateSolution* b) const
{
    if (result != 0)
        return (result > 0);
    
    if (a->getBirth() != b->getBirth()) {
        // Oldest goes first
        return a->getBirth() < b->getBirth();
    }
    // Same age. To make the ordering deterministic, we order by IDs.
    // Those are unique. We get a strict total ordering.
    return a->getId() < b->getId();
}

bool Population::compareClones(const CandidateSolution* a, const CandidateSolution* b) const
{
    // Regroup zombies at the end of the group of clones
    if (a->isZombie() != b->isZombie())
        // `a' XOR `b' is a zombie
        return b->isZombie(); // a goes first iff b is the zombie
        
    // a and b are both alive or both zombies, order them
    return useResultOrTakeOldestOrTakeFirstId(0, a, b);
}

bool Population::compareHeroes(const CandidateSolution* a, const CandidateSolution* b) const
{
    return useResultOrTakeOldestOrTakeFirstId(a->getFitness().compareTo(b->getFitness()), a, b);
}

bool Population::compareForSelection(const CandidateSolution* a, const CandidateSolution* b) const
{
    return useResultOrTakeOldestOrTakeFirstId(a->getFitness().compareTo(b->getFitness()), a, b);
}

bool Population::compareForFitnessHole(const CandidateSolution* a, const CandidateSolution* b) const
{
    return useResultOrTakeOldestOrTakeFirstId(a->getDeltaEntropy().compareTo(b->getDeltaEntropy()), a, b);
}

bool Population::compareRawBestWorst(const CandidateSolution* a, const CandidateSolution* b) const
{
    return useResultOrTakeOldestOrTakeFirstId(a->getRawFitness().compareTo(b->getRawFitness()), a, b);
}

bool Population::compareScaledBestWorst(const CandidateSolution* a, const CandidateSolution* b) const
{
    return useResultOrTakeOldestOrTakeFirstId(a->getFitness().compareTo(b->getFitness()), a, b);
}

bool Population::compareOperatorPerformance(const CandidateSolution* a, const CandidateSolution* b) const
{
    int result = a->getFitness().compareTo(b->getFitness());
    if (result != 0) {
        return result > 0;
    }
    result = a->getDeltaEntropy().compareTo(b->getDeltaEntropy());
    if (result != 0) {
        return result > 0;
    }
    // Same fitness and same entropy, it's not an improvement.
    // TODO add other criteria example: size (bloat)
    return false;
}

std::unique_ptr<Population> Population::fromFile(
    const string& xmlFileName, const EvolutionaryAlgorithm& evolutionaryAlgorithm)
{
    _STACK;
    
    xml::Document populationFile;
    populationFile.LoadFile(xmlFileName);
    
    return Population::instantiate(*populationFile.RootElement(), evolutionaryAlgorithm);
}

unique_ptr<Population> Population::fromParametersFile(
    const EvolutionaryAlgorithm& algorithm,
    const string& fileName)
{
    _STACK;
    
    xml::Document parametersFile;
    
    // possible exceptions here, if file does not exist or problems during PopulationParameters::instantiate()
    try
    {
        parametersFile.LoadFile(fileName);
        const xml::Element& element = *parametersFile.RootElement();
        if (element.ValueStr() != PopulationParameters::XML_NAME) {
            throw xml::SchemaException("expected element 'parameters' (found '" + element.ValueStr() + "').", LOCATION);
        }
        
        const string& type = xml::Utility::attributeValueToString(element, PopulationParameters::XML_ATTRIBUTE_TYPE);
        unique_ptr<Population> population = Population::instantiate(algorithm, type);
        population->getParameters().readXml(element);
        return population;
    }
    catch(const Exception& e)
    {
        //LOG_DEBUG << "I am PopulationParameters::fromFile and I got this exception: \"" << e.what() << "\"." << ends;
        throw;
    } 
}

std::tuple<double, double> Population::computeSharingApport(Group* a, Group* b)
{
    LOG_DEBUG << "Computing entropic distance between groups " << *a << " and " << *b << ends;
    
    double distance = ugp3::Distances::entropic(a->getMessage(), b->getMessage());
    
    double apport = -1;
    double radius = dynamic_cast<GroupPopulationParameters&>(getParameters()).getGroupFitnessSharingRadius();
    if (distance < radius) {
        // actually sh(distance) = 1 - (distance / radius)^alpha, but alpha = 1
        apport = 1 - (distance / radius);
    }
    return std::make_tuple(distance, apport);
    
}

std::tuple<double, double> Population::computeSharingApport(Individual* i1, Individual* i2)
{
    double distance = 0.0;
    
    // if "hamming" is specified in the parameters, use the Hamming distance
    if( this->getParameters().getFitnessSharingDistance() == "hamming")
    {
        LOG_DEBUG << "Computing Hamming distance between individual " << *i1 << " and individual " << *i2 << ends;
        
        LOG_DEBUG << "Individual " << *i1 << " is " << i1->getExternalRepresentation() << "\"" << ends;
        LOG_DEBUG << "Individual " << *i2 << " is " << i2->getExternalRepresentation() << "\"" << ends;
        
        distance = ugp3::Distances::hamming(i1->getExternalRepresentation(), i2->getExternalRepresentation());
    }
    // if "editing" is specified in the parameters, use the Editing distance
    else if( this->getParameters().getFitnessSharingDistance() == "editing")
    {
        LOG_DEBUG << "Computing editing distance between individual " << *i1 << " and individual " << *i2 << ends;
        
        distance = ugp3::Distances::levenshtein(i1->getGraphContainer().getNodeHashSequence(),
                                                i2->getGraphContainer().getNodeHashSequence());
        
        // The following implementation is slow
        //distance = ugp3::Distances::levenshtein(i1->getExternalRepresentation(), i2->getExternalRepresentation());
    }
    else if( this->getParameters().getFitnessSharingDistance() == "entropic")
    {
        LOG_DEBUG << "Computing entropic distance between individual " << *i1 << " and individual " << *i2 << ends;
        
        distance = ugp3::Distances::entropic(i1->getMessage(), i2->getMessage());
    }
    // add other types of phenotypic/genotypic distances here...
    else
    {
        throw Exception("Distance measure \"" + this->getParameters().getFitnessSharingDistance() + "\" not recognized.",
                        LOCATION);
    }
    
    LOG_DEBUG << "Distance is " << distance << ends;
    
    double apport = -1;
    if (distance < this->getParameters().getFitnessSharingRadius()) {
        // actually sh(distance) = 1 - (distance / radius)^alpha, but alpha = 1
        apport = 1 - (distance / this->getParameters().getFitnessSharingRadius() );
    }
    return std::make_tuple(distance, apport);
}

void Population::merge(unique_ptr<Population> population)
{
    _STACK;
    
    LOG_INFO << "Merging populations " << population->getName() << " into population " << this->getName() << ends;
    
    /* 
     * TODO unify the concepts/mechanisms for merging and migrations so that they can also work for
     * populations of groups.
     */
    
    #if 0
    
    if( this->getType() != population->getType() )
    {
    throw Exception("Cannot merge two populations of different type.", LOCATION);
}
vector<Individual*> newIndividuals;

// put a copy of all the individuals in this population
for( unsigned int i = 0; i < population->individuals.size(); i++ )
{

const Individual& individual = population->getIndividual(i);

// clone the individual setting this population as
// the parent container
unique_ptr<Individual> newIndividual = individual.clone(*this);
newIndividual->setLineage(
    unique_ptr<Lineage<Individual> >(new Lineage<Individual>(individual.getLineage().getGeneticOperatorName(), individual.getLineage().getParentNames()))
    );
    
    newIndividuals.push_back(newIndividual.release());
}

this->addIndividuals(newIndividuals);
#endif 
}

void Population::reloadStatistics(const xml::Element* element)
{
    // TODO 
}

