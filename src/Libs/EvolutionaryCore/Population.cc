/***********************************************************************\
|                                                                       |
| Population.cc                                                         |
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

#include <regex>

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

// utility function, later used to split strings 
//FIXME TODO create an appropriate static class? it bothers me to create a new class just for this...
void tokenize(const string& str, vector<string>& tokens, const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

// utility function, that is later used to sort macros
bool sortMacros( pair<string, ugp3::constraints::Macro*> i, pair<string, ugp3::constraints::Macro*> j)
{
	return ( i.first.length() < j.first.length() );
}


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
// NOTE
// I had to chang the following methods
// 1) ::getRegex() in about every class derived from Constraints/Parameter and inside Constraints/Expression
// 2) CGraphContainer::setPrologue() and CGraphContainer::setEpilogue()
// 3) Individual::setCGraphContainer() has been added

// FIXME does not manage outerLabel parameters
// FIXME does not manage multiple occurrences of the same subsection, I think
// FIXME maybe this should all be moved to IndividualPopulation, as we are dealing with individuals, only...
CandidateSolution* Population::assimilate(std::string fileName, ugp3::constraints::Constraints* modifiedConstraints)
{
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
	LOG_DEBUG << "I entered the first loop!" << ends;
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
            
	    // however, to match a label inside a string, you need a different regex
	    string labelInString = ".*(" + textToAssimilate[l].label + ").*";
                
            // search for the labels and note the corresponding lines?
            for(unsigned int l2 = 0; l2 < textToAssimilate.size(); l2++)
            {
                vector<string> matches2;
		
                if( l2 != l && 
                    RegexMatch::regexMatch( textToAssimilate[l2].text, labelInString/*textToAssimilate[l].label*/, matches2 ) > 0 )
                {
                    textToAssimilate[l2].referenceTo = l;
                    LOG_DEBUG	<< "Line #" << l2 << " : \"" << textToAssimilate[l2].text 
                    << "\" contains a reference from line #" << l 
		    << "\"" << textToAssimilate[l].text << "\""
		    << ends;
                }
		
		// TODO: send a warning if a label is not referenced anywhere?
            }
        } // if the string matches
	LOG_DEBUG << "And I am now outside of the \"if\" condition!" << ends;
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

    string globalProloguePath = constraints.getPrologue().getPath();
    string globalEpiloguePath = constraints.getEpilogue().getPath();

    LOG_DEBUG << "Regular expression for the global prologue is: \"" << globalPrologueRegex << "\"" << ends;
    LOG_DEBUG << "Regular expression for the global epilogue is: \"" << globalEpilogueRegex << "\"" << ends;

    // if the globalPrologue or the globalEpilogue are empty, skip the corresponding matching
    if( globalPrologueRegex.length() > 0 )
    {
	// avoid issues with '\n'

	// TODO matching, starting from the top
    }
    else
	LOG_DEBUG << "Global prologue is empty, skipping matching..." << ends;

    if( globalEpilogueRegex.length() > 0 )
    {
	// TODO matching, starting from the bottom
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
	
	// these two will be updated later, as section prologue and epilogue are matched
	unsigned int subsectionsBeginning = sectionBeginning;
	unsigned int subsectionsEnd = sectionEnd;
		
	string sectionPrologueRegex = section.getPrologue().getExpression().getRegex();
	string sectionEpilogueRegex = section.getEpilogue().getExpression().getRegex();
	
	string sectionProloguePath = section.getPrologue().getPath();
	string sectionEpiloguePath = section.getEpilogue().getPath();
	
	LOG_DEBUG << "Regex for section \"" << section.getId() << "\" prologue is: \"" << sectionPrologueRegex << "\"" << ends; 
	LOG_DEBUG << "Regex for section \"" << section.getId() << "\" epilogue is: \"" << sectionEpilogueRegex << "\"" << ends; 
	
	if( sectionPrologueRegex.length() > 0 )
	{
		// add some optional spaces at the beginning and at the end of the function
		sectionPrologueRegex = "[\\s]*" + sectionPrologueRegex + "[\\s]*";
		
		unsigned int upperLimit = sectionBeginning;
		unsigned int lowerLimit = sectionEnd;

		// try to match the prologue
		// note: subsectionsEnd is used here as a limit, and it will be modified by the function to its correct value
		if( RegexMatch::incrementalRollbackMatch( sectionPrologueRegex, sectionProloguePath, textToAssimilate, upperLimit, lowerLimit, RegexMatch::topDown) )
		{
			LOG_DEBUG << "Section prologue found!" << ends;
			
			// modify the beginning of the subsections: they shall start after the end of the section prologue
			subsectionsBeginning = lowerLimit + 1;
		}
		else
		{
			LOG_DEBUG << "Section prologue not found." << ends;
		}
	}
	
	// TODO now, this check is wrong, because even an empty regex is not empty; it contains [\\s]*[\\s]*
	if( sectionEpilogueRegex.length() > 0 )
	{
		// add some optional spaces at the beginning and at the end of the function
		sectionEpilogueRegex = "[\\s]*" + sectionEpilogueRegex + "[\\s]*";
		unsigned int upperLimit = subsectionsBeginning;
		unsigned int lowerLimit = sectionEnd;

		// try to match the prologue
		if( RegexMatch::incrementalRollbackMatch( sectionEpilogueRegex, sectionEpiloguePath, textToAssimilate, upperLimit, lowerLimit, RegexMatch::bottomUp) )
		{
			LOG_DEBUG << "Section epilogue found!" << ends;
			
			// note the point where the subsections might end
			subsectionsEnd = upperLimit - 1;
		}
		else
		{
			LOG_DEBUG << "Section epilogue not found." << ends;
		}
	}
	
	// TODO check if we actually found both boundaries of the section; if not, it's BAD
	unsigned int ssBeginning = subsectionsBeginning;
	unsigned int ssEnd = subsectionsEnd;
	
	// and now, let's start with the subsections!
	LOG_DEBUG << "Now looking for subsections between line #" << subsectionsBeginning << " and line #" << subsectionsEnd << ends;
	for(unsigned int ss = 0; ss < section.getSubSectionCount(); ss++)
	{
		const ugp3::constraints::SubSection& subsection = section.getSubSection(ss);
		
		// check the possible occurrences of this subsection
		unsigned int ssOccurrences = 0;

		// TODO some subsections can appear multiple times! check if this is one of those, and eventually
		//	bring back the counter (ss--) (maybe?!?)
		unsigned int macrosBeginning = ssBeginning;
		unsigned int macrosEnd = ssEnd;
		
		// first, try to match the prologue and the epilogue
		string subsectionPrologueRegex = subsection.getPrologue().getExpression().getRegex();
		string subsectionEpilogueRegex = subsection.getEpilogue().getExpression().getRegex();
		
		string subsectionProloguePath = subsection.getPrologue().getPath();
		string subsectionEpiloguePath = subsection.getEpilogue().getPath();
		
		// check if the prologue/epilogue is empty, and try to match accordingly
		if( subsectionPrologueRegex.length() > 0 )
		{
			subsectionPrologueRegex = "[\\s]*" + subsectionPrologueRegex + "[\\s]*";
			unsigned int upperLimit = ssBeginning;
			unsigned int lowerLimit = ssEnd;

			if( RegexMatch::incrementalRollbackMatch( subsectionPrologueRegex, subsectionProloguePath, textToAssimilate, upperLimit, lowerLimit, RegexMatch::topDown) )
			{
				LOG_DEBUG << "Subsection prologue found!" << ends;
				
				// the boundary for macros will lower to the end of the subsection prologue
				macrosBeginning = lowerLimit + 1;
			} 
			else LOG_DEBUG << "Subsection prologue not found!" << ends;
		}
		else LOG_DEBUG << "Empty subsection prologue, skipping..." << ends;

		if( subsectionEpilogueRegex.length() > 0 )
		{
			subsectionEpilogueRegex = "[\\s]*" + subsectionEpilogueRegex + "[\\s]*";
			unsigned int upperLimit = macrosBeginning;
			unsigned int lowerLimit = ssEnd;

			if( RegexMatch::incrementalRollbackMatch( subsectionEpilogueRegex, subsectionEpiloguePath, textToAssimilate, macrosBeginning, ssEnd, RegexMatch::topDown) )
			{
				LOG_DEBUG << "Subsection epilogue found!" << ends;
				
				// boundary for macros is raised
				macrosEnd = upperLimit - 1;
			}
		}
		else LOG_DEBUG << "Empty subsection epilogue, skipping..." << ends;
		
		// now, let's try to match the macros!
		// first, create a regex vector for the macros themselves
		vector< pair<string, ugp3::constraints::Macro*> > macrosRegex; 
		for(unsigned int m = 0; m < subsection.getMacroCount(); m++)
		{
			ugp3::constraints::Macro* currentMacro = subsection.getMacro( subsection.getMacro(m).getId() );
			string currentMacroRegex = currentMacro->getExpression().getRegex();
			
			macrosRegex.push_back( make_pair( currentMacroRegex, currentMacro) );
		}
		
		// sort the macros by length
		sort( macrosRegex.begin(), macrosRegex.end(), sortMacros );
		
		// LET THE MATCHING BEGIN! starting from the shortest macro, we try to match everything we can
		// eventually overwriting previous matches (TODO find better solutions)
		for(unsigned int m = 0; m < macrosRegex.size(); m++)
		{
			for(unsigned int l = macrosBeginning; l <= macrosEnd; l++)
			{
				unsigned int upperLimit = l;
				unsigned int lowerLimit = macrosEnd;
				if( RegexMatch::incrementalRollbackMatch( macrosRegex[m].first, macrosRegex[m].second->getPath(), textToAssimilate, upperLimit, lowerLimit, RegexMatch::topDown ) )
				{
					LOG_DEBUG 	<< "Found instance of macro \"" << macrosRegex[m].second->getId() 
							<< "\", from line #" << upperLimit << " to line #" << lowerLimit 
							<< ends;
				}
			}
		} 
		
	}

    }

    // and now, for the main act: BUILD AN INDIVIDUAL using the matched parts!
    LOG_DEBUG << "Now building an individual..." << ends;
    LOG_DEBUG << "Creating CGraphContainer..." << ends;

    // this is debugging, and should probably be removed
    unsigned int numberOfLinesMatched = 0;
    for(unsigned int i = 0; i < textToAssimilate.size(); i++)
	if( textToAssimilate[i].matched == true ) 
		numberOfLinesMatched++;
	else
		LOG_DEBUG << "Line #" << i << " is unmatched:\"" << textToAssimilate[i].text << "\"" << ends;
    LOG_DEBUG << "Lines matched: " << numberOfLinesMatched << " / " << textToAssimilate.size() << ends;
    
    unique_ptr<CGraphContainer> graphContainer( new CGraphContainer() );
    graphContainer->setConstrain( constraints ); 
    
    // now, the bad part is, we have to immediately create the global prologue and epilogue, as
    // there is no way to access them if they are not initialized to a value; so, existing nodes without tags!
    unique_ptr<CNode> nodeGlobalPrologue( new CNode( *graphContainer ) );
    unique_ptr<CNode> nodeGlobalEpilogue( new CNode( *graphContainer ) );
    nodeGlobalPrologue->setConstrain( constraints.getPrologue() );
    nodeGlobalEpilogue->setConstrain( constraints.getEpilogue() );
    graphContainer->setPrologue( nodeGlobalPrologue );
    graphContainer->setEpilogue( nodeGlobalEpilogue );

    // some pointers that need to be defined here
    //CNode* ssPrologueNode;
    //CNode* ssEpilogueNode;
    unique_ptr<CNode> ssPrologueNode;
    unique_ptr<CNode> ssEpilogueNode;

    // iterate over the text lines, checking for macros; global prologue and epilogue are compulsory, 
    // so they should be added even if not found

    // here are some references to the "current" stuff in this iteration
    string currentSection = ""; // I am forced to use a macro, because all methods return const
    string currentSubSection = "";
    
    CGraph* currentGraph = nullptr;
    CSubGraph* currentSubGraph = nullptr;

    for(int l = 0; l < textToAssimilate.size(); l++)
    {
	// first, read and parse the path
	if( textToAssimilate[l].matched == true )
	{
		// split the path over '//'
		vector<string> tokens;
		tokenize( textToAssimilate[l].macro, tokens, "/" );
		
		LOG_DEBUG << "Line #" << l << " is matched!" << ends;
		for(unsigned int t = 0; t < tokens.size(); t++)
			LOG_DEBUG << "Token: \"" << tokens[t] << "\"" << ends;
		
		// tokens[0] is the name of the constraints;
		// tokens[1] is the section (and it should always be there, except for global prologue and epilogue
		// tokens[2] is the subsection (that is not there for section prologue and epilogue)
		// tokens[3] is the macro itself
		if( tokens.size() == 2 )
		{
			// global prologue or global epilogue
			// not much to do here, since the node has already been created just tag it with the parameters 
			// TODO
		}
		else 
		{
			// section prologue or section epilogue
			// if the section is different from the current one, create a new section;
			const ugp3::constraints::Section* currentSectionRef;

			if( currentSection != tokens[1] )
			{
				currentSection = tokens[1];
				LOG_DEBUG << "New section detected: \"" << currentSection << "\"" << ends;
				
				// find the reference to the section, by id; if it is not found, it's a huge problem!
				currentSectionRef = constraints.getSection( currentSection );
				if( currentSectionRef == nullptr )
				{
					LOG_ERROR 	<< "Section \"" << currentSection 
							<< "\" not found during reconstruction of an individual. Aborting..." 
							<< ends;
					return nullptr;
				}
				
				// so, if the current graph is not empty, push it back inside the graph container
				if( currentGraph != nullptr )
				{
					unique_ptr<CGraph> cgPtr( currentGraph );
					graphContainer->addCGraph( cgPtr  );
				}

				// re-initialize the graph pointer
				currentGraph = new CGraph( *graphContainer );
				currentGraph->setConstrain( *currentSectionRef );
				if( currentGraph->getConstrain() == nullptr ) LOG_DEBUG << "What is happening here?!?!?" << ends;

				// TODO also, double-check that the previous section already contains prologue and epilogue
				// if not, add them; and do the same for the current subsection

				// TODO is it really ok to build the graph here?
				
			}
			else
			{
				// get a reference to the current section
				currentSectionRef = constraints.getSection( currentSection );
			}
			
			if( tokens.size() == 3 )
			{
				// now, this macro is either a section prologue or an epilogue
				if( tokens[2] == currentSectionRef->getPrologue().getId() )
				{
					LOG_DEBUG << "Section prologue found!" << ends;
					// TODO this is basically a cut/paste from the macro below, maybe
					// 	it could be appropriate to create a function...
					ugp3::constraints::Prologue& macroRef = currentSectionRef->getPrologue();
					
					LOG_DEBUG << "Creating node..." << ends;
					//unique_ptr<CNode> node( new CNode( *currentGraph ) );
					CNode* sPrologueNode = new CNode(*currentGraph);
					sPrologueNode->setConstrain( macroRef );
					
					LOG_DEBUG << "Setting node " << sPrologueNode->getId() << " parameter values..." << ends;
					for(	unsigned int p = 0; 
						p < macroRef.getParameterCount() && p < textToAssimilate[l].macroParameters.size(); 
						p++)
					{
						ugp3::constraints::Parameter& parameter = macroRef.getParameter(p);
						sPrologueNode->addTag( parameter.getName(), textToAssimilate[l].macroParameters[p] ); 
					}
					
					// FIXME missing the management of labels...
					currentGraph->setPrologue( sPrologueNode );
					//node.release();
				}

				if( tokens[2] == currentSectionRef->getEpilogue().getId() )
				{
					LOG_DEBUG << "Section epilogue found!" << ends;
					// TODO this is basically a cut/paste from the macro below, maybe
					// 	it could be appropriate to create a function...
					ugp3::constraints::Epilogue& macroRef = currentSectionRef->getEpilogue();
					
					LOG_DEBUG << "Creating node..." << ends;
					//unique_ptr<CNode> node( new CNode( *currentGraph ) );
					CNode* sEpilogueNode = new CNode( *currentGraph );
					sEpilogueNode->setConstrain( macroRef );
					
					LOG_DEBUG << "Setting node " << sEpilogueNode->getId() << " parameter values..." << ends;
					for(	unsigned int p = 0; 
						p < macroRef.getParameterCount() && p < textToAssimilate[l].macroParameters.size(); 
						p++)
					{
						ugp3::constraints::Parameter& parameter = macroRef.getParameter(p);
						sEpilogueNode->addTag( parameter.getName(), textToAssimilate[l].macroParameters[p] ); 
					}

					// FIXME missing the management of labels...
					currentGraph->setEpilogue( sEpilogueNode );
					//node.release();
				}
			}
			else if( tokens.size() == 4 )
			{
				// TODO this check is WRONG, because there might be several instances of the same subsection...
				// regular macro, or subsection prologue/epilogue
				// check if the subsection is the same; if not, create a new subsection inside the currents section
				const ugp3::constraints::SubSection* currentSubSectionRef;
				if( tokens[2] != currentSubSection )
				{
					currentSubSection = tokens[2];
					LOG_DEBUG << "New subsection detected: \"" << currentSubSection << "\"" << ends;
					
					// if the current subgraph is not empty, push it back inside the current graph
					// before that, add the current ssEpilogueNode...if there is one
					if( currentSubGraph != nullptr )
					{
						LOG_DEBUG << "Pushing back current SubGraph..." << ends;
						
						/*
						currentSubGraph->addNode( *ssEpilogueNode );
						currentSubGraph->getSlice().append( *ssEpilogueNode );
						currentSubGraph->setEpilogue( currentSubGraph->getNode( ssEpilogueNode->getId() ) );
						*/
						// TODO as the setEpilogue() function releases the ssEpilogueNode unique_ptr,
						// I should probably keep this in mind when using ssEpilogueNode again; it
						// might make everything crash!
						currentSubGraph->setEpilogue( ssEpilogueNode );
						currentGraph->attachSubGraph( *currentSubGraph );
					}

					// reset the subgraph pointer
					currentSubSectionRef = currentSectionRef->getSubSection( currentSubSection );
					currentSubGraph = new CSubGraph( *graphContainer->getCGraph(currentSection) );
					currentSubGraph->setConstrain( *currentSubSectionRef );
					
					// also, create the "untagged" nodes for prologue and epilogue
					LOG_DEBUG << "Creating subsection prologue and epilogue..." << ends;
					ssPrologueNode = unique_ptr<CNode>( new CNode( *currentSubGraph ) );
					ssEpilogueNode = unique_ptr<CNode>( new CNode( *currentSubGraph ) );
					ssPrologueNode->setConstrain( currentSubSectionRef->getPrologue() );
					ssEpilogueNode->setConstrain( currentSubSectionRef->getEpilogue() );

					// FIXME this part should be changed; probably it can work with the
					//	 current CSubGraph::setPrologue() function...
					/*
					LOG_DEBUG << "Adding prologue node to the subgraph..." << ends;
					currentSubGraph->addNode( *ssPrologueNode );
					LOG_DEBUG << "Appending prologue node to the slice..." << ends;
					currentSubGraph->getSlice().append( *ssPrologueNode );
					LOG_DEBUG << "Setting prologue node reference as subgraph prologue..." << ends;
					currentSubGraph->setPrologue( currentSubGraph->getNode( ssPrologueNode->getId() ) );
					*/
					
					// ok, this is really interesting: the setPrologue function RELEASES the ssPrologueNode
					// unique_ptr, so when I try to access ssPrologueNode in Population::assimilate()
					// everything crashes, as the reference is no longer valid.
					currentSubGraph->setPrologue( ssPrologueNode );

					LOG_DEBUG 	<< "Created subsection prologue (attached) "
							 << currentSubGraph->getPrologue().getId()
							<< " and subsection epilogue (not attached yet) " 
							<< ssEpilogueNode->getId()
							<< ends; 
					
				}
				else
				{
					//  or just get a reference to the old subsection
					currentSubSectionRef = currentSectionRef->getSubSection( currentSubSection );
				}
				
				// now, check if we need to create an epilogue/prologue
				if( tokens[3] == currentSubSectionRef->getPrologue().getId() )
				{
					LOG_DEBUG << "Subsection prologue detected!" << ends;
					// TODO tag stuff
				}
				else if( tokens[3] == currentSubSectionRef->getEpilogue().getId() )
				{
					LOG_DEBUG << "Subsection epilogue detected!" << ends;
					// TODO tag stuff
				}
				else
				{
					LOG_DEBUG << "Regular macro \"" << tokens[3] << "\" detected!" << ends;

					// so, it must be a regular macro! get the constraints for that macro	
					ugp3::constraints::Macro* macroRef = currentSubSectionRef->getMacro( tokens[3] );
					
					if( macroRef == nullptr )
					{
						LOG_ERROR 	<< "Error: while assimilating an individual, macro \""
								<< tokens[3] << "\" was not found in SubSection \""
								<< currentSubSectionRef->getId() << "\". Aborting..."
								<< ends;

						return nullptr;
					}
					
					// now, create a node!
					CNode* node = new CNode( *currentSubGraph );
					node->setConstrain( *macroRef );
					LOG_DEBUG << "Created node " << *node << "..." << ends;
					
					// set parameters for the node, taken from macroParameters attribute
					// inside the textToAssimilate array; however, in order to add the values
					// it is compulsory to add tags, through the methods that the nodes inherit
					// from the Taggable parent class
					vector<ugp3::constraints::InnerLabelParameter*> labelParameters;
					LOG_DEBUG << "Setting node parameter values..." << ends;
					for(	unsigned int p = 0; 
						p < macroRef->getParameterCount() /* && p < textToAssimilate[l].macroParameters.size() */; 
						p++)
					{
						ugp3::constraints::Parameter& parameter = macroRef->getParameter(p);
						
						// if a parameter is a label parameter, keep it on the side and deal with it later
						ugp3::constraints::InnerLabelParameter* innerLabelParameter = dynamic_cast<ugp3::constraints::InnerLabelParameter*>( &parameter );
						if( innerLabelParameter == nullptr )
						{
							node->addTag( CNode::Escape + parameter.getName(), textToAssimilate[l].macroParameters[p] ); 
						}
						else
						{
							LOG_DEBUG << "Label parameter found!" << ends;
							labelParameters.push_back( innerLabelParameter );
						}
					}
					
					// FIXME this part takes for granted that there is ONLY ONE inner/outer label parameter per macro
					//	 which is of course totally false. referenceTo should be modified to a vector.
					//	 additionally, there might be negative offsets; right now, the "unitialized" value
					//	 for ::referenceTo is -1, and this is also wrong.

					// now, labels are not parameters: if there are any labels, we should try to attach them
					// so, first create an Edge
					if( textToAssimilate[l].referenceTo != -1 )
					{
						LOG_DEBUG << "Adding an edge for parameter " << labelParameters[0]->getName() << ends;

						tgraph::Edge* edge = new tgraph::Edge(*node);
						ostringstream offsetValue;
						offsetValue << (textToAssimilate[l].referenceTo - l);
						// two tags are added to an edge: the name of the parameter it refers to, and the destination;
						// the destination can be expressed in several ways, but here we are using an offset 
						edge->addTag( tgraph::Edge::parameterTagName, labelParameters[0]->getName() ); 
						edge->addTag( tgraph::Edge::offsetTagName, offsetValue.str() );
						node->addEdge( *edge );
						
						LOG_DEBUG << "Offset value is \"" << offsetValue.str() << "\"" << ends;
					}

					// finally, push the node inside the current subgraph
					currentSubGraph->addNode( *node );
					// it also has to be appended to the slice, an internal class of the SubGraph...
					currentSubGraph->getSlice().append( *node );
					
					LOG_DEBUG << "Node " << *node << " contains the following tags:" << ends;
					for(unsigned int t = 0; t < node->getTagCount(); t++)
					{
						LOG_DEBUG << *node->getTag(t) << ends; 
					}
					// actually, this is the function used by OperatorToolbox...
					//currentSubGraph->getSlice().spliceSlice(unique_ptr<Slice>(new Slice(std::move(newNode))), insertPoint);
				}
			} // end if tokens.size()
		} // end if tokens.size() == 2
		
		// before iterating to the next line, skip all lines that are an instance of the same macro
		while( 	l+1 < textToAssimilate.size() && // this line is first, so it doesn't crash on the following checks
			textToAssimilate[l+1].macro.compare( textToAssimilate[l].macro ) == 0 &&
			textToAssimilate[l+1].occurrence == textToAssimilate[l].occurrence)
		{
			l++;
		}
	}

     } // end for
    	
     // TODO push back stuff that is still not pushed back (last subsection and last section)
     LOG_DEBUG << "Adding last elements to the graph container..." << ends;
     // add the current ssEpilogueNode to the current SubGraph
     /*
     currentSubGraph->addNode( *ssEpilogueNode );
     currentSubGraph->getSlice().append( *ssEpilogueNode );
     currentSubGraph->setEpilogue( currentSubGraph->getNode( ssEpilogueNode->getId() ) );
     */
     currentSubGraph->setEpilogue( ssEpilogueNode );

     currentGraph->attachSubGraph( *currentSubGraph );
     unique_ptr<CGraph> cgPtr ( currentGraph );
     graphContainer->addCGraph( cgPtr );

     // it's no longer the unique_ptr responsability to erase this pointers
     // that, by the way, should have already been copied inside the graphContainer
	
     // check if there are still unmatched parts
     vector<unsigned int> unmatchedLines;
     for(unsigned int i = 0; i < textToAssimilate.size(); i++)
     {
	if( textToAssimilate[i].matched == false ) unmatchedLines.push_back( i );
     }
     LOG_DEBUG << "There are #" << unmatchedLines.size() << " unmatched lines that will be added to a modified constraints file." << ends;
    
     if( unmatchedLines.size() > 0 )
     {
	for(unsigned int i = 0; i < unmatchedLines.size(); i++)
		LOG_DEBUG << "Unmatched line #" << i << ": \"" << textToAssimilate[ unmatchedLines[i] ].text << "\"" << endl;
     }
    
     // TODO add unmatched parts to the constraints
     
     // create and return an individual with the graphContainer
     // TODO 	with the new classes organization, the "superclass" for all individuals and groups is in fact "CandidateSolution";
     //		so, it would make sense that the assimilate() method for Population would return "CandidateSolution*".
     //		As far as I can tell, there are several possible solutions:
     //		- re-implement the method in each class that inherits from Population, and return the appropriate type;
     //		- use a switch with the type of population, to return the appropriate type of Individual;
     //		- re-implement this at the level of IndividualPopulation;
     EnhancedIndividual* individual = new EnhancedIndividual( *this ); 
     individual->setCGraphContainer( graphContainer );
	
     // this is the point where everything explodes!
     LOG_DEBUG << "Attaching floating edges..." << ends;
     individual->getGraphContainer().attachFloatingEdges();
    
     return individual;
}


bool Population::checkMaximumFitnessReached()
{
    if (getBestCandidate() && getBestCandidate()->getRawFitness().getValues() >= getParameters().getMaximumFitness()) 
    {
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

