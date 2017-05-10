/***********************************************************************\
|                                                                       |
| IndividualPopulation.cc |
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
| $Revision: 653 $
| $Date: 2015-04-14 11:34:46 +0200 (Tue, 14 Apr 2015) $
\***********************************************************************/

/**
 * @file IndividualPopulation.cc
 *
 */

#include "IndividualPopulation.h"
#include "IndividualPopulationParameters.h"
#include "Individual.h"
#include "GEIndividual.h"
#include "MOIndividual.h"
#include "EnhancedIndividual.h"
#include "GeneticOperator.h"
#include "FitnessEvaluator.h"
#include "Statistics.h"
#include <File.h>

#include <unordered_set>

using namespace std;

namespace ugp3 {
namespace core {
    
template <class IndividualType>
SpecificIndividualPopulation<IndividualType>::~SpecificIndividualPopulation()
{
    for (auto ind: m_individuals) {
        delete ind;
    }
}


template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::addIndividual(unique_ptr< Individual > individual)
{
    _STACK;
    
    if (!individual) {
        throw ArgumentNullException("individual", LOCATION);
    }
    for (auto ind: m_individuals) {
        if (ind == individual.get()) {
            throw ArgumentException("The individual '" + individual->getId() + "' is already contained in the population.", LOCATION);
        } else if (ind->getId() == individual->getId()) {
            throw ArgumentException("Another individual with the id '" + individual->getId() + "' already exists in the population.", LOCATION);
        }
    }
    Assert(dynamic_cast<IndividualType*>(individual.get()));
    m_individuals.push_back(static_cast<IndividualType*>(individual.release()));
}

template <class IndividualType>
double SpecificIndividualPopulation<IndividualType>::computeAverageIndividualSize() const
{
    unsigned long int totNodes = 0;
    
    for (IndividualType* individual: m_individuals) {
        using namespace ugp3::ctgraph;
        
        const CGraphContainer& container = individual->getGraphContainer();
        totNodes += 2; // global prologue and epilogue
        
        for( unsigned int c = 0; c < container.getCGraphCount(); c++ )
        {
            const CGraph& graph = container.getCGraph(c);
            totNodes += 2; // graph prologue and epilogue
            
            for( unsigned int s = 0; s < graph.getSubGraphCount(); s++ )
            {
                const CSubGraph& subGraph = graph.getSubGraph(s);
                totNodes += subGraph.getSize();
            }
        }
    }
    
    double individualSize = totNodes / (double)getIndividualCount();
    
    return individualSize;
}


template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::setupPopulation()
{
    _STACK;
    
    LOG_VERBOSE << "Creating a population with " << getParameters().getNu()
    << " random individuals ..." << std::ends;
    
    // Continue generating new individuals until there are 'nu' individuals.
    unsigned int deadIndividuals = 0;
    while (m_individuals.size() < getParameters().getNu() )
    {
        LOG_INFO << "Creating " << getParameters().getNu() << " individuals" << Progress(m_individuals.size() / (double)getParameters().getNu()) << std::ends;
        
        // Create a new random individual.
        std::unique_ptr<IndividualType> individual = newRandomSpecificIndividual();
        
        // Check if the individual is valid.
        if (individual->validate()) 
	{
            // Save the new individual in the vector.
            addIndividual(std::move(individual));
        } 
	else 
	{
            // Discard the individual because it is not valid.
            deadIndividuals++;
        }
    }
    
    LOG_INFO << "Creating " << getParameters().getNu() << " individuals" << Progress::END << std::ends;
    
    // Print a warning message if at least one of the new individuals was not
    // valid.
    if( deadIndividuals > 0 )
    {
        LOG_WARNING << "Some individuals (" << deadIndividuals
        << ") were not correct." << std::ends;
    }
}


template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::evaluateAndHandleClones()
{
    _STACK;
    
    Assert(checkHashGenoPhenoEqualitiesAndHashQuality(m_individuals.begin(), m_individuals.end()));

    LOG_VERBOSE << "About to evaluate the whole individual " << TypeName<IndividualType>::name << " population..." << ends;

    size_t evaluated = runEvaluator(m_individuals.begin(), m_individuals.end());
    
    // Because just after a recovery, we might not need to compute anything if the recovery file had all the information
    if (evaluated > 0) 
    {
        // Compute scaled fitness and/or kill the clones
        detectAndHandleClones(m_individuals.begin(), m_individuals.end());
        
        m_entropy = updateDeltaEntropy(m_individuals.begin(), m_individuals.end());
        
        if (getParameters().getFitnessSharingEnabled()) 
	{
            shareFitness(m_individuals.begin(), m_individuals.end());
        }
    }
}    

template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::checkFitnessValidity()
{
    // Check that all fitnesses of not-dead invidividuals are valid.
    for (auto ind: m_individuals) 
    {
        if (!ind->isDead()) 
	{
            Assert(ind->getRawFitness().getIsValid());
            Assert(ind->getFitness().getIsValid());
        }
    }
}

template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::updateOperatorStatistics(const std::vector< CandidateSolution* >& newGeneration)
{
    std::vector<Individual*> newGenerationInds;
    for (auto candidate: newGeneration) 
    {
        Assert(candidate);
        if (IndividualType* ind = dynamic_cast<IndividualType*>(candidate)) 
	{
            newGenerationInds.push_back(ind);
        }
    }

    std::vector<Individual*> everyone;
    for (auto individual: m_individuals) 
    {
        everyone.push_back(individual);
    }

    getParameters().getActivations().template updateOperatorsStatistics<Individual>(
        std::move(everyone), newGenerationInds, getBestScaledIndividual(), getWorstScaledIndividual());
}

template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::selectNewZombifiableCandidates()
{
    auto liveBegin = regroupAndSkipDeadCandidates(m_individuals.begin(), m_individuals.end());
    
    m_bloodMagicWaitingList.clear();
    m_bloodMagicWaitingList.push_back(*std::max_element(
        liveBegin, m_individuals.end(),
        [this] (CandidateSolution* a, CandidateSolution* b) {
            return !compareRawBestWorst(a, b);
        }));
}


template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::slaughtering()
{
    _STACK;
        
    describePopulation("Before slaughtering", m_individuals.begin(), m_individuals.end());
    
    simpleSelection(getParameters().getMu(), m_individuals.begin(), m_individuals.end());
    
    describePopulation("After slaughtering", m_individuals.begin(), m_individuals.end());
}

template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::mergeNewGeneration(const std::vector< CandidateSolution* >& newGeneration)
{
    for (auto candidate: newGeneration) 
    {
        if (IndividualType* ind = dynamic_cast<IndividualType*>(candidate)) 
	{
            addIndividual(unique_ptr<Individual>(ind));
        }
    }
}

template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::prepareForCommit()
{
    m_entropy = updateDeltaEntropy(m_individuals.begin(), m_individuals.end());
    
    if (getParameters().getFitnessSharingEnabled()) 
    {
        shareFitness(m_individuals.begin(), m_individuals.end());
    }
}

template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::commit()
{
    _STACK;
    
    // save the best and the worst individual
    auto minmax = std::minmax_element(
        m_individuals.begin(), m_individuals.end(), [this] (IndividualType* a, IndividualType* b) {
            return !compareRawBestWorst(a, b);
        });

    m_worstRawIndividual = *minmax.first;
    m_bestRawIndividual = *minmax.second;

    minmax = std::minmax_element(
        m_individuals.begin(), m_individuals.end(), [this] (IndividualType* a, IndividualType* b) {
            return !compareScaledBestWorst(a, b);
        });

    m_worstScaledIndividual = *minmax.first;
    m_bestScaledIndividual = *minmax.second;
    
    LOG_VERBOSE << "The best raw individual is " << this->m_bestRawIndividual << " " << m_bestRawIndividual->getRawFitness() << ends;
    LOG_VERBOSE << "The worst raw individual is " << this->m_worstRawIndividual << " " << m_worstRawIndividual->getRawFitness() << ends;
    LOG_VERBOSE << "The best scaled individual is " << this->m_bestScaledIndividual << " " << m_bestScaledIndividual->getFitness() << ends;
    LOG_VERBOSE << "The worst scaled individual is " << this->m_worstScaledIndividual << " " << m_worstScaledIndividual->getFitness() << ends;
}

template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::dumpStatisticsHeader(ostream& output) const
{
    IndividualPopulation::dumpStatisticsHeader(output);
    
    output << "," << getName() << "_AvgAge";
    output << "," << getName() << "_AvgIndSize";
    // FITNESS: AVERAGE
    for(unsigned int i = 0; i < getParameters().getFitnessParametersCount(); i++)
    {
        output << "," << getName() << "_Avg_f" << i;
    }
    
    // FITNESS: BEST INDIVIDUAL (IF DEFINED)
    for(unsigned int i = 0; i < getParameters().getFitnessParametersCount(); i++)
    {
        output << "," << getName() << "_Best_f" << i;
    }
    
    // FITNESS: WORST INDIVIDUAL (IF DEFINED)
    for(unsigned int i = 0; i < getParameters().getFitnessParametersCount(); i++)
    {
        output << "," << getName() << "_Worst_f" << i;
    }
    
}

template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::dumpStatistics(ostream& output) const
{
    IndividualPopulation::dumpStatistics(output);
    
    output << "," << computeAverageAge(m_individuals.begin(), m_individuals.end());
    output << "," << computeAverageIndividualSize();
    
    // TODO: Also insert scaled fitness inside the statistics?
    vector<double> averageFitness = computeAverageRawFitness(m_individuals.begin(), m_individuals.end());
    
    // FITNESS: AVERAGE
    for(unsigned int i = 0; i < averageFitness.size(); i++)
    {
        output << "," << averageFitness[i];
    }
    
    // FITNESS: BEST INDIVIDUAL (IF DEFINED)
    if (this->getBestRawIndividual() != nullptr)
    {
        this->getBestRawIndividual()->getRawFitness().writeCSV(output);
    }
    else 
    {
        for(unsigned int i = 0; i < averageFitness.size(); i++)
        {
            output << ",na";
        }
    }
    
    // FITNESS: WORST INDIVIDUAL (IF DEFINED)
    if (this->getWorstRawIndividual() != nullptr)
    {
        this->getWorstRawIndividual()->getRawFitness().writeCSV(output);
    }
    else 
    {
        for(unsigned int i = 0; i < averageFitness.size(); i++)
        {
            output << ",na";
        }
    }
}


template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::showStatistics(void) const
{
    vector<double> averageFitness = computeAverageRawFitness(m_individuals.begin(), m_individuals.end());
    double averageAge = computeAverageAge(m_individuals.begin(), m_individuals.end());
    double averageDeltaEntropy = computeAverageDeltaEntropy(m_individuals.begin(), m_individuals.end());
    
    LOG_INFO << "[Individuals] Count: " << m_individuals.size()
    << "; average age: " << averageAge
    << "; average size: " << computeAverageIndividualSize()
    << "; average delta entropy: " << averageDeltaEntropy
    << std::ends;

    LOG_INFO << "[Individuals] Average fitness:";
    for (unsigned int i = 0; i <  averageFitness.size(); i++)
    {
        LOG_INFO << " " << averageFitness[i];
    }
    LOG_INFO << ends;

    if (getBestRawIndividual()) 
    {
        LOG_INFO << "[Individuals] Best fitness: " << this->getBestRawIndividual() << " " << this->getBestRawIndividual()->getRawFitness() << ends;
    }
    if (getWorstRawIndividual()) 
    {
        LOG_INFO << "[Individuals] Worst fitness: " << this->getWorstRawIndividual() << " " << this->getWorstRawIndividual()->getRawFitness() << ends;
    }

    IndividualPopulation::showStatistics();
}

template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::dumpAllCandidates()
{
    // save the list of individuals of the population in a file
    ofstream individualList("individualsInPopulation.txt");
    if (!individualList.is_open()) {
        throw Exception("Cannot open the file 'individualsInPopulation.txt' for writing.", LOCATION);
    }

    for (auto ind: m_individuals) {
        Assert(ind);

        // create the name of the file
        const string& fileName = File::formatToName(
            this->getParameters().getEvaluator().getInputFile(), ind->getId());

        ind->toCode(fileName);
        individualList << fileName << endl;
    }
    individualList.close();
}

template <class IndividualType>
const string SpecificIndividualPopulation<IndividualType>::XML_CHILD_ELEMENT_INDIVIDUALS = "individuals";

template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::readXml(const xml::Element& element)
{
    _STACK;

    IndividualPopulation::readXml(element);

    // get the inner elements
    bool individualsFound = false;
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        const string& elementName = childElement->ValueStr();
        if(elementName == XML_CHILD_ELEMENT_INDIVIDUALS)
        {
            if(individualsFound == true)
            {
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/individuals", LOCATION);
            }

            individualsFound = true;

            this->parseIndividuals(*childElement);
        }

        childElement = childElement->NextSiblingElement();
    }

    if(individualsFound == false)
    {
        throw xml::MissingElementSchemaException("/evolutionaryAlgorithm/populations/population/individuals", LOCATION);
    }

    if(m_individuals.empty())
    {
        throw Exception("The population xml file does not contain any individual.", LOCATION);
    }
}

template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::parseIndividuals(const xml::Element& element)
{
    _STACK;

    // get the name of the xml element
    if(element.ValueStr() != XML_CHILD_ELEMENT_INDIVIDUALS)
    {
        throw xml::SchemaException("expected element '/evolutionaryAlgorithm/populations/population/individuals'", LOCATION);
    }

    LOG_DEBUG << "Parsing /evolutionaryAlgorithm/populations/population/individuals ..." << ends;

    // loop on child elements
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        const string& elementName = childElement->ValueStr();
        if(elementName == Individual::XML_NAME)
        {
            unique_ptr<Individual> individual = Individual::instantiate(*childElement, *this);

            addIndividual(std::move(individual));
        }
        else throw xml::SchemaException("unexpected element \"" + elementName + "\"", LOCATION);

        childElement = childElement->NextSiblingElement();
    }

    if(m_individuals.size() == 0)
    {
        LOG_WARNING << "The population xml does not contain any individual" << ends;
    }
    
    string bestRawIndividualId = "";
    string worstRawIndividualId = "";
    string bestScaledIndividualId = "";
    string worstScaledIndividualId = "";
    if(element.Attribute(XML_ATTRIBUTE_RAW_BEST) != nullptr)
    {
        bestRawIndividualId = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_RAW_BEST);
    }
    if(element.Attribute(XML_ATTRIBUTE_RAW_WORST) != nullptr)
    {
        worstRawIndividualId = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_RAW_WORST);
    }
    if(element.Attribute(XML_ATTRIBUTE_SCALED_BEST) != nullptr)
    {
        bestScaledIndividualId = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_SCALED_BEST);
    }
    if(element.Attribute(XML_ATTRIBUTE_SCALED_WORST) != nullptr)
    {
        worstScaledIndividualId = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_SCALED_WORST);
    }
    // assign best and worst
    for (auto* individual: m_individuals)
    {
        if (bestRawIndividualId == individual->getId()) {
            m_bestRawIndividual = individual;
        }
        if (worstRawIndividualId == individual->getId()) {
            m_worstRawIndividual = individual;
        }
        if (bestScaledIndividualId == individual->getId()) {
            m_bestScaledIndividual = individual;
        }
        if (worstScaledIndividualId == individual->getId()) {
            m_worstScaledIndividual = individual;
        }
    }
}

template <class IndividualType>
void SpecificIndividualPopulation<IndividualType>::writeInnerXml(ostream& output) const
{
    IndividualPopulation::writeInnerXml(output);
    
    LOG_DEBUG << "Serializing object ugp3::core::SpecificIndividualPopulation" << ends;

    output << "<" << XML_CHILD_ELEMENT_INDIVIDUALS;
    if (m_bestRawIndividual) {
        output << ' ' << XML_ATTRIBUTE_RAW_BEST << "='" << m_bestRawIndividual->getId() << "'";
    }
    if(m_worstRawIndividual) {
        output << ' ' << XML_ATTRIBUTE_RAW_WORST << "='" << m_worstRawIndividual->getId() << "'";
    }
    if (m_bestScaledIndividual) {
        output << ' ' << XML_ATTRIBUTE_SCALED_BEST << "='" << m_bestScaledIndividual->getId() << "'";
    }
    if (m_worstScaledIndividual) {
        output << ' ' << XML_ATTRIBUTE_SCALED_WORST << "='" << m_worstScaledIndividual->getId() << "'";
    }
    output << ">" << endl;

    output << "<!-- total " << m_individuals.size() << " individuals -->" << endl;

    for(unsigned int i = 0; i < m_individuals.size(); i++)
    {
        m_individuals[i]->writeXml(output);
    }

    output << "</" << XML_CHILD_ELEMENT_INDIVIDUALS << ">" << endl;
}

template <class IndividualType> 
void SpecificIndividualPopulation<IndividualType>::discardFitnessValues() 
{
	LOG_DEBUG << "Invalidating the fitness value of individuals inside IndividualPopulation..." << ends;
	for (auto ind: this->m_individuals) 
	{
	    ind->getRawFitness().invalidate();
	    ind->getFitness().invalidate();
	}
}

template <class IndividualType> 
void SpecificIndividualPopulation<IndividualType>::seeding(string fileName)
{
	if( ! File::exists(fileName) )
	{
		LOG_WARNING << "Error: could not read file \"" << fileName << "\". Skipping population seeding..." << ends;
		return;
	}

	// open file, there should be a file name on every line	
	vector<string> listOfFiles;
	ifstream inputFile(fileName);
	if( inputFile.is_open() )
	{
		string buffer;
		while( getline(inputFile, buffer) )
		{
			listOfFiles.push_back(buffer);
		}
	}
	
	// and now, assimilate ALL the files!
	ugp3::constraints::Constraints* modifiedConstraints = nullptr; //FIXME this pointer should be changed, or the function should be overloaded
	for(unsigned int i = 0; i < listOfFiles.size(); i++)
	{
		LOG_INFO << "Reading file \"" << listOfFiles[i] << "\"..." << ends;
		CandidateSolution* candidateSolution = Population::assimilate(listOfFiles[i], modifiedConstraints);
		
		if( dynamic_cast<IndividualType*>(candidateSolution) != nullptr)
		{
			unique_ptr<IndividualType> individual( (IndividualType*)candidateSolution );
			addIndividual( std::move(individual) ); 
			LOG_INFO << "Individual successfully added to the population!" << ends;
		}
		else
		{
			LOG_WARNING 	<< "Could not add the individual from file \"" 
					<< listOfFiles[i] + "\" to the population. Skipping to the next file..." 
					<< ends;
		}
	}

	return;
}

template class SpecificIndividualPopulation<MOIndividual>;
template class SpecificIndividualPopulation<GEIndividual>;
template class SpecificIndividualPopulation<EnhancedIndividual>;

}
}
