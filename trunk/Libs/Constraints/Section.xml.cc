/***********************************************************************\
|                                                                       |
| Section.xml.cc                                                        |
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
#include "Debug.h"
#include "Constraints.h"
using namespace std;
using namespace ugp3::constraints;

const string Section::XML_NAME = "section";

void Section::readXml(const xml::Element& element)
{
	_STACK;

	GenericSection::readXml(element);

	LOG_DEBUG << "Parsing ./section ..." << std::ends;

	// get the name of the element
	std::string elementString = element.ValueStr();
	if(elementString != this->getXmlName())
	{
		throw xml::SchemaException("expected element '.section'", LOCATION);
	}

	this->isCompulsoryPrologueEpilogue = xml::Utility::attributeValueToBool(element,"prologueEpilogueCompulsory");

	// parse child nodes

	bool subSectionsFound = false;

	const xml::TiXmlNode* childNode = element.FirstChild();
	while(childNode != nullptr)
	{
		if(childNode->ToElement() != nullptr)
		{
			const xml::Element* childElement = childNode->ToElement();

			std::string elementName = childElement->ValueStr();

			if(elementName == "subSections")
			{
				if(subSectionsFound == true)
					throw xml::DuplicateElementSchemaException("./subSections", LOCATION);

				subSectionsFound = true;
				
				try
				{
					this->parseSubSections(*childElement);
				}
				catch(const Exception& ex)
				{
					//LOG_DEBUG << "I am Section::readXml and I got this exception: \"" << ex.what() << "\"." << ends;
					throw;
				}
			}
			else if(elementName == "prologue" || elementName == "epilogue" || elementName == "labelFormat" || elementName == "uniqueTagFormat" || elementName == "commentFormat"|| elementName == "identifierFormat")
			{
				//DO nothing, already parsed by base class
			}
			else
			{
				throw xml::SchemaException("Schema error: unexpected element \"" + elementName + "\"", LOCATION);
			}

		}
		else if(childNode->ToComment() != nullptr)
		{
			LOG_DEBUG << "Xml comment encountered while parsing the xml file." << std::ends;
			// do nothing
		}
		else if(childNode->ToUnknown() != nullptr)
		{
			LOG_WARNING << "Unknown entity encountered while parsing the xml file." << std::ends;
			// do nothing
		}

		childNode = element.IterateChildren(childNode);
	}

	if(subSectionsFound == false)
	{
		throw xml::MissingElementSchemaException("constraints/sections/section/subSections", LOCATION);
	}

}

void Section::parseSubSections(const xml::Element& element)
{
	_STACK;

	LOG_DEBUG << "Parsing /constraints/sections/section/subSections ..." << std::ends;

	if(element.ValueStr() != "subSections")
	{
		throw xml::SchemaException("expected element '/constraints/sections/section/subSections'", LOCATION);
	}

	// parse child nodes
	const xml::Element* childElement = element.FirstChildElement();
	while(childElement != nullptr)
	{
		SubSection* subSection = new SubSection(this);
		try
		{
			subSection->readXml(*childElement);
			this->addSubSection(subSection);
		}
		catch(const Exception& ex)
		{
			//LOG_DEBUG << "I am Section::parseSubSections, and I got this exception: \"" << ex.what() << "\"." << ends;
			throw;
		}

		childElement = childElement->NextSiblingElement();
	}

	if(this->subSections.empty() == true)
	{
		throw xml::MissingElementSchemaException("/constraints/sections/section/subSections/subSection", LOCATION);
	}
}

void Section::writeXml(ostream& output) const
{
	_STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::Section." << std::ends;

	output.setf(std::ostringstream::boolalpha);

	output << "<" << this->getXmlName() << " " << XML_ATTRIBUTE_ID << "=\"" << xml::Utility::transformXmlEscChar(this->id) << "\" prologueEpilogueCompulsory=\"" << this->isCompulsoryPrologueEpilogue << "\">" << std::endl;

	if(this->commentFormat != nullptr) this->commentFormat->writeXml(output);
	if(this->labelFormat != nullptr)  this->labelFormat->writeXml(output);
	if(this->uniqueTagFormat != nullptr)  this->uniqueTagFormat->writeXml(output);
	if(this->identifierFormat != nullptr)  this->identifierFormat->writeXml(output);


	this->getPrologue().writeXml(output);
	this->getEpilogue().writeXml(output);


	if(this->subSections.size() > 0)
	{
		output << "<subSections>" << std::endl;

		for(unsigned int i = 0; i < this->subSections.size(); i++)
		{
			this->subSections[i]->writeXml(output);
		}

		output << "</subSections>" << std::endl;
	}

	output << "</" << this->getXmlName() << ">" << std::endl;
}

