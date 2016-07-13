/***********************************************************************\
|                                                                       |
| Graph.xml.cc                                                          |
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
#include "TaggedGraph.h"
using namespace std;
using namespace ugp3;
using namespace tgraph;

const string Graph::XML_NAME = "graph";

void Graph::buildFromXml(const xml::Element *element)
{
    _STACK;

    if(element == nullptr)
    {
        throw ArgumentNullException("element", LOCATION);
    }

    LOG_DEBUG << "Deserializing tgraph::Graph object." << std::ends;

    if(element->ValueStr() != this->getXmlName())
    {
        throw xml::SchemaException("expected element 'graph'", LOCATION);
    }

    const xml::Element *childElement = element->FirstChildElement();
    while(childElement != nullptr)
    {
        if(childElement->ValueStr() != Node::XML_NAME)
         // the graph should contain only 'node' elements
        {
            throw xml::SchemaException("expected element 'node'", LOCATION);
        }
    
        // Create the new node and add it to the graph
        Node* node = new Node();
        node->readXml(*childElement);
        // NOTE: the edges are left as floating with the following tags:
        //       - "target: <target_node_id>"
        // They must be restored when all the nodes have been parsed.
     
        this->addNode(*node);
    }

    // restore floating edges
    for(unsigned int i = 0; i < this->nodes.size(); i++)
    {
        // for each node
        Node* node = this->nodes[i];
        for(unsigned int e = 0; e < node->getEdgeCount(); e++)
        {
            // for each edge
            Edge& edge = node->getEdge(e);
        
            if(edge.getTo() == nullptr)
            // the edge is floating, attach it to the target node
            {
                // the target tag contains the id of the target node
                Tag& targetTag = edge.getTag(Edge::targetTagName);

                const string targetId = targetTag.getValue();

                // search the target node
                Node* target = nullptr;
                for(unsigned int n = 0; n < this->nodes.size(); n++)
                {
                    if(this->nodes[n]->getId() == targetId)
                    {
                        target = this->nodes[n];
                    }
                }

                if(target == nullptr)
                {
                    throw xml::SchemaException("Could not find target node " + targetId + ".", LOCATION);
                }

                // set the target and delete the auxiliary tags
                edge.setTo(target);
                edge.removeTag(Edge::targetTagName);
               // edge.removeTag(Edge::floatingTagName);
            }
        }

        // check for id uniqueness
        for(unsigned int n = i + 1; n < this->nodes.size(); n++)
        {
            if(this->nodes[n]->getId() == node->getId())
            {
                throw xml::SchemaException("The id " + node->getId() + " of the node is not unique.", LOCATION);
            }
        }
    }

    LOG_DEBUG << "Deserialization of Graph completed successfully." << std::ends;
}

void Graph::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object tgraph::Graph." << std::ends;

    output << "<" << this->getXmlName() << ">" << endl;
    for(unsigned int i = 0; i < this->nodes.size(); i++)
    {
        this->nodes[i]->writeXml(output);
    }
    output << "</" << this->getXmlName() << ">" << endl;
}


