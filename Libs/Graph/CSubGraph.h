/***********************************************************************\
|                                                                       |
| CSubGraph.h                                                           |
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
#ifndef HEADER_UGP3_CTGRAPH_CSUBGRAPH
#define HEADER_UGP3_CTGRAPH_CSUBGRAPH

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <memory>
#include <vector>
#include <string>
#include <ostream>

#include "Constraints.h"
#include "TaggedGraph.h"

#include "NodeContainer.h"

#include "Hashable.h"
#include "IString.h"
#include "ICloneable.h"
#include "IContainer.h"
#include "InfinityString.h"
#include "CGraph.h"
#include "CNode.h"
#include "XMLIFace.h"
#include "Utility.h"
#include "Slice.h"

namespace ugp3
{
   
namespace ctgraph
{
class CNode;

class CSubGraph : public NodeContainer,
    public IEquatable<CSubGraph>
{
private: // static fields
    static InfinityString idCounter;

private: // fields
    std::string id;

    IContainer<CSubGraph>* parentContainer;
    std::map<std::string, CNode*> nodes;
    Slice slice;

    CNode* prologue;
    CNode* epilogue;
    
private: // constructors
    CSubGraph(const CSubGraph& subGraph);
    CSubGraph();

private: // methods

    CNode* getTargetFromFloatingEdgeUsingOffset(
        const CNode& node,
        const tgraph::Edge& edge) const;
    bool restoreInnerLabel(
        CNode& node,
        const ugp3::constraints::InnerLabelParameter& innerLabel);

    friend void CGraph::setAsParent(
        CSubGraph* subGraph,
        CGraph* newParent) const;

public:
    static const std::string XML_NAME;

public: // constructors and destructors
	CSubGraph(IContainer<CSubGraph>& parentContainer);
    virtual ~CSubGraph();

public: // methods
    void addNode(CNode& node);
    bool attachFloatingEdges();

    bool isBefore(
        const CNode& node1,
        const CNode& node2) const;

    bool isStrictlyBefore(
        const CNode& node1,
        const CNode& node2) const;

    CNode* getRandomTargetForInnerLabel(
        const CNode& node,
        const ugp3::constraints::InnerLabelParameter& innerLabel) const;

    /** Gets the number of positions between the two nodes.
        @param node The first node.
        @param targetNode The last node.
        @return The offset between the two nodes. */
    int getOffset(const CNode& node, const CNode& targetNode) const;

   Slice& getSlice();
   bool validateConstraints() const;

   virtual CNode& getEpilogue() const;
   virtual CNode& getPrologue() const;

public: // getters
    unsigned int getSize() const;
    IContainer<CSubGraph>* getParentContainer() const;
    const ugp3::constraints::SubSection& getSubSection() const;

public: // NodeContainer interface
    virtual CNode*  getNode(const std::string& id) const;
    virtual void    setPrologue(std::unique_ptr<CNode>& value);
    virtual void    setEpilogue(std::unique_ptr<CNode>& value);

public: // IContainer interface
    virtual bool contains(const CNode& node) const;

public: // IString interface
    virtual const std::string toString() const;

public: // IEquatable interface
    virtual bool equals(const CSubGraph& subGraph) const;

public: // ICloneable interface
    virtual std::unique_ptr<CSubGraph> clone() const;

public: // Xml interface
	virtual void writeXml(std::ostream& output) const;
	virtual void readXml(const xml::Element& element);
	virtual const std::string& getXmlName() const;

public: // ConstrainedElement interface
	virtual void clear();
	virtual void buildRandom();
	virtual bool validate() const;
	virtual void writeExternalRepresentation(std::ostream& stream, Relabeller& relabeller) const;

public: // Hashable interface
	 virtual hash_t calculateHashCode(Purpose purpose) const;
};


inline IContainer<CSubGraph>* CSubGraph::getParentContainer() const
{
    return this->parentContainer;
}

inline const std::string CSubGraph::toString() const
{
   if(this->getConstrain() != nullptr)
    return "{id=\"" + this->id + "\" type=\"" +  this->getConstrain()->getId() + "\"}";

   return "{id=\"" + this->id + "\"}";
}

inline Slice& CSubGraph::getSlice()
{
    return this->slice;
}

inline const ugp3::constraints::SubSection& CSubGraph::getSubSection() const
{
    Assert(this->getConstrain() != nullptr);

    return (const ugp3::constraints::SubSection&)*this->getConstrain();
}

inline const std::string& CSubGraph::getXmlName() const
{
	return XML_NAME;
}

inline CNode& CSubGraph::getPrologue() const
{
    Assert(this->prologue != nullptr);
	return *this->prologue;
}

inline CNode& CSubGraph::getEpilogue() const
{
    Assert(this->epilogue != nullptr);
	return *this->epilogue;
}

inline unsigned int CSubGraph::getSize() const
{
    _STACK;

    return (unsigned int)this->slice.getSize();
}

}

}
#endif
