/***********************************************************************\
|                                                                       |
| Expression.xml.cc                                                     |
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
#include "Constraints.h"

using namespace std;
using namespace ugp3::constraints;

const std::string Expression::XML_NAME = "expression";

void Expression::readXml(const xml::Element& element)
{
	this->clear();
	
	if(element.ValueStr() != this->getXmlName())
	{
		throw xml::SchemaException("expected element ./expression", LOCATION);
	}

	const xml::TiXmlNode* exprNode = element.FirstChild();
	while(exprNode != nullptr)
	{
		if(exprNode->ToElement() != nullptr)
		{
			const xml::Element& el = *exprNode->ToElement();
			string elemName = el.ValueStr();

			if(elemName != "param")
			{
				throw xml::SchemaException("expected element ./expression/param", LOCATION);
			}

			string paramID = xml::Utility::attributeValueToString(el, "ref");

			bool parameterFound = false;
			for(unsigned int i = 0; i < this->parent->getParameterCount(); i++)
			{
				if(this->parent->getParameter(i).getName() == paramID)
				{
					ExpressionElement* element = new ExpressionElement(this->parent->getParameter(i));
					this->elements.push_back(element);
					
					parameterFound = true;
					break;
				}
			}

			if(parameterFound == false)
			{
				throw xml::SchemaException("the foreign key ./expression/param/[@ref=\"" + paramID +"\"] does not match the ./../parameters/item/@name primary key.", LOCATION);	
			}
		}
		else if(exprNode->ToText() != nullptr)
		{
			const char* value = exprNode->ToText()->Value();

			if(value == nullptr)
			{
				throw Exception("The expression text was null", LOCATION);
			}

			this->elements.push_back(new ExpressionElement(value));
		}
		else if(exprNode->ToComment() != nullptr)
		{
			LOG_DEBUG << "Xml comment encountered while parsing the xml file" << ends;
			// do nothing 
		}
		else if(exprNode->ToUnknown() != nullptr)
		{
			LOG_WARNING << "Unknown entity encountered while parsing the xml file" << ends;
			// do nothing 
		}

		exprNode = element.IterateChildren(exprNode);
	}
}

void Expression::writeXml(ostream& output) const
{
	_STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::Expression" << ends;

	output << "<expression>";

	for(unsigned int i = 0 ; i < this->elements.size() ; i++)
	{
		if(this->elements[i]->getParameter() != nullptr  ) 
		{
			// search for the referenced parameter
			for(unsigned int j = 0 ; j < this->parent->getParameterCount() ; j++)
			{
				if(&this->parent->getParameter(j) == this->elements[i]->getParameter())
				{
					output << "<param ref=\"" <<  xml::Utility::transformXmlEscChar(this->elements[i]->getParameter()->getName()) << "\"/>";
					break;
				}
			}
		}
		else
		{
			output << xml::Utility::transformXmlEscChar(*this->elements[i]->getCharacters());
		}
	}

	output << "</expression>" << endl;
}

