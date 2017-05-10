/***********************************************************************\
|                                                                       |
| CSubGraph.cc                                                          |
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

using namespace std;
using namespace ugp3;
using namespace tgraph;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;

InfinityString CSubGraph::idCounter;
InfinityString Slice::idCounter;

CSubGraph::CSubGraph()
    : id(CSubGraph::idCounter.toString()),
    parentContainer(nullptr),
    prologue(nullptr), epilogue(nullptr)
{
    _STACK;

    CSubGraph::idCounter++;
}

CSubGraph::CSubGraph(IContainer<CSubGraph>& parentContainer)
 : id(CSubGraph::idCounter.toString()),
    parentContainer(&parentContainer),
    prologue(nullptr), epilogue(nullptr)
{
    _STACK;

    CSubGraph::idCounter++;
}

void CSubGraph::buildRandom()
{
    _STACK;

    Assert(this->parentContainer != nullptr);
    Assert(this->getConstrain() != nullptr);

	this->clear();

	const SubSection& subSection = (const SubSection&)*this->getConstrain();

    LOG_DEBUG << "Creating sub-graph " << this << ends;

    LOG_DEBUG << "Creating sub-graph prologue" << ends;
    this->prologue = new CNode(*this);
	this->prologue->setConstrain(subSection.getPrologue());
	this->prologue->buildRandom();

    this->addNode(*this->prologue);
    this->slice.append(*this->prologue);

    // choose how many macros should have this subgraph

    unsigned long macroInstances = 0;
    do
    {
        double displacement = Random::nextNormal(this->getSubSection().getSigma());
    	macroInstances = (unsigned long)(displacement
    	    + this->getSubSection().getAverageMacroInstances());
    }
    while(
        macroInstances < subSection.getMinimumMacroInstances() ||
        macroInstances > subSection.getMaximumMacroInstances());


    LOG_DEBUG << "Instantiating " << macroInstances << " inner macros" << ends;
    for(unsigned int i = 0; i < macroInstances; i++)
    {
        Macro* macro = subSection.getRandomMacro();

        CNode* node = new CNode(*this);
		node->setConstrain(*macro);
		node->buildRandom();

        this->addNode(*node);
        this->slice.append(*node);
    }

    LOG_DEBUG << "Creating sub-graph epilogue" << ends;
    this->epilogue = new CNode(*this);
	this->epilogue->setConstrain(subSection.getEpilogue());
	this->epilogue->buildRandom();


    this->addNode(*this->epilogue);
    this->slice.append(*this->epilogue);

    LOG_DEBUG << "Resolving next and prev references" << ends;
    this->slice.attachNextAndPrev();

    /* DO NOT attach floating edges here */
}

void CSubGraph::addNode(CNode& node)
{
    _STACK;

    if(this->nodes.count(node.getId()) == 0)
    {
        this->nodes[node.getId()] = &node;
    }
    else throw Exception("Duplicate node id.", LOCATION);
}

void CSubGraph::clear()
{
    _STACK;

	this->nodes.clear();

	this->prologue = nullptr;
	this->epilogue = nullptr;
}

bool CSubGraph::validate() const
{
    _STACK;

    // check prologue and epilogue
    if(this->prologue == nullptr)
    {
        LOG_VERBOSE << "The prologue of the subgraph " << this << " is null." << std::ends;
        return false;
    }

    if(this->epilogue == nullptr)
    {
        LOG_VERBOSE << "The epilogue of the subgraph " << this << " is null." << std::ends;
        return false;
    }

    if(this->epilogue->getNext() != nullptr || this->epilogue->getPrev() == nullptr)
    {
        LOG_VERBOSE << "The epilogue " << this->epilogue->toString() << " has invalid next and prev references" << ends;
        return false;
    }

    if(this->prologue->getPrev() != nullptr || this->prologue->getNext() == nullptr)
    {
        LOG_VERBOSE << "The prologue " << this->prologue->toString() << " has invalid next and prev references" << ends;
        return false;
    }

    if(this->prologue->representsPrologue() == false)
    {
        LOG_VERBOSE << "The prologue " << this->prologue->toString() << " of the subgraph " << this << " does not represent a prologue." << std::ends;
        return false;
    }

    if(this->epilogue->representsEpilogue() == false)
    {
        LOG_VERBOSE << "The epilogue of the subgraph " << this << " does not represent a epilogue." << std::ends;
        return false;
    }

    if(this->prologue->validate() == false || this->epilogue->validate() == false)
    {
        LOG_VERBOSE << "The epilogue or the epilogue of the subgraph " << this << " is not valid." << std::ends;
        return false;
    }



    // check connections
    unsigned int position = 0;
    CNode* cursor = this->prologue;
    while(cursor->getNext() != nullptr)
    {
        if(cursor != &this->slice.getNode(position))
        {
            LOG_VERBOSE
                << "The node " << cursor << " of subgraph " << this
                << " is not in the expected position." << std::ends;
            return false;
        }

        if (cursor->containsTag(CNode::TAG_PLACE)
         && Convert::toUInt(cursor->getTag(CNode::TAG_PLACE).getValue())!=position)
        {
            LOG_VERBOSE
                << "The node " << cursor << " of subgraph " << this
                << " in position " << position << " is not where it expects to be ("
                << Convert::toUInt(cursor->getTag(CNode::TAG_PLACE).getValue()) << ")."
                << std::ends;
            return false;
        }

        if(cursor->validate() == false)
        {
            LOG_VERBOSE << "The node " << cursor << " of subgraph " << this << " is invalid." << std::ends;
            return false;
        }

		if(cursor != this->prologue && cursor->representsPrologue())
		{
			LOG_VERBOSE << "A prologue was found inside the subsection." << std::ends;
            return false;
		}

        cursor = cursor->getNext();
        position++;
    }

    if (cursor != this->epilogue)
    {
        LOG_VERBOSE << "The subgraph " << this << " ends with node " << cursor << " that is not an epilogue." << std::ends;
        return false;
    }


    return this->validateConstraints();
}

bool CSubGraph::validateConstraints() const
{
    // check constraints
    if(this->getConstrain() != nullptr)
    {
        size_t macroCount = this->slice.getSize();
        Assert(macroCount >= 2);
        macroCount -= 2;

        if(macroCount > this->getSubSection().getMaximumMacroInstances()
            || macroCount < this->getSubSection().getMinimumMacroInstances())
        {
            LOG_VERBOSE << "The subgraph " << this << " has an invalid number of nodes ("
                << macroCount<< " instead of min="
                << this->getSubSection().getMinimumMacroInstances() << ", max="
                << this->getSubSection().getMinimumMacroInstances()<< ")." << std::ends;

            return false;
        }

        if(this->getSubSection().getMaximumReferences() < this->prologue->countIncomingOuterLabels())
        {
            LOG_VERBOSE
                    << "Subgraph " << this << ": the number of incoming outer labels is greater than the maximum allowed" << ends;
            return false;
        }
    }

    return true;
}

CSubGraph::~CSubGraph()
{
    _STACK;

#ifndef NDEBUG
    LOG_DEBUG << "Desctructor ugp3::core::CSubGraph " << this->id << ends;
#endif

	this->clear();
}

int CSubGraph::getOffset(const CNode& node, const CNode& targetNode) const
{
    _STACK;

    // The specified nodes must be contained in the subgraph
    Assert(this->contains(node));
    Assert(this->contains(targetNode));

    unsigned int nodePosition = 0;
    unsigned int targetPosition = 0;

    if(node.containsTag(CNode::TAG_PLACE)
        && targetNode.containsTag(CNode::TAG_PLACE))
    {
        nodePosition = Convert::toUInt(
                            node.getTag(CNode::TAG_PLACE).getValue());
        targetPosition = Convert::toUInt(
                            targetNode.getTag(CNode::TAG_PLACE).getValue());
        LOG_DEBUG << "Getting offset between node " << node << " in place " << nodePosition << " and node " << targetNode << " in place " << targetPosition << ends;
    }
    else
    {
        unsigned int position = 0;
        CNode* cursor = this->prologue;
        while(cursor != nullptr)
        {
            if(cursor == &node)
            {
                nodePosition = position;
            }
            if(cursor == &targetNode)
            {
                targetPosition = position;
            }

            cursor = cursor->getNext();
            position++;
        }
        LOG_DEBUG << "Scanning for offset between node " << node << " in place " << nodePosition << " and node " << targetNode << " in place " << targetPosition << ends;
   }

    return targetPosition - nodePosition;
}

bool CSubGraph::isStrictlyBefore(const CNode& node1, const CNode& node2) const
{
    _STACK;

    LOG_DEBUG << "Checking" << node1.getId() << " < " << node2.getId() << ends;
    // 'node1' and 'node2' cannot refer to the same node
    Assert(&node1 != &node2);

    return this->getOffset(node1, node2) > 0;
}

bool CSubGraph::isBefore(const CNode& node1, const CNode& node2) const
{
    _STACK;

    LOG_DEBUG << "Checking" << node1.getId() << " < " << node2.getId() << ends;
    // 'node1' and 'node2' can refer to the same node!

    return this->getOffset(node1, node2) > 0;
}

// To modify
CNode* CSubGraph::getTargetFromFloatingEdgeUsingOffset(const CNode& node, const Edge& edge) const
{
    _STACK;

    // The specified node must be contained in the subgraph
    Assert(this->contains(node));
    // the target must not be set
    Assert(edge.getTo() == nullptr);
    // the edge must contain the offset information
    Assert(edge.containsTag(Edge::offsetTagName));

#ifndef NDEBUG
    bool edgeFound = false;
    for(unsigned int i = 0; i < node.getEdgeCount(); i++)
    {
        if(&edge == &static_cast<const Node*>(&node)->getEdge(i))
        {
            edgeFound = true;
        }
    }

    // The specified edge must belong to the node
    Assert(edgeFound == true);
#endif


    const Tag& offsetTag = edge.getTag(Edge::offsetTagName);
    // get the relative position of the target node
    const int offset = Convert::toInt(offsetTag.getValue());

    // get the parameter associated to the edge
    Assert(edge.containsTag(Edge::parameterTagName));
    Tag& parameterTag = edge.getTag(Edge::parameterTagName);
    const string& parameterName = parameterTag.getValue();

    const Parameter* parameter = node.getGenericMacro().getParameter(parameterName);
    Assert(parameter != nullptr);

    Assert(dynamic_cast<const InnerLabelParameter*>(parameter) != nullptr);
    const InnerLabelParameter& labelParameter = (const InnerLabelParameter&)*parameter;

    // locate the target node
    const CNode* cursor = &node;
    if(offset > 0)
    // move forward
    {
        Assert(labelParameter.getForwardJumpIsValid() == true);
        int count = 0;
        while(cursor->representsEpilogue() == false && count < offset)
        {
            cursor = cursor->getNext();
            Assert(cursor != nullptr);

            count++;
        }

        if(cursor->representsEpilogue() == true && labelParameter.getEpilogueIsValid() == false)
        // the end of the subgraph has been reached and the epilogue is not a valid target, move back to the previous node
        {
            LOG_VERBOSE << "the end of the subgraph has been reached and the epilogue is not a valid target" << ends;
            cursor = cursor->getPrev();
        }
        if(cursor == &node && labelParameter.getItselfIsValid() == false)
        {
            if (labelParameter.getBackwardJumpIsValid() == false){
                cursor = nullptr;
            }
            else{
                cursor = cursor->getPrev();
                if (cursor == nullptr || (cursor->representsPrologue() == true && labelParameter.getPrologueIsValid() == false))
                    cursor = nullptr;
            }
        }
    }
    else if(offset < 0)
    // move backward
    {
        Assert(labelParameter.getBackwardJumpIsValid() == true);
        int count = 0;
        while(cursor->representsPrologue() == false && count > offset)
        {
            cursor = cursor->getPrev();
            Assert(cursor != nullptr);

            count--;
        }

        if(cursor->representsPrologue() == true && labelParameter.getPrologueIsValid() == false)
        // the start of the subgraph has been reached and the prologue is not a valid target, move forward to the next node
        {
            LOG_VERBOSE << "the start of the subgraph has been reached and the prologue is not a valid target" << ends;
            cursor = cursor->getNext();
        }
        if(cursor == &node && labelParameter.getItselfIsValid() == false)
        {
            if (labelParameter.getForwardJumpIsValid() == false){
                cursor = nullptr;
            }
            else
            {
                cursor = cursor->getNext();
                if (cursor == nullptr || (cursor->representsEpilogue() == true && labelParameter.getEpilogueIsValid() == false))
                    cursor = nullptr;
            }
        }
    }

    return (CNode*)cursor;
}

CNode* CSubGraph::getRandomTargetForInnerLabel(
    const CNode& node,
    const InnerLabelParameter& innerLabel) const
{
    _STACK;

    // the specified inner label must belong to the node
    Assert(node.getGenericMacro().getParameter(innerLabel.getName()) == &innerLabel);

    LOG_DEBUG << "Node " << node << ": computing random target for inner label " << innerLabel << "." << std::ends;

    // collect all the possible targets
    vector<const CNode*> candidateList;

    if(innerLabel.getItselfIsValid())
    {
        candidateList.push_back(&node);
    }

    if(innerLabel.getBackwardJumpIsValid() && innerLabel.getForwardJumpIsValid())
    {
        CNode* nodeCursor = prologue;
        if(innerLabel.getPrologueIsValid() == false)
        {
            // skip prologue
            nodeCursor = nodeCursor->getNext();
        }

        while(nodeCursor->representsEpilogue() == false)
        {
            if(nodeCursor != &node) // avoid adding the node twice
            {
                candidateList.push_back(nodeCursor);
            }
            nodeCursor = nodeCursor->getNext();
        }

        if(innerLabel.getEpilogueIsValid() == true)
        {
            // include epilogue
            candidateList.push_back(nodeCursor);
        }
    }
    else if(innerLabel.getBackwardJumpIsValid() == true)
    {
        CNode* nodeCursor = node.getPrev();
        while(nodeCursor->representsPrologue() == false)
        {
            candidateList.push_back(nodeCursor);
            nodeCursor = nodeCursor->getPrev();
        }

        if(innerLabel.getPrologueIsValid() == true)
        {
            // include prologue
            candidateList.push_back(nodeCursor);
        }
    }
    else // forward jump is valid
    {
        Assert(innerLabel.getForwardJumpIsValid() == true);

        CNode* nodeCursor = node.getNext();

        while(nodeCursor->representsEpilogue() == false)
        {
            candidateList.push_back(nodeCursor);
            nodeCursor = nodeCursor->getNext();
        }

        if(innerLabel.getEpilogueIsValid() == true)
        {
            // include epilogue
            candidateList.push_back(nodeCursor);
        }
    }


    if(candidateList.size() == 0)
    {
        LOG_DEBUG
            << "No target node was found for the inner label parameter "
            << innerLabel << ends;

        return nullptr;
    }

    unsigned long randomSample = Random::nextUInteger(0, candidateList.size() - 1);
    return (CNode*)candidateList[randomSample];
}

void CSubGraph::writeExternalRepresentation(ostream& stream, Relabeller& relabeller) const
{
    _STACK;

    Assert(this->validate() == true);

    CNode* cursor = this->prologue;
    while(cursor != nullptr)
    {
        cursor->writeExternalRepresentation(stream, relabeller);
        cursor = cursor->getNext();
    }
}

bool CSubGraph::equals(const CSubGraph& subGraph) const
{
    _STACK;

    LOG_DEBUG << "Subgraph " << this << ": comparing with subgraph " << subGraph << ends;


     // check if the specified subgraph refers to the same instance
    if(this == &subGraph)
    {
        return true;
    }


#ifdef NDEBUG
    if(this->getHashCode(GENOTYPE) != subGraph.getHashCode(GENOTYPE))
    {
        LOG_DEBUG << "Subgraph " << this << ": the hash code is different from the one of the sub-graph " << subGraph << ends;
        return false;
    }
#endif

    // check if the subgraphs have the same size
    if(this->getSize() != subGraph.getSize())
    {
        LOG_DEBUG << "Subgraph " << this << ": its number of nodes differs from the one of the subgraph " << subGraph << ends;
        return false;
    }

    // scan the two subgraphs starting from the prologues
    const CNode* cursor1 = this->prologue;
    const CNode* cursor2 = subGraph.prologue;

    while(cursor1 != nullptr)
    {
        Assert(cursor2 != nullptr);

        // compare the two nodes
        if(*cursor1 != *cursor2)
        {
            return false;
        }

        // move to the next nodes
        cursor1 = cursor1->getNext();
        cursor2 = cursor2->getNext();
    }

    Assert(cursor1 == nullptr && cursor2 == nullptr);

    LOG_DEBUG << "Subgraph " << this << ": is equal to the subgraph "
        << subGraph << ends;
    return true;
}

CNode* CSubGraph::getNode(const string& id) const
{
    _STACK;

    map<string, CNode*>::const_iterator iterator = this->nodes.find(id);

    if(iterator != this->nodes.end())
    {
        return (*iterator).second;
    }

    return nullptr;
}

unique_ptr<CSubGraph> CSubGraph::clone() const
{
    _STACK;

    LOG_DEBUG << "SubGraph " << this << ": cloning ..." << ends;
    Assert(parentContainer != nullptr);

    // create a new subgraph
    unique_ptr<CSubGraph> subGraph (new CSubGraph());
    subGraph->parentContainer = nullptr;
	if(this->getConstrain() != nullptr)
	{
	    subGraph->setConstrain(*this->getConstrain());
	}

    // copy all the nodes of the sub-graph
    CNode* cursor = (CNode*)&this->getPrologue();
    do
    {
        Assert(cursor != nullptr);

        CNode* node = cursor->clone().release();
        NodeContainer::setAsParent(node, subGraph.get());
        subGraph->addNode(*node);

        if(node->representsPrologue())
        {
            subGraph->prologue = node;
        }
        else if (node->representsEpilogue())
        {
            subGraph->epilogue = node;
        }

        subGraph->slice.append(*node);

        cursor = cursor->getNext();
    }
    while(cursor != nullptr);


    // now set next and prev
    subGraph->slice.attachNextAndPrev();

    ////////////DEBUG//////////////////
#ifndef NDEBUG
    cursor = subGraph->prologue;
    while(cursor != subGraph->epilogue)
    {
        cursor = cursor->getNext();
        Assert(cursor != nullptr);
    }
#endif
    ///////////////////////////////


    LOG_DEBUG << "SubGraph " << this << ": clone " << subGraph->toString() << " created."<< endl << ends;

    // return the new subgraph
    return subGraph;
}

bool CSubGraph::attachFloatingEdges()
{
    _STACK;

    this->slice.attachNextAndPrev();
    this->nodes.clear();

    this->prologue = &this->slice.getNode(0);
    this->epilogue = &this->slice.getNode(this->slice.getSize() - 1);

    Assert(this->prologue->representsPrologue());
    Assert(this->epilogue->representsEpilogue());

    for(unsigned int i = 0; i < this->slice.getSize(); i++)
    {
        CNode& node = this->slice.getNode(i);

        this->nodes[node.getId()] = &node;

        setAsParent(&node, this);
    }

    LOG_DEBUG <<  "SubGraph " << this << ": restoring inner labels" << ends;

    for(unsigned int i = 0; i < this->slice.getSize(); i++)
    {
        CNode& node = this->slice.getNode(i);

        // search for inner labels in the parameters of the macro
        for(unsigned int p = 0; p < node.getGenericMacro().getParameterCount(); p++)
        {
            const Parameter& parameter = node.getGenericMacro().getParameter(p);

            // consider inner labels only
            const InnerLabelParameter* innerLabel = dynamic_cast<const InnerLabelParameter*>(&parameter);
            if(innerLabel == nullptr)
                continue;

            bool success = this->restoreInnerLabel(node, *innerLabel);
            if(success == false)
            {
                return false;
            }

            Assert(node.initialized() == true);
        }
    }

    LOG_DEBUG <<  "SubGraph " << this << ": inner labels restored successfully" << ends;
    return true;
}

bool CSubGraph::restoreInnerLabel(CNode& node, const InnerLabelParameter& innerLabel)
{
    _STACK;

    // The specified inner label must be associated to the macro that the node represents
    Assert(&innerLabel == node.getGenericMacro().getParameter(innerLabel.getName()));

    LOG_DEBUG << "Node " << node << ": restoring inner label " << innerLabel << "." << std::ends;

    // get the edge associated to the inner label
    Assert(node.getEdge(innerLabel.getName()) != nullptr); // there must always be an edge for every inner label
    Edge& edge = *node.getEdge(innerLabel.getName());


    if(edge.getTo() == nullptr)
    // the edge is floating
    {
        LOG_DEBUG << "Node " << node << ": restoring edge " << edge << "." << std::ends;

        CNode* targetNode = nullptr;

        if(edge.containsTag(Edge::targetTagName))
        // a target exists
        {
            Tag& targetTag = edge.getTag(Edge::targetTagName);

            // retrieve the target node
            targetNode = this->getNode(targetTag.getValue());
            LOG_DEBUG << "Target node " << targetNode << " retrieved." << std::ends;
        }

        if(targetNode == nullptr) //(!)gx20111221: why not an else!?
        {
            if(edge.containsTag(Edge::offsetTagName))
            {
                // relink it to the target node using offset
                const string& offset = edge.getTag(Edge::offsetTagName).getValue();
                targetNode = this->getTargetFromFloatingEdgeUsingOffset(node, edge);
                LOG_DEBUG << "Node " << targetNode << " found at requested offset " << offset << "." << std::ends;
            }

            if(targetNode == nullptr)  
            // still no offset specified (maybe offset was not valid), random choice!
            {
                targetNode = this->getRandomTargetForInnerLabel(node, innerLabel);
                LOG_DEBUG << "Node " << targetNode << " chosen randomly." << std::ends;
                if(targetNode == nullptr)
                {
                    // no proper target for the inner label
                    LOG_DEBUG << "Node " << node << ": could not restore inner label " << innerLabel << "." << std::ends;
                    return false;
                }
            }
        }

        Assert(targetNode != nullptr);
        edge.setTo(targetNode);
        edge.removeTag(Edge::targetTagName);
        edge.removeTag(Edge::offsetTagName);
    }

    LOG_DEBUG << "Node " << node << ": inner label " << innerLabel <<
        " restored (edge=" << edge << ")." << std::ends;

    return true;
}

bool CSubGraph::contains(const CNode& node) const
{
    _STACK;

    return this->nodes.find(node.getId()) != this->nodes.end();
}

hash_t CSubGraph::calculateHashCode(Purpose purpose) const
{
    _STACK;

    hash_t hashCode = startValue;

    CNode* cursor = this->prologue;
    while(cursor != nullptr) {
        hashCode = Hashable::djbHash(hashCode, cursor->getHashCode(purpose));
        cursor = cursor->getNext();
    }

    LOG_DEBUG << "HASH of SubGraph " << this << " is "
        << Convert::toString(hashCode, Base::Hexadecimal) << std::ends;
    return hashCode;
}

void CSubGraph::setPrologue(unique_ptr<CNode>& value)
{
	if(value->getNext() != nullptr)
	{
		throw Exception("You should have detached the 'next' reference first.", LOCATION);
	}

	CNode* nextNode = nullptr;
	if(this->prologue != nullptr)
	// detach the old prologue
	{
		this->prologue->detachOuterLabels();

		// save the next reference (if present)
		nextNode = this->prologue->getNext();

		this->prologue->detachInnerLabels();;

		this->nodes.erase(
				this->nodes.find(this->prologue->getId())
				);

		this->prologue->setNext(nullptr);
	}

	if(nextNode == nullptr)
	{
		nextNode = this->epilogue;
		if(nextNode != nullptr)
		while(nextNode->getPrev() != nullptr)
		{
			nextNode = nextNode->getPrev();
		}
	}
	else
	{
	    nextNode->setPrev(value.get());
	    value->setNext(nextNode);
	}

	this->nodes[value->getId()] = value.get();
	this->prologue = value.release();

	// what happens if the node has no TAG_PLACE?
	if( this->prologue->containsTag(CNode::TAG_PLACE) == false )
	{
		Tag* tag = new Tag(CNode::TAG_PLACE, Convert::toString(0));
		this->prologue->addTag(tag);
	}
	else
	{
		this->prologue->getTag(CNode::TAG_PLACE).setValue(
	    		Convert::toString(0));
	}
	
	// add node to slice
	this->slice.append(*this->prologue);
}

void CSubGraph::setEpilogue(unique_ptr<CNode>& value)
{
    // a new epilogue cannot be set if the current epilogue
    // is not detached from the previous node
	if(value->getPrev() != nullptr)
	{
		throw Exception("You should have detached the 'prev' reference first.", LOCATION);
	}

	CNode* prevNode = nullptr;
	if(this->epilogue != nullptr)
	// remove the old epilogue
	{
		this->epilogue->detachOuterLabels();

		// save the prev reference (if present)
		prevNode = this->epilogue->getPrev();

		this->epilogue->detachInnerLabels();
		this->epilogue->detachNextAndPrev();

		this->nodes.erase(
			this->nodes.find(this->prologue->getId())
			);

	    this->epilogue->setPrev(nullptr);
	}

	if(prevNode == nullptr)
	{
		prevNode = this->prologue;
		if(prevNode != nullptr)
		while(prevNode->getNext() != nullptr)
		{
			prevNode = prevNode->getNext();
		}
	}
	else
	{
	    prevNode->setNext(value.get());
	    value->setPrev(prevNode);
	}

	// set the new epilogue
	this->nodes[value->getId()] = value.get();
	this->epilogue = value.release();

	// what happens if the tag place is not there?
	if( this->epilogue->containsTag(CNode::TAG_PLACE) == false)
	{
		this->epilogue->addTag(CNode::TAG_PLACE,
			Convert::toString(this->getSize() - 1));
	}
	else
	{
		this->epilogue->getTag(CNode::TAG_PLACE).setValue(
	    		Convert::toString(this->getSize() - 1));
	}
	
	// add node to slice
	this->slice.append(*this->epilogue);
}

#if 0
// Additional information regarding the individual, in order to
// compute a more accurate entropy value
void CSubGraph::setMessageInformation()
{
	unsigned int index = 0;
	unsigned int maxTuples = 3;

	// info on the order of the nodes
    for( CNode* cursor = &this->getPrologue(); cursor != nullptr;
		cursor = cursor->getNext() )
    {
        this->message.push_back( Hashable::djbHash(cursor->getHashCode(ENTROPY), index) );
		index++;
    }

	// in the variable vector_size, we store the index of the last hash value
	// computed only on the value of the node and on its position in the subgraph
	unsigned int vector_size = index;

	// here is the info regarding the tuples (or n-uples)
	for(unsigned int maxOffset = 1; maxOffset < maxTuples; maxOffset++)
	for(unsigned int i = 0; (i + maxOffset) < vector_size; i++)
	{
		hash_t hashCode = this->message.at(i);

		// we consider the maxOffset values starting from i; e.g. if maxOffset
		// is 2, we'll obtain an hash of values i, i+1, i+2 (thus considering
		// a set of three values)
		for(unsigned int j = i+1; j <= i + maxOffset; j++)
			hashCode = Hashable::djbHash( hashCode, this->message.at(j) );

		this->message.push_back(hashCode);
	}

	return;
}

// getter
vector<hash_t> CSubGraph::getMessageInformation()
{
	// if the message of the subgraph has not been computed yet,
	// it will be computed now
	if( this->isMessageSet == false )
	{
		this->setMessageInformation2();
		this->isMessageSet = true;
	}

	return this->message;
}

// setter 2
void CSubGraph::setMessageInformation2()
{
	// Just putting all the information from the single nodes
	// into vector CSubGraph::message
	for( CNode* cursor = &this->getPrologue(); cursor != nullptr; cursor = cursor->getNext() )
    	{
        	this->message.push_back( cursor->getHashCode(ENTROPY) );
    	}

	return;
}

// get a hash map with corresponance between symbol and its hash
map<hash_t,string> CSubGraph::getMessageMap()
{
	map<hash_t,string> symbolMap;

	for( CNode* cursor = &this->getPrologue(); cursor != nullptr; cursor = cursor->getNext() )
    	{
        IdentityRelabeller relabeller;
		ostringstream ss;
		cursor->writeExternalRepresentation(ss, relabeller);
		
		// if the symbol is empty, replace it with \"\"
		if(ss.str().size() > 0)
        		symbolMap[cursor->getHashCode(ENTROPY)] = ss.str();
		else
			symbolMap[cursor->getHashCode(ENTROPY)] = "\"\"";
    	}
	
	return symbolMap;
}
#endif
