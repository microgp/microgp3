/***********************************************************************\
|                                                                       |
| GroupPopulation.cc                                                    |
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
| $Revision: 659 $
| $Date: 2015-04-23 11:31:26 +0200 (Thu, 23 Apr 2015) $
\***********************************************************************/

/**
 * @file GroupPopulation.cc
 * Implementation of the GroupPopulation class.
 * @see GroupPopulation.h
 * @see GroupPopulation.xml.cc
 */

#include "GroupPopulation.h"
#include "ArgumentException.h"
#include "GEIndividual.h"
#include "GeneticOperator.h"

#include <set>
#include <algorithm>

using namespace ugp3;
using namespace ugp3::core;
using namespace ugp3::log;
using namespace std;

GroupPopulation::GroupPopulation(const EvolutionaryAlgorithm& parent)
  : SpecificIndividualPopulation(parent),
  m_parameters(this)
{
    LOG_WARNING << "*************************************************" << std::ends;
    LOG_WARNING << "*                                               *" << std::ends;
    LOG_WARNING << "*                W A R N I N G                  *" << std::ends;
    LOG_WARNING << "*     Group evolution is a work in progress.    *" << std::ends;
    LOG_WARNING << "*                                               *" << std::ends;
    LOG_WARNING << "*************************************************" << std::ends;
}

const Group& GroupPopulation::getGroup(unsigned int i) const
{
    Assert(this->m_groups.at(i) != nullptr);

    return *this->m_groups.at(i);
}

void GroupPopulation::addGroup(unique_ptr<Group> group)
{
    if (group.get() == nullptr)
    {
        throw ArgumentNullException("group", LOCATION);
    }

    for (unsigned int i = 0; i < this->m_groups.size(); i++)
    {
        if (this->m_groups[i] == group.get())
        {
            throw ArgumentException("The group '" + group->getId() + "' is already contained in the population.", LOCATION);
        }
        else if (this->m_groups[i]->getId() == group->getId())
        {
            throw ArgumentException("Another group with the id '" + group->getId() + "' already exists in the population.", LOCATION);
        }
    }

    this->m_groups.push_back(group.release());
}

void GroupPopulation::addGroups(std::vector<Group*> groups)
{
    for (unsigned int i=0;i<groups.size();i++)
        addGroup(unique_ptr<Group>(groups[i]));
}

unique_ptr<Group> GroupPopulation::newRandomGroup()
{
    unique_ptr<Group> g = unique_ptr<Group>(new Group(this->getGeneration(), *this));

    unsigned int min = this->getParameters().getMinIndividualsPerGroup();
    unsigned int max = this->getParameters().getMaxIndividualsPerGroup();
    
    Assert(m_individuals.size() >= max);
    
    unsigned int n = Random::nextUInteger(min, max);

    LOG_DEBUG << "Creating new group " << g.get() << " of " << n << " individuals..." << ends;
    
    // NOTE/DET We sort individuals by id before each selection to get deterministic results.
    std::sort(m_individuals.begin(), m_individuals.end(), CandidateSolution::OrderById());

    for (unsigned int i = 0; i < n; ++i) {
        unsigned int selected = Random::nextUInteger(i, m_individuals.size() - 1);
        GEIndividual* ind = m_individuals[selected];
        bool done = g->addIndividual(ind);
        Assert(done);
        ind->addGroup(g.get());
        LOG_DEBUG << "Added individual " << ind << " to the group " << g.get() << ends;
        // Move the selected individual to the beginning so that it won't be selected again next time
        std::swap(m_individuals[i], m_individuals[selected]);
    }

    LOG_DEBUG << "Group " << g.get() << " built" << ends;

    return g;
}

void GroupPopulation::setupPopulation()
{
    SpecificIndividualPopulation::setupPopulation();
    
    unsigned int nu = getParameters().getGroupNu();

    LOG_INFO << "Creating " << nu << " groups of individuals" << Progress(Progress::START) << ends;

    for (unsigned long i = 0; i < nu; ++i)
    {
        // Create a new random group with individuals already created.
        unique_ptr<Group> group = this->newRandomGroup();
        LOG_INFO << "Creating " << nu << " groups of individuals" << Progress((double)i/(double)nu) << ends;
        // Save the new group in the vector.
        this->addGroup(std::move(group));
    }

    LOG_INFO << "Creating " << nu << " groups of individuals" << Progress::END << ends;
}

void GroupPopulation::removeDeadCandidates()
{
    removeCorpses(m_groups);
    removeCorpses(m_individuals);
}

bool GroupPopulation::compareForFitnessHole(const CandidateSolution* a, const CandidateSolution* b) const
{
    return Population::compareForFitnessHole(a, b);
    
    // On string coverage problems, this leads to loss of diversity
    // const GEIndividual* ia = dynamic_cast<const GEIndividual*>(a);
    // const GEIndividual* ib = dynamic_cast<const GEIndividual*>(b);
    // if (ia && ib) {
    //     // Get best groups
    //     Group* ga = ia->getBestGroup();
    //     Group* gb = ib->getBestGroup();
    //     if (ga && gb) {
    //         int result = ga->getFitness().compareTo(gb->getFitness());
    //         if (result != 0) {
    //             return result > 0;
    //         }
    //     }
    // }
}

void GroupPopulation::killInvalidGroups(vector<Group*>& groups)
{ 
    // Removes groups with a number of individuals out of the specified range
    unsigned int min = this->getParameters().getMinIndividualsPerGroup();
    unsigned int max = this->getParameters().getMaxIndividualsPerGroup();

    LOG_VERBOSE << "Killing invalid groups..." << ends;
    
    // NOTE/DET Ensure deterministic order of deaths
    std::sort(groups.begin(), groups.end(), CandidateSolution::OrderById());
    
    for (Group* group: groups) {
        size_t n = group->getIndividualCount();
        if (n < min || max < n) {
            LOG_DEBUG << "Group " << group << " killed because it is invalid, it has " << n << " individuals" << ends;
            group->setState(CandidateSolution::DEAD);
        }
    }
}

void GroupPopulation::evaluateAndHandleClones()
{
    Assert(checkHashGenoPhenoEqualitiesAndHashQuality(m_groups.begin(), m_groups.end()));
    Assert(checkHashGenoPhenoEqualitiesAndHashQuality(m_individuals.begin(), m_individuals.end()));
    
    /*
     * WARNING here we must detect and remove individual clones before group clones,
     * because we based group clone detection on the names of group members instead
     * of the genomes of group members. So for example: G1 = {A, B} and G2 = {A', B}
     * with A' being a clone of A can only be detected as clones if we first
     * substitute A' with A in G2.
     */
    detectAndHandleClones(m_individuals.begin(), m_individuals.end());
    /*
     * A group can contain both a master and its clone(s). In that case, the group
     * size can drop below the minimum when we remove the clone(s). That's why
     * we must kill all invalid groups after individual clone detection.
     */
    killInvalidGroups(m_groups);
    
    size_t evaluated = 0;
    evaluated += runEvaluator(m_groups.begin(), m_groups.end());
    evaluated += runEvaluator(m_individuals.begin(), m_individuals.end());
    
    if (evaluated > 0) {
        // Kill group clones and/or scale their fitness (maybe in the future?)
        detectAndHandleClones(m_groups.begin(), m_groups.end());
        
        updateDeltaEntropy(m_individuals.begin(), m_individuals.end());
        m_entropy = updateDeltaEntropy(m_groups.begin(), m_groups.end());
        
        if (getParameters().getFitnessSharingEnabled()) {
            shareFitness(m_individuals.begin(), m_individuals.end());
        }
        if (getParameters().getGroupFitnessSharingEnabled()) {
            shareFitness(m_groups.begin(), m_groups.end());
        }
        
        scaleIndividualContribution();
    }
    
#ifndef NDEBUG
    // Check that all group-individual relationships are coherent
    for (GEIndividual* ind: m_individuals) {
        for (Group* group: m_groups) {
            Assert(group->contains(ind) == ind->belongsTo(group));
        }
    }
#endif
}

void GroupPopulation::checkFitnessValidity()
{
    SpecificIndividualPopulation::checkFitnessValidity();
    
    for (auto group: m_groups) {
        if (!group->isDead()) {
            Assert(group->getRawFitness().getIsValid());
            Assert(group->getFitness().getIsValid());
        }
    }
}

void GroupPopulation::selectNewZombifiableCandidates()
{
    auto liveBegin = regroupAndSkipDeadCandidates(m_groups.begin(), m_groups.end());
    
    m_bloodMagicWaitingList.clear();
    m_bloodMagicWaitingList.push_back(*std::max_element(
        liveBegin, m_groups.end(),
        [this] (CandidateSolution* a, CandidateSolution* b) {
            return !compareRawBestWorst(a, b);
        }));
}

void GroupPopulation::age()
{
    // Don't age the individuals
    // We must still call the step function beacause of lineages
    // NOTE/DET Ensure deterministic order of calls to step()
    std::sort(m_individuals.begin(), m_individuals.end(), CandidateSolution::OrderById());
    for (GEIndividual* ind: m_individuals)
        ind->step(false);

    // Ages the groups
    promoteHeroesAndAge(m_groups.begin(), m_groups.end());
}

void GroupPopulation::handleZombies()
{
    LOG_VERBOSE << "Removing old group zombies" << ends;
    
    for (Group* group: m_groups) {
        if (group->isZombie() && !isZombifiable(group)) {
            group->setState(CandidateSolution::DEAD);
        }
    }
}

void GroupPopulation::slaughtering()
{
    allopatricSelection(m_groups.begin(), m_groups.end());

    // Removes the worst groups
    unsigned int groupMu = getParameters().getGroupMu();
    simpleSelection(groupMu, m_groups.begin(), m_groups.end());
    
    // Move individuals with groups to the beginning
    auto withoutGroupBegin = std::partition(m_individuals.begin(), m_individuals.end(), [] (GEIndividual* ind) {
        return ind->getGroupsCount() > 0;
    });
    
    allopatricSelection(withoutGroupBegin, m_individuals.end());
    
    unsigned int mu = getParameters().getMu();
    simpleSelection(mu, withoutGroupBegin, m_individuals.end());
}

void GroupPopulation::prepareForCommit()
{
    // Rescale the fitness of group clones (maybe in the future?)
    detectAndHandleClones(m_groups.begin(), m_groups.end());
    
    updateDeltaEntropy(m_individuals.begin(), m_individuals.end());
    m_entropy = updateDeltaEntropy(m_groups.begin(), m_groups.end());
    
    if (getParameters().getFitnessSharingEnabled()) {
        shareFitness(m_individuals.begin(), m_individuals.end());
    }
    if (getParameters().getGroupFitnessSharingEnabled()) {
        shareFitness(m_groups.begin(), m_groups.end());
    }
    
    scaleIndividualContribution();
}

double GroupPopulation::getAverageAge() const
{
    return computeAverageAge(m_groups.begin(), m_groups.end());
}

vector< double > GroupPopulation::getAverageRawFitness() const
{
    return computeAverageRawFitness(m_groups.begin(), m_groups.end());
}

double GroupPopulation::computeGroupsAverageSize() const
{
    double groupsSize = 0;
    
    for (auto group: m_groups)
        groupsSize += group->getIndividualCount();

    return groupsSize / m_groups.size();
}

void GroupPopulation::commit()
{
    SpecificIndividualPopulation::commit();
    
    // save the best and the worst group
    auto minmax = std::minmax_element(
        m_groups.begin(), m_groups.end(), [this] (Group* a, Group* b) {
            return !compareRawBestWorst(a, b);
        });
    m_worstRawGroup = *minmax.first;
    m_bestRawGroup = *minmax.second;
    minmax = std::minmax_element(
        m_groups.begin(), m_groups.end(), [this] (Group* a, Group* b) {
            return !compareScaledBestWorst(a, b);
        });
    m_worstScaledGroup = *minmax.first;
    m_bestScaledGroup = *minmax.second;

    LOG_VERBOSE << "The best raw group is " << this->m_bestRawGroup << " " << m_bestRawGroup->getRawFitness() << ends;
    LOG_VERBOSE << "The worst raw group is " << this->m_worstRawGroup << " " << m_worstRawGroup->getRawFitness() << ends;
    LOG_VERBOSE << "The best scaled group is " << this->m_bestScaledGroup << " " << m_bestScaledGroup->getFitness() << ends;
    LOG_VERBOSE << "The worst scaled group is " << this->m_worstScaledGroup << " " << m_bestScaledGroup->getFitness() << ends;
}


void GroupPopulation::showStatistics(void) const
{
    vector<double> averageFitness = computeAverageRawFitness(m_groups.begin(), m_groups.end());
    double averageAge = computeAverageAge(m_groups.begin(), m_groups.end());
    double averageSize = this->computeGroupsAverageSize();
    double averageDeltaEntropy = computeAverageDeltaEntropy(m_groups.begin(), m_groups.end());
    
    LOG_INFO << "[Groups] Count: " << m_groups.size() 
    << "; average age: " << averageAge
    << "; average size: " << averageSize
    << "; average delta entropy: " << averageDeltaEntropy
    << std::ends;
    LOG_INFO << "[Groups] Average fitness:";
    for (unsigned int i = 0; i <  averageFitness.size(); i++)
    {
        LOG_INFO << " " << averageFitness[i];
    }
    LOG_INFO<< ends;
    
    if (getBestRawGroup()) {
        LOG_INFO << "[Groups] Best fitness: " << this->getBestRawGroup() << " " << this->getBestRawGroup()->getRawFitness() << ends;
    }
    if (getWorstRawGroup()) {
        LOG_INFO << "[Groups] Worst fitness: " << this->getWorstRawGroup() << " " << this->getWorstRawGroup()->getRawFitness() << ends;
    }

    SpecificIndividualPopulation::showStatistics();
}

void GroupPopulation::dumpStatistics(ostream& output) const
{
    SpecificIndividualPopulation::dumpStatistics(output);
    
    output << "," << computeAverageAge(m_groups.begin(), m_groups.end());
    output << "," << computeGroupsAverageSize();
    
    // TODO: Also insert scaled fitness inside the statistics?

    // FITNESS: AVERAGE
    vector<double> averageFitness = computeAverageRawFitness(m_groups.begin(), m_groups.end());
    for(unsigned int i = 0; i < averageFitness.size(); i++)
    {
        output << "," << averageFitness[i];
    }

    // FITNESS: BEST INDIVIDUAL (IF DEFINED)
    if (getBestRawGroup()) {
        getBestRawGroup()->getRawFitness().writeCSV(output);
    }
    else 
    {
        for(unsigned int i = 0; i < averageFitness.size(); i++)
        {
            output << ",na";
        }
    }

    // FITNESS: WORST INDIVIDUAL (IF DEFINED)
    if (getWorstRawGroup()) {
        getWorstRawGroup()->getRawFitness().writeCSV(output);
    }
    else 
    {
        for(unsigned int i = 0; i < averageFitness.size(); i++)
        {
            output << ",na";
        }
    }
}

void GroupPopulation::dumpStatisticsHeader(ostream& output) const
{
    SpecificIndividualPopulation::dumpStatisticsHeader(output);
    
    output << "," << this->name << "_GrAvgAge";
    output << "," << this->name << "_GrAvgSize";

    // FITNESS: AVERAGE
    for(unsigned int i = 0; i < this->getParameters().getFitnessParametersCount(); i++)
    {
        output << "," << this->name << "_GrAvg_f" << i;
    }

    // FITNESS: BEST INDIVIDUAL (IF DEFINED)
    for(unsigned int i = 0; i < this->getParameters().getFitnessParametersCount(); i++)
    {
        output << "," << this->name << "_GrBest_f" << i;
    }

    // FITNESS: WORST INDIVIDUAL (IF DEFINED)
    for(unsigned int i = 0; i < this->getParameters().getFitnessParametersCount(); i++)
    {
        output << "," << this->name << "_GrWorst_f" << i;
    }
    
}

void GroupPopulation::writeGroupsAndIndividualsComposition() const
{
    LOG_INFO << "Groups composition: " << ends;
    for (auto g: m_groups) 
    {
        LOG_INFO << "Group " << g <<":";
        for (auto ind: g->getIndividuals()) 
	{
            LOG_INFO << " " << ind;
        }
        LOG_INFO << ends;
    }

    LOG_INFO << "Individuals composition: " << ends;
    for (auto ind: m_individuals) 
    {
        LOG_INFO << "Individual " << ind <<":";
        for (auto g: static_cast<GEIndividual*>(ind)->getGroups()) 
	{
            LOG_INFO << " " << g; 
        }
        LOG_INFO << ends;
    }
}

std::vector<Group*> GroupPopulation::selectParentsGroupsFrom(const vector<Group*>& parentsGroups, unsigned int numberToSelect)
{
    std::vector<Group*> selectedGroups;
    unsigned int remainingToSelect = numberToSelect;
    /*
     * If we want to produce more new groups than the number of 
     * available parents, we select several times each parent.
     */
    while (remainingToSelect >= parentsGroups.size()) {
        selectedGroups.insert(selectedGroups.end(), parentsGroups.begin(), parentsGroups.end());
        remainingToSelect -= parentsGroups.size();
    }
    /*
     * Select the remaining parents using a tournament.
     */
    if (remainingToSelect > 0) {
        LOG_DEBUG << "Parent-Groups selection (count = " << remainingToSelect << ")"<<  ends;
        CandidateSelection& selector = getParameters().getSelector();
        std::vector<CandidateSolution*> selectedCandidates = selector.selectCandidates(
            parentsGroups.begin(), parentsGroups.end(), *this, remainingToSelect, 3);
        // We took care of supplying enough selectable groups
        Assert(selectedCandidates.size() == remainingToSelect);
        for (CandidateSolution* selected: selectedCandidates) {
            Assert(dynamic_cast<Group*>(selected));
            selectedGroups.push_back(static_cast<Group*>(selected));
        }
    }
    Assert(selectedGroups.size() == numberToSelect);
    return selectedGroups;
}
    
// TODO take into account the allopatric tags from the genetic operator?
// TODO allopatric tags as is the paper:
// A benchmark for cooperative coevolution, Alberto Tonda, Evelyne Lutton,
// Giovanni Squillero

// Example: when we place an individual in a group, we could set the allopatric tag
// of the group to the same value as the individual.
std::vector< Group* > GroupPopulation::generateSonsGroups(
    const OperatorSelector::Result& selectedOperator,
    const std::vector< GEIndividual* >& sons)
{
    if (sons.empty())
        throw ArgumentException("The vector of individuals can't be empty.", LOCATION);
    
    const std::size_t minProducedGroups = 1; // TODO choose this constant
    std::vector<Group*> validNewGroups;
    
    std::vector<GEIndividual*> parents;
    for (Individual* ind: sons.front()->getLineage().getParents()) {
        GEIndividual* geInd = dynamic_cast<GEIndividual*>(ind);
        Assert(geInd);
        parents.push_back(geInd);
    }
    LOG_DEBUG << "The individual " << sons.front() << " has "
    << parents.size() << " parent" << (parents.size() > 1 ? "s" : "")
    << ". Picking Parent Groups" << ends;
    
    // Gets the groups of the parents
    vector<Group*> parentsGroups;
    for (GEIndividual* ind: parents) {
        const vector<Group*>& parentGroups = ind->getGroups();
        parentsGroups.insert(parentsGroups.end(), parentGroups.begin(), parentGroups.end());
    }
    
    // Removes the repeated groups
    std::sort(parentsGroups.begin(), parentsGroups.end());
    auto newEnd = std::unique(parentsGroups.begin(), parentsGroups.end());
    parentsGroups.resize(std::distance(parentsGroups.begin(), newEnd));
    
    /* 
     * Remove groups that have been created during this generation, because they
     * don't have a valid fitness yet (they can't be tournament-selected) and
     * because we separate the new and the old generation during the offspring generation.
     * 
     * Also remove all groups that are not alive.
     */
    parentsGroups.erase(std::remove_if(parentsGroups.begin(), parentsGroups.end(), [this] (Group* g) {
        return g->getBirth() == getGeneration() || !g->isAlive();
    }), parentsGroups.end());
    
    /*
     * It can happen that new individuals have no parents, when for example
     * they have been generated by the `randomizer' operator, or that
     * the parents have no group. In those cases, we first create one group
     * specifically for these individuals (if possible given the min and
     * max group sizes), and we select some other groups from the population
     * to insert the individuals into.
     * 
     * FIXME: what is the goal of this function? No orphans, or a comparison
     * of collaborative quality between parents and their children?
     * If the answer is the second, just return instead of creating a group
     * on purpose.
     */
    if (parentsGroups.empty()) {
        // First the new group
        Group* g = new Group(getGeneration(), *this);
        for (auto i: sons) {
            g->addIndividual(i);
            i->addGroup(g);
        }
        if (g->validate()) {
            LOG_VERBOSE << "generateSonsGroups: no parents, successfully created a new group: " << g << std::ends;
            g->getLineage().set(selectedOperator.op->getName(), std::vector<Group*>());
            validNewGroups.push_back(g);
        } else {
            delete g;
        }
        
        // Then try to insert into existing groups
        std::vector<Group*> selectable;
        std::copy_if(m_groups.begin(), m_groups.end(), back_inserter(selectable), [this] (Group* g) {
                         return g->getBirth() < getGeneration() && g->isAlive();
                     });
        // NOTE/DET Ensure deterministic selections by sorting the selectable groups by id.
        std::sort(selectable.begin(), selectable.end(), CandidateSolution::OrderById());
        while (validNewGroups.size() < minProducedGroups && selectable.size() > 0) {
            const std::vector<Group*>& selectedGroups = selectParentsGroupsFrom(selectable, 1);
            Group* selected = selectedGroups.front();
            selectable.erase(std::remove(selectable.begin(), selectable.end(), selected), selectable.end());
            Group* group = new Group(selected, parents, sons);
            if (group->validate()) {
                group->getLineage().set(selectedOperator.op->getName(), {selected});
                validNewGroups.push_back(group);
            } else {
                delete group;
            }
        }
        
        return validNewGroups;
    }
    // From now on the sons have parents and there are selectable parentsGroups
    
    Assert(!parentsGroups.empty());
    Assert(std::all_of(parentsGroups.begin(), parentsGroups.end(), [] (Group* group) {
        return group->getRawFitness().getIsValid() && group->getFitness().getIsValid();
    }));
    
    // NOTE/DET Ensure deterministic results by ordering the parentsGroups by id
    std::sort(parentsGroups.begin(), parentsGroups.end(), CandidateSolution::OrderById());
    
    /* 
     * First case: only one parent. We assume this is a mutation operator
     * and we substitute the parent with each of its mutants.
     * This cannot fail, since the 1 -> 1 substitution does not change
     * the group size, and each parent group contains surely the parent.
     */
    if (parents.size() == 1) {
        const size_t producedGroups = std::max(minProducedGroups, sons.size());
        std::vector<Group*> selectedGroups = selectParentsGroupsFrom(parentsGroups, producedGroups);
        Assert(selectedGroups.size() >= sons.size());
        // TODO here use allopatric tags
        for (std::size_t i = 0; i < selectedGroups.size(); ++i) {
            Group* group = new Group(selectedGroups[i], parents, std::vector<GEIndividual*>({sons[i % sons.size()]}));
            Assert(group->validate());
            group->getLineage().set(selectedOperator.op->getName(), {selectedGroups[i]});
            validNewGroups.push_back(group);
        }
        Assert(validNewGroups.size() == producedGroups);
        return validNewGroups;
    }
    
    /*
     * General case: more than two parents. We first try to replace
     * the two parents with all their sons, at the risk of creating
     * invalid groups because:
     *  1.  #parents != #sons, so we change the size of the group, or
     *  2.  #parents == #sons but the parent group does not contain
     *      all the parents, so we also change the size of the group.
     */
    {
        std::vector<Group*> selectable = parentsGroups;
        while (validNewGroups.size() < minProducedGroups && selectable.size() > 0) {
            const std::vector<Group*>& selectedGroups = selectParentsGroupsFrom(selectable, 1);
            Group* selected = selectedGroups.front();
            selectable.erase(std::remove(selectable.begin(), selectable.end(), selected), selectable.end());
            Group* group = new Group(selected, parents, sons);
            if (group->validate()) {
                group->getLineage().set(selectedOperator.op->getName(), {selected});
                validNewGroups.push_back(group);
            } else {
                delete group;
            }
        }
        
        if (validNewGroups.size() == minProducedGroups) {
            return validNewGroups;
        }
    }
    
    /*
     * If this strategy did not produce enough groups, we now perform
     * secure substitutions: when a group is selected, we count the number
     * of parents it contains. Then we select the same number of sons
     * and perform the substitution.
     * 
     * Since the goal of this function is to place all the sons into at
     * least one valid group, we keep track of this by removing sons from
     * the following `orphans' array as soon as they are assigned to a
     * valid group.
     * 
     * If the previous strategy produced at least one group, there are no
     * orphans, since this one valid group contains all the sons.
     */
    std::set<GEIndividual*> orphans;
    if (validNewGroups.empty()) {
        orphans.insert(sons.begin(), sons.end());
    }
    
    // We select sons independently of how the operator returned them.
    // It matters if we select the same son twice, we don't want to favorize the firsts.
    std::vector<GEIndividual*> shuffledSons = sons;
    // NOTE/DET Before shuffling we order by id so that the result of the shuffle depends only on the random state
    std::sort(shuffledSons.begin(), shuffledSons.end(), CandidateSolution::OrderById());
    ugp3::Random::shuffle(shuffledSons.begin(), shuffledSons.end());
    // We use this iterator to select sons.
    auto sonsIterator = shuffledSons.begin();
    
    while (validNewGroups.size() < minProducedGroups || !orphans.empty()) {
        const std::vector<Group*>& selectedGroups = selectParentsGroupsFrom(parentsGroups, 1);
        Group* selected = selectedGroups.front();
        unsigned int nbParents = std::count_if(
            selected->getIndividuals().begin(), selected->getIndividuals().end(),
            [&] (GEIndividual* ind) {
                return std::find(parents.begin(), parents.end(), ind) != parents.end();
            });
        std::vector<GEIndividual*> sonsToSubstitute;
        for (unsigned int i = 0; i < nbParents; ++i) {
            sonsToSubstitute.push_back(*sonsIterator);
            orphans.erase(*sonsIterator);
            ++sonsIterator;
            if (sonsIterator == shuffledSons.end())
                sonsIterator = shuffledSons.begin();
        }
        Group* group = new Group(selected, parents, sonsToSubstitute);
        Assert(group->validate());
        group->getLineage().set(selectedOperator.op->getName(), {selected});
        validNewGroups.push_back(group);
    }
    
    return validNewGroups;
}

std::vector< CandidateSolution* > GroupPopulation::applyOperator(
    CallData* callData, const OperatorSelector::Result& selected)
{
    std::vector< CandidateSolution* > generated = Population::applyOperator(callData, selected);
    
    vector<GEIndividual*> newGenerationInds;
    vector<Group*> newGenerationGroups;
    for (auto candidate: generated) {
        if (GEIndividual* ind = dynamic_cast<GEIndividual*>(candidate)) {
            newGenerationInds.push_back(ind);
        } else if (Group* group = dynamic_cast<Group*>(candidate)) {
            newGenerationGroups.push_back(group);
        }
    }
    
    // TEST
#if 0
    // Eliminate "free riders": generate all groups minus one individual
    for (Group* group: newGenerationGroups) {
        if (group->getIndividualCount() > getParameters().getMinIndividualsPerGroup()) {
            for (GEIndividual* ind: group->getIndividuals()) {
                Group* gMinus = new Group(group, {ind}, {});
                gMinus->getLineage() = group->getLineage();
                generated.push_back(gMinus);
            }
        }
    }
#endif
    
    // Create the new groups for the new individuals
    if (!newGenerationInds.empty()) {
        vector<Group*> newGroups = generateSonsGroups(selected, newGenerationInds);
        
        LOG_VERBOSE
        << "The operator " << selected.op->getName() << " generated "
        << (unsigned int) newGroups.size() << " new group"
        << (newGroups.size() > 1 ? "s." : ".") << ends;
        
        // Add the generated groups to the statistics of the genetic operator
        callData->setValidChildrenCount(callData->getValidChildrenCount() + newGroups.size());
        for (auto newGroup: newGroups)
            newGroup->getLineage().setCallData(callData);
        
        generated.insert(generated.end(), newGroups.begin(), newGroups.end());
        
        LOG_DEBUG << "New individuals and groups successfully added to the population!" << ends;
    }
    
    return generated;
}

void GroupPopulation::mergeNewGeneration(const std::vector< CandidateSolution* >& newGeneration)
{
    // Check that all the produced individuals and groups are valid
    Assert(std::all_of(newGeneration.begin(), newGeneration.end(), [] (CandidateSolution* c) {
        return c->validate();
    }));
    // Check that only GEIndividuals and groups are produced
    Assert(std::all_of(newGeneration.begin(), newGeneration.end(), [] (CandidateSolution* c) {
        return dynamic_cast<Group*>(c) || dynamic_cast<GEIndividual*>(c);
    }));
    
    SpecificIndividualPopulation::mergeNewGeneration(newGeneration);
    
    for (CandidateSolution* candidate: newGeneration) {
        if (Group* group = dynamic_cast<Group*>(candidate)) {
            addGroup(unique_ptr<Group>(group));
        }
    }
}

void GroupPopulation::discardFitnessValues()
{
    SpecificIndividualPopulation::discardFitnessValues();
    
    for (auto group: m_groups) {
        group->getFitness().invalidate();
    }
}

void GroupPopulation::updateOperatorStatistics(const std::vector< CandidateSolution* >& newGeneration)
{
    SpecificIndividualPopulation::updateOperatorStatistics(newGeneration);
    
    std::vector<Group*> newGenerationGroups;
    for (auto candidate: newGeneration) {
        if (Group* group = dynamic_cast<Group*>(candidate)) {
            newGenerationGroups.push_back(group);
        }
    }
    getParameters().getActivations().updateOperatorsStatistics<Group>(
        m_groups, newGenerationGroups, getBestScaledGroup(), getWorstScaledGroup());
}


GroupPopulation::~GroupPopulation()
{
    LOG_DEBUG << "Destructor: ugp3::core::GroupPopulation" << ends;
    
    for (Group* group: m_groups) {
        delete group;
    }
}

void GroupPopulation::handleClone(CandidateSolution& master, CandidateSolution& clone,
                                  unsigned int number, unsigned int total)
{
    if (number != 0) {
        if (GEIndividual* indMaster = dynamic_cast<GEIndividual*>(&master)) {
            GEIndividual* indClone = dynamic_cast<GEIndividual*>(&clone);
            Assert(indClone);
            for (Group* group: indClone->getGroupsCopy()) {
                group->substituteClonedIndividual(indClone, indMaster);
                indClone->safeRemoveGroup(group);
                indMaster->safeAddGroup(group);
            }
            indClone->setDeath(getGeneration());
        } else if (dynamic_cast<Group*>(&master)) {
            clone.setDeath(getGeneration());
        }
    }
}

void GroupPopulation::dumpAllCandidates()
{
    // save the list of individuals of the population in a file
    ofstream groupList("groupsInPopulation.txt");
    if (!groupList.is_open()) {
        throw Exception("Cannot open the file 'groupsInPopulation.txt' for writing.", LOCATION);
    }
    
    // NOTE/DET Sort by id before dumping
    std::sort(m_groups.begin(), m_groups.end(), CandidateSolution::OrderById());

    for (auto group: m_groups) {
        Assert(group);

        // create the name of the file
        const string& fileName = File::formatToName(
            this->getParameters().getEvaluator().getInputFile(), group->getId());

        group->toCode(fileName);
        groupList << fileName << endl;
    }
    groupList.close();
}

/*
 * FIXME it's forbidden to compute fitness deltas.
 * TODO use a group selection scheme as Banzhaf does (Traulsen's model)
 */
void GroupPopulation::scaleIndividualContribution()
{
    auto begin = regroupAndSkipDeadCandidates(m_groups.begin(), m_groups.end());
    
    if (begin == m_groups.end()) { return; }
    
    // Get the current lower and upper bounds of each fitness parameter
    std::vector<double> upper = (*begin)->getRawFitness().getValues();
    std::vector<double> lower = upper;
    std::for_each(std::next(begin), m_groups.end(), [&] (Group* g) {
        const std::vector<double>& raw = g->getRawFitness().getValues();
        for (unsigned int i = 0; i < getParameters().getGroupFitnessParametersCount(); ++i) {
            upper[i] = std::max(upper[i], raw[i]);
            lower[i] = std::min(lower[i], raw[i]);
        }
    });
    
    // Perform the scaling
    double j = getParameters().getIndividualContributionScalingFactor();
    for (GEIndividual* ind: m_individuals) {
        if (!ind->isDead()) {
            vector<Group*> groups = ind->getGroups();
            auto begin = regroupAndSkipDeadCandidates(groups.begin(), groups.end());
            if (!groups.empty()) {
                Group* maxRaw = *std::max_element(begin, groups.end(), [this] (Group* a, Group* b) {
                    return !compareRawBestWorst(a, b);
                });
                // Compute one scaling coefficient per fitness component
                const std::vector<double>& max = maxRaw->getRawFitness().getValues();
                std::vector<double> scaling;
                for (unsigned int i = 0; i < getParameters().getGroupFitnessParametersCount(); ++i) {
                    if (upper[i] <= lower[i]) {
                        // Avoid division by zero
                        scaling.push_back(1);
                    } else {
                        scaling.push_back(std::exp(j * (max[i] - lower[i]) / (upper[i] - lower[i])));
                    }
                }
                ind->getFitness().setComponentWiseScaling(ScaledFitness::CONTRIBUTION_SCALING, scaling);
            }
        }
    }
}


