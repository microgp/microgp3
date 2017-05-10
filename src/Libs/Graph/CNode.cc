/***********************************************************************\
|                                                                       |
| CNode.cc                                                              |
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
#include "CNode.h"

using namespace std;
using namespace ugp3;
using namespace tgraph;
using namespace ugp3::constraints;
using namespace ugp3::ctgraph;
using namespace xml;


InfinityString CNode::uniqueTagGenerator;

const string CNode::Escape = "$";
const string CNode::TAG_PLACE = "place";
const string CNode::TAG_NEXT = "next";
const string CNode::TAG_PREV = "prev";

CNode::CNode()
    : Node(),
    parentContainer(nullptr), next(nullptr), prev(nullptr)
{ }

CNode::CNode(const CNode& node)
    : CNode()
{
    _STACK;

    //Do not use the copy constructor of CNode: use CNode::clone instead.
    Assert(false);
}

void CNode::buildRandom()
{
    _STACK;

	if(this->getConstrain() == nullptr)
	{
		throw Exception("Constrain not found", LOCATION);
	}

	LOG_DEBUG << "Creating a new CNode for the macro " << this->getConstrain() << ends;

    for(unsigned int i = 0; i < this->getGenericMacro().getParameterCount(); i++)
    {
        const Parameter& parameter = this->getGenericMacro().getParameter(i);

        const DataParameter* dataParameter = dynamic_cast<const DataParameter*>(&parameter);
        if(dataParameter != nullptr)
        {
	    LOG_DEBUG << "Inserting a new DataParameter" << ends;
            this->addTag(CNode::Escape + parameter.getName(), dataParameter->randomize());
        }
        else if(dynamic_cast<const UniqueTagParameter*>(&parameter) != nullptr)
        {
            LOG_DEBUG << "Inserting a new UniqueTagParameter with unique ID" << ends;

            CNode::uniqueTagGenerator++;
            this->addTag(CNode::Escape + parameter.getName(), CNode::uniqueTagGenerator.toString());
        }
        else if(dynamic_cast<const SelfRefParameter*>(&parameter) != nullptr)
        {
            LOG_DEBUG << "Inserting a new SelfRefParameter" << ends;

            this->addTag(CNode::Escape + parameter.getName(), this->getId());
        }
        else
        {
            LOG_DEBUG << "Inserting a new StructuralParameter " <<  ends;

            // do not choose the target node now.
            const StructuralParameter* structuralParameter = dynamic_cast<const StructuralParameter*>(&parameter);
            Assert(structuralParameter != nullptr);
            this->addFloatingEdge(*structuralParameter);
        }
    }

    LOG_DEBUG << "CNode " << this << " created" << ends;
}

Edge* CNode::addFloatingEdge(const StructuralParameter& parameter)
{
    _STACK;

    // check that there is not already another edge associated to the parameter
    Assert(this->getEdge(parameter.getName()) == nullptr);

	Assert(this->getGenericMacro().getParameter(parameter.getName()) == &parameter);

    if( dynamic_cast<const InnerLabelParameter*>(&parameter) == nullptr &&
        dynamic_cast<const OuterLabelParameter*>(&parameter) == nullptr)
    {
        throw ArgumentException("The specified parameter is not an inner nor an outer label.", LOCATION);
    }

    Edge* edge = this->addEdgeTo(nullptr);
    Assert(edge != nullptr);

    edge->addTag(Edge::parameterTagName, parameter.getName());

    return edge;
}

void CNode::clear()
{
    _STACK;

	Taggable::clear();
}

bool CNode::validate() const
{
    _STACK;

	if(this->getConstrain() == nullptr)
	{
		LOG_WARNING << "Nothing to validate" << ends;
		return true;
	}

    LOG_DEBUG << "Node " << this << ": validating" << ends;

    if(this->initialized() == false)
    {
        LOG_VERBOSE << "Node " << this << ": not initialized" << ends;
        return false;
    }

    // validate tags
    for(unsigned int i = 0; i < getTagCount(); i++)
    {
        const Tag* tag = this->getTag(i);


        if(tag->getName() == CNode::TAG_PLACE)
            continue;

        LOG_DEBUG << "Node " << this << ": validating tag " << tag << "." <<  ends;
        string tagName = tag->getName();
        // Delete leading CNode::Escape
        tagName.erase(0, 1);

        const Parameter* parameter = this->getGenericMacro().getParameter(tagName);
        if(parameter == nullptr)
        {
            LOG_VERBOSE << "Node " << this << ": no parameter \"" << tagName << "\" is associated to the macro" << ends;
            return false;
        }

        const DataParameter* dataParameter = dynamic_cast<const DataParameter*>(parameter);
        if(dataParameter != nullptr)
        {
            if(dataParameter->validate(tag->getValue()) == false)
            {
                LOG_VERBOSE << "Node " << this << ": the parameter " << dataParameter << " has not a valid value ('" << tag->getValue()<< "')" << ends;
                return false;
            }
        }
    }

    // check if there are floating edges
    for(unsigned int i = 0; i < this->getGenericMacro().getParameterCount(); i++)
    {
        const Parameter& parameter = this->getGenericMacro().getParameter(i);
        if(dynamic_cast<const DataParameter*>(&parameter) == nullptr && dynamic_cast<const UniqueTagParameter*>(&parameter) == nullptr && dynamic_cast<const SelfRefParameter*>(&parameter) == nullptr)
        {
             Edge* edge = this->getEdge(parameter.getName());
             if(edge->getTo() == nullptr)
             {
                 LOG_VERBOSE
                     << "Node " << this << ": not initialized: the edge for the label "
                    << parameter << " is floating" << ends;

                 return false;
             }
        }
    }

    // validate edges
    for(unsigned int i = 0; i < getEdgeCount(); i++)
    {
        CSubGraph* subGraph = dynamic_cast<CSubGraph*>(this->parentContainer);
        if(subGraph == nullptr)
        {
            LOG_VERBOSE << "Node " << this << ": the node is not part of a subGraph therefore it should not have inner or outer labels" << ends;
            return false;
        }

        Edge& edge = Node::getEdge(i);
        Assert(dynamic_cast<CNode*>(edge.getTo())!=nullptr);

        CNode* target = (CNode*)(edge.getTo());
        if (edge.containsTag(Edge::parameterTagName))
        {
            const Parameter* parameter = this->getGenericMacro().getParameter(edge.getTag(Edge::parameterTagName).getValue());
            const InnerLabelParameter* innerLabel = dynamic_cast<const InnerLabelParameter*>(parameter);
            const OuterLabelParameter* outerLabel = dynamic_cast<const OuterLabelParameter*>(parameter);

            if(innerLabel != nullptr)
            {
                if(target->getParentContainer() != this->getParentContainer())
                // the two nodes don't belong to the same graph
                {
                    LOG_DEBUG << "Node " << this << ": the target of the inner label " << parameter << " does not belong to the same subgraph" << ends;
                    return false;
                }
                else if(target->representsPrologue() == true && innerLabel->getPrologueIsValid() == false)
                {
                    LOG_DEBUG << "Node " << this << ": the target of the inner label " << parameter << " is the prologue while it should not" << ends;
                    return false;
                }
                else if(target->representsEpilogue() == true && innerLabel->getEpilogueIsValid() == false)
                {
                    LOG_DEBUG << "Node " << this << ": the target of the inner label " << parameter << " is the epilogue while it should not" << ends;
                    return false;
                }
                else if(target == this && innerLabel->getItselfIsValid() == false)
                {
                    LOG_DEBUG << "Node " << this << ": the target of the inner label " << parameter << " is the node itself while it should not" << ends;
                    return false;
                }
                else if(innerLabel->getItselfIsValid() == true && subGraph->isBefore(*target, *this) && innerLabel->getBackwardJumpIsValid() == false)
                {
                    LOG_DEBUG << "Node " << this << ": the inner label " << parameter << " represents a backward label (including self)" << ends;
                    return false;
                }
                else if(innerLabel->getItselfIsValid() == false && subGraph->isStrictlyBefore(*target, *this) && innerLabel->getBackwardJumpIsValid() == false)
                {
                    LOG_DEBUG << "Node " << this << ": the inner label " << parameter << " represents a strict backward label" << ends;
                    return false;
                }
                else if(innerLabel->getItselfIsValid() == true && subGraph->isBefore(*this, *target) && innerLabel->getForwardJumpIsValid() == false)
                {
                    LOG_DEBUG << "Node " << this << ": the inner label " << parameter << " represents a forward label (including self)" << ends;
                    return false;
                }
                else if(innerLabel->getItselfIsValid() == false && subGraph->isStrictlyBefore(*this, *target) && innerLabel->getForwardJumpIsValid() == false)
                {
                    LOG_DEBUG << "Node " << this << ": the inner label " << parameter << " represents a strict forward label" << ends;
                    return false;
                }
            }
            else if(outerLabel != nullptr)
            {
                if(target->representsPrologue() == false)
                {
                    LOG_DEBUG << "Node " << this << ": the target for the outer label " << outerLabel << " does not represent a prologue" << ends;
                    return false;
                }

		        const CSubGraph* targetSubGraph = dynamic_cast<const CSubGraph*>(target->getParentContainer());

                const CGraph* graph = dynamic_cast<const CGraph*>(targetSubGraph->getParentContainer());
                const SubSection *ss = dynamic_cast<const SubSection*>(target->getGenericMacro().getParent());
                const string& labelTarget = graph->getSection().getId() + " " + ss->getId();

                if(outerLabel->validate(labelTarget) == false)
                {
                    LOG_DEBUG << "Node " << this << ": the target \"" << labelTarget << "\" for the outer label " << outerLabel << " is not valid" << ends;
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }

    LOG_DEBUG << "Node " << this << ": the node is valid" << ends;
    return true;
}

void CNode::writeExternalRepresentation( ostream& stream, Relabeller& relabeller ) const
{
    _STACK;

	if(getConstrain() == nullptr)
	    throw Exception("Constrain not available.", LOCATION);

    LOG_DEBUG << "Dumping object ugp3::ctgraph::CNode " << this << ends;

    if(getBackEdgeCount() > 0)
    // Referenced node. Print label
    {
        bool expand = false;
         if(dynamic_cast<const CSubGraph*>(this->getParentContainer()) != nullptr)
        {
            const CSubGraph* parent = (const CSubGraph*)this->getParentContainer();
            expand = parent->getSubSection().getExpand();
        }

        // if no expansion, then print. otherwise print label only if there is at least one inner label
        if(expand == false
        || (expand == true && getBackEdgeCount() > countIncomingOuterLabels()) )
        {
        	const string& identifier = this->getConstrain()->getIdentifierFormat()->get(relabeller.translate(this->getId()));
        	const string& label = this->getConstrain()->getLabelFormat()->get(identifier);

        	stream << label;
        }
    }


    for( unsigned int i=0; i< this->getGenericMacro().getExpression().getElements().size(); i++ )
    {
        const ExpressionElement *currentElement = this->getGenericMacro().getExpression().getElements().at(i);

        if( currentElement->getParameter() != nullptr)
       	// this is a parameter
        {
            const Parameter* parameter = currentElement->getParameter();
            LOG_DEBUG << "Writing parameter " << parameter << "" << ends;

            if(dynamic_cast<const DataParameter*>(parameter) != nullptr)
            {
                const Tag& parameterTag = this->getTag(CNode::Escape + parameter->getName());
                stream << parameterTag.getValue();
            }
            else
            {
                string value = "";
                if(dynamic_cast<const UniqueTagParameter*>(parameter) != nullptr)
                {
                    const Tag& parameterTag = this->getTag(CNode::Escape + parameter->getName());

                    value = parameterTag.getValue();

                    const string& identifier = this->getConstrain()->getIdentifierFormat()->get(value);
                	const string& uniqueTag = this->getConstrain()->getUniqueTagFormat()->get(identifier);
				    stream << uniqueTag;
                }
                else if(dynamic_cast<const SelfRefParameter*>(parameter) != nullptr)
                {
                    const Tag& parameterTag = this->getTag(CNode::Escape + parameter->getName());

                    value = parameterTag.getValue();

                    const string& identifier = this->getConstrain()->getIdentifierFormat()->get(value);
				    stream << identifier;
                }
                else if(dynamic_cast<const InnerLabelParameter*>(parameter) != nullptr)
                {
                    Edge* edge = this->getEdge(parameter->getName());

                    Assert(edge != nullptr);
                    Assert(edge->getTo() != nullptr);
                    Assert(dynamic_cast<CNode*>(edge->getTo()) != nullptr);

                    CNode* node = (CNode*) edge->getTo();
                    value = relabeller.translate(node->getId());

                    const string& identifier = this->getConstrain()->getIdentifierFormat()->get(value);
				    stream << identifier;
                }
                else if(dynamic_cast<const OuterLabelParameter*>(parameter) != nullptr)
                {
                    Edge* edge = this->getEdge(parameter->getName());

                    Assert(edge != nullptr);
                    Assert(edge->getTo() != nullptr);
                    Assert(dynamic_cast<CNode*>(edge->getTo()) != nullptr);

                    CNode* node = (CNode*) edge->getTo();
                    Assert(dynamic_cast<const CSubGraph*>(node->getParentContainer()) != nullptr);
                    const CSubGraph* targetParent = (const CSubGraph*)node->getParentContainer();
                    if(targetParent->getSubSection().getExpand() == true)
                    {

                            targetParent->writeExternalRepresentation(stream, relabeller);
                    }
                    else
                    {
                        value = relabeller.translate(node->getId());

                        const string& identifier = this->getConstrain()->getIdentifierFormat()->get(value);
				        stream << identifier;
                    }
                }
            }
        }
        else
        {
            LOG_DEBUG << "Writing token \"" << *currentElement->getCharacters() << "\"" << ends;
            stream << *currentElement->getCharacters();
        }
    }
}

bool CNode::initialized() const
{
    _STACK;

    if(this->parentContainer == nullptr)
    {
        LOG_VERBOSE << "The node " << this << " is not initialized: it does not have a parent container" << ends;
        return false;
    }

    for(unsigned int i = 0; i < this->getGenericMacro().getParameterCount(); i++)
    {
        const Parameter& parameter = this->getGenericMacro().getParameter(i);
        if(dynamic_cast<const DataParameter*>(&parameter) != nullptr
            || dynamic_cast<const UniqueTagParameter*>(&parameter) != nullptr
            || dynamic_cast<const SelfRefParameter*>(&parameter) != nullptr)
        {
            // retrieve the tags associated to the parameter
            if(this->containsTag(CNode::Escape + parameter.getName()) == false)
            {
                LOG_VERBOSE
                    << "The node " << this << " is not initialized: it does not have the tag for the parameter "
                    << parameter << ends;
                return false;
            }
        }
        else
        {
             Edge* edge = this->getEdge(parameter.getName());
             if(edge == nullptr)
             {
                 LOG_VERBOSE
                    << "The node " << this << " is not initialized: it does not have the edge for the label "
                    << parameter << ends;

                 return false;
             }


             /* the node MAY have floating edges, skip the check */
        }
    }

    return true;
}

bool CNode::equals(const CNode& otherNode) const
{
    _STACK;

    LOG_DEBUG << "Node " << this << ": making a comparison with node " << otherNode << ends;

    if(this == &otherNode)
    {
        LOG_DEBUG << "Node " << this << ": the node is being compared to itself" << ends;
        return true;
    }

	if(this->getConstrain() == nullptr || otherNode.getConstrain() == nullptr)
		throw Exception("Constrain is missing.", LOCATION);

#ifdef NDEBUG
    if(this->getHashCode(GENOTYPE) != otherNode.getHashCode(GENOTYPE))
    {
        LOG_DEBUG << "Node " << this << ": the hash code is different from the one of the node " << otherNode << ends;
        return false;
    }
#endif

    if(this->initialized() == false || otherNode.initialized() == false)
    {
        throw ArgumentException("One of the two nodes is not initialized.", LOCATION);
    }

    if(this->getConstrain() != otherNode.getConstrain())
    // if the nodes refer to different macros then they are different
    {
        LOG_DEBUG << "Node " << this << ": the node represents a macro that is not the same of the one represented by the node " << otherNode << ends;
        return false;
    }

    for(unsigned int i = 0; i < this->getGenericMacro().getParameterCount(); i++)
        // compare all the parameters
    {
        // get the i-th parameter
        const Parameter& parameter = this->getGenericMacro().getParameter(i);
        LOG_DEBUG << "Comparing parameter " << parameter << ends;

        if(dynamic_cast<const DataParameter*>(&parameter) != nullptr)
        {
            // retrieve the tags associated to the parameter
            const string tagName = CNode::Escape + parameter.getName();

            if(this->containsTag(tagName) == false || otherNode.containsTag(tagName) == false)
            // the parameter tag is missing
            {
                throw Exception("One of the two nodes is missing the tag for the parameter " + parameter.toString() + ".", LOCATION);
            }

            if(this->getTag(tagName).getValue() != otherNode.getTag(tagName).getValue())
            // just compare the tags
            {
                LOG_DEBUG << "Node " << this << ": its tag " << tagName << " has not the same value of the one oh the node " << otherNode << ends;
                return false;
            }
        }
        else if(dynamic_cast<const UniqueTagParameter*>(&parameter) == nullptr &&
            dynamic_cast<const SelfRefParameter*>(&parameter) == nullptr)
        {
            Edge* thisEdge = this->getEdge(parameter.getName());
            Edge* otherEdge = otherNode.getEdge(parameter.getName());
            if(thisEdge == nullptr || otherEdge == nullptr)
            {
                throw Exception("One of the two nodes is missing the edge for the parameter " + parameter.toString() + ".", LOCATION);
            }

            // get the target nodes
            Assert(dynamic_cast<CNode*>(thisEdge->getTo()) != nullptr);
            Assert(dynamic_cast<CNode*>(otherEdge->getTo()) != nullptr);

            CNode* thisTarget = (CNode*) thisEdge->getTo();
            CNode* otherTarget = (CNode*) otherEdge->getTo();


            // get the parent subgraphs of the two nodes
            CSubGraph* thisSubGraph = dynamic_cast<CSubGraph*>(this->parentContainer);
            CSubGraph* otherSubGraph = dynamic_cast<CSubGraph*>(otherNode.parentContainer);
            if(thisSubGraph == nullptr || otherSubGraph == nullptr)
            {
                throw Exception("One of the two nodes is not the child of a subGraph: the inner/outer label cannot be compared.", LOCATION);
            }

            const InnerLabelParameter* innerLabel = dynamic_cast<const InnerLabelParameter*>(&parameter);
            const OuterLabelParameter* outerLabel = dynamic_cast<const OuterLabelParameter*>(&parameter);
            if(innerLabel != nullptr)
            {
                // compare the offsets of the inner label
                // do not compare the target nodes or a recursion may occur
                if(thisSubGraph->getOffset(*this, *thisTarget) != otherSubGraph->getOffset(otherNode, *otherTarget))
                {
                    LOG_DEBUG << "Inner labels " << innerLabel << " have different target offset" << ends;
                    return false;
                }
            }
            else if(outerLabel != nullptr)
            {
                LOG_DEBUG << "Comparing targets for outer labels " << outerLabel << ends;
                // TODO find a way to compare
                // For now we always consider individuals with outerlabel different
                return false;
            }
        }
    }

    LOG_DEBUG << "Node " << this << ": is equal to node " << otherNode << ends;
    return true;
}

bool CNode::randomize()
{
    _STACK;

    if(this->initialized() == false)
    {
        throw Exception("The node is not properly initialized.", LOCATION);
    }

    for( unsigned int i = 0; i < this->getGenericMacro().getParameterCount(); i++ )
    {
        bool success = this->randomizeParameter(i);
        if(success == false)
        {
            return false;
        }
    }

    return true;
}

bool CNode::randomizeParameter(unsigned int i)
{
	_STACK;

	Assert(this->parentContainer != nullptr);
	if(this->getConstrain() == nullptr)
	{
		throw Exception("Constrain is missing.", LOCATION);
	}

	const GenericMacro& macro = this->getGenericMacro();
	if(i >= macro.getParameterCount() )
	{
		throw IndexOutOfBoundsException("macro::getParameter", LOCATION);
	}

	const Parameter& parameter = macro.getParameter(i);
	LOG_DEBUG << "Generating a random value for parameter " << parameter << ends;

	const DataParameter* dataParameter = dynamic_cast<const DataParameter*>(&parameter);
	if(dataParameter != nullptr)
	{
		Tag& parameterTag = this->getTag(CNode::Escape + parameter.getName());

		// Request a random value for the parameter
		parameterTag.setValue(dataParameter->randomize());
	}
	else if(dynamic_cast<const UniqueTagParameter*>(&parameter) == nullptr && dynamic_cast<const SelfRefParameter*>(&parameter) == nullptr)
	// it is a label: change its target
	{
		Assert(this->parentContainer != nullptr);

		// get the edge associated to the label
		Edge* edge = this->getEdge(parameter.getName());
		Assert(edge != nullptr); // the node must be properly initialized

		// in fact, it should check whether the parameter has the tag "target" or "offset"...
		LOG_DEBUG 	<< "Edge is going to be randomized. Currently edge points to \"" << edge->getTo() 
				<< "\" and has target \"" << edge->toString() << "\"." << ends;

		// the edge contains the target name and/or offset so, remove them
		edge->setTo(nullptr); // but first, set the edge as "floating"

		if(edge->containsTag(Edge::targetTagName))
		{
			edge->removeTag(Edge::targetTagName);
			LOG_DEBUG << "Removing \"" << Edge::targetTagName << "\"..." << ends;
		}

		if(edge->containsTag(Edge::offsetTagName))
		{
			edge->removeTag(Edge::offsetTagName);
			LOG_DEBUG << "Removing \"" << Edge::offsetTagName << "\"..." << ends;
		}
	
		/*  DO NOT compute a new target here, it will be randomly assigned when CSubGraph::attachFloatingEdge() is called */
	}

	LOG_DEBUG << "Parameter " << parameter << " randomized" << ends;
	return true;
}

bool CNode::randomizeParameter()
{
    _STACK;

    if(this->getGenericMacro().getParameterCount() > 0)
    {
        unsigned long randomSample = Random::nextUInteger(0, this->getGenericMacro().getParameterCount() - 1);
        return this->randomizeParameter(randomSample);
    }

    LOG_VERBOSE << "Node " << this << ": no parameters to randomize" << ends;
    return false;
}

const string CNode::toString() const
{
    _STACK;

    const string& value = "{" + this->getId();
    if(this->representsPrologue() == true)
    {
        return value + ", p}";
    }
    else if(this->representsEpilogue() == true)
    {
        return value + ", e}";
    }

    return value + "}";
}

void CNode::detachOuterLabels()
{
    _STACK;

	if(this->getConstrain() == nullptr) throw Exception("Constrain missing.", LOCATION);

    LOG_DEBUG << "Node " << this << ": detaching outer labels" << ends;

    // foreach outer label parameter ...
    for(unsigned int i = 0; i < this->getGenericMacro().getParameterCount(); i++)
    {
        const Parameter& parameter = this->getGenericMacro().getParameter(i);

        const OuterLabelParameter* outerLabel = dynamic_cast<const OuterLabelParameter*>(&parameter);
        if(outerLabel == nullptr)
        // detach only outer labels
        {
            continue;
        }

        this->detachOuterLabel(*outerLabel);
    }

     // collect all incoming outer labels
    vector<pair<CNode*, const OuterLabelParameter*> > edgesToDetach;
    for(unsigned int i = 0; i < this->getBackEdgeCount(); i++)
    {
        Edge& incomingEdge = this->getBackEdge(i);

        // retrieve the parameter name
        if(incomingEdge.containsTag(Edge::parameterTagName) == false)
            continue;

        const string& parameterName = incomingEdge.getTag(Edge::parameterTagName).getValue();

        // get the source node of the incoming edge
        Assert(dynamic_cast<CNode*>(&incomingEdge.getFrom()) != nullptr);
        CNode& sourceNode = (CNode&) incomingEdge.getFrom();

        // retrieve the parameter
        const Parameter* parameter = sourceNode.getGenericMacro().getParameter(parameterName);
        Assert(parameter != nullptr);

        // check if it is an inner label
        const OuterLabelParameter* outerLabel = dynamic_cast<const OuterLabelParameter*>(parameter);
        if(outerLabel == nullptr)
            continue;

        edgesToDetach.push_back(pair<CNode*, const OuterLabelParameter*>(&sourceNode, outerLabel));
    }

    // detach all incoming outer labels
    while(edgesToDetach.size() > 0)
    {
        CNode* node = edgesToDetach[0].first;
        const OuterLabelParameter* outerLabel = edgesToDetach[0].second;

        edgesToDetach.erase(edgesToDetach.begin());

        node->detachOuterLabel(*outerLabel);
    }
}

void CNode::detachInnerLabels()
{
    _STACK;

    LOG_DEBUG << "Node " << this << ": detaching inner labels" << ends;

    // for each inner label of the node, retrieve and save the offset
    for(unsigned int i = 0; i < this->getGenericMacro().getParameterCount(); i++)
    {
        const Parameter& parameter = this->getGenericMacro().getParameter(i);

        const InnerLabelParameter* innerLabel = dynamic_cast<const InnerLabelParameter*>(&parameter);
        if(innerLabel == nullptr)
            continue;

        this->detachInnerLabel(*innerLabel);
    }

    // detach incoming edges


    // collect all incoming inner labels
    vector<pair<CNode*, const InnerLabelParameter*> > edgesToDetach;
    for(unsigned int i = 0; i < this->getBackEdgeCount(); i++)
    {
        Edge& incomingEdge = this->getBackEdge(i);

        // retrieve the parameter name
        if(incomingEdge.containsTag(Edge::parameterTagName) == false) continue;

        const string& parameterName = incomingEdge.getTag(Edge::parameterTagName).getValue();

        // get the source node of the incoming edge
        Assert(dynamic_cast<CNode*>(&incomingEdge.getFrom()) != nullptr);
        CNode& sourceNode = (CNode&) incomingEdge.getFrom();


        // retrieve the parameter
        const Parameter* parameter = sourceNode.getGenericMacro().getParameter(parameterName);
        Assert(parameter != nullptr);

        // check if it is an inner label
        const InnerLabelParameter* innerLabel = dynamic_cast<const InnerLabelParameter*>(parameter);
        if(innerLabel == nullptr) continue;

        LOG_DEBUG << "Node " << this << ": detaching incoming inner label " << innerLabel<< " from node " << sourceNode << ends;
        edgesToDetach.push_back(pair<CNode*, const InnerLabelParameter*>(&sourceNode, innerLabel));
    }

    // detach all incoming inner labels
    while(edgesToDetach.size() > 0)
    {
        CNode* node = edgesToDetach[0].first;
        const InnerLabelParameter* innerLabel = edgesToDetach[0].second;

        edgesToDetach.erase(edgesToDetach.begin());

        node->detachInnerLabel(*innerLabel);
    }
}

void CNode::detachOuterLabel(const OuterLabelParameter& outerLabel)
{
    _STACK;

    const CSubGraph* subGraph = dynamic_cast<const CSubGraph*>(this->parentContainer);

    if(subGraph == nullptr) throw Exception("Only outer labels of nodes of sub-graphs can be detached.", LOCATION);

    LOG_DEBUG << "Node " << this << ": detaching outer label " << outerLabel << ends;

    // get the edge associated to the parameter
    Edge* edge = this->getEdge(outerLabel.getName());
    Assert(edge != nullptr);

    if(edge->getTo() == nullptr)
    // edge is already floating
    {
        return;
    }

    // get the target node
    Assert(dynamic_cast<CNode*>(edge->getTo()) != nullptr);
    CNode* targetNode = (CNode*) edge->getTo();

    Assert(targetNode->representsPrologue() == true);

    // The current tags will be added to the edge:
    //   - "floating"
    //   - "target: <old_target_id>"
    //   - "parameter: <outer_label_name>"
    //   - "section: <target_section_name>"
    //   - "offset: <index_of_subgraph>"
	edge->setTo(nullptr);
	edge->addTag(Edge::targetTagName, targetNode->getId());

    // save the position of the subSection in the list of subSections
    Assert(dynamic_cast<const Section*>(subGraph->getSubSection().getParent()) != nullptr);
    const Section* section = (const Section*) subGraph->getSubSection().getParent();


    for(unsigned int pos = 0; pos < section->getSubSectionCount(); pos++)
    {
        if(&subGraph->getSubSection() == &section->getSubSection(pos))
        {
            edge->addTag(Edge::offsetTagName, Convert::toString(pos));

            // save the path "section"
            edge->addTag("section", section->getId());
            break;
        }
    }
}

void CNode::detachNextAndPrev()
{
    _STACK;

    LOG_DEBUG << "Detaching next and prev references" << ends;

    if(this->next != nullptr)
    {
        this->next->prev = nullptr;
    }

    if(this->prev != nullptr)
    {
        this->prev->next = nullptr;
    }

    if(this->next != nullptr && this->prev != nullptr)
    {
        this->prev->next = this->next;
        this->next->prev = this->prev;
    }

    this->prev = nullptr;
    this->next = nullptr;
}

void CNode::detachInnerLabel(const InnerLabelParameter& innerLabel)
{
    _STACK;

    Assert(this->getGenericMacro().getParameter(innerLabel.getName()) == &innerLabel);

    LOG_DEBUG << "Node " << this << ": detaching inner label " << innerLabel << ends;

    Assert(dynamic_cast<const CSubGraph*>(this->parentContainer) != nullptr);
    const CSubGraph* subGraph = (const CSubGraph*) this->parentContainer;


    // get the edge associated to the inner label
    Edge* edge = this->getEdge(innerLabel.getName());
    Assert(edge != nullptr);

    // skip it if it is already floating
    if(edge->getTo() == nullptr)
        return;

    // get the target node instance
    Assert(dynamic_cast<CNode*>(edge->getTo()) != nullptr);
    CNode* targetNode = (CNode*) edge->getTo();


    this->removeEdge(edge);
    targetNode->removeBackEdge(edge);
    delete edge;
    edge = nullptr;

    // add a new floating edge with the offset information
    int offset = subGraph->getOffset(*this, *targetNode);
    Edge* newEdge = this->addFloatingEdge(innerLabel);
    newEdge->addTag(Edge::targetTagName, targetNode->getId());
    newEdge->addTag(Edge::offsetTagName, Convert::toString(offset));
}

unique_ptr<CNode> CNode::clone() const
{
    _STACK;

    if(parentContainer == nullptr)
    {
        throw ArgumentNullException("parentContainer", LOCATION);
    }

    LOG_DEBUG << "Node " << this << ": cloning" << ends;

    if(this->initialized() == false)
    {
        throw Exception("The node " + this->toString() + " is not properly initialized.", LOCATION);
    }

    // create a new node
    CNode* node = new CNode();
    node->parentContainer = nullptr;
	if(this->getConstrain() != nullptr)
	{
		node->setConstrain(*this->getConstrain());
	}

    for(unsigned int i = 0; i < this->getGenericMacro().getParameterCount(); i++)
    {
        const Parameter& parameter = this->getGenericMacro().getParameter(i);
        LOG_DEBUG << "Node " << this << ": cloning parameter " << parameter << ends;

        const StructuralParameter* structuralParameter = dynamic_cast<const StructuralParameter*>(&parameter);
        if(dynamic_cast<const DataParameter*>(&parameter) != nullptr)
        {
            Tag& tag = this->getTag(CNode::Escape + parameter.getName());
            node->addTag(tag.getName(), tag.getValue());
        }
        else if(dynamic_cast<const SelfRefParameter*>(&parameter) != nullptr)
        {
            Tag& tag = this->getTag(CNode::Escape + parameter.getName());
            node->addTag(tag.getName(), node->getId());
        }
        else if(dynamic_cast<const UniqueTagParameter*>(&parameter) != nullptr)
        {
            Tag& tag = this->getTag(CNode::Escape + parameter.getName());

            CNode::uniqueTagGenerator++;
            node->addTag(tag.getName(), CNode::uniqueTagGenerator.toString());
        }
        else if(dynamic_cast<const InnerLabelParameter*>(&parameter) != nullptr)
        {
            // get the edge associated to the label
            Edge* edge = this->getEdge(parameter.getName());
            Assert(edge != nullptr);

            // the node MAY have floating edges
            if(edge->getTo() != nullptr)
            {
                // get the target node
                CNode* targetNode = dynamic_cast<CNode*>(edge->getTo());
                Assert(targetNode != nullptr && targetNode->getParentContainer() == this->getParentContainer());

                Assert(dynamic_cast<CSubGraph*>(this->parentContainer) != nullptr);
                CSubGraph* subGraph = (CSubGraph*) this->parentContainer;


                // compute the offset to the target node
                int offset = subGraph->getOffset(*this, *targetNode);


                // The current tags will be added to the edge:
                //   - "target: <old_target_id>"
                //   - "offset: <offset_to_target>"
                //   - "parameter: <inner_label_name>"
                Edge* newEdge = node->addFloatingEdge(*structuralParameter);
                newEdge->addTag(Edge::offsetTagName, Convert::toString(offset));
                newEdge->addTag(Edge::targetTagName, targetNode->getId());
            }
            else
            // the edge is floating
            {
                Edge* newEdge = node->addFloatingEdge(*structuralParameter);

                if(edge->containsTag(Edge::offsetTagName))
                {
                    const Tag& offsetTag = edge->getTag(Edge::offsetTagName);
                    newEdge->addTag(offsetTag.getName(), offsetTag.getValue());
                }

                if(edge->containsTag(Edge::targetTagName))
                {
                    const Tag& targetTag = edge->getTag(Edge::targetTagName);
                    newEdge->addTag(targetTag.getName(), targetTag.getValue());
                }
            }
        }
        else if(dynamic_cast<const OuterLabelParameter*>(&parameter) != nullptr)
        {
            // get the edge associated to the label
            Edge* edge = this->getEdge(parameter.getName());
            Assert(edge != nullptr);

            Edge* newEdge = node->addFloatingEdge(*structuralParameter);

            if(edge->getTo() != nullptr)
            // edge is not floating
            {
                // get the target node
                Assert(dynamic_cast<CNode*>(edge->getTo()) != nullptr);
                CNode* targetNode = (CNode*) edge->getTo();
                Assert(targetNode->representsPrologue() == true);


                // The current tags will be added to the edge:"
                //   - "target: <old_target_id>"
                //   - "offset: <subsection_index>"
                //   - "parameter: <outer_label_name>"

                newEdge->addTag(Edge::targetTagName, targetNode->getId());

                CSubGraph* sGraph = dynamic_cast<CSubGraph*>(targetNode->getParentContainer());
                CGraph* graph = dynamic_cast<CGraph*>(sGraph->getParentContainer());

                const string& sectionName = graph->getSection().getId();
                unsigned int subGraphOffset = 0;
                for(unsigned int ss = 0; ss < graph->getSubGraphCount(); ss++)
                {
                	if(&graph->getSubGraph(ss) == sGraph)
                	{
                		subGraphOffset = ss;
                	}
                }


                newEdge->addTag(Edge::offsetTagName, Convert::toString(subGraphOffset));
                newEdge->addTag("section", sectionName);
            }
            else
            // the edge is floating
            {
                // additional information must be present on the edge
                Assert(edge->containsTag("section"));
                Assert(edge->containsTag(Edge::offsetTagName));

                // copy the information
                Tag& sectionTag = edge->getTag("section");
                newEdge->addTag(sectionTag.getName(), sectionTag.getValue());

                Tag& offsetTag = edge->getTag(Edge::offsetTagName);
                newEdge->addTag(offsetTag.getName(), offsetTag.getValue());
            }
        }
    }

    LOG_DEBUG << "Node " << this << ": clone " << node << " successfully created" << ends;
    return unique_ptr<CNode>(node);
}

void CNode::setInfinityString(const string& value)
{
    _STACK;

	// update the id generator for the nodes

	// create a new infinity string from the specified value
    InfinityString newVal(value);

    Node::idCounter = newVal;
}

Edge* CNode::getEdge(const string& parameterName) const
{
    _STACK;

    for(unsigned int i = 0; i < this->getEdgeCount(); i++)
    {
        Edge& edge = Node::getEdge(i);

        if(edge.containsTag(Edge::parameterTagName))
        {
            const Tag& parameterTag = edge.getTag(Edge::parameterTagName);
            if(parameterTag.getValue() == parameterName)
            {
                return &edge;
            }
        }
    }

    return nullptr;
}

unsigned int CNode::countIncomingOuterLabels() const
{
	_STACK;

	unsigned int references = 0;
	for(unsigned int e = 0; e < this->getBackEdgeCount(); e++)
	{
		// follow the incoming edge
		const Edge& edge = this->getBackEdge(e);
		const CNode& sourceNode = (const CNode&)edge.getFrom();

		// look if it is associated to an outer label
		if(edge.containsTag(Edge::parameterTagName))
		{
			const string& parameterName = edge.getTag(Edge::parameterTagName).getValue();
			const Parameter* parameter = sourceNode.getGenericMacro().getParameter(parameterName);
			if(dynamic_cast<const OuterLabelParameter*>(parameter) != nullptr)
			{
				references++;
			}
		}
	}

	return references;
}

hash_t CNode::calculateHashCode(Purpose purpose) const
{
    _STACK;

    hash_t hashCode = startValue;

	// In order to avoid having the same hash values for different macros
	// with the same parameters' name, we'll use the macro ID in the hash as well
	hashCode = Hashable::djbHash( hashCode, this->getGenericMacro().getId() );

	LOG_DEBUG
		<< "Hash for macro " << this->getGenericMacro().getId() << " is "
		<< hashCode << ends;

    for(unsigned int i = 0; i < this->getGenericMacro().getParameterCount(); i++)
    {
        const Parameter& parameter = this->getGenericMacro().getParameter(i);
        // Added by Alberto Tonda 2008-03-26
		// Since hashing for symbols using parameters with real value is
		// meaningless, we try to quantize the possible values
		if(purpose == ENTROPY && dynamic_cast<const FloatParameter*>(&parameter) != nullptr)
		{
			FloatParameter *parameterFloat = (FloatParameter *) &parameter;
			double quantum, min, max;

			sscanf(parameterFloat->getMax().c_str(), "%lf", &max);
			sscanf(parameterFloat->getMin().c_str(), "%lf", &min);

			quantum = (max - min) / 42;

			const Tag& tag = this->getTag(CNode::Escape + parameter.getName());
			hashCode = Hashable::djbHash(hashCode, tag.toString(quantum));

			LOG_DEBUG << "Hashing float parameter " << tag.toString(quantum) << ends;
		}
		// end part added
		else if(dynamic_cast<const DataParameter*>(&parameter) != nullptr)
        {
            const Tag& tag = this->getTag(CNode::Escape + parameter.getName());
            hashCode = Hashable::djbHash(hashCode, tag.toString());

			LOG_DEBUG
				<< "Hashing parameter " << tag.toString() << ends;
        }
        // FIXME added in order to reduce collisions
        else if(dynamic_cast<const UniqueTagParameter*>(&parameter) == nullptr &&
            dynamic_cast<const SelfRefParameter*>(&parameter) == nullptr)
        {
            Edge* thisEdge = this->getEdge(parameter.getName());
            if (thisEdge) {

            // get the target nodes
            Assert(dynamic_cast<CNode*>(thisEdge->getTo()) != nullptr);

            CNode* thisTarget = (CNode*) thisEdge->getTo();

            // get the parent subgraphs of the two nodes
            CSubGraph* thisSubGraph = dynamic_cast<CSubGraph*>(this->parentContainer);
            if (thisSubGraph) {
            const InnerLabelParameter* innerLabel = dynamic_cast<const InnerLabelParameter*>(&parameter);
            const OuterLabelParameter* outerLabel = dynamic_cast<const OuterLabelParameter*>(&parameter);
            if (innerLabel) {
                hashCode = Hashable::djbHash(hashCode, "innerOffset=" + Convert::toString(thisSubGraph->getOffset(*this, *thisTarget)));
            }
            else if (outerLabel)
            {
                LOG_DEBUG << "Comparing targets for outer labels " << outerLabel << ends;
                // FIXME maybe we should do something here?
            }
            }
            }
        }
    }

    LOG_DEBUG
        << "HASH of Node " << this << " is "
        << Convert::toString(hashCode, Base::Hexadecimal) << ends;

     return hashCode;
}

