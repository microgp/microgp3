/***********************************************************************\
|                                                                       |
| CGraph.xml.cc                                                         |
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
using namespace ugp3::constraints;
using namespace ctgraph;
using namespace tgraph;

void CGraph::readXml(const xml::Element& element)
{
	_STACK;

	if(element.ValueStr() != this->getXmlName())
    {
        throw xml::SchemaException("Expected element 'graph'.", LOCATION);
    }

	this->clear();

	if(this->getConstrain() == nullptr)
	{
		throw new Exception("No constraining rule is associated to this Graph.", LOCATION);
	}
	else if(dynamic_cast<const Constraints*>(this->getConstrain()) == nullptr)
	{
		throw new Exception("No constraining rule is associated to this Graph.", LOCATION);
	}

	
    
    if(parentContainer == nullptr)
    {
        throw ArgumentNullException("parentContainer", LOCATION);
    }

    
	const Constraints& constraints = (const Constraints&)*this->getConstrain();

	// retrieve the Section definition
    const string sectionName = xml::Utility::attributeValueToString(element, ConstrainedElement::XML_ATTRIBUTE_CONSTRAINTSREF);
    for(unsigned int i = 0; i < constraints.getSectionCount(); i++)
    {
        if(constraints.getSection(i).getId() == sectionName)
        {
            this->setConstrain(constraints.getSection(i));
        }
    }

	if(parentContainer == nullptr)
    {
        throw ArgumentNullException("parentContainer", LOCATION);
    }

    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        const string& elementName = childElement->ValueStr();

        if(elementName == CNode::XML_NAME)
        {
            //const string nodeId = xml::Utility::attributeValueToString(*childElement, CNode::XML_ATTRIBUTE_ID);
            
            unique_ptr<CNode> node( new CNode(*this) );
			node->setConstrain(constraints);
            node->readXml(*childElement);

            if(node->representsPrologue() && this->prologue.get() == nullptr)
            {
            	this->prologue.reset(node.release());
            }
            else if(node->representsEpilogue() && this->epilogue.get() == nullptr)
            {
            	this->epilogue.reset(node.release());
            }
            else
            {
                throw Exception("Unexpected element 'node'.", LOCATION);
            }
        }
        else if(elementName == CSubGraph::XML_NAME)
        {
            CSubGraph* subGraph = new CSubGraph(*this);
			Assert(this->getConstrain() != nullptr);
			subGraph->setConstrain(*this->getConstrain());
			subGraph->readXml(*childElement);

            this->subGraphs.push_back(subGraph);
        }
        else
        {
            throw xml::SchemaException("unexpected element '" + elementName + "'.", LOCATION);
        }

        childElement = childElement->NextSiblingElement();
    }

    if(this->prologue.get() == nullptr)
    {
        throw xml::SchemaException("Could not find the global prologue.", LOCATION);
    }

    if(this->epilogue.get() == nullptr)
    {
        throw xml::SchemaException("Could not find the global epilogue.", LOCATION);
    }

    if(this->getConstrain() == nullptr)
    {
        throw xml::SchemaException("Could not find the section " + sectionName + ".", LOCATION);
    }

	// some edges may not be attached
    this->attachFloatingEdges();
}

void CGraph::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object ugp3::ctgraph::CGraph" << ends;

    output << "<" << XML_NAME << " ";

	if(this->getConstrain() != nullptr)
	{
		output << ConstrainedElement::XML_ATTRIBUTE_CONSTRAINTSREF << "=\"" << this->getConstrain()->getId() << "\" ";
	}

	output << " xmlns=\"http://www.cad.polito.it/ugp3/schemas/constrained-graph\">" << endl;

    this->prologue->writeXml(output);
    this->epilogue->writeXml(output);

    for(unsigned int i = 0; i < this->subGraphs.size(); i++)
    {
        this->subGraphs[i]->writeXml(output);
    }

    output << "</" << XML_NAME << ">" << endl;
}

