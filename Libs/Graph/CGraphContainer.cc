/***********************************************************************\
|                                                                       |
| CGraphContainer.cc                                                    |
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
 * @file CGraphContainer.cc
 * Implementation of the CGraphContainer class.
 * @see CGraphContainer.h
 */

#include "ugp3_config.h"
#include "ConstrainedTaggedGraph.h"

using namespace std;
using namespace ugp3;
using namespace tgraph;
using namespace ugp3::constraints;
using namespace ugp3::ctgraph;

CGraphContainer::CGraphContainer()
{ }

CGraphContainer::CGraphContainer(const CGraphContainer& container)
: CGraphContainer()
{
    _STACK;

    throw Exception("Do not use the copy constructor of CGraphContainer: use CGraphContainer::clone instead.", LOCATION);
}

void CGraphContainer::clear()
{
    _STACK;

	this->prologue = unique_ptr<CNode>();
	this->epilogue = unique_ptr<CNode>();

	this->graphs.clear();
}

bool CGraphContainer::validate() const
{
    _STACK;

	if(this->getConstrain() == nullptr)
	{
		LOG_WARNING << "Nothing to validate" << ends;
		return true;
	}

	if(this->getPrologue().validate() == false) return false;
	if(this->getEpilogue().validate() == false) return false;

	for(unsigned int i = 0; i < this->getCGraphCount(); i++)
	{
		if(this->getCGraph(i).validate() == false) return false;
	}

	const ugp3::constraints::Constraints& constraints = (const ugp3::constraints::Constraints&) *this->getConstrain();
	for(unsigned int i = 0; i < constraints.getSectionCount(); i++)
	{
		if(this->getCGraph(constraints.getSection(i).getId()) == nullptr)
			return false;
	}

	return true;
}

void CGraphContainer::buildRandom()
{
    _STACK;

    Assert(this->getConstrain() != nullptr);
    Assert(dynamic_cast<const constraints::Constraints*>(this->getConstrain()) != nullptr);

	const constraints::Constraints& constraints = (const constraints::Constraints&)*this->getConstrain();
    this->prologue = unique_ptr<CNode>(new CNode(*this));
	this->prologue->setConstrain(constraints.getPrologue());
	this->prologue->buildRandom();

    this->epilogue = unique_ptr<CNode>(new CNode(*this));
	this->epilogue->setConstrain(constraints.getEpilogue());
	this->epilogue->buildRandom();

    for(unsigned int i = 0; i < constraints.getSectionCount(); i++)
    {
        unique_ptr<CGraph> newGraph ( new CGraph(*this) );
		newGraph->setConstrain(constraints.getSection(i));
		newGraph->buildRandom();

        this->addCGraph(newGraph);
    }

    // Resolve previously unassigned edges,
    // but do not check if they are attached correctly.
    // In an individual has some floating edges
    // the validate() routine will fail: the individual
    // should be checked for consistency and discarded
    // if not valid.
    this->attachFloatingEdges();

	return;
}

bool CGraphContainer::attachOuterLabel(CNode& node, const OuterLabelParameter& outerLabel)
{
    _STACK;

    Assert(this->getConstrain() != nullptr);
    Assert(node.getGenericMacro().getParameter(outerLabel.getName()) == &outerLabel);

    Edge* edge = node.getEdge(outerLabel.getName());
    Assert(edge != nullptr);

    if(edge->getTo() != nullptr)
    // edge has already a target
    {
        return true;
    }

	LOG_DEBUG << "Node " << node << ": attaching label " << outerLabel << ends;
	bool targetFound = false;

    // chance 1: see if there is the same target to which the edge was referring before it was detached
    if(edge->containsTag(Edge::targetTagName) == true)
    {
    	const string& targetNodeId = edge->getTag(Edge::targetTagName).getValue();
    	CNode* targetNode = this->getNode(targetNodeId);
    	edge->removeTag(Edge::targetTagName);

    	// the target must not belong to the source subgraph (otherwise it isn't an outer label)
    	if(targetNode != nullptr && targetNode->getParentContainer() != node.getParentContainer())
    	// the target has been found
	    {
	    	// targets for outer labels can only be prologues
	    	Assert(targetNode->representsPrologue());

	    	// get the parent container of the target node (needed to check for maximumReferences constraint)
	    	const CSubGraph* subGraph = dynamic_cast<const CSubGraph*>(targetNode->getParentContainer());
	    	Assert(subGraph != nullptr);

	    	if((int)targetNode->getParentContainer()->getPrologue().countIncomingOuterLabels() < (int)subGraph->getSubSection().getMaximumReferences())
	    	{
		        edge->setTo(targetNode);
		        targetFound = true;

		        LOG_DEBUG << "Node " << node << ": the target for the label " << outerLabel
		                     << " is " << edge->getTo() << " (from previous target information)." <<ends;
	    	}
	    }
	    else LOG_DEBUG << "Node " << node << ": the previous target information cannot be used" << ends;
    }
    else LOG_DEBUG << "Node " << node << ": previous target information was not found" << ends;

    // chance 2: see if there is an offset (used on cloned individuals)
    if(targetFound == false && edge->containsTag(Edge::offsetTagName) && edge->containsTag("section"))
    // offset found
    {
    	// search for the target graph
        const string sectionName = edge->getTag("section").getValue();
        CGraph* graph = this->getCGraph(sectionName);
        edge->removeTag("section");

		// get the subgraph's index
        const string& offsetString = edge->getTag(Edge::offsetTagName).getValue();
        unsigned int offset = Convert::toUInt(offsetString);
        edge->removeTag(Edge::offsetTagName);

        // the information is valid only if the index does not exceed the sub-graph count
        if(offset < graph->getSubGraphCount())
        {
        	// get the target subgraph
            CSubGraph& subGraph = graph->getSubGraph(offset);

            // the target node is the prologue of the sub-graph
            CNode* targetNode = &subGraph.getPrologue();

            if(outerLabel.validate(sectionName + " " + subGraph.getSubSection().getId()) == true )
            {
            	if((int)targetNode->countIncomingOuterLabels() < (int)subGraph.getSubSection().getMaximumReferences())
            	{
                	edge->setTo(targetNode);


                	LOG_DEBUG << "Node " << node << ": the target sub-graph for the label " << outerLabel
                     	<< " is " << subGraph << " (from previous offset information)." <<ends;

                     targetFound = true;
            	}
            	else LOG_DEBUG << "Node " << node << ": could not restore outer label using sub-graph offset: maximum references reached" << ends;
            }
            else LOG_DEBUG << "Node " << node << ": could not restore outer label using sub-graph offset: invalid section/subsection target" << ends;
        }
        else LOG_DEBUG << "Node " << node << ": could not restore outer label using sub-graph offset: invalid index" << ends;
    }

	if(targetFound == false)
    // no target for the outer label, choose a new one
    {
    	LOG_DEBUG << "Node " << node << ": searching for a valid random target" << ends;
        const string& newTargetLocation = outerLabel.randomize();
        LOG_DEBUG << "Node " << node << ": the outer label " << outerLabel << " will point to \"" << newTargetLocation << "\"" << ends;

        // extract section and subsection names
        istringstream stream;
        stream.str(newTargetLocation);
        string sectionName, subSectionName;
        stream >> sectionName >> subSectionName;

        // collect all the sub-graphs representing the target sub-section
        const CGraph* targetGraph = this->getCGraph(sectionName);
		if(targetGraph != nullptr)
		{
			vector<CSubGraph*> subGraphs = targetGraph->getSubGraphs(subSectionName);
			LOG_DEBUG << "Node " << node << ": there are " << subGraphs.size() << " candidate targets" << ends;

			// try until a valid sub-graph is found
			while(subGraphs.size() > 0 && targetFound == false)
			{
				// random extraction of a candidate sub-graph
				unsigned long randomSample = Random::nextUInteger(0, subGraphs.size() - 1);
				CSubGraph* targetSubGraph = subGraphs[randomSample];
				subGraphs.erase(subGraphs.begin() + randomSample);

				// source and target subgraphs must be different
				if(targetSubGraph == node.getParentContainer()) continue;

				CNode* targetNode = &targetSubGraph->getPrologue();

				// count the number of incoming outer labels on the target node
				unsigned int references = targetNode->countIncomingOuterLabels();

				// the number of references to the target sub-section must be lower than the maximum allowed
				if(targetSubGraph->getSubSection().getMaximumReferences() > references)
				{
					LOG_DEBUG << "Node " << node << ": the target sub-graph for the label " << outerLabel
						 << " is " << targetSubGraph << " (from random selection)." <<ends;

					// update the edge target
					edge->setTo(targetNode);
					targetFound = true;
				}
				else LOG_DEBUG << "Node " << node << ": the target " << targetNode << " has reached the maximum number of outer references (" << targetSubGraph->getSubSection().getMaximumReferences() << ")" << ends;
			}
		}
    }

	if(targetFound == false)
	// nothing else can be done
    {
    	LOG_DEBUG << "Node " << node << ": could not find a target for the outer label " << outerLabel << " " << ends;
    	return false;
    }

    return true;
}

bool CGraphContainer::attachFloatingEdges()
{
    _STACK;

    Assert(this->getConstrain() != nullptr);

	LOG_DEBUG <<  "GraphContainer: restoring inner labels" << ends;
    // attach inner labels
    for( unsigned int i = 0; i < this->graphs.size(); i++ )
    {
        bool success = this->graphs[i]->attachFloatingEdges();
        if(success == false)
        {
            return false;
        }
    }

    LOG_DEBUG <<  "GraphContainer: restoring outer labels" << ends;
    // attach outer labels
    for(unsigned int g = 0; g < this->getCGraphCount(); g++)
    {
        // for each graph ...
        CGraph& graph = this->getCGraph(g);
        for(unsigned int s = 0; s < graph.getSubGraphCount(); s++)
        {
            // ... for each subGraph ...
            CSubGraph& subGraph = graph.getSubGraph(s);

            // ... for each node ...
            CNode* node = &subGraph.getPrologue();
            while(node != nullptr)
            {
            	// ... for each parameter ...
                for(unsigned int p = 0; p < node->getGenericMacro().getParameterCount(); p++)
                {
                    const Parameter& parameter = node->getGenericMacro().getParameter(p);

					// select only outer labels
                    const OuterLabelParameter* outerLabel = dynamic_cast<const OuterLabelParameter*>(&parameter);
                    if(outerLabel == nullptr) continue;

					// select a target for outerlabels that do not have one
                    bool success = this->attachOuterLabel(*node, *outerLabel);
                    if(success == false) return false;
                }

                node = node->getNext();
            }
        }
    }

	LOG_DEBUG <<  "GraphContainer: all edges restored." << endl << ends;
    return true;
}


unique_ptr<CGraphContainer> CGraphContainer::clone() const
{
    _STACK;

	LOG_DEBUG <<  "GraphContainer: cloning ..." << ends;
    CGraphContainer* graphContainer = new CGraphContainer();
	if(this->getConstrain() != nullptr)
	{
		graphContainer->setConstrain(*this->getConstrain());
	}

    LOG_DEBUG << "Cloning container's prologue " << this->prologue->toString() << ends;
    unique_ptr<CNode> node = this->prologue->clone();
    NodeContainer::setAsParent(node.get(), graphContainer);
    graphContainer->prologue.reset(node.release());

    LOG_DEBUG << "Cloning container's epilogue " << this->epilogue->toString() << ends;
    node = this->epilogue->clone();
    NodeContainer::setAsParent(node.get(), graphContainer);
    graphContainer->epilogue.reset(node.release());

	LOG_DEBUG << "Cloning container's graphs" << ends;
    for(unsigned int i = 0; i < this->graphs.size(); i++)
    {
        unique_ptr<CGraph> graph = this->graphs[i]->clone();
        this->setAsParent(*graph.get(), graphContainer);

        graphContainer->addCGraph(graph);
    }

    /** Lazy cloning: do not restore floating edges. The caller will do that. */

	LOG_DEBUG <<  "GraphContainer: cloned." << endl << ends;
    return unique_ptr<CGraphContainer>(graphContainer);
}

CGraphContainer::~CGraphContainer()
{
    _STACK;

#ifndef NDEBUG
    LOG_DEBUG << "Destructor ugp3::core::CGraphContainer" << ends;
#endif

    for(unsigned int i = 0; i < this->getCGraphCount(); i++)
    {
        delete this->graphs[i];
    }
}

CGraph* CGraphContainer::getCGraph(const string& sectionName) const
{
    _STACK;

    for(unsigned int i = 0; i < this->getCGraphCount(); i++)
    {
        if(sectionName == this->getCGraph(i).getSection().getId())
        {
            return &this->getCGraph(i);
        }
    }

   return nullptr;
}

void CGraphContainer::setAsParent(CGraph& graph, CGraphContainer* container) const
{
    graph.parentContainer = container;
}

CSubGraph* CGraphContainer::getRandomSubGraph(const string& sectionName, const string& subSectionName)
{
    _STACK;

	try
	{
		CGraph* graph = this->getCGraph(sectionName);
		if(graph == nullptr) return nullptr;

		// collect all the prologues that refer to the subsection having the specified name
		vector<CSubGraph*> candidateList = graph->getSubGraphs(subSectionName);
		if(candidateList.empty() == true)
		{
			LOG_WARNING << "No subSections with name \"" << subSectionName << "\" were found in the graph container to perform a random selection.";
			return nullptr;
		}

		// select a random prologue
		unsigned long randomSample = Random::nextUInteger(0, candidateList.size() - 1);
		Assert(randomSample < candidateList.size());

		return candidateList[randomSample];
	}
	catch(...)
	{
		return nullptr;
	}
}

CNode* CGraphContainer::getNode(const string& id) const
{
    _STACK;

    // search for node inside the graphs
    for(unsigned int i = 0; i < this->graphs.size(); i++)
    {
        CNode* node = this->graphs[i]->getNode(id);
        if(node != nullptr)
        {
            return node;
        }
    }
    
    // but the node could also be the global prologue/epilogue
    if( this->prologue->getId().compare(id) == 0 ) return this->prologue.get();
    if( this->epilogue->getId().compare(id) == 0 ) return this->epilogue.get();

    return nullptr;
}

void CGraphContainer::writeExternalRepresentation(ostream& stream, Relabeller& relabeller) const
{
    _STACK;

    Assert(this->getConstrain() != nullptr);

    LOG_DEBUG << "Requested a dump for CGraphContainer" << ends;

    Assert(prologue.get() != nullptr);
    LOG_DEBUG << "Dumping the global prologue " << this->prologue->toString() << ends;
    prologue->writeExternalRepresentation(stream, relabeller);

    for( unsigned int i=0; i<graphs.size(); i++ )
    {
        graphs[i]->writeExternalRepresentation(stream, relabeller);
    }

    Assert(epilogue.get() != nullptr);
    LOG_DEBUG << "Dumping the global epilogue " << this->epilogue->toString() << ends;
    epilogue->writeExternalRepresentation(stream, relabeller);
}

bool CGraphContainer::contains(const CGraph& graph) const
{
    for(unsigned int i = 0; i < this->graphs.size(); i++)
    {
        if(this->graphs[i] == &graph)
        {
            return true;
        }
    }

    return false;
}

bool CGraphContainer::contains(const CNode& node) const
{
    if(this->prologue.get() == &node) return true;

    if(this->epilogue.get() == &node) return true;

    for(unsigned int i = 0; i < this->graphs.size(); i++)
    {
        if(this->graphs[i]->contains(node))
        {
            return true;
        }
    }

    return false;
}

void CGraphContainer::addCGraph(unique_ptr<CGraph>& graph)
{
    _STACK;

    if(graph.get() == nullptr)
    {
        throw ArgumentNullException("graph", LOCATION);
    }

    // the graph mustn't be already inside the container
    Assert(this->contains(*graph) == false);

    //TODO: should check that there is no other graph representing the same section
	if(this->getConstrain() != nullptr)
	{
		const Constraints& constraints = (const Constraints&) *this->getConstrain();

		for(unsigned int i = 0; i < this->getCGraphCount(); i++)
		{
			CGraph& otherGraph = this->getCGraph(i);

			if(otherGraph.getConstrain() != &constraints.getSection(i))
			{
				throw Exception("The container is not valid", LOCATION);
			}
		}

		if(graph->getConstrain() != &constraints.getSection(this->getCGraphCount()))
		{
			throw Exception("The new graph should be of type " + constraints.getSection(this->getCGraphCount()).toString() + ".", LOCATION);
		}
	}

    if(graph->getParentContainer() == nullptr || graph->getParentContainer() == this)
    {
        LOG_DEBUG << "Adding graph " << graph->toString() << ends;

        this->setAsParent(*graph, this);
        graphs.push_back(graph.release());
    }
    else throw ArgumentException("Cannot add the graph to the container because it already belongs to another container.", LOCATION);
}

bool CGraphContainer::equals(const CGraphContainer& container) const
{
    _STACK;

    LOG_DEBUG << "Requested a comparison between two CGraphContainers" << ends;

    Assert(this->validate() == true);
    Assert(container.validate() == true);

#ifdef NDEBUG
    // In debug mode we want `isGenotypeEqual' to be independent from the hash
    if(this->getHashCode(GENOTYPE) != container.getHashCode(GENOTYPE))
    {
		LOG_DEBUG << "The containers have different hash code" << ends;
        return false;
    }
#endif

	if(this->getConstrain() == nullptr || container.getConstrain() == nullptr)
	{
		if(this->getConstrain() != this->getConstrain())
		{
			LOG_DEBUG << "The containers refer to two different constraints definition" << ends;
			return false;
		}
	}

    LOG_DEBUG << "Comparing containers' prologues and epilogues" << ends;
    if(*prologue != *container.prologue)
    {
		LOG_DEBUG << "The prologues are different" << ends;
        return false;
    }

    if( *epilogue != *container.epilogue )
    {
		LOG_DEBUG << "The epilogues are different" << ends;
        return false;
    }

    LOG_DEBUG << "Prologues and epilogues are identical" << ends;

	LOG_DEBUG << "Comparing graphs of the containers" << ends;
	if(this->getCGraphCount() != container.getCGraphCount()) return false;

	for(unsigned int i = 0; i < this->getCGraphCount(); i++)
	{
		CGraph& thisGraph = this->getCGraph(i);
		CGraph* otherGraph = container.getCGraph(thisGraph.getSection().getId());
		if(otherGraph == nullptr) return false;

        if(*otherGraph != thisGraph)
        {
            return false;
        }
	}

    LOG_DEBUG << "The graph containers are identical" << ends;
    return true;
}

hash_t CGraphContainer::calculateHashCode(Purpose purpose) const
{
     hash_t hashCode = startValue;

     for (auto graph: graphs)
         hashCode = Hashable::djbHash(hashCode, graph->getHashCode(purpose));

     LOG_DEBUG << "HASH of Graphcontainer(?) is 0x" << Convert::toString(hashCode, Base::Hexadecimal) << std::ends;
     return hashCode;
}

void CGraphContainer::computeMessage(Message& message) const
{
    for (unsigned int i = 0; i < this->getCGraphCount(); i++)
    {
        CGraph& graph = this->getCGraph(i);
        message += graph.getMessage();
    }
}

const vector< hash_t >& CGraphContainer::getNodeHashSequence() const
{
    if (m_nodeHashSequence.empty()) {
        for (unsigned int i = 0; i < this->getCGraphCount(); i++) {
            CGraph& graph = this->getCGraph(i);
            for (unsigned int iSubGraph = 0; iSubGraph < graph.getSubGraphCount(); iSubGraph++) {
                CSubGraph& subGraph = graph.getSubGraph(iSubGraph);
                for (CNode* cursor = &subGraph.getPrologue(); cursor != nullptr; cursor = cursor->getNext()) {
                    m_nodeHashSequence.push_back(cursor->getHashCode(Hashable::ENTROPY));
                }
            }
        }
    }
    return m_nodeHashSequence;
}

