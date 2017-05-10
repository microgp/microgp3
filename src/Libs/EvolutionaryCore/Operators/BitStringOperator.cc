/***********************************************************************\
|                                                                       |
| BitStringOperator.cc |
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
 * @file BitStringOperator.cc
 *
 */

#include "BitStringOperator.h"
#include <IndividualPopulation.h>

#include <set>

using namespace ugp3::ctgraph;
using namespace ugp3::constraints;

namespace ugp3 {
namespace core {

void BitStringOperator::generate(const std::vector<Individual*>& parents,
                                 std::vector<Individual*>& outChildren,
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

    // select all the nodes that contain at least one bitarray parameter
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
    if(BitArrayParameter* parameter = dynamic_cast<BitArrayParameter*>(params[randomSample]))
    {
        // bitArray parameter
        LOG_VERBOSE << this << " : possible values for bitArray parameter (TODO)" << ends;
        
        // read the original value and transform it in a string of bits
        Tag& tag = cursor->getTag(CNode::Escape + parameter->getName());
        const string originalValueInBase = tag.getValue();
        LOG_DEBUG << this << " : the original value of the parameter is \"" << originalValueInBase << "\"" << ends;
        
        const string originalValue = Convert::toBitString( originalValueInBase, parameter->getBase() );   
        LOG_DEBUG << this << " : the bitstring for the original value \"" << tag.getValue() 
        << "\" of the parameter is \"" << originalValue << "\"" << ends;
        
        // start the children generation
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
        
        // Extract the modifiable part
        const string& pattern = parameter->getPattern();
        string modifiableBits;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i] == '-') {
                modifiableBits.push_back(originalValue[i]);
            }
        }
        do {
            mutateBitString(modifiableBits);
        } while(useSigma && Random::nextDouble() <= sigma);
        
        // Write back the modified part
        string newValue = pattern;
        for (size_t i = 0; i < pattern.size(); ++i) {
            if (pattern[i] == '-') {
                newValue[i] = modifiableBits[i];
            }
        }
        
        // Fail if no modification
        if (newValue == originalValue) {
            LOG_DEBUG << this << ": did not modify anything, failing." << std::ends;
            return;
        }
        
        Tag& childTag = childCursor->getTag(CNode::Escape + parameter->getName());
        childTag.setValue(Convert::fromBitString(newValue, parameter->getBase()));
        
        bool success = child->getGraphContainer().attachFloatingEdges();
        if (success)
        {
            outChildren.push_back(child.release());
            LOG_DEBUG << this << " : created child with value \"" << tag.getValue() << "\" derived from bitstring \"" << newValue << "\"" << ends;
        }
    }
    else
    {
        LOG_DEBUG << this << " : parameter " << params[randomSample]->toString() << " does not belong to the manageable parameters."  << ends;
        return;
    }
    
}

bool BitStringOperator::majority(const string& bits, size_t position, bool global) const
{
    size_t b, e;
    if (global || bits.size() <= 3) {
        b = 0;
        e = bits.size();
    } else {
        b = max<long>(0, min<long>(bits.size() - 3, static_cast<long>(position) - 1));
        e = max<long>(3, min<long>(bits.size(), static_cast<long>(position) + 2));
    }
    
    size_t nb_ones = std::count(begin(bits) + b, begin(bits) + e, '1');
    nb_ones *= 2;
    
    if (nb_ones > e - b) {
        return true;
    } else if (nb_ones < e - b) {
        return false;
    } else {
        return bits[position] == '1';
    }
}

void BitStringResetOneMutation::mutateBitString(string& bits) const
{
    auto position = Random::nextUInteger(0, bits.size() - 1);
    bits[position] = '0';
}

void BitStringSetOneMutation::mutateBitString(string& bits) const
{
    auto position = Random::nextUInteger(0, bits.size() - 1);
    bits[position] = '1';
}

void BitStringFlipOneMutation::mutateBitString(string& bits) const
{
    auto position = Random::nextUInteger(0, bits.size() - 1);
    if (bits[position] == '0') {
        bits[position] = '1';
    } else {
        bits[position] = '0';
    }
}

void BitStringGlobalMajorityMutation::mutateBitString(string& bits) const
{
    auto position = Random::nextUInteger(0, bits.size() - 1);
    bits[position] = majority(bits, position, true) ? '1' : '0';
}

void BitStringGlobalMinorityMutation::mutateBitString(string& bits) const
{
    auto position = Random::nextUInteger(0, bits.size() - 1);
    bits[position] = majority(bits, position, true) ? '0' : '1';
}

void BitStringLocalMajorityMutation::mutateBitString(string& bits) const
{
    auto position = Random::nextUInteger(0, bits.size() - 1);
    bits[position] = majority(bits, position, false) ? '1' : '0';
}

void BitStringLocalMinorityMutation::mutateBitString(string& bits) const
{
    auto position = Random::nextUInteger(0, bits.size() - 1);
    bits[position] = majority(bits, position, false) ? '0' : '1';
}

void BitStringFlipUniformMutation::mutateBitString(string& bits) const
{
    // Flip each bit with probability 1/length
    for (auto& bit: bits) {
        auto position = Random::nextUInteger(0, bits.size() - 1);
        if (position == 0) {
            if (bit == '0') {
                bit = '1';
            } else {
                bit = '0';
            }
        }
    }
}

void BitStringFlip1Mutation::mutateBitString(string& bits) const
{
    auto position = Random::nextUInteger(0, bits.size() - 1);
    if (bits[position] == '0') {
        bits[position] = '1';
    } else {
        bits[position] = '0';
    }
}

void BitStringFlip3Mutation::mutateBitString(string& bits) const
{
    std::set<long unsigned int> positions;
    // FIXME possibly inefficient for bits.size <= 3
    for (size_t i = 0; i < min<size_t>(bits.size(), 3); ++i) {
        do {
            auto position = Random::nextUInteger(0, bits.size() - 1);
            if (positions.insert(position).second) {
                if (bits[position] == '0') {
                    bits[position] = '1';
                } else {
                    bits[position] = '0';
                }
            }
        } while (positions.size() != i + 1);
    }
}

void BitStringFlip5Mutation::mutateBitString(string& bits) const
{
    std::set<long unsigned int> positions;
    // TODO factorize with before
    for (size_t i = 0; i < min<size_t>(bits.size(), 5); ++i) {
        do {
            auto position = Random::nextUInteger(0, bits.size() - 1);
            if (positions.insert(position).second) {
                if (bits[position] == '0') {
                    bits[position] = '1';
                } else {
                    bits[position] = '0';
                }
            }
        } while (positions.size() != i + 1);
    }
}

}
}
