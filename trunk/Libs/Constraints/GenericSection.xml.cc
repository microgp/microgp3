/***********************************************************************\
|                                                                       |
| GenericSection.xml.cc                                                 |
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
using namespace ugp3::constraints;
using namespace std;

void GenericSection::readXml(const xml::Element& element)
{
	_STACK;

    this->clear();

    ConstrainingElement::readXml(element);

	bool prologueFound = false;
	bool epilogueFound = false;

	const xml::TiXmlNode* childNode = element.FirstChild();
	while(childNode != nullptr)
	{
		if(childNode->ToElement() != nullptr)
		{
			const xml::TiXmlElement* childElement = childNode->ToElement();

			string elementName = childElement->ValueStr();
			if(elementName == Prologue::XML_NAME)
			{
				if(prologueFound == true) 
					throw xml::DuplicateElementSchemaException(Prologue::XML_NAME, LOCATION);
				
				prologueFound = true;
				Prologue* prologue = new Prologue(*this);
				prologue->readXml(*childElement);
				this->setPrologue(*prologue);
			}
			else if(elementName == Epilogue::XML_NAME)
			{
				if(epilogueFound == true) 
					throw xml::DuplicateElementSchemaException(Epilogue::XML_NAME, LOCATION);

				epilogueFound = true;
				Epilogue* epilogue = new Epilogue(*this);
				epilogue->readXml(*childElement);
				this->setEpilogue(*epilogue);
			}
		}
		else if(childNode->ToComment() != nullptr)
		{
			LOG_DEBUG << "Xml comment encountered while parsing the xml file" << ends;
			// do nothing 
		}
		else if(childNode->ToUnknown() != nullptr)
		{
			LOG_WARNING << "Unknown entity encountered while parsing the xml file" << ends;
			// do nothing 
		}

		childNode = element.IterateChildren(childNode);
	}

	if(prologueFound == false)
		throw xml::MissingElementSchemaException(Prologue::XML_NAME, LOCATION);

	if(epilogueFound == false)
		throw xml::MissingElementSchemaException(Epilogue::XML_NAME, LOCATION);
}

