/***********************************************************************\
|                                                                       |
| CNode.h                                                               |
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
#ifndef HEADER_UGP3_CTGRAPH_CNODE
#define HEADER_UGP3_CTGRAPH_CNODE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Node.h"
#include "Constraints.h"
#include "Utility.h"
#include "CGraph.h"
#include "Edge.h"
#include "CSubGraph.h"
#include "IContainer.h"
#include "IEquatable.h"
#include "IValidable.h"
#include "ICloneable.h"
#include "Hashable.h"

#include "FloatParameter.h"

namespace ugp3
{

namespace ctgraph
{

/** Extends the Node class of the TaggedGraph and makes it constraint-aware.
@author Luca Motta, Alessandro Salomone */
class CNode :
	public tgraph::Node,
	public IEquatable<CNode>,
	public Hashable,
	public ConstrainedElement
{
private: // static fields
	static InfinityString uniqueTagGenerator;

private: // fields
	NodeContainer* parentContainer;
	CNode* next;
	CNode* prev;

private: // constructors
	/** Default constructor. It is declared private so it cannot be accessed.*/
	CNode();

	/** Copy constructor. It is declared private so it cannot be accessed.*/
	CNode(const CNode&);


	friend void NodeContainer::setAsParent(CNode* node, NodeContainer* newParent) const;
	void detachOuterLabel(const constraints::OuterLabelParameter& outerLabel);
	void detachInnerLabel(const constraints::InnerLabelParameter& innerLabel);

public: // static fields
	static const std::string Escape;
	static void setInfinityString(const std::string& value);
	static const std::string getInfinityString();
	static const std::string TAG_PLACE;
	static const std::string TAG_NEXT;
	static const std::string TAG_PREV;

public: // constructors and destructors
	CNode(NodeContainer& parentContainer);
	virtual ~CNode();

public: // methods
	/** Returns true if this CNode represents a Prologue. */
	bool representsPrologue() const;

	/** Returns true if this CNode represents an Epilogue. */
	bool representsEpilogue() const;

	tgraph::Edge* addFloatingEdge(const constraints::StructuralParameter& parameter);

	void detachOuterLabels();
	void detachInnerLabels();
	void detachNextAndPrev();

	/** Randomizes every parameter of the node. */
	bool randomize();

	/** Randomizes the indicated parameter.
	@param i The parameter to be randomized */
	bool randomizeParameter( unsigned int );

	/// This method randomize a random parameter.
	bool randomizeParameter();

	/// This method validates the node looking at Constraints.
	bool initialized() const;
	unsigned int countIncomingOuterLabels() const;


public: // getters and setters
	CNode* getNext() const;
	CNode* getPrev() const;
	void setNext(CNode* node);
	void setPrev(CNode* node);

	NodeContainer* getParentContainer() const;
	const ugp3::constraints::GenericMacro& getGenericMacro() const;
	virtual tgraph::Edge* getEdge(const std::string& parameterName) const;

public: // IEquatable interface
	virtual bool equals(const CNode& node) const;

public: // Hashable interface
	virtual hash_t calculateHashCode(Purpose purpose) const;

public: // ConstrainedElement interface
	virtual void clear();
	virtual void buildRandom();
	virtual bool validate() const;
	virtual void writeExternalRepresentation(std::ostream& stream, Relabeller& relabeller) const;

public: // Xml interface
	virtual void writeXml(std::ostream& output) const;
	virtual void readXml(const xml::Element& element);

public: // IString interface
	virtual const std::string toString() const;

public: // ICloneable interface
	std::unique_ptr<CNode> clone() const;
};




inline const ugp3::constraints::GenericMacro& CNode::getGenericMacro() const
{
    Assert(this->getConstrain() != nullptr);

	return (const ugp3::constraints::GenericMacro&)*this->getConstrain();
}

inline CNode::CNode(NodeContainer& parentContainer)
	: parentContainer(&parentContainer), next(nullptr), prev(nullptr)
{ }

inline CNode::~CNode()
{ }

inline CNode* CNode::getNext() const
{
    return this->next;
}

inline CNode* CNode::getPrev() const
{
    return this->prev;
}

inline bool CNode::representsPrologue() const
{
	if(this->getConstrain() == nullptr) throw Exception("", LOCATION);

	return dynamic_cast<const constraints::Prologue*>(this->getConstrain()) != nullptr;
}

inline bool CNode::representsEpilogue() const
{
	if(this->getConstrain() == nullptr) throw Exception("", LOCATION);

	return dynamic_cast<const constraints::Epilogue*>(this->getConstrain()) != nullptr;
}

inline NodeContainer* CNode::getParentContainer() const
{
	return this->parentContainer;
}

inline void CNode::setNext(CNode* node)
{
    this->next = node;
}

inline void CNode::setPrev(CNode* node)
{
    this->prev = node;
}

inline const std::string CNode::getInfinityString()
{
	return idCounter.toString();
}

}

}

#endif

