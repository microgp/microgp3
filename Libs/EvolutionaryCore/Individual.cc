/*********************************************************************** \
|                                                                       |
| Individual.cc                                                         |
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
| $Revision: 647 $
| $Date: 2015-02-24 22:32:04 +0100 (Tue, 24 Feb 2015) $
\***********************************************************************/

/**
 * @file Individual.cc
 * Implementation of the Individual class.
 * @see Individual.h
 * @see Individual.xml.cc
 * @see Individual.instantiate.cc
 */

#include "Individual.h"
#include "IndividualPopulation.h"

using namespace std;
using namespace ugp3::core;

InfinityString Individual::idCounter;

string Individual::getIndividualCounter()
{
    return Individual::idCounter.toString();
}

void Individual::setIndividualCounter(const string& value)
{
    _STACK;

    InfinityString newVal(value);
    Assert(Individual::idCounter <= newVal);
    Individual::idCounter = newVal;
}

Individual::Individual(long unsigned int birth, const string& geneticOperator, const vector< string >& parents, const Population& population)
: CandidateSolution(birth, population, idCounter.toString()),
m_lineage(geneticOperator, parents)
{
    _STACK;
    
    idCounter++;

    LOG_DEBUG << "Creating new individual " << this << " ..." << ends;
    this->m_graphContainer = unique_ptr<ctgraph::CGraphContainer>(new ctgraph::CGraphContainer());

    this->m_graphContainer->setConstrain(population.getParameters().getConstraints());
    this->m_graphContainer->buildRandom();

    LOG_DEBUG << "Individual " << this << " built" << ends;
}

Individual::Individual(const Individual& individual)
: CandidateSolution(individual, idCounter.toString())
{
    _STACK;
    
    idCounter++;

    m_graphContainer = individual.m_graphContainer->clone();

    LOG_DEBUG 
    << "Cloned individual " << this 
    << " from parent " << individual << ends;
}

Individual::Individual(const Population& population)
: CandidateSolution(population, idCounter.toString())
{     
    idCounter++;
}

void Individual::step(bool age)
{
    CandidateSolution::step(age);
    m_lineage.step();
}


bool Individual::validate() const
{
    _STACK;

    if (this->m_graphContainer.get() == nullptr)
    {
        return false;
    }

    return this->m_graphContainer->validate();
}

void Individual::toCode(const string& fileName, vector< string >* outfiles) const
{
    _STACK;
    
    ugp3::core::CandidateSolution::toCode(fileName, outfiles);

    ofstream file;
    file.open(fileName.c_str());

    if (file.is_open() == false)
    {
        throw Exception("Could not open file \"" + fileName + "\".", LOCATION);
    }
    
    if (outfiles)
        outfiles->push_back(fileName);

    LOG_DEBUG 
    << "Saving individual " << this
    << " to file \"" << fileName << "\" ..." << ends;
    file << getExternalRepresentation();
    file.close();
    
}

void Individual::computeNormalizedPhenotype(string& code) const
{
    ostringstream stream;
    ugp3::ctgraph::NormalizingRelabeller relabeller;
    this->m_graphContainer->writeExternalRepresentation(stream, relabeller);
    code = stream.str();
}

const string& Individual::getExternalRepresentation() const
{
    if (m_externalRepresentation.empty()) {
        ostringstream stream;
        ugp3::ctgraph::IdentityRelabeller relabeller;
        this->m_graphContainer->writeExternalRepresentation(stream, relabeller);
        m_externalRepresentation = stream.str();
    }
    return m_externalRepresentation;
}


Individual::~Individual()
{
    _STACK;

    LOG_DEBUG << "Destructor: ugp3::core::Individual " << this << ends;
}

const IndividualPopulation& Individual::getPopulation() const
{
    return static_cast<const IndividualPopulation&>(CandidateSolution::getPopulation());
}

bool Individual::isGenotypeEqual(const CandidateSolution& candidateSolution) const
{
    _STACK;
    
    try {
        const Individual& individual = dynamic_cast<const Individual&>(candidateSolution);
        if (this->getGraphContainer() == individual.getGraphContainer())
        {
            LOG_DEBUG 
            << "The individuals " << this << " and " << individual 
            << " have the same genotype" << ends;
            
            return true;
        }
        
        LOG_DEBUG 
        << "The individuals " << this << " and " << individual 
        << " have a different genotype" << ends;
        return false;
        
    } catch (std::bad_cast& e) {
        Assert(false);
        LOG_ERROR << "Tried to compare individual genotype with candidate solution of incompatible type." << ends;
        // TODO maybe throw something?
    }
    return false;
}

/** removed! (!)20111120
 
void Individual::setAllopatricTagCounter(const string& value)
{
    InfinityString newVal(value);
    Assert(Individual::allopatricTagCounter <= newVal);
    Individual::allopatricTagCounter = newVal;
}
 
string Individual::getAllopatricTagCounter()
{
    return Individual::allopatricTagCounter.toString();
}

**/

#if 0
// Added by Alberto Tonda - 2008/04/01
std::vector<hash_t> Individual::getMessageInformation()
{
    // Debug
    LOG_DEBUG << "The message of individual " << this->getId() << " is ";
    
    for(unsigned int i = 0; i < getGraphContainer().getMessageInformation().size(); i++)
    {
        LOG_DEBUG << getGraphContainer().getMessageInformation().at(i)
            << " ";
    }
    
    LOG_DEBUG << " !!!! " << ends;
        
    // this line returns all the information gathered in
    // std::vector<hash_t> CGRaphContainer::message
    return getGraphContainer().getMessageInformation();
}
#endif

const string Individual::getDisplayTypeName() const
{
    return TypeName< Individual >::name;
}

void Individual::outputLineage(ostream& lineageFile, unsigned int recursion, const string& indent) const
{
    if (recursion) 
    {
        lineageFile <<
        indent << "parents:\n";
        for (auto parent: getLineage().getParents()) 
	{
            lineageFile <<
            indent << "  -\n";
            parent->outputLineage(lineageFile, recursion - 1, indent + "    ");
        }
    }
}

void Individual::setCGraphContainer( unique_ptr<ugp3::ctgraph::CGraphContainer>& graphContainer)
{
	if(this->m_graphContainer.get() == nullptr )
	{
		LOG_DEBUG << "Setting graph container of individual " << *this << " to a new value..." << ends;
		this->m_graphContainer.reset( graphContainer.release() );
	}
	else
	{
		LOG_ERROR << "Graph container of individual " << *this << " is already set. Cannot set it to another value." << ends;
	}

	return;
}
