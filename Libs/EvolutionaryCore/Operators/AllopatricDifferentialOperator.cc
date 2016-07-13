/***********************************************************************\
|                                                                       |
| AllopatricDifferentialOperator.cc                                     |
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
#include "Operators/AllopatricDifferentialOperator.h"
#include "FloatParameter.h"
#include "EvolutionaryCore.h"


using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace tgraph;
using namespace std;
using namespace ugp3::constraints;


AllopatricDifferentialOperator::AllopatricDifferentialOperator()
{ }

void AllopatricDifferentialOperator::apply(Population& unkownPop,
                                           std::vector< CandidateSolution* >& newGeneration) const
{
    _STACK;
    
    IndividualPopulation& indPop = dynamic_cast<IndividualPopulation&>(unkownPop);
    IndividualPopulationParameters& parameters = indPop.getParameters();
    
    std::vector<Individual*> population = parameters.individualSelector(parameters.getMu(), 0);
    
    if (population.size() != parameters.getMu()) {
        LOG_WARNING << "Operator " << getName() << " failed because it could not select enough parents ("
        << population.size() << " returned vs " << parameters.getMu() << " requested)" << std::ends;
        return;
    }
    
    double differential_weight = parameters.getDiffEvoDW();
    double crossover_probability = parameters.getDiffEvoCP();
    
    
    for (unsigned int index = 0; index < parameters.getMu(); index++){
        
        std::vector<Individual*>agents;
        bool diversity = true;
        do{
            agents = parameters.individualSelector( this->getParentsCardinality(), 0);
            diversity = true;
            for (unsigned int index2 = 0; index2 < this->getParentsCardinality(); index2++){
                if (agents[index2] == population[index]){
                    diversity = false;
                }
            }
        }while(diversity == false);
        
        
        // clone the parent
        Individual* selected_parent = population[index];
        Assert(selected_parent->getGraphContainer().validate() == true);
        
        unique_ptr<Individual> child = selected_parent->clone();
        LOG_VERBOSE << this << ": created individual " << child->toString() << " from parent " <<  selected_parent << ends;
        
        // set the lineage for the new individual
        child->getLineage().set(this->getName(), agents);
        
        if(child->getGraphContainer().attachFloatingEdges() == false) return;
        
        //TO FIX
        //child->setAllopatricTag(population[index]->getAllopatricTag());
        
        std::vector<Individual*> differential_agents;
        
        
        differential_agents.push_back(agents[0]);
        differential_agents.push_back(agents[1]);
        differential_agents.push_back( &(*child) );
        differential_agents.push_back( agents[2] );
        
        
        
        for (unsigned int a = 0; a < this->getParentsCardinality(); a++){
            if (differential_agents[a]->getGraphContainer().getCGraphCount()  != differential_agents[a + 1]->getGraphContainer().getCGraphCount() )
                return;
        }
        
        bool add;
        vector<bool> graph_add( differential_agents[0]->getGraphContainer().getCGraphCount() );
        
        for (unsigned int b = 0; b < differential_agents[0]->getGraphContainer().getCGraphCount(); b++){
            graph_add[b] = false;
        }
        
        for (unsigned int x = 0; x < differential_agents[0]->getGraphContainer().getCGraphCount(); x++){
            add = true;
            for (unsigned int c = 0; c < this->getParentsCardinality(); c++){
                CGraph& graph1 = differential_agents[c]->getGraphContainer().getCGraph(x);
                CGraph& graph2 = differential_agents[c+1]->getGraphContainer().getCGraph(x);
                //compare sections
                if (graph1.getSubGraphCount() != graph2.getSubGraphCount() ){
                    add = false;
                }
                else{
                    for(unsigned int y=0; y < graph1.getSubGraphCount(); y++){
                        if (graph1.getSubGraph(y).getSize() != graph2.getSubGraph(y).getSize()){
                            add = false;
                        }
                        else{
                            for(unsigned int z=0; z < graph1.getSubGraph(y).getSize(); z++){
                                Slice& slice1 =  graph1.getSubGraph(y).getSlice();
                                Slice& slice2 =  graph2.getSubGraph(y).getSlice();
                                if (slice1.getSize() != slice2.getSize() ){
                                    add = false;
                                }
                                else{
                                    for(unsigned int d=0; d < slice1.getSize(); d++){
                                        if (slice1.getNode(d).getGenericMacro().getParameterCount() != slice2.getNode(d).getGenericMacro().getParameterCount()){
                                            add = false;
                                        }
                                        else{
                                            for (unsigned int e=0; e < slice1.getNode(d).getGenericMacro().getParameterCount(); e++){
                                                if (slice1.getNode(d).getGenericMacro().getParameter(e).getName().compare(slice2.getNode(d).getGenericMacro().getParameter(e).getName()) != 0)
                                                    add = false;
                                            }
                                        }
                                    }     
                                }
                            }
                        }
                    }
                }
            }
            if (add == true){
                graph_add[x] = true;
            }
        }
        
        //save all the floating point parameters
        vector< vector<FloatParameter*> > params(this->getParentsCardinality() + 1);
        vector< vector<CNode*> > params_node(this->getParentsCardinality() + 1);
        
        for (unsigned int x = 0; x < this->getParentsCardinality() + 1; x++){
            //gets the graph container of the individual
            ugp3::ctgraph::CGraphContainer& graphContainer = differential_agents[x]->getGraphContainer();
            //collect all the nodes of the all graphs
            for (unsigned int i = 0; i < graphContainer.getCGraphCount(); i++){
                if (graph_add[i] == true){
                    ugp3::ctgraph::CGraph& graph = graphContainer.getCGraph(i); 
                    for (unsigned int j = 0; j < graph.getSubGraphCount(); j++){
                        ugp3::ctgraph::CSubGraph& subGraph = graph.getSubGraph(j);
                        CNode* node = &subGraph.getPrologue();
                        do{
                            for(unsigned int k = 0; k < node->getGenericMacro().getParameterCount(); k++){
                                FloatParameter* parameter = dynamic_cast<FloatParameter*>(&node->getGenericMacro().getParameter(k));
                                if(parameter != nullptr){
                                    params[x].push_back(parameter);
                                    params_node[x].push_back(node);
                                }
                            }
                        }while((node = node->getNext()) != nullptr);
                        
                    }
                }
            }
        }
        
        
        unsigned long randomSample = Random::nextUInteger(0, (unsigned long)(params[0].size() - 1));
        
        
        bool change = false;
        
        for (unsigned int i = 0; i < params[0].size(); i++){
            change = false;
            if (i == randomSample){	
                change = true;
            }
            else{
                double r = Random::nextDouble(0, 1);
                if ( r < crossover_probability){
                    change = true;
                }
            }
            if (change == true){
                Tag& tag1 = params_node[0][i]->getTag(CNode::Escape + params[0][i]->getName());
                double a = Convert::toDouble(tag1.getValue());
                Tag& tag2 = params_node[1][i]->getTag(CNode::Escape + params[1][i]->getName());
                double b = Convert::toDouble(tag2.getValue());
                Tag& tag3 = params_node[2][i]->getTag(CNode::Escape + params[2][i]->getName());
                Tag& tag4 = params_node[3][i]->getTag(CNode::Escape + params[3][i]->getName());
                double best_param = Convert::toDouble(tag4.getValue());
                tag3.setValue(Convert::toString( best_param + differential_weight * (a - b)));
            }
        }
        
        
        if (child->validate() != false){
            newGeneration.push_back(child.release());
        }
        
    }
    
    LOG_VERBOSE << this << ": succeeded" << ends;
    return;
}


const string AllopatricDifferentialOperator::getName() const
{
    return "allopatricDifferential";
}

const string AllopatricDifferentialOperator::getAcronym() const
{
    return "ADO.D";
}

const string AllopatricDifferentialOperator::getDescription() const
{
    return 	"" + this->getName() + " acts mimics the Differential Evolution paradigm. It generates a single child individual."
    "The operator picks three random individuals A,B,C (other than the parent) with a tournament selection, "
    "finds the vector that goes from B to C, and applies the vector to A, creating the temporary individual Z. "
    "The resulting child is the one-point crossover between individual Z and the original parent. It is important to "
    "notice that this operator only works with INDIVIDUALS OF FIXED LENGTH, and only with floating-point parameters. "
    "your individuals can present a mixture of non-float and float parameters, but they MUST HAVE at least one "
    "floating-point parameter. WARNING: THIS OPERATOR IS EXPERIMENTAL AND UNTESTED.";
}
