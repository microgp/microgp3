/***********************************************************************\
|                                                                       |
| CSubGraph.xml.cc                                                      |
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
#include "CSubGraph.h"

using namespace std;
using namespace ugp3;
using namespace tgraph;
using namespace ugp3::ctgraph;
using namespace ugp3::constraints;

const string CSubGraph::XML_NAME = "subGraph";

void CSubGraph::readXml(const xml::Element& element)
{
    _STACK;

    if(this->getConstrain() == nullptr)
    {
        throw Exception("Constrain not present.", LOCATION);
    }
	else if(dynamic_cast<const Section*>(this->getConstrain()) == nullptr)
	{
		 throw Exception("Constrain is not a Section.", LOCATION);
	}

    if(this->parentContainer == nullptr)
    {
        throw Exception("parentContainer is nullptr", LOCATION);
    }

    if(element.ValueStr() != this->getXmlName())
    {
        throw xml::SchemaException("expected element '" + this->getXmlName() + "'.", LOCATION);
    }

	const Section& section = (const Section&)*this->getConstrain();
    // retrieve the subsection definition associated to the subgraph
    const string subSectionId = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_CONSTRAINTSREF);

	SubSection* ss = section.getSubSection(subSectionId);
    if(ss  == nullptr)
    {
        throw xml::SchemaException(
            "No subsection with name " + subSectionId 
            + " was found in constraints definition.", LOCATION);
    }
	this->setConstrain(*ss);

    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        if(childElement->ValueStr() == CNode::XML_NAME)
        {
            unique_ptr<CNode> node ( new CNode(*this) );
			node->setConstrain(*section.getParent());
            node->readXml(*childElement);
            LOG_DEBUG << "Read node " << node->getId() << ends;
            this->addNode(*node);
            
            if(node->representsPrologue() == true)
            {
                Assert(this->prologue == nullptr);
                this->prologue = node.release();
            }
            else if(node->representsEpilogue() == true)
            {
                Assert(this->epilogue == nullptr);
                this->epilogue = node.release();
            }
			else node.release();
        }
        else
        {
            throw xml::SchemaException("Expected element 'node'.", LOCATION);
        }
        
        childElement = childElement->NextSiblingElement();
    }

    // restore next and prev
    CNode* cursor = this->prologue;
    do
    {
        this->slice.append(*cursor);

        if(cursor->containsTag(CNode::TAG_PREV))
        {
            cursor->removeTag(CNode::TAG_PREV);
        }

        if(cursor->containsTag(CNode::TAG_NEXT))
        {
            const string& nextNodeId = cursor->getTag(CNode::TAG_NEXT).getValue();
            
            CNode* nextNode = this->getNode(nextNodeId);
            cursor->removeTag(CNode::TAG_NEXT);

            Assert(nextNode != nullptr);
            
            // move to the next node
            cursor = nextNode;
        }
        else if(cursor->representsEpilogue() == false)
        {
            throw Exception("The last node of the subsection is not an epilogue", LOCATION);
        }
        else break;
    }
    while(cursor != nullptr);
}

void CSubGraph::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object ugp3::ctgraph::CSubGraph." << std::ends;

    output 
        << "<" << XML_NAME << " ";

	if(this->getConstrain() != nullptr)
	{
       output << XML_ATTRIBUTE_CONSTRAINTSREF << "=\"" << this->getSubSection().getId() << "\" ";
	}

	output << ">" << endl;

    for(unsigned int i = 0; i < this->slice.getSize(); i++)
    {
        this->slice.getNode(i).writeXml(output);
    }

    output << "</" << XML_NAME << ">" << endl;
}

