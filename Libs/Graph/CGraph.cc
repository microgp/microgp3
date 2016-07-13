/***********************************************************************\
|                                                                       |
| CGraph.cc                                                             |
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
#include "ConstrainedTaggedGraph.h"
#include <Entropy.h>

using namespace std;
using namespace ugp3;
using namespace ugp3::constraints;
using namespace ctgraph;
using namespace tgraph;

CGraph::CGraph()
    : parentContainer(nullptr)
{ }

CGraph::CGraph(const CGraph&)
    : CGraph()
{
    //Do not use the copy constructor of CGraph: use CGraph::clone instead.
    Assert(false);
}

CGraph::CGraph(IContainer<CGraph>& parentContainer)
	: parentContainer(&parentContainer)
{ }

void CGraph::buildRandom()
{
    _STACK;

	Assert(this->getConstrain() != nullptr);

	this->clear();

	const Section& section = (const Section&)*this->getConstrain();

    LOG_DEBUG << "Creating a new Constrained Tagged Graph from section " << section << ends;

    LOG_DEBUG << "Building section prologue " << section.getPrologue() << ends;
    this->prologue = unique_ptr<CNode>(new CNode(*this));
	this->prologue->setConstrain(section.getPrologue());
	this->prologue->buildRandom();

    LOG_DEBUG << "Building section prologue " << section.getEpilogue() << ends;
    this->epilogue = unique_ptr<CNode>(new CNode(*this));
	this->epilogue->setConstrain(section.getEpilogue());
	this->epilogue->buildRandom();

    for( unsigned int i = 0; i < section.getSubSectionCount(); i++ )
    {
        const SubSection& subSection = section.getSubSection(i);
        unsigned long subSectionInstances = Random::nextUInteger(
            subSection.getMinimumInstances(),
            subSection.getMaximumInstances());

        for(unsigned int s = 0; s < subSectionInstances; s++)
        {
            CSubGraph* subGraph = new CSubGraph(*this);
			subGraph->setConstrain(subSection);
			subGraph->buildRandom();

            this->subGraphs.push_back(subGraph);
        }
    }

	/* DO NOT attach floating edges here */

    LOG_DEBUG << "Constrained Tagged Graph " << section << " built successfully" << ends;
}

unique_ptr<CGraph> CGraph::clone() const
{
    _STACK;

	LOG_DEBUG << "Graph " << this << ": cloning ..." << ends;
	Assert(parentContainer != nullptr);


    // create a new graph
    CGraph* graph = new CGraph();
    graph->parentContainer = nullptr;

	if(this->getConstrain() != nullptr)
		graph->setConstrain(*this->getConstrain());

    // copy global prologue
    graph->prologue = this->prologue->clone();
    NodeContainer::setAsParent(graph->prologue.get(), graph);


    // copy global epilogue
    graph->epilogue  = this->epilogue->clone();
    NodeContainer::setAsParent(graph->epilogue.get(), graph);


    // copy subgraphs
    for(unsigned int i = 0; i < this->subGraphs.size(); i++)
    {
        unique_ptr<CSubGraph> subGraph = this->subGraphs[i]->clone();
        this->setAsParent(subGraph.get(), graph);
        graph->subGraphs.push_back(subGraph.release());
    }

    // DO NOT restore floating edges

	LOG_DEBUG << "Graph " << this << ": clone " << graph << " created." << endl << ends;

    // return the new graph
    return unique_ptr<CGraph>(graph);
}

bool CGraph::contains(const CSubGraph& subGraph) const
{
    for(unsigned int i = 0; i < this->subGraphs.size(); i++)
    {
        if(this->subGraphs[i] == &subGraph)
        {
            return true;
        }
    }

    return false;
}

vector<CSubGraph*> CGraph::getSubGraphs(const string& subSectionName) const
{
    _STACK;

    LOG_DEBUG 	<< "Now looking for all subgraphs with id=\""
		<< subSectionName << "\" inside graph " << this << ends;

    vector<CSubGraph*> candidateList;

    for(unsigned int i = 0; i < this->subGraphs.size(); i++)
    {
        CSubGraph* subGraph = this->subGraphs[i];

        if(subGraph->getSubSection().getId() == subSectionName)
        {
            candidateList.push_back(subGraph);
        }
    }
	
    LOG_DEBUG 	<< "There are " << candidateList.size() << " instances of subgraph \"" 
		<< subSectionName << "\"" << ends;

    return candidateList;
}

bool CGraph::contains(const CNode& node) const
{
    if(this->prologue.get() == &node) return true;

    if(this->epilogue.get() == &node) return true;

    for(unsigned int i = 0; i < this->subGraphs.size(); i++)
    {
        if(this->subGraphs[i]->contains(node) == true)
        {
            return true;
        }
    }

    return false;
}

bool CGraph::attachFloatingEdges()
{
    _STACK;

    LOG_DEBUG << "Graph " << this << ": attaching floating edges" << ends;
    for(unsigned int i = 0; i < this->subGraphs.size(); i++)
    {
        bool success = this->subGraphs[i]->attachFloatingEdges();
        if(success == false)
        {
            return false;
        }
    }

    // do not attach outer labels: let the parent container do that
    LOG_DEBUG << "Graph " << this << ": floating edges restored" << ends;
    return true;
}

IContainer<CGraph>* CGraph::getParentContainer() const
{
    return this->parentContainer;
}

CSubGraph& CGraph::getSubGraph(unsigned int index) const
{
    _STACK;

	Assert(this->subGraphs[index] != nullptr);
    return *this->subGraphs.at(index);
}

bool CGraph::insertSubGraph(const SubSection& subSection)
{
    _STACK;

	if(this->getConstrain() != nullptr)
	{
	    Assert(dynamic_cast<const Section*>(this->getConstrain()) != nullptr);
		const Section& section = (const Section&)*this->getConstrain();

		// check if the specified sub-section belongs to this section
		Assert(section.getSubSection(subSection.getId()) != nullptr);

		size_t subSectionInstances = this->getSubGraphs(subSection.getId()).size();

        Assert(subSectionInstances >= subSection.getMinimumInstances());
        Assert(subSectionInstances <= subSection.getMaximumInstances());

        // check if the maximum number of instances for the subsection has already been reached
		if(subSectionInstances == subSection.getMaximumInstances())
		// cannot add other subSections
		{
			return false;
		}
	}

    CSubGraph* subGraph = new CSubGraph(*this);
	subGraph->setConstrain(subSection);
	subGraph->buildRandom();

    this->subGraphs.push_back(subGraph);

    return true;
}

CGraph::~CGraph()
{
    _STACK;

    for(unsigned int i = 0; i < this->subGraphs.size(); i++)
    {
        delete this->subGraphs[i];
    }
}


bool CGraph::validate() const
{
    _STACK;

	if(this->getConstrain() == nullptr)
	{
		LOG_WARNING << "Nothing to validate" << ends;
		return true;
	}

    if(this->prologue.get() == nullptr || this->epilogue.get() == nullptr)
    {
		LOG_WARNING << "Prologue and/or epilogue are null" << ends;
        return false;
    }

    if(this->prologue->validate() == false)
    {
		LOG_WARNING << "Prologue is not valid" << ends;
        return false;
    }

    if(this->epilogue->validate() == false)
    {
		LOG_WARNING << "Epilogue is not valid" << ends;
        return false;
    }

    if(this->prologue->representsPrologue() == false || this->epilogue->representsEpilogue() == false)
    {
        return false;
    }

    for(unsigned int i = 0; i < this->subGraphs.size(); i++)
    {
        if(this->subGraphs[i]->validate() == false)
        {
            return false;
        }
    }

    // check if every SubSection has a coherent number of instances
	const Section& section = (const Section&) *this->getConstrain();
	 for(unsigned int i = 0; i < section.getSubSectionCount(); i++)
	 {
		const SubSection& subSection = section.getSubSection(i);

		std::size_t subSectionInstances = this->getSubGraphs(subSection.getId()).size();

		if(subSectionInstances > subSection.getMaximumInstances()
        || subSectionInstances < subSection.getMinimumInstances())
		{
			LOG_WARNING << "The number of sub-sections is not valid ("
				<< subSectionInstances<< " instead of "
				<< subSection.getMinimumInstances() << "-"
				<< subSection.getMaximumInstances() << ")" << ends;

			return false;
		}
	 }

    return true;
}

void CGraph::clear()
{
	this->prologue = unique_ptr<CNode>();
	this->epilogue = unique_ptr<CNode>();

	this->subGraphs.clear();
}

CNode* CGraph::getNode(const string& id) const
{
    _STACK;

    for(unsigned int i = 0; i < this->subGraphs.size(); i++)
    {
        CNode* node = this->subGraphs[i]->getNode(id);

        if(node != nullptr)
        {
            return node;
        }
    }

    if(this->prologue->getId() == id)
    {
        return this->prologue.get();
    }

    if(this->epilogue->getId() == id)
    {
        return this->epilogue.get();
    }

    return nullptr;
}

void CGraph::writeExternalRepresentation(ostream& stream, Relabeller& relabeller) const
{
    _STACK;

    LOG_DEBUG << "Requested a dump for the CGraph " << this << ends;

    unsigned int maximumSubGraphSize = 0;
    for(unsigned int i = 0; i < this->subGraphs.size(); i++)
    {
        if(this->subGraphs[i]->getSize() > maximumSubGraphSize)
        {
            maximumSubGraphSize = this->subGraphs[i]->getSize();
        }
    }

	if(this->getConstrain() != nullptr)
	{
		const Section& section = (const Section&) *this->getConstrain();

		if((this->subGraphs.size() == 0 || maximumSubGraphSize <= 2)
			&& section.getIsCompulsoryPrologueEpilogue() == false)
		{
			return;
		}
	}

    Assert(prologue.get() != nullptr);
    LOG_DEBUG << "Dumping the section prologue " << prologue->toString() << ends;
    prologue->writeExternalRepresentation(stream, relabeller);

    for(unsigned int i = 0; i < this->subGraphs.size(); i++)
    {
        this->subGraphs[i]->writeExternalRepresentation(stream, relabeller);
    }

     Assert(epilogue.get() != nullptr);
     LOG_DEBUG << "Dumping the section epilogue " << epilogue->toString() << ends;
     epilogue->writeExternalRepresentation(stream, relabeller);
}

bool CGraph::equals( const CGraph& graph ) const
{
    _STACK;

    if(this == &graph)
    {
		LOG_DEBUG << "Comparing the graph with itself" << ends;
        return true;
    }

#ifdef NDEBUG
    if(this->getHashCode(GENOTYPE) != graph.getHashCode(GENOTYPE))
    {
		LOG_DEBUG << "The hash code of the two graphs is different" << ends;
        return false;
    }
#endif

	LOG_DEBUG << "Comparing prologues and epilogues" << ends;
    if(*prologue != *graph.prologue)
    {
        return false;
    }

    if(*epilogue != *graph.epilogue)
    {
        return false;
    }
	LOG_DEBUG << "Prologues and epilogues are identical" << ends;

    if(this->subGraphs.size() != graph.subGraphs.size())
    {
		LOG_DEBUG << "The graphs contain a different number of sub-graphs" << ends;
        return false;
    }

	LOG_DEBUG << "Comparing sub-graphs" << ends;
    for(unsigned int i = 0; i < this->subGraphs.size(); i++)
    {
        if(*this->subGraphs[i] != *graph.subGraphs[i])
        {
           return false;
        }
    }

	LOG_DEBUG << "The two graphs are equal" << ends;

    return true;
}


const string CGraph::toString() const
{
   if(this->getConstrain() != nullptr)
   {
		return "{" + this->getConstrain()->getId() + "}";
   }
   else return "";
}

bool CGraph::replaceSubGraph(CSubGraph* originalSubGraph, CSubGraph* newSubGraph)
{
    _STACK;

    Assert(originalSubGraph != nullptr);
    Assert(newSubGraph != nullptr);

    return this->detachSubGraphNoSizeCheck(*originalSubGraph)
        && this->attachSubGraphNoSizeCheck(*newSubGraph);
}

void CGraph::setAsParent(CSubGraph* subGraph, CGraph* newParent) const
{
    _STACK;

    Assert(subGraph != nullptr);

    subGraph->parentContainer = newParent;
}

bool CGraph::attachSubGraphNoSizeCheck(CSubGraph& subGraph)
{
    _STACK;

    // the sub-graph must not be already contained in this graph
    Assert(this->contains(subGraph) == false);

    this->setAsParent(&subGraph, this);

    this->subGraphs.push_back(&subGraph);

    return true;
}

bool CGraph::detachSubGraphNoSizeCheck(CSubGraph& subGraph)
{
    _STACK;

    Assert(this->contains(subGraph));

    this->subGraphs.erase(
        std::remove(begin(this->subGraphs), end(this->subGraphs), &subGraph),
        end(this->subGraphs));

    CNode* cursor = &subGraph.getPrologue();
    while(cursor != nullptr)
    {
        cursor->detachOuterLabels();

        cursor = cursor->getNext();
    }

    this->setAsParent(&subGraph, nullptr);

    return true;
}

bool CGraph::detachSubGraph(CSubGraph& subGraph)
{
    _STACK;


    LOG_DEBUG << "Detaching subgraph " << subGraph << " from graph " << this << ends;

    size_t subGraphCount = this->getSubGraphs(subGraph.getSubSection().getId()).size();
    if(subGraph.getSubSection().getMinimumInstances() == subGraphCount)
    {
        LOG_DEBUG
            << "Cannot detach the subgraph "
            << subGraph << " because the minimum instance count has already been reached" << ends;

        return false;
    }

    return detachSubGraphNoSizeCheck(subGraph);
}

bool CGraph::attachSubGraph(CSubGraph& subGraph)
{
    _STACK;

    Assert(this->contains(subGraph) == false);

    LOG_DEBUG << "Attaching sub-graph " << subGraph << " to graph " << this << ends;

	if(this->getConstrain() != nullptr)
	{
		size_t subGraphCount = this->getSubGraphs(subGraph.getSubSection().getId()).size();
		if(subGraph.getSubSection().getMaximumInstances() == subGraphCount)
		{
			LOG_DEBUG
				<< "Cannot attach the subgraph "
				<< subGraph << " because the maximum instance count has already been reached" << ends;

			return false;
		}
	}
	else
	{
		LOG_DEBUG << "Maximum instance not reached, can attach subgraph..." << ends;
	}

    return this->attachSubGraphNoSizeCheck(subGraph);
}

hash_t CGraph::calculateHashCode(Purpose purpose) const
{
     hash_t hashCode = startValue;

     for (auto subGraph: subGraphs)
         hashCode = Hashable::djbHash(hashCode, subGraph->getHashCode(purpose));

     LOG_DEBUG << "HASH of Graph(" << this << ") is " << Convert::toString(hashCode, Base::Hexadecimal) << std::ends;
     return hashCode;
}

void CGraph::computeMessage(Message& message) const
{
    // Extract symbols at the subgraph level
    std::vector<hash_t> sequence;
    sequence.reserve(getSubGraphCount());
    for (unsigned int iSubGraph = 0; iSubGraph < this->getSubGraphCount(); iSubGraph++) {
        sequence.push_back(getSubGraph(iSubGraph).getHashCode(Hashable::ENTROPY));
    }
    message.extractSymbols(sequence);
    
    // Extract symbols at the node level
    sequence.clear();
    for (unsigned int iSubGraph = 0; iSubGraph < this->getSubGraphCount(); iSubGraph++) {
        CSubGraph& subGraph = getSubGraph(iSubGraph);
        for (CNode* cursor = &subGraph.getPrologue(); cursor != nullptr; cursor = cursor->getNext()) {
            sequence.push_back(cursor->getHashCode(Hashable::ENTROPY));
        }
    }
    message.extractSymbols(sequence);
}

#if 0
// message map with relationship between symbol and its hash
map<hash_t,string> CGraph::getMessageMap()
{
	LOG_DEBUG << "Getting information from a graph..." << ends;
	unsigned int maxTuples = 3;
	unsigned int indexSingleNode = 0;

	map<hash_t,string> tempSymbolMap;
	map<hash_t,string> symbolMap;

	vector<hash_t> subGraphsMessage;

	// adding up the message of each CSubGraph
	for(unsigned int iSubGraph = 0; iSubGraph < this->getSubGraphCount(); iSubGraph++)
	{
		map<hash_t,string> subGraphMap = this->getSubGraph(iSubGraph).getMessageMap();
		vector<hash_t> subGraphMessage = this->getSubGraph(iSubGraph).getMessageInformation();

		LOG_DEBUG << "Map from subGraph #" << iSubGraph << " has size " << subGraphMap.size() << ends;
		// add symbols in temporary map
		tempSymbolMap.insert( subGraphMap.begin(), subGraphMap.end() );
		// add symbols to hash vector
		for(unsigned int i = 0; i < subGraphMessage.size(); i++)
		{
			subGraphsMessage.push_back( subGraphMessage[i]  );
			// keep track of the nodes
			indexSingleNode++;
		}
	}
	LOG_DEBUG << "Gathered information from each subgraph." << ends;

	// Computing all the information on the n-uples
	for(unsigned int maxOffset = 1; maxOffset < maxTuples; maxOffset++)
	{
		LOG_DEBUG << "Gathering information for " << (maxOffset+1) << "-ples." << ends;
		for(unsigned int i = 0; (i + maxOffset) < indexSingleNode; i++)
		{
			ostringstream symbol;
			
			// get string for the symbol and remove line feeds
			string symbolString = tempSymbolMap[ subGraphsMessage.at(i) ];
			//while( symbolString.find("\n") != string::npos ) symbolString.erase( symbolString.find("\n"),1 );

			symbol << symbolString; 
			hash_t hashCode = subGraphsMessage.at(i);

			// we consider the maxOffset values starting from i; e.g. if maxOffset
			// is 2, we'll obtain an hash of values i, i+1, i+2 (thus considering
			// a set of three values)
			for(unsigned int j = i+1; j <= i + maxOffset; j++)
			{
				// get string for the symbol and remove line feeds
				symbolString = tempSymbolMap[ subGraphsMessage.at(j) ];
				//while( symbolString.find("\n") != string::npos ) symbolString.erase( symbolString.find("\n"),1 );
				
				symbol << symbolString;
				hashCode = Hashable::djbHash( hashCode, subGraphsMessage.at(j) );
			}

			// symbol map
			symbolMap[hashCode] = symbol.str();
			LOG_DEBUG << "Symbol map added entry: Key \"" << hashCode << "\" -> \"" << symbol.str() << "\"" << ends;
		}
	}
	LOG_DEBUG << "Gathered information for the n-uples." << ends;

	// Finally, adding the information on the nodes' position
	for(unsigned int i = 0; i < indexSingleNode; i++)
	{
		ostringstream symbol;
		symbol << tempSymbolMap[ subGraphsMessage.at(i) ] << "->pos#" << i;
		hash_t hashCode = Hashable::djbHash( subGraphsMessage.at(i), i );

		symbolMap[ hashCode ] = symbol.str();
		LOG_DEBUG << "Symbol map added entry: Key \"" << hashCode << "\" -> \"" << symbol.str() << "\"" << ends;
	}
	LOG_DEBUG << "Gathered information for the nodes+position." << ends;
	
	LOG_DEBUG << "Content of the symbolMap (" << symbolMap.size() << " elements) at graph level:" << ends;
	for(map<hash_t,string>::iterator it = symbolMap.begin(); it != symbolMap.end(); ++it)
	{
		LOG_DEBUG << "Key:\"" << it->first << "\" -> Value: \"" << it->second << "\"" << ends;
	}

	return symbolMap;
}
#endif
