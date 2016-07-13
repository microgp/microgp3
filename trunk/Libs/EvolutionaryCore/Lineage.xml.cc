/***********************************************************************\
|                                                                       |
| Lineage.xml.cc                                                        |
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

#include "Lineage.h"
#include "Operator.h"

using namespace std;
using namespace ugp3::core;

const string Lineage::XML_NAME = "lineage";
const string Lineage::XML_CHILD_OPERATOR = "operator";
const string Lineage::XML_CHILD_PARENTS = "parents";
const string Lineage::XML_CHILD_PARENT = "parent";
const string Lineage::XML_ATTRIBUTE_REF = "ref";

void Lineage::readXml(const xml::Element& element)
{
	_STACK;

	// get the name of the element
	if(element.ValueStr() != this->getXmlName())
	{
		throw xml::SchemaException("expected element '" + this->getXmlName() + "'", LOCATION);
	}

	LOG_DEBUG << "Parsing individual's lineage from xml element ... " << ends;

	bool parentsFound = false;
	bool operatorFound = false;
	const xml::Element* childElement = element.FirstChildElement();
	while (childElement) {
		string elementName = childElement->ValueStr();
		if (elementName == XML_CHILD_OPERATOR) {
			if (operatorFound) {
				throw xml::DuplicateElementSchemaException("/lineage/operator", LOCATION);
			}

			operatorFound = true;
			
			string m_operatorName = xml::Utility::attributeValueToString(*childElement, XML_ATTRIBUTE_REF);
            LOG_DEBUG << "Found generating operator '" << m_operatorName << "'" << ends;

            if (m_operatorName != Operator::NO_OPERATOR && !Operator::getOperator(m_operatorName)) {
				LOG_WARNING 
				    << "Lineage: the operator \"" << m_operatorName 
				    << "\" that generated the candidate was not found. " 
				    << "Did you remember to define it before loading the individual?" << ends;
			}
			
		} else if (elementName == XML_CHILD_PARENTS) {
			if (parentsFound) {
				throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/individuals/individual/lineage/parents", LOCATION);
			}
			parentsFound = true;

			this->parseParents(*childElement);
            
		} else {
			throw xml::SchemaException("unexpected element \"" + elementName + "\"", LOCATION);
		}
		
		childElement = childElement->NextSiblingElement();
	}

	if (!operatorFound) {
		throw xml::MissingElementSchemaException("/evolutionaryAlgorithm/populations/population/individuals/individual/lineage/operator", LOCATION);
	}
	
	LOG_DEBUG << "Lineage parsed" << ends;
}

void Lineage::writeXml(ostream& output) const
{
	_STACK;

    LOG_DEBUG << "Serializing object ugp3::core::Lineage" << ends;

	output
		<< "<" << XML_NAME << ">" << endl
		<< "<" << XML_CHILD_OPERATOR << " " << XML_ATTRIBUTE_REF << "=\"" 
        << xml::Utility::transformXmlEscChar(m_operatorName) << "\" />" << endl;
	
	if (!m_parentNames.empty()) {
		output << "<" << XML_CHILD_PARENTS << ">" << endl;

        for (auto parentName: m_parentNames) {
			output 
                << "<" << XML_CHILD_PARENT << " " << XML_ATTRIBUTE_REF 
                << "=\"" << xml::Utility::transformXmlEscChar(parentName) << "\"/>" << endl;
		}

		output << "</" << XML_CHILD_PARENTS << ">" << endl;
	}

	output << "</" << XML_NAME << ">" << endl;
}

void Lineage::parseParents(const xml::Element& element)
{
	if (element.ValueStr() != XML_CHILD_PARENTS) {
		throw xml::SchemaException("expected element '/lineage/parents'", LOCATION);
    }

	const xml::Element* childElement = element.FirstChildElement();
	while (childElement) {
		string elementName = childElement->ValueStr();
        
		if (elementName == XML_CHILD_PARENT) {
			const string& parent = xml::Utility::attributeValueToString(*childElement, XML_ATTRIBUTE_REF);
			m_parentNames.push_back(parent);
            LOG_DEBUG << "Found parent" << parent << ends;
		} else {
			throw xml::SchemaException("unexpected element \"" + elementName + "\"", LOCATION);
		}

		childElement = childElement->NextSiblingElement();
	}
}