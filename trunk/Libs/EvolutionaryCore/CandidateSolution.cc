/***********************************************************************\
|                                                                       |
| CandidateSolution.cc |
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
| $Revision: 644 $
| $Date: 2015-02-23 14:50:30 +0100 (Mon, 23 Feb 2015) $
\***********************************************************************/

/**
 * @file CandidateSolution.cc
 *
 */

#include "CandidateSolution.h"
#include "Population.h"

using namespace std;

namespace ugp3 {
namespace core {

InfinityString CandidateSolution::allopatricTagCounter;

void CandidateSolution::setAllopatricTagCounter(const string& value)
{
    _STACK;

    InfinityString newVal(value);
    Assert(allopatricTagCounter <= newVal);
    allopatricTagCounter = newVal;
}

CandidateSolution::CandidateSolution(const Population& population, const string& id)
: m_id(id),
m_allopatricTag(allopatricTagCounter.toString()),
m_state(ALIVE),
m_birthGen(0),
m_deathGen(std::numeric_limits<unsigned int>::max()),
m_age(0),
m_population(&population),
m_rawFitness(population.getParameters().getFitnessParametersCount()),
m_deltaEntropy(0)
{
    allopatricTagCounter++;
}

CandidateSolution::CandidateSolution(const ugp3::core::CandidateSolution& other, const string& id)
: CandidateSolution(*other.m_population, id)
{
    m_birthGen = other.m_population->getGeneration();
}

CandidateSolution::CandidateSolution(unsigned int birth, const Population& population, const string& id)
: CandidateSolution(population, id)
{
    m_birthGen = birth;
}

CandidateSolution::~CandidateSolution()
{
}

void CandidateSolution::setDeath(unsigned int deathDate)
{
    /* FIXME this comment is not completely true anymore. TODO clarify this situation
     * + 1 because newborns have age 1. At generation 0 all new candidates
     * produced by setupPopulation() should have age 1. During successive
     * generations, operators produce candidates of age 0, which are brought
     * to age 1 during the generation. If such a candidate was to be killed
     * before the end of the generation in which it is born, it would have
     * m_birthGen == m_deathGen and age = 0 or 1.
     */
    if (deathDate + 1 < m_birthGen + m_age)
    {
        LOG_DEBUG << "The " << getDisplayTypeName() << " " << this << " has deathDate=" << deathDate << ", birth=" << this->m_birthGen << ", age=" << this->m_age << "; maximum age permitted is " << this->m_population->getParameters().getMaximumAge() << ends;
        throw ArgumentException("The " + getDisplayTypeName() + " " + this->toString() + " death date is earlier than its birth + age - 1.", LOCATION);
    }
    
    if (m_population->isZombifiable(this)) {
        LOG_VERBOSE << "The " << getDisplayTypeName() << " " << this << " escaped death and became a zombie." << ends;
        setState(ZOMBIE);
    } else {
        LOG_DEBUG << "The " << getDisplayTypeName() << " " << this << " is now dead." << ends;
        setState(DEAD);
        
        if (isHero()) {
            /*
             * We signal hero deaths because they should happen very rarely
             * (when two heroes are part of the same allopatric group, or when
             * the population size goes under the elite size).
             */
            LOG_VERBOSE << "Being a hero did not save the " << getDisplayTypeName() << " " << this << " from death..." << ends;
        }
    }
    m_deathGen = deathDate;
}

void CandidateSolution::step(bool age)
{
    // do not age heroes nor zombies
    if (age && !isHero() && !isZombie()) {
        LOG_DEBUG << "Aging the " << getDisplayTypeName() << " " << getId() << ends;
        // the individual is alive and may age
        if (isAlive() == true) {
            // small change: from now on, maximumAge == 0 means that individuals will not die
            unsigned int maxAge = m_population->getParameters().getMaximumAge();
            if (m_age >= maxAge && maxAge > 0) {
                LOG_VERBOSE << "The " << getDisplayTypeName() << " " << *this
                << " died of old age " << this->m_age
                << " (maximum is " << maxAge << ")" << ends; 
                setDeath(m_population->getGeneration());
            }
            m_age++;
        }
        LOG_DEBUG << "The " << getDisplayTypeName() << " " << *this << " has age " << this->m_age << ends;
    }
}

const string CandidateSolution::toString() const
{
    // add an asterisk if it is a hero
    //return "\"" + this->getId() + (this->zombie? "+" : "" ) + (this->isHero? "E" : "" ) + "\"";
    return "\"" + this->getId() + (isHero() ? "*" : "" ) + (isZombie() ? "+" : "" ) + "\"";
}

const string& CandidateSolution::getNormalizedPhenotype() const
{
    if (m_normalizedPhenotype.empty()) {
        computeNormalizedPhenotype(m_normalizedPhenotype);
    }
    return m_normalizedPhenotype;
}

void CandidateSolution::setFitnessStructure(Fitness fitness)
{
    m_rawFitness = fitness;
}

void CandidateSolution::setState(CandidateSolution::State state)
{
#ifndef NDEBUG
    /*
     * Since the fitness of dead individuals could be either valid or invalid,
     * in debug mode we always invalidate it so that the parts of ugp that do
     * not check fitness validity will crash.
     * 
     * In release mode we should not invalidate it since it can be used to
     * compute operator performance.
     */
    if (m_state != DEAD && state == DEAD) {
        getRawFitness().invalidate();
        getFitness().invalidate();
    }
#endif

    m_state = state;
}

void CandidateSolution::toCode(const string& fileName, vector< string >* outfiles) const
{
#ifdef TEST_OPERATOR_SELECTION
    /*
     * We are testing operator selection schemes: we need to output the lineage of
     * this object.
     */
    if (outfiles) {
        const string& lineageFileName = fileName + ".lineage.yaml";
        outfiles->push_back(lineageFileName);
        ofstream lineageFile(lineageFileName);
        if (!lineageFile) {
            throw Exception("Could not open individual lineage file for writing", LOCATION);
        }
        outputLineage(lineageFile, LINEAGE_RECURSION_DEPTH);
        lineageFile.close();
    }
#endif
}

void CandidateSolution::outputLineage(ostream& lineageFile, unsigned int recursion, const string& indent) const
{
    lineageFile <<
    indent << "id: " << getId() << "\n" <<
    indent << "operator: " << getLineage().getOperatorName() << "\n" <<
    indent << "parent_ids: [";
    for (auto id: getLineage().getParentNames()) {
        lineageFile << id << ", ";
    }
    lineageFile << "]\n";
}



}
}