/***********************************************************************\
|                                                                       |
| GroupRandomInsertionMutationOperator.cc                               |
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

#include "EvolutionaryCore.h"
#include "Operators/GroupRandomInsertionMutation.h"

using namespace std;
using namespace ugp3::core;


GroupRandomInsertionMutationOperator::GroupRandomInsertionMutationOperator()
{ }

void GroupRandomInsertionMutationOperator::generate( const vector<Group*> &parents, std::vector<Group*>& outChildren, GroupPopulation *population) const
{ 
    const GroupPopulationParameters &parameters=population->getParameters();
    double sigma=parameters.getSigma();

    LOG_DEBUG << "Now entering GroupRandomInsertionMutationOperator::generate()" << ends;

    if(sigma <= 0 || sigma >= 1)
    {
        throw ArgumentException("sigma should be in (0, 1)", LOCATION);
    }

    if(parents.size() != this->getParentsCardinality())
    {
        throw ArgumentException("The number of input arguments (parents count) for the genetic operator " + this->toString() + " is incorrect.", LOCATION);
    }

    // there is only one parent
    Group* parent = parents[0];

    // check: if the group chosen as parent has already the maximum size, then the operator fails
    if(parent->getIndividualCount() == parameters.getMaxIndividualsPerGroup())
    {
        LOG_DEBUG << "Operator " << this->toString() << " fails: cannot add individuals to a group with "
                  << parent->getIndividualCount() << " individuals (Maximum size is " << parameters.getMaxIndividualsPerGroup() << ")" << ends;

        return;
    }

    LOG_DEBUG << "About to clone the parent, group " << *parents[0] << ends;

    // clone the parent
    unique_ptr<Group> child(parent->clone());
    LOG_DEBUG << "Now adding individuals to group " << *child << ", child of group " << *parent << ends;
    LOG_DEBUG << "Sigma is " << sigma << ends;

    bool stop = false;
    do
      {
    
        const std::vector<Individual*>& selected = parameters.individualSelector(1, 0.0);
        Assert(!selected.empty());
        GEIndividual* individual = dynamic_cast<GEIndividual*>(selected.front());
        Assert(individual);
    
      LOG_DEBUG << "Individual " << *individual << " selected!" << ends;

      if(child->addIndividual(individual))
        {
        individual->addGroup(child.get());
        LOG_DEBUG << "Insertion of individual " << *child->getIndividuals()[child->getIndividualCount() - 1] << " in group " << *child << " completed successfully!" << ends;

        if(child->getIndividualCount()>=parameters.getMaxIndividualsPerGroup())
          {
          LOG_DEBUG << "Reached the maximum number of individuals for the groups" << ends;
          stop=true;
          }
        }

      if(!stop)
        {
        if(sigma <= Random::nextDouble())
          stop=true;
        }

      }
    while(!stop);

    LOG_DEBUG << "Size of the child is now " << child->getIndividualCount() << ends;
    LOG_VERBOSE << this->getName() << ": created group " << *child << " from parent " <<  *parent << ends;

    child->getLineage().set(this->getName(), parents);


    outChildren.push_back(child.release());
}

const string GroupRandomInsertionMutationOperator::getName() const
{
    return "groupRandomInsertionMutation";
}

const string GroupRandomInsertionMutationOperator::getAcronym() const
{
    return "G.R.INS.M";
}

const string GroupRandomInsertionMutationOperator::getDescription() const
{
	return 	"" + this->getName() + " operates only when the group evolution is enabled. It takes a group, and creates a "
		"child group by adding a random individual. Then, a random value [0,1] is generated. If the value is lower than "
		"sigma, the process is repeated.";
}
