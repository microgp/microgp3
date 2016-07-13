/***********************************************************************\
|                                                                       |
| GroupExternalInsertionMutationOperator.cc                             |
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
#include "Operators/GroupExternalInsertionMutation.h"

using namespace std;
using namespace ugp3::core;

GroupExternalInsertionMutationOperator::GroupExternalInsertionMutationOperator()
{
    this->selectorFileName="";
    this->numberOfIndividuals=0;
    this->fitnessHole=0.0;
}

Individual* GroupExternalInsertionMutationOperator::callExternalSelector(Group *g, std::vector<Individual*> inds) const
{
    string scriptCommandline = "";
    string output = "output";
    vector<string> inputFiles;
    
    string fileName = File::appendStringToName("externalGroup", g->getId());
    
    // generate the code from the group
    g->toCode(fileName, &inputFiles);
    
    // add the file name as a paremeter of the command line
    scriptCommandline = scriptCommandline + fileName;
    
    // set environment
    SETENV("UGP3_EXTERNAL_OPERATOR_OUTPUT_FILE", output );
    
    scriptCommandline = this->selectorFileName + " " + scriptCommandline;
    LOG_DEBUG << "Executing command \"" << scriptCommandline << "\" ... "<< ends;
    
    // call the external selector
    int returnValue = system(scriptCommandline.c_str());
    
    LOG_DEBUG
    << "The process \"" <<  this->selectorFileName
    << "\" exited with code " << returnValue  << ends;
    
    // retrieve the results
    if(File::exists(output) == false)
    {
        throw Exception("The external selector did not create the result file \"" + output + "\".", LOCATION);
    }
    
    ifstream resultFile;
    resultFile.open(output.c_str());
    if(resultFile.is_open() == false)
    {
        throw Exception("The file \"" + output + "\" is not accessible.", LOCATION);
    }
    
    LOG_DEBUG << "Loading selected individual from file \"" + output + "\" ..." << ends;
    if(resultFile.eof() == true)
    {
        throw Exception("Unexpected end-of-file reached.", LOCATION);
    }
    
    // get the line
    string line;
    getline(resultFile, line);
    istringstream lineStream(line);
    
    int value = 0;
    lineStream >> value;
    if(lineStream.fail() == true)
    {
        throw Exception("Bad result output format.", LOCATION);
    }
    
    if(value < 0) throw Exception("The selector produced a negative index value.", LOCATION);
    
    LOG_VERBOSE << "The individual selected is " << inds[value] << ends;
    
    resultFile.close();
    
    // clean up the temporary files
    if(File::remove(output) == false)
    {
        LOG_WARNING << "Could not delete file \"" + output + "\"" << ends;
    }
    
    g->removeIndividualsCodeFiles(File::appendStringToName("externalGroup", g->getId()));
    
    for(unsigned int i = 0; i < inputFiles.size(); i++)
    {
        File::remove(inputFiles[i]);
    }
    
    return inds[value];
}

void GroupExternalInsertionMutationOperator::generate( const vector<Group*> &parents, std::vector<Group*>& outChildren, GroupPopulation *population) const
{ 
    GroupPopulationParameters &parameters=population->getParameters();
    double sigma=parameters.getSigma();
    
    LOG_DEBUG << "Now entering GroupExternalInsertionMutationOperator::generate()" << ends;
    
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
    
    // check: if the number of individuals is 0 the operator can't do anything 
    if(this->numberOfIndividuals <= 0)
    {
        LOG_DEBUG << "Operator " << this->toString() << " fails: cannot make a selection without individuals (The number of individuals is "
        << this->numberOfIndividuals << ")" << ends;
        
        return;
    }
    
    LOG_DEBUG << "About to clone the parent, group " << parents[0] << ends;
    
    // clone the parent
    unique_ptr<Group> child(parent->clone());
    LOG_DEBUG << "Now adding individuals to group " << *child << ", child of group " << parent << ends;
    LOG_DEBUG << "Sigma is " << sigma << ends;
    
    bool stop = false;
    do
    {
        std::vector<Individual*>individuals = parameters.individualSelector(this->numberOfIndividuals, 1);
        
        GEIndividual *individual;
        if(Random::nextDouble(0,1) < this->fitnessHole)
        {
            //instead of calling the external selector, the highest fitness individual is chosen for the group
            individual = (GEIndividual*)*std::max_element(individuals.begin(), individuals.end(),
                                                          [&] (Individual* a, Individual* b) {
                                                              return !population->compareForSelection(a, b);
                                                          });
                                                          
        }
        else
        {
            //call the external selector
            individual=(GEIndividual*)this->callExternalSelector(child.get(),individuals);
        }
        
        LOG_DEBUG << "Individual " << individual << " selected!" << ends;
        
        if(child->addIndividual(individual))
        {
            individual->addGroup(child.get());
            LOG_DEBUG << "Insertion of individual " << child->getIndividuals()[child->getIndividualCount() - 1]
            << " in group " << *child << " completed successfully!" << ends;
            
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
    LOG_VERBOSE << this->getName() << ": created group " << *child << " from parent " <<  parent << ends;
    
    child->getLineage().set(this->getName(), parents);
    
    outChildren.push_back(child.release());
}

const string GroupExternalInsertionMutationOperator::getName() const
{
    return "groupExternalInsertionMutation";
}

const string GroupExternalInsertionMutationOperator::getAcronym() const
{
    return "G.E.INS.M";
}

const string GroupExternalInsertionMutationOperator::getDescription() const
{
    return 	"" + this->getName() + " operates only when the group evolution is enabled. It takes a group, and generated a "
    "child group by adding an individual that is chosen by an external evaluator. WARNING: THIS OPERATOR IS STILL "
    "EXPERIMENTAL";
}
