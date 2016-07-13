/***********************************************************************\
|                                                                       |
| ScanMutationBitArrayOperator.cc                                               |
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
#include "Operators/ScanMutationBitArray.h"
using namespace std;
using namespace ugp3::core;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;


ScanMutationBitArrayOperator::ScanMutationBitArrayOperator()
{ }

// small utility function to generate all combinations of unique k unsigned integers among n elements
// it is used in ScanMutationBitArray::generate() for parameters of type bitArray
void recursive_combination(vector<unsigned int>::iterator nbegin, vector<unsigned int>::iterator nend, int n_column,
    vector<unsigned int>::iterator rbegin, vector<unsigned int>::iterator rend, int r_column, int loop, vector< vector<unsigned int> >* v)
{
  int r_size=rend-rbegin;

  int localloop=loop;
  int local_n_column=n_column;

  //A different combination is out
  if(r_column>(r_size-1))
  {
    vector<unsigned int> temp;
    vector<unsigned int>::iterator cursor = rbegin;

    while(cursor != rend)
    {
	temp.push_back(*cursor);
	++cursor;
    }
    
    v->push_back(temp);

    return;
  }
  //===========================

  for(int i=0;i<=loop;++i)
  {
    vector<unsigned int>::iterator it1=rbegin;
    for(int cnt=0;cnt<r_column;++cnt)
    {
      ++it1;
    } 
    vector<unsigned int>::iterator it2=nbegin;
    for(int cnt2=0;cnt2<n_column+i;++cnt2)
    {
      ++it2;
    } 

    *it1=*it2;

    ++local_n_column;

    recursive_combination(nbegin,nend,local_n_column,
      rbegin,rend,r_column+1,localloop,v);
      
    --localloop;
  }
}

// sort of wrapper function for the recursive_combinations() function above
vector< vector<unsigned int> > getAllCombinations(const string& pattern, unsigned int k)
{
	// vector of vector of integers to return
	vector< vector<unsigned int> > toReturn;
	
	// the maximum Hamming distance is also limited by the pattern of the bitArray parameter;
	// if a bitArray parameter has a pattern of length 5, but with only 2 "-" (free values), then the maximum
	// Hamming distance of valid children will be 2
	vector<unsigned int> vn;
	vector<unsigned int> vk;
	for(unsigned int i = 0; i < pattern.length(); i++) if( pattern[i] == '-' ) vn.push_back(i);
	for(unsigned int i = 0; i < k; i++) if( pattern[i] == '-' ) vk.push_back(i);
	
	recursive_combination(vn.begin(), vn.end(), 0, vk.begin(), vk.end(), 0, vn.size()-k, &toReturn);

	return toReturn;
}

// scanMutation is currently working with:
// - bitArray (generate one child per iteration, increasing the Hamming distance) 
// - definedType (generate one child for each possible value)
// - inner(Forward|Backward)Label (generate one child for each possible target for the jump)
// - ranged integers and float (generate several children, sampling the range, two for each iteration on sigma)
// - outerLabel (generate one child for each instance of subsection it can jump to)
void ScanMutationBitArrayOperator::generate(const std::vector< Individual* >& parents,
                          std::vector< Individual* >& outChildren,
                          IndividualPopulation& population) const
{
	_STACK;

	double sigma = population.getParameters().getSigma();

	// there is only one parent
	Individual& parent = *parents[0];
	Assert(parent.validate() == true);

	// select the target (/graph/subGraph/macro/parameter)
	OperatorToolbox toolbox(parent.getGraphContainer());

	// select a graph
	CGraph* graph =  toolbox.getRandomGraph();
	if(graph == nullptr)
	{
		LOG_INFO << "No graph selected" << ends;
		return;
	}
	const string& sectionName = graph->getSection().getId();

	// select a subgraph
	CSubGraph* subGraph = toolbox.getRandomSubGraph(*graph);
	if(subGraph == nullptr)
	{
		LOG_INFO << "No subGraph selected" << ends;
		return;
	}

	// find index of chosen subgraph (used later to find the corresponding part in the children)
	unsigned int subGraphIndex = 0;
	for(unsigned int i = 0; i < graph->getSubGraphCount(); i++)
	{
		if(&graph->getSubGraph(i) == subGraph)
		{
			subGraphIndex = i;
		}
	}

	// select all the nodes that contain at least one integer parameter
	vector<CNode*> candidates;
	CNode* node = &subGraph->getPrologue();
	do
	{
		candidates.push_back(node);
    	}
	while((node = node->getNext()) != nullptr);

	// collect all the parameters in a random node (until at least one parameter is found)
	vector<Parameter*> params;
	do
	{
		// choose random node
		unsigned long randomSample = Random::nextUInteger(0, (unsigned long)(candidates.size() - 1));
		Assert(randomSample <= candidates.size() - 1);
		node = candidates[randomSample];
		
		// remove node from candidates
		candidates.erase(candidates.begin() + randomSample);
		params.clear();

		// collect all the available parameters for the selected macro
		for(unsigned int i = 0; i < node->getGenericMacro().getParameterCount(); i++)
		{
			// the only class of parameters usable is: 
			// - BitArrayParameter
			if( dynamic_cast<BitArrayParameter*>(&node->getGenericMacro().getParameter(i)) != nullptr )
				params.push_back(&node->getGenericMacro().getParameter(i));
		}
	}
	while(params.size() == 0 && candidates.size() > 0);
	
	// if no parameters are found, return
	if( params.size() == 0 )
	{
		LOG_DEBUG << this << " : no appropriate parameters found" << ends;
		return;
	}

	// get the index of the selected node (used later to find the corresponding part in the children)
	unsigned int nodeIndex = 0;
	CNode* cursor = &subGraph->getPrologue();
	while(cursor != nullptr && cursor != node)
	{
		cursor = cursor->getNext();
		nodeIndex++;
	}

	// now, choose randomly the parameter to operate on and behave accordingly
	unsigned long randomSample = Random::nextUInteger(0, (unsigned long)(params.size() - 1));
	Assert(randomSample <= params.size() - 1);

	// first of all, the allopatric tag shared by all children individuals is chosen
	string allopatricTag = Individual::allopatricTagCounter.toString();
	Individual::allopatricTagCounter++;
	LOG_DEBUG << "All children produced by " << this << " will share the allopatric tag \"" << allopatricTag << "\"" << ends;

	// Terse output!
    	std::ostringstream terse;
	if( dynamic_cast<BitArrayParameter*>(params[randomSample]) != nullptr )
	{
		// bitArray parameter
		BitArrayParameter* parameter = dynamic_cast<BitArrayParameter*>(params[randomSample]);
		LOG_VERBOSE << this << " : possible values for bitArray parameter (TODO)" << ends;
	
		// read the original value and transform it in a string of bits
		Tag& tag = cursor->getTag(CNode::Escape + parameter->getName());
		const string originalValueInBase = tag.getValue();
		LOG_DEBUG << this << " : the original value of the parameter is \"" << originalValueInBase << "\"" << ends;
		
		string originalValue = Convert::toBitString( originalValueInBase, parameter->getBase() );	
		LOG_DEBUG << this << " : the bitstring for the original value \"" << tag.getValue() 
			  << "\" of the parameter is \"" << originalValue << "\"" << ends;

		// the Hamming distance is a measure of distance between two strings of bits. 
		// basically, if they differ for the value of one bit, the distance is 1; 
		// if they differ for the value of 2 bits, the distance is 2; and so on.
		// since individuals will be generated increasing the Hamming distance, we need a maximum distance, 
		// that is intuitively the number of bits in the bitArray parameter; 
		// for example, 00000 and 11111 have a Hamming distance of 5 (the maximum for 5-bit strings)

		// the maximum Hamming distance is also limited by the pattern of the bitArray parameter;
		// if a bitArray parameter has a pattern of length 5, but with only 2 "-" (free values), then the maximum
		// Hamming distance of valid children will be 2
		unsigned int maxHammingDistance = 0;
		for(unsigned int i = 0; i < parameter->getPattern().length(); i++) if( parameter->getPattern().at(i) == '-') maxHammingDistance++;
		
		unsigned int hammingDistance = 1;

		vector< vector<unsigned int> > combinations = getAllCombinations(parameter->getPattern(), hammingDistance); 
		LOG_DEBUG << this << " : for Hamming distance = " << hammingDistance << ", a maximum of " << combinations.size() << " individuals will be produced" << ends;

		// start the children generation
		unsigned int c = 0;
		do
		{
			LOG_DEBUG << this << " : Hamming distance = " << hammingDistance << ", now using combination of indexes \"";
			for(unsigned int i = 0; i < hammingDistance; i++) LOG_DEBUG << " " << combinations[c][i];
			LOG_DEBUG << "\"" << ends;

			// produce new individual
			// clone the parent
			unique_ptr<Individual> child = parent.clone();
           		terse << " " << child->toString();
            		LOG_DEBUG << this->getName() << ": created individual " << child->toString() << " from parent " <<  parent << ends;

			// set the lineage for the new individual
			child->getLineage().set(this->getName(), parents);

			// set allopatric tag
			child->setAllopatricTag(allopatricTag);
			LOG_DEBUG << "Individual " << child->toString() << " has now allopatric tag " << child->getAllopatricTag() << ends;

			// find corresponding parameter in the child
			CGraph* childGraph = child->getGraphContainer().getCGraph(sectionName);
			CSubGraph& childSubGraph = childGraph->getSubGraph(subGraphIndex);
			CNode* childCursor = &childSubGraph.getPrologue();
			unsigned int childNodeIndex = 0;
			while(childCursor != nullptr && childNodeIndex != nodeIndex)
			{
			    childCursor = childCursor->getNext();
			    childNodeIndex++;
			}

			// some asserts, then change the value
			assert(childCursor->getGenericMacro().getParameter(parameter->getName()) != nullptr);
			assert(childCursor->containsTag(CNode::Escape + parameter->getName()) == true);
			
			// changing the value is easy: following the indexes stored in the current combination,
			// access the string at index i and flip the character (0 -> 1, 1 -> 0)
			Tag& tag = childCursor->getTag(CNode::Escape + parameter->getName());
			string newValue = originalValue;
			
			for(unsigned int i = 0; i < combinations[c].size(); i++)
			{
				if( newValue[ combinations[c][i] ] == '0' )
					newValue[ combinations[c][i] ] = '1';
				else
					newValue[ combinations[c][i] ] = '0';
			}
			
			tag.setValue( Convert::fromBitString(newValue, parameter->getBase()) );

			bool success = child->getGraphContainer().attachFloatingEdges();
			if(success == true)
			{
			    outChildren.push_back(child.release());
			    LOG_DEBUG << this << " : created child with value \"" << tag.getValue() << "\" derived from bitstring \"" << newValue << "\"" << ends;
			}

			// if all the combinations have been used
			c++;
			if( c == combinations.size() )
			{
				c = 0;
				hammingDistance++;
				if( hammingDistance <= maxHammingDistance )
				{
					combinations = getAllCombinations(parameter->getPattern(), hammingDistance); 
					LOG_DEBUG << this << " : for Hamming distance = " << hammingDistance << ", a maximum of " 
						  << combinations.size() << " individuals will be produced" << ends;
				}
			}

		}
        	while(Random::nextDouble() <= sigma && hammingDistance <= maxHammingDistance);
	}
	else
	{
		LOG_DEBUG << this << " : parameter " << params[randomSample]->toString() << " does not belong to the manageable parameters."  << ends;
		return;
	}

    LOG_VERBOSE << this->getName() << ": created individuals" << terse.str() << " from parent " <<  parent << ends;
}	

const string ScanMutationBitArrayOperator::getName() const
{
    return "scanMutationBITARRAY";
}

const string ScanMutationBitArrayOperator::getAcronym() const
{
    return "SCBA.M";
}

const string ScanMutationBitArrayOperator::getDescription() const
{
	return "" + this->getName() + " performs a local quasi-exhaustive search in the proximity of the parent individual. One parameter of type \"bitArray\" is chosen at random in the individual, and several children are produced with values of the parameter close to the parent's. The number of individuals generated depends on sigma: every time a new individual is produced, a random value [0,1] is generated, and if the value is less than the current sigma, the process is repeated. At each iteration, an individual at a certain Hamming distance from the parent is created. Once all individuals at a certain Hamming distance have been created, the Hamming distance is increased. All individuals produced will share the same allopatric tag. This means that (at the most) ONE of the offspring will survive in the slaughtering step."; 
}
