/***********************************************************************\
|                                                                       |
| CGraphContainer.xml.cc                                                |
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
using namespace ugp3::constraints;
using namespace ugp3::ctgraph;

const string CGraphContainer::XML_NAME = "genome";

void CGraphContainer::readXml(const xml::Element& element)
{
	_STACK;
	
	if(this->getConstrain() == nullptr)
	{
		throw new Exception("No constraining rule is associated to this GraphContainer.", LOCATION);
	}
	else if(dynamic_cast<const Constraints*>(this->getConstrain()) == nullptr)
	{
		throw new Exception("No constraining rule is associated to this GraphContainer.", LOCATION);
	}

	const Constraints& constraints = (const Constraints&) *this->getConstrain();

	this->clear();

	bool graphFound = false;
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        string elementName = childElement->ValueStr();
        if(elementName == CGraph::XML_NAME)
        {
            graphFound = true;

            unique_ptr<CGraph> graph ( new CGraph(*this) );
			graph->setConstrain(constraints);
			graph->readXml(*childElement);

            this->addCGraph(graph);
        }
        else if(elementName == CNode::XML_NAME)
        {
			unique_ptr<CNode> node ( new CNode(*this) );
			node->setConstrain(constraints);
			node->readXml(*childElement);

  
			if(node->representsPrologue())
            {
                this->prologue.reset(node.release());
            }
			else if(node->representsEpilogue())
            {
				this->epilogue.reset(node.release());
            }
            else
            {
                throw Exception("Unexpected element 'node'.", LOCATION);
            }
        }
        else
        {
            throw xml::SchemaException("unexpected element \"" + elementName + "\"", LOCATION);
        }

        childElement = childElement->NextSiblingElement();
    }

    if(graphFound == false)
    {
        throw xml::MissingElementSchemaException("/evolutionaryAlgorithm/populations/population/individuals/individual/genome/graph", LOCATION);
    }

    if(this->epilogue.get() == nullptr)
    {
        throw xml::SchemaException("Could not find the epilogue.", LOCATION);
    }

    if(this->prologue.get() == nullptr)
    {
        throw xml::SchemaException("Could not find the prologue.'", LOCATION);
    }

    this->attachFloatingEdges();

    if(this->validate() == false)
    {
        throw Exception("The graph container built from xml is not valid.", LOCATION);
    }
}

void CGraphContainer::writeXml(ostream& output) const
{
	output << "<" << XML_NAME << ">" << endl;

    this->getPrologue().writeXml(output);
    this->getEpilogue().writeXml(output);

    for(unsigned int i = 0; i < this->getCGraphCount(); i++)
    {
        this->getCGraph(i).writeXml(output);
    }

    output << "</" << XML_NAME << ">" << endl;
}

