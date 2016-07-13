/***********************************************************************\
|                                                                       |
| Group.cc                                                              |
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
 * @file Group.cc
 * Implementation of the Group class.
 * @see Group.h
 * @see Group.xml.cc
 */

#include "EvolutionaryCore.h"

using namespace ugp3::core;
using namespace ugp3::log;
using namespace std;

InfinityString Group::idCounter;
InfinityString Group::temporaryIdCounter;

void Group::setGroupCounter(const string& value)
{
    InfinityString newVal(value);
    Assert(Group::idCounter <= newVal);
    Group::idCounter = newVal;
}

string Group::getGroupCounter()
{
    return Group::idCounter.toString();
}


Group::Group(
    unsigned long birth, 
    const GroupPopulation &population)
    : CandidateSolution(birth, population, idCounter.toString())
{
    idCounter++;
    
    LOG_DEBUG << "Constructed new Group (1) " << this << ends;
}

Group::Group(long unsigned int birth, const GroupPopulation& population, bool)
: CandidateSolution(birth, population, "TMP_" + temporaryIdCounter.toString())
{
    temporaryIdCounter++;
    
    LOG_DEBUG << "Constructed new temporary Group (0) " << this << ends;
}


Group::Group(const xml::Element& element, const GroupPopulation& population)
    : CandidateSolution(population, idCounter.toString())
{
    idCounter++;
    
//	if(element.ValueStr() != Group::XML_NAME) 
//	{
//		throw xml::SchemaException("expected element '" + Group::XML_NAME + "', found " + element.ValueStr(), LOCATION);
//	}

    this->readXml(element);
    LOG_DEBUG << "Constructed new Group (2) " << this << ends;
    Assert(checkNoDuplicateIndividual());
}

Group::Group(const Group* g, const std::vector<GEIndividual*>& oldIndividuals, const std::vector<GEIndividual* >& newIndividuals)
    : CandidateSolution(*g, idCounter.toString())
{
    idCounter++;
    
    m_individuals = g->getIndividuals();

    for (auto ind: oldIndividuals) {
        removeIndividual(ind);
    }
    for (auto ind: newIndividuals) {
        addIndividual(ind);
    }
    for (auto ind: m_individuals) {
        ind->addGroup(this);
    }
    
    LOG_DEBUG << "Group " << getId() << " created with the individuals";
    for (auto ind: m_individuals) {
        LOG_DEBUG << " " << ind;
    }
    LOG_DEBUG << ends;
    
    LOG_DEBUG << "Constructed Group (3) " << this << ends;
    Assert(checkNoDuplicateIndividual());
}

unique_ptr<Group> Group::clone() const
{
    // create copy of this group with new id
    Group* group = new Group(this->getPopulation().getGeneration(), this->getPopulation());

    LOG_DEBUG << "Group " << *group << " obtained cloning group " << *this << std::ends;

    // associate all individuals (which were part of the old group) with the new group
    for (auto individual: m_individuals) {
        group->addIndividual(individual);
        individual->addGroup(group);
    }

    return unique_ptr<Group>(group);
}

Group::~Group()
{
    LOG_DEBUG << "Destructor: ugp3::core::Group " << this << ends;

    for (auto ind: m_individuals) {
        // The memory taken by the individuals is released by GroupPopulation
        ind->safeRemoveGroup(this);
    }
    Assert(checkNoDuplicateIndividual());
}

bool Group::contains(const GEIndividual* individual) const
{
    if (!individual)
        throw ArgumentNullException("individual", LOCATION);
    
    return std::find(m_individuals.begin(), m_individuals.end(), individual) != m_individuals.end();
}

bool Group::addIndividual(GEIndividual* individual)
{
    if (!individual)
        throw ArgumentNullException("individual", LOCATION);
    
    if (contains(individual))
        return false;
    
    m_individuals.push_back(individual);
    getRawFitness().invalidate();
    getFitness().invalidate();
    invalidateNormalizedPhenotype();
    invalidateHashCodes();
    invalidateMessage();
    Assert(checkNoDuplicateIndividual());
    return true;
}

bool Group::removeIndividual(GEIndividual* individual)
{
    if (!individual)
        throw ArgumentNullException("individual", LOCATION);
    
    auto it = std::remove(m_individuals.begin(), m_individuals.end(), individual);
    if (it != m_individuals.end()) {
        m_individuals.erase(it, m_individuals.end());
        getRawFitness().invalidate();
        getFitness().invalidate();
        invalidateNormalizedPhenotype();
        invalidateHashCodes();
        invalidateMessage();
        Assert(checkNoDuplicateIndividual());
        return true;
    }
    Assert(checkNoDuplicateIndividual());
    return false;
}

void Group::substituteClonedIndividual(GEIndividual* clone, GEIndividual* master)
{
    if (!clone)
        throw ArgumentNullException("clone", LOCATION);
    if (!master)
        throw ArgumentNullException("master", LOCATION);
    
    Assert(clone->isGenotypeEqual(*master));
    Assert(clone->getHashCode(GENOTYPE) == master->getHashCode(GENOTYPE));
    
    // Maybe remove the master and invalidate fitness and hash code
    removeIndividual(master);
    
    // Find the clone and replace it with the master.
    auto it = std::find(m_individuals.begin(), m_individuals.end(), clone);
    Assert(it != m_individuals.end());
    *it = master;
    
    // We invalidate the clone-detection code because it is based on individual names
    invalidateNormalizedPhenotype();
    Assert(checkNoDuplicateIndividual());
}

void Group::toCode(const string& fileName, vector< string >* outfiles) const
{
    CandidateSolution::toCode(fileName, outfiles);
    
    ofstream file;
    file.open(fileName.c_str());
    
    if(file.is_open() == false)
    {
        throw Exception("Could not open file \"" + fileName + "\".", LOCATION);
    }
    
    string indFileName;
    for (auto ind: m_individuals) {
        indFileName = File::appendStringToName(fileName, "individual" + ind->getId());
        file << indFileName << endl;
        if (outfiles)
            outfiles->push_back(fileName);
        ind->toCode(indFileName, outfiles); 
    }
        
    LOG_DEBUG 
    << "Saving group " << this
    << " to file \"" << fileName << "\"..." << ends;
    
    file.close();
}

void Group::outputLineage(ostream& lineageFile, unsigned int recursion, const string& indent) const
{
    CandidateSolution::outputLineage(lineageFile, recursion, indent);
    
    if (recursion) {
        lineageFile <<
        indent << "parents:\n";
        for (auto parent: getLineage().getParents()) {
            lineageFile <<
            indent << "  -\n";
            parent->outputLineage(lineageFile, recursion - 1, indent + "    ");
        }
    }
}

void Group::removeIndividualsCodeFiles(std::string fileName) const
{
    string indFileName;
    for (auto ind: m_individuals) {
        indFileName = File::appendStringToName(fileName, "individual" + ind->getId());
        if (!File::remove(indFileName)) {
            LOG_WARNING << "Could not delete file \"" << indFileName << "\"" << ends;
        }
    }
}

GEIndividual* Group::getRandomIndividual()
{
    // NOTE/DET order by id
    std::sort(m_individuals.begin(), m_individuals.end(), CandidateSolution::OrderById());
    return static_cast<GEIndividual*>(m_individuals.at(Random::nextUInteger(0,getIndividualCount()-1)));
}

hash_t Group::calculateHashCode(Purpose purpose) const
{
    // FIXME da verificare
    hash_t result = startValue;
    // Since the order in which we combine hash codes matter, we standardize the order
    // This is required so that {A, B} and {B, A} have the same hash code
    
    std::vector<GEIndividual*> individuals = m_individuals;
    std::sort(individuals.begin(), individuals.end(), [&] (GEIndividual* ind1, GEIndividual* ind2) {
        return ind1->getHashCode(purpose) < ind2->getHashCode(purpose);
    });
    for (auto ind: m_individuals) {
        result = djbHash(result, ind->getHashCode(purpose));
    }
    return result;
}

void Group::computeNormalizedPhenotype(string& code) const
{
    std::vector<GEIndividual*> individuals = m_individuals;
    std::sort(individuals.begin(), individuals.end(), [] (GEIndividual* ind1, GEIndividual* ind2) {
        return ind1->getId() < ind2->getId();
    });
    for (auto individual: individuals) {
        code += individual->getId();
        code += " "; // Ids never contain spaces
    }
}

bool Group::validate() const
{
    Assert(checkNoDuplicateIndividual());
    
    // Check group size
    if (m_individuals.size() < getPopulation().getParameters().getMinIndividualsPerGroup() 
        || getPopulation().getParameters().getMaxIndividualsPerGroup() < m_individuals.size()) {
        return false;
    }
    // Check individuals
    // FIXME needed?
    // NOTE it is slow, according to callgrind
    // for (auto ind: m_individuals) {
    //     if (!ind->validate()) {
    //         return false;
    //     }
    // }
    return true;
}

const GroupPopulation& Group::getPopulation() const
{
    return dynamic_cast<const GroupPopulation&>(CandidateSolution::getPopulation());
}

void Group::computeMessage(ugp3::Message& message) const
{
    for (GEIndividual* ind: m_individuals) {
        message += ind->getMessage();
    }
}

const string Group::getDisplayTypeName() const
{
    return TypeName< Group >::name;
}

bool Group::checkNoDuplicateIndividual() const
{
    std::vector<GEIndividual*> individuals = m_individuals;
    std::sort(individuals.begin(), individuals.end());
    auto uend = std::unique(individuals.begin(), individuals.end());
    return distance(individuals.begin(), uend) == distance(m_individuals.begin(), m_individuals.end());
}

void Group::setState(CandidateSolution::State state)
{
    ugp3::core::CandidateSolution::setState(state);
    
    // NOTE no need to zombify individuals, now they are authorized to have no group.
    // if (isZombie()) {
    //     // Zombify all individuals that only belong to zombie groups
    //     for (GEIndividual* ind: m_individuals) {
    //         ind->maybeZombify();
    //     }
    // }
}

