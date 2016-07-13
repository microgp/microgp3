/***********************************************************************\
|                                                                       |
| Graph.h                                                               |
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
#ifndef HEADER_TGRAPH_GRAPH
#define HEADER_TGRAPH_GRAPH

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Node.h"
#include "XMLIFace.h"
#include "UniqueIdCounter.h"
#include "IValidable.h"

#include <vector>

namespace tgraph
{
    /** Describes a graph of Nodes.
        @author Luca Motta, Alessandro Salomone. */
    class Graph : public ugp3::Taggable, public ugp3::IValidable
    {
        /// private copy constructor. It should never be used.
        Graph( const Graph& );

    protected:
        std::vector<Node*> nodes;
        /// Create a new Node from XML description
        
        virtual Node* createNode( const xml::Element* );
        
        /// Build a Graph from XML description
        void buildFromXml( const xml::Element* );
        
    public:
        static const std::string XML_NAME;
        
    public:
        /// create a new instance of the graph
        Graph();
        /** Creates a Graph described in XML.
            @throws std::exception
            @param element An XML element that describes the graph. */
        Graph( const xml::Element* );
        virtual ~Graph();

         /** Adds a Node to the graph 
            @param node The node to be added. */
        void addNode( Node& );

        void removeNode( Node& );

        /** Adds an Edge between two nodes. It fails if the two nodes aren't on this graph.
            @param from The starting node of the edge
            @param to The ending node of the edge */
        bool addEdge( Node*, Node* );

        /// get the node at a given position
        Node *getNode( unsigned int ) const;

        /// get the number of nodes in this graph
        unsigned int getNodeCount() const;

        /// validate the graph
        virtual bool validate() const;
        
    public: // Xml interface
		virtual void writeXml(std::ostream& output) const;
		virtual const std::string& getXmlName() const;
    };
}

namespace tgraph
{
    inline unsigned int Graph::getNodeCount() const
    {
        return (unsigned int)nodes.size();
    }

    inline bool Graph::validate() const
    {
        return true;
    }
    
    inline const std::string& Graph::getXmlName() const
	{
		return XML_NAME;
	}
}

#endif
