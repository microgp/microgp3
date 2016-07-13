/***********************************************************************\
|                                                                       |
| Constraints.xml.cc                                                    |
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

const string Constraints::XML_NAME = "constraints";
const string Constraints::XML_NAMESPACE = "http://www.cad.polito.it/ugp3/schemas/constraints";

void Constraints::parseTypeDefinitions(const xml::Element& element)
{
	_STACK;

	LOG_DEBUG << "Parsing /constraints/typeDefinitions..." << ends;

    Assert(this->typedefs == nullptr);
    this->typedefs = new vector<Parameter*>();

	if(element.ValueStr() != "typeDefinitions")
    {
		throw xml::SchemaException("expected element '/constraints/typeDefinitions'", LOCATION);
    }

	// parse child nodes
	const xml::Element* childElement = element.FirstChildElement();
	while(childElement != nullptr)
	{
		Parameter* typeDefinition = nullptr;
		this->parseParameter(childElement, typeDefinition);

		for(unsigned int i = 0; i < typedefs->size(); i++)
		{
			if(typeDefinition->getName() == (*typedefs)[i]->getName())
				throw xml::SchemaException("A type definition with the name " + typeDefinition->getName() + " already exists.", LOCATION);
		}

		this->typedefs->push_back(typeDefinition);


		childElement = childElement->NextSiblingElement();
	}

	if(this->typedefs->size() == 0)
    {
		throw xml::MissingElementSchemaException("/constraints/typeDefinitions/item", LOCATION);
    }
}

void Constraints::parseSections(const xml::Element& element)
{
    _STACK;

    LOG_DEBUG << "Parsing /constraints/sections..." << ends;

    string elementName = element.ValueStr();
    if(elementName != "sections")
    {
        throw xml::SchemaException("expected element '/constraints/sections'",LOCATION);
    }

    // parse child nodes
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        Section* section = new Section(this);
        section->readXml(*childElement);
        this->addSection(section);


        childElement = childElement->NextSiblingElement();
    }

    if(this->sections.empty() == true)
    {
        throw xml::MissingElementSchemaException("/constraints/sections/section",LOCATION);
    }
}


void Constraints::readXml(const xml::Element& element)
{
    _STACK;

    this->clear();

    GenericSection::readXml(element);

    string elementName = element.ValueStr();
    if(elementName != this->getXmlName())
    {
        throw xml::SchemaException("expected element './" + this->getXmlName() + "'.", LOCATION);
    }

    LOG_DEBUG << "Parsing './" << this->getXmlName() << "'..." << ends;

    // parse child nodes
    bool sectionsFound = false;
    bool typeDefinitionsFound = false;

    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        string elementName = childElement->ValueStr();
        if(elementName == "sections")
        {
            if(sectionsFound == true)
            {
                throw xml::DuplicateElementSchemaException("/constraints/sections",LOCATION);
            }

            sectionsFound = true;
            this->parseSections(*childElement);
        }
        else if(elementName == "typeDefinitions")
        {
            if(typeDefinitionsFound == true)
            {
                 throw xml::DuplicateElementSchemaException("/constraints/typeDefinitions", LOCATION);
            }

            typeDefinitionsFound = true;
            this->parseTypeDefinitions(*childElement);
        }

        childElement = childElement->NextSiblingElement();
    }

    if(sectionsFound == false)
    {
        throw xml::MissingElementSchemaException("/constraints/sections",LOCATION);
    }

    if(this->getCommentFormat() == nullptr)
    {
        throw xml::MissingElementSchemaException("/constraints/commentFormat",LOCATION);
    }

    if(this->getIdentifierFormat() == nullptr)
    {
        throw xml::MissingElementSchemaException("/constraints/identifierFormat",LOCATION);
    }

    if(this->getUniqueTagFormat() == nullptr)
    {
        throw xml::MissingElementSchemaException("/constraints/uniqueTagFormat",LOCATION);
    }

    if(this->getLabelFormat() == nullptr)
    {
        throw xml::MissingElementSchemaException("/constraints/labelFormat",LOCATION);
    }

    if(this->validate() == false)
    {
        throw Exception("Constraints are not valid.", LOCATION);
    }

}

void Constraints::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::Constraints" << ends;

    output
        << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << endl
        << "<" << this->getXmlName()
        << " "
        << XML_ATTRIBUTE_ID << "=\"" << this->id << "\"" << endl
        << " xmlns=\"" << XML_NAMESPACE << "\"" << endl
        << " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" << endl
        << " xsi:schemaLocation=\"" << XML_NAMESPACE << " " << XML_NAMESPACE << ".xsd\""
        << ">" << endl;

    if(this->typedefs != nullptr)
    {
        if(this->typedefs->empty() == false)
        {
            output << "<" << "typeDefinitions" << ">" << endl;

            for(unsigned int i = 0; i < this->typedefs->size(); i++)
            {
                this->typedefs->at(i)->writeXml(output);
            }

            output << "</" << "typeDefinitions" << ">" << endl;
        }
    }

    this->getPrologue().writeXml(output);
    this->getEpilogue().writeXml(output);

	this->commentFormat->writeXml(output);
	this->labelFormat->writeXml(output);
	this->identifierFormat->writeXml(output);
    this->uniqueTagFormat->writeXml(output);



    if(this->sections.size() > 0)
    {
        output << "<" << "sections" << ">" << endl;

        for(unsigned int i = 0; i < this->sections.size(); i++)
        {
            this->sections[i]->writeXml(output);
        }

        output << "</" << "sections" << ">" << endl;
    }

    output << "</" << this->getXmlName() << ">" << endl;
}

