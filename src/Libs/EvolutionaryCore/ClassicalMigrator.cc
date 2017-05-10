/***********************************************************************\
|                                                                       |
| IMigrator.h                                                           |
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

#include "ugp3_config.h"
#include "EvolutionaryCore.h"

using namespace ugp3::core;
using namespace std;

const string ClassicalMigrator::XML_SCHEMA_TYPE = "classical";
const string ClassicalMigrator::XML_ATTRIBUTE_INTERVAL = "interval";
const string ClassicalMigrator::XML_ATTRIBUTE_SIZE = "migration";

ClassicalMigrator::ClassicalMigrator() :
interval(0),
size(0),
sizeMin(0),
sizeMax(0),
intervalMin(0),
intervalMax(0)
{ }

ClassicalMigrator::ClassicalMigrator(unsigned int size, unsigned int interval)
{
	this->size = size;
	this->interval = interval;
}

ClassicalMigrator::~ClassicalMigrator()
{
}

template <class PopType>
bool ClassicalMigrator::tryMigrate(std::vector<Population*>* populations)
{
    std::vector<PopType*> castPopulations;
    for (auto pop: *populations) {
        if (PopType* castPop = dynamic_cast<PopType*>(pop)) {
            castPopulations.push_back(castPop);
        } else {
            break;
        }
    }
    if (populations->size() == castPopulations.size()) {
        doMigrate(&castPopulations);
        return true;
    }
    return false;
}

template <class PopType>
void ClassicalMigrator::doMigrate(std::vector<PopType*>* populations)
{
    // obtain the step
    unsigned int step = populations->front()->getAlgorithm().getStep();       

    // check if the interval is correct
    if (step % this->interval != 0) return; 

    // if there are less than two population, migrating is useless
    if (populations->size() < 2) {
        LOG_WARNING << "A migration option has been specified, but there is only " << populations->size() << " population. Migration impossible" << ends;
        return;
    }
    
    // otherwise, select the best individual(s) from each population and clone them into other populations
    LOG_INFO << "Now migrating individuals between populations..." << ends;
    for(unsigned int p = 0; p < populations->size(); p++)
    {
        // for each other population, send a clone of the selected individual(s)
        for(unsigned int i = 0; i < size && i < populations->at(p)->getIndividualCount(); i++)
        {
            for(unsigned int p2 = 0; p2  < populations->size(); p2++)
            if( p != p2) // we don't want to put the individual in the island it comes from
            {
                // i-th best individual in source population
                const Individual& selected = populations->at(p)->getIndividual(i);

                // individual is cloned, but the reference is now the target population
                unique_ptr<Individual> selectedClone = selected.clone( * populations->at(p2) );

                // after cloning an individual, it is necessary to reattach all its edges
                if( selectedClone->getGraphContainer().attachFloatingEdges() == false) 
                {
                    LOG_ERROR   << "Attaching floating edges of individual " << *selectedClone 
                            << " was impossible. Aborting migrations..." << ends;
                    return;
                }
                
                LOG_DEBUG   << "Individual \"" << *selectedClone << "\" clone of parent \"" 
                        << selected << "\" created." << ends;

                // set lineage
                stringstream lineageString;
                vector<string> parentId;
                lineageString   << "Migrated from " << populations->at(p)->getName() 
                        << " at step " << step; // ends removed due to problems in status file

                parentId.push_back( selected.getId() );
                selectedClone->getLineage().set(lineageString.str(), parentId);

                // set new fitness for individual and invalidate fitness value
                LOG_VERBOSE     << "Individual \"" << *selectedClone << "\" old fitness size was "
                        << selectedClone->getFitness().getValues().size() << ends;

                Fitness fitness( populations->at(p2)->getParameters().getFitnessParametersCount() );
                selectedClone->setFitnessStructure( fitness );
                selectedClone->getFitness().invalidate();

                LOG_VERBOSE     << "Individual \"" << *selectedClone << "\" new fitness size is "
                        << selectedClone->getFitness().getValues().size() << ends;
                
                // these lines are just debug output to solve an issue
                EnhancedIndividual* ei = dynamic_cast<EnhancedIndividual*>( selectedClone.get() ); 
                if( ei != nullptr )
                {
                    LOG_DEBUG << "Getting there..." << ends;
                    LOG_VERBOSE     << "Individual \"" << *selectedClone << "\" ScaledFitness size is "
                            << ei->getFitness().getValues().size() << ends;
                }


                LOG_INFO << "Individual " << *selectedClone << " clone of individual " << selected 
                     << " will be migrated from " << populations->at(p)->getName() << " to " 
                     << populations->at(p2)->getName() << ends;

                // check if the target population has reached a stop condition
                if( populations->at(p2)->checkStopCondition() == true )
                {
                    LOG_WARNING     << "Population \"" << populations->at(p2)->getName() 
                            << "\" has reached stop condition. Migration impossible. " << ends;
                    return;
                }

                // the individual may be already part of the target population(?): so, we have to catch the exception
                try
                {
                    populations->at(p2)->addIndividual(std::move(selectedClone));
                }
                catch (const Exception& e)
                {
                    LOG_WARNING << e.what() << ends;
                }
            }
        }
    }   
}

void ClassicalMigrator::migrate(vector<Population*>* populations)
{
    if (tryMigrate<EnhancedPopulation>(populations))
        return;
    if (tryMigrate<MOPopulation>(populations))
        return;
    /* TODO specialize template doMigrate to preserve groups during migration
     * or even better: add functions to transfer a candidate from one population to
     * another (more complex than just moving a pointer for groups: need to 
     * recursively move all members.
     * 
     * if (tryMigrate<GroupPopulation>(populations))
     *     return;
     */
} 
