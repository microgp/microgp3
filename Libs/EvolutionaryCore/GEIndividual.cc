/***********************************************************************\
|                                                                       |
| GEIndividual.cc                                                       |
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
 * @file GEIndividual.cc
 * Implementation of the GEIndividual class.
 * @see GEIndividual.h
 * @see GEIndividual.xml.cc
 */

#include "EvolutionaryCore.h"
#include "GEIndividual.h"

using namespace ugp3::core;

GEIndividual::GEIndividual(const Population& population)
: Individual(population)
{}

GEIndividual::GEIndividual(
                          long unsigned int birth, const string& geneticOperator, const vector< string >& parents, const Population& population)
: Individual(birth, geneticOperator, parents, population)
{}

GEIndividual::GEIndividual(const GEIndividual& individual)
: Individual(individual)
{}

GEIndividual::~GEIndividual()
{
    for (Group* group: m_groups) {
        group->removeIndividual(this);
    }
}

const GroupPopulation& GEIndividual::getPopulation() const
{
    return *dynamic_cast<const GroupPopulation*>(&Individual::getPopulation());
}

unique_ptr<Individual> GEIndividual::clone() const
{
    return unique_ptr<Individual> ( new GEIndividual(*this) );
}

void GEIndividual::addGroup(Group* group)
{
    if (group == nullptr)
    {
        throw ArgumentNullException("group", LOCATION);
    }

    for (unsigned int i = 0; i < this->m_groups.size(); i++)
    {
        if (this->m_groups[i] == group)
        {
            throw ArgumentException("The group '" + group->getId() + "' is already contained in the individual " + this->getId() + ".", LOCATION);
        } else if (this->m_groups[i]->getId() == group->getId())
        {
            throw ArgumentException("Another group with the id '" + group->getId() + "' already exists in the individual "+this->getId()+".", LOCATION);
        }
    }

    this->m_groups.push_back(group);
    m_groupContributions.push_back(std::vector<double>(getPopulation().getParameters().getIndividualFitnessParametersCount(), 0));
}

void GEIndividual::removeGroup(Group* group)
{
    if (group == nullptr) {
        throw ArgumentNullException("group", LOCATION);
    }

    bool found=false;
    unsigned int i=0;
    while (!found && i<this->m_groups.size())
    {
        if (this->m_groups[i]->getId() == group->getId())
        {
            found=true;
        } else
        {
            i++;
        }
    }

    if (!found)
        throw Exception("Group '" + group->getId() + "' not found.", LOCATION);

    this->m_groups.erase(m_groups.begin()+i);
    m_groupContributions.erase(m_groupContributions.begin() + i);
}

bool GEIndividual::safeAddGroup(Group* group) noexcept
{
    if (!group || std::find(m_groups.begin(), m_groups.end(), group) != m_groups.end())
        return false;
    
    m_groups.push_back(group);
    m_groupContributions.push_back(std::vector<double>(getPopulation().getParameters().getIndividualFitnessParametersCount(), 0));
    return true;
}

bool GEIndividual::safeRemoveGroup(Group* group) noexcept
{
    if (group) {
        auto it = std::remove(m_groups.begin(), m_groups.end(), group);
        if (it != m_groups.end()) {
            m_groups.erase(it);
            m_groupContributions.erase(m_groupContributions.begin() + std::distance(m_groups.begin(), it));
            return true;
        }
    }
    return false;
}

void GEIndividual::setState(CandidateSolution::State state)
{
    if (state == DEAD) {
        for (Group* group: m_groups) {
            group->removeIndividual(this);
        }
        m_groups.clear();
        m_groupContributions.clear();
    }
        
    ugp3::core::CandidateSolution::setState(state);
}


void GEIndividual::setGroupContribution(Group* group, vector< double >&& contribution)
{
    auto it = std::find(m_groups.begin(), m_groups.end(), group);
    Assert(it != m_groups.end());
    m_groupContributions[std::distance(m_groups.begin(), it)] = std::move(contribution);
    
    // Find the best contribution and set is as the fitness.
    getFitness().setValues(
        *std::max_element(m_groupContributions.begin(), m_groupContributions.end(),
                         [] (const std::vector<double>& a, const std::vector<double>& b) {
                             return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
                         }));
}

Group* GEIndividual::getBestGroup() const
{
    // We filter groups to consider only those with a valid fitness
    std::vector<Group*> valid_groups;
    std::copy_if(m_groups.begin(), m_groups.end(), back_inserter(valid_groups),
                 [] (Group* g) { return g->getFitness().getIsValid(); });
    
    if (valid_groups.empty())
        return nullptr;
    
    return *std::max_element(valid_groups.begin(), valid_groups.end(), [this] (Group* a, Group* b) {
        return !getPopulation().compareForSelection(a, b);
    });
}

const string GEIndividual::getDisplayTypeName() const
{
    return TypeName< GEIndividual >::name;
}

#if 0
void GEIndividual::maybeZombify()
{
    /*
     * The undeadness of individuals is linked to the state of its groups.
     * If at least one group is alive, the candidate is alive.
     * Else, it's a zombie.
     */
    if (isAlive()) {
        if (!m_groups.empty() && std::all_of(m_groups.begin(), m_groups.end(), [] (Group* g) {
            return g->isZombie();
        })) {
            LOG_VERBOSE << "Zombified " << this << " because all of its groups are zombies { ";
            for (Group* g: m_groups) {
                LOG_VERBOSE << g << " ";
            }
            LOG_VERBOSE << "}" << std::ends;
            setState(ZOMBIE);
        }
    }
}

void GEIndividual::maybeUnzombify()
{
    /*
     * The undeadness of individuals is linked to the state of its groups.
     * If at least one group is alive, the candidate is alive.
     * Else, it's a zombie.
     */
    if (isZombie()) {
        if (std::any_of(m_groups.begin(), m_groups.end(), [] (Group* g) {
            return g->isAlive();
        })) {
            LOG_VERBOSE << "Unzombified " << this << " because at least one of its groups is alive { ";
            for (Group* g: m_groups) {
                LOG_VERBOSE << g << " ";
            }
            LOG_VERBOSE << "}" << std::ends;
            setState(ALIVE);
        }
    }
    
    // Should not happen in the current implementation.
    // But: good point! what do we do when individuals die? keep them inside groups?
    // TODO decide what to do.
    // FIXME in fact the same thing is implemented in 
#if 0
    if (isDead())
    {
        for (unsigned int i=0;i<this->m_groups.size();i++)
        {
            this->m_groups[i]->removeIndividual(this);
            this->m_groups[i]->getFitness().invalidate();
        }
        this->m_groups.clear();
        m_groupContributions.clear();
    }
#endif
}
#endif

