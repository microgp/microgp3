/***********************************************************************\
|                                                                       |
| Node.h                                                                |
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
 * @file Node.h
 * Definition of the Node class.
 * @see Node.cc
 * @see Node.xml.cc
 */

#ifndef HEADER_TGRAPH_NODE
/** Defines that this file has been included */
#define HEADER_TGRAPH_NODE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>
#include <string>

#include "Taggable.h"
#include "XMLIFace.h"
#include "Utility.h"
#include "UniqueIdCounter.h"
#include "IContainer.h"
#include "IString.h"
#include "Exceptions/IndexOutOfBoundsException.h"
#include "Edge.h"

/**
 * tgraph namespace
 */
namespace tgraph
{
class Edge;

/**
 * @class Node
 * Represents a node of a graph.
 * @author Luca Motta
 * @version 1
 */
class Node :
    public ugp3::Taggable,
    public ugp3::IContainer<Edge>,
    public ugp3::IString
{
protected:
    /** Identifier of this Node */
    std::string id;
    /** Output edges from this Node */
    std::vector<Edge*> edges;
    /** Input edges to this Node */
    std::vector<Edge*> backEdges;

protected:
    /** Static counter to set the ids to new Nodes */
    static InfinityString idCounter;


    /** Private copy constructor. It should never be used. Not implementes and not used. */
    Node( const Node& );

protected:
    /**
     * Builds a Node from XML description. NOT IMPLEMENTED
     * @param xml::Element Element to take the Node information from
     * @throws std::exception.
     */
    void buildFromXml( const xml::Element* );

    /**
     * Removes an Edge from/to the node. NOT IMPLEMENTED.
     * @param edge The Edge to be removed
     * @param from If it is true, this method removes the edge from the startinf of the node, otherwise it removes the edge pointing to the node.
     */
    bool removeEdge(Edge*, bool);

public:
    /** Name of this xml element */
    static const std::string XML_NAME;
    /** Tag to specify the id in a xml element */
    static const std::string XML_ATTRIBUTE_ID;

public: // constructors and destructors
    /** 
     * Constructor of the class. Builds a new instance of the Node class.
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    Node();
    /** 
     * Destructor of the class. Frees the memory used by this object
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual ~Node();


public:
    /**
     * Add an Edge from/to the node
     * @param edge The Edge to be added.
     * @throws std::exception. ugp3::ArgumentException if the edge is not related with this node or the edge is already in the node
     */
    void addEdge(Edge& edge);

    /**
     * Adds a new Edge to this node
     * @param targetNode The target node.
     * @returns Edge The new edge added
     * @throws std::exception.
     */
    Edge* addEdgeTo(Node* targetNode);

    /**
     * Removes an edge from this node
     * @param edge The edge to remove
     * @throws std::exception. ugp3::ArgumentNullException if a null edge is specified, ugp3::ArgumentException if this node is not the source of the edge
     */
    void removeEdge(Edge* edge);
    /**
     * Removes an Edge pointing to the passed node.
     * @param targetNode The 'to' node that has to be unreferenced.
     * @throws std::exception. ugp3::ArgumentNullException if a null edge is specified, ugp3::ArgumentException if this node is not the destination of the edge
     */
    void removeBackEdge(Edge* edge);

    /**
     * Search the edge in this node and returns true if it is found
     * @param edge edge to search
     * @returns bool True if the edge is found, false otherwise
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    bool contains(const Edge& edge) const;

    /**
     * Returns the id of this node
     * @returns string The id of this node
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    const std::string& getId() const;

public: // IString interface
		virtual const std::string toString() const;

public: // getters
    /**
     * Returns the number of outgoing edges in this node.
     * @returns unsigned int The number of outgoing edges in this node
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    unsigned int getEdgeCount() const;

    /**
     * Returns the ith outgoing Edge of this node.
     * @param index The position of the outgoing Edge to be retrieved.
     * @throws std::exception. ugp3::IndexOutOfBoundsException if the index is bigger than the size of the edges vector
     */
    Edge& getEdge(unsigned int index) const;

    /**
     * Get the numbe of back edges in this node.
     * @returns unsigned int The number of back edges in this node
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    unsigned int getBackEdgeCount() const;

    /**
     * Get the i-th back Edge of this node.
     * @param index The position of the back Edge to be retrieved.
     * @throws std::exception. ugp3::IndexOutOfBoundsException if the index is bigger than the size of the back edges vector
     */
    Edge& getBackEdge(unsigned int index) const;

public: // Xml interface
	virtual void writeXml(std::ostream& output) const;
	virtual void readXml(const xml::Element& element);
	virtual const std::string& getXmlName() const;
};

inline const std::string& Node::getId() const
{
    return this->id;
}

inline const std::string Node::toString() const
{
    return "{" + this->id + "}";
}

inline const std::string& Node::getXmlName() const
{
	return XML_NAME;
}

inline unsigned int Node::getEdgeCount() const
{
    return (unsigned int)edges.size();
}

inline unsigned int Node::getBackEdgeCount() const
{
    return (unsigned int)backEdges.size();
}

}

#endif
