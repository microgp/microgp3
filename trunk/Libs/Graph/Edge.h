/***********************************************************************\
|                                                                       |
| Edge.h                                                                |
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
#ifndef HEADER_TGRAPH_EDGE
#define HEADER_TGRAPH_EDGE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include "Taggable.h"
#include "Node.h"
#include "XMLIFace.h"
#include "Utility.h"
#include "IString.h"
#include "UniqueIdCounter.h"

namespace tgraph
{

class Node;

/** Represents an Edge between two Nodes of the graph.
    @author Luca Motta, Alessandro Salomone. */
class Edge : public ugp3::Taggable, public ugp3::IString
{
private:
    Node& from;
    Node* to;

    /// private copy constructor. It should never be used
    Edge( const Edge& );
    Edge& operator=(const Edge& edge);
    
    void clear();
    
protected:
    static const std::string XML_ATTRIBUTE_TO;

public: // static fields
    static const std::string offsetTagName;
    static const std::string parameterTagName;
    static const std::string targetTagName;
    static const std::string XML_NAME;

public: // constructors and destructors
    /** Makes a new instance of the Edge class.
        @param sourceNode The node from which the edge starts
        @param targetNode The node from which the edge points to */
    Edge(Node& sourceNode, Node* targetNode);

    /** Makes an edge from XML description
        @throws std::exception
        @param sourceNode The node from which the edge starts
        @param targetNode The node from which the edge points to
        @param element The pointer to XML element. */
    Edge(Node& sourceNode);

    virtual ~Edge();

public: // getters and setters
    /** Returns the from node pointer. */
    Node& getFrom() const;

    /** Returns the to node pointer. */
    Node* getTo() const;

    /** Set the to node pointer
        @param to The new to pointer */
    void setTo(Node*);

public: // Xml interface
	virtual void writeXml(std::ostream& output) const;
	virtual void readXml(const xml::Element& element);
	virtual const std::string& getXmlName() const;

public: // IString interface
    const std::string toString() const;
};


inline Node& Edge::getFrom() const
{
    return this->from;
}

inline Node* Edge::getTo() const
{
    return this->to;
}

inline const std::string& Edge::getXmlName() const
{
	return XML_NAME;
}
    
}

#endif

