/***********************************************************************\
|                                                                       |
| InnerLabelParameter.xml.cc                                            |
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

void InnerLabelParameter::writeXml(ostream& output) const
{
	_STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::InnerLabelParameter" << ends;

	if(this->typeDefinition == nullptr)
	{
		if(this->getBackwardJumpIsValid() && this->getForwardJumpIsValid())
		{
			output << "<item xsi:type=\"innerGenericLabel\" name=\"" << xml::Utility::transformXmlEscChar(this->getName()) << "\"";
			output << " prologue=\"";
			if(this->getPrologueIsValid())
				output << "true\"";
			else
				output << "false\"";
			output << " epilogue=\"";
			if(this->getEpilogueIsValid())
				output << "true\"";
			else
				output << "false\"";
		}
		else if(this->getBackwardJumpIsValid())
		{
			output << "<item xsi:type=\"innerBackwardLabel\" name=\"" << xml::Utility::transformXmlEscChar(this->getName()) << "\"";
			output << " prologue=\"";
			if(this->getPrologueIsValid())
				output << "true\"";
			else
				output << "false\"";
		}
		else if(this->getForwardJumpIsValid())
		{
			output << "<item xsi:type=\"innerForwardLabel\" name=\"" << xml::Utility::transformXmlEscChar(this->getName()) << "\"";
			output << " epilogue=\"";
			if(this->getEpilogueIsValid())
				output << "true\"";
			else
				output << "false\"";
		}

		output << " itself=\"";
		if(this->getItselfIsValid())
			output << "true\"";
		else
			output << "false\"";
		output << "/>" << endl;
	}
	else output << this->writeXmlAsTypeDefinition();
}

void InnerLabelParameter::readXml(const xml::Element& element)
{
    _STACK;

    Parameter::readXml(element);

    // get the "type" attribute
    bool typeAttributeFound = false;
    string type = "";
    for(const xml::Attribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
    {
        string attributeName = attribute->Name();

        string::size_type position = attributeName.find("type");
        if(position != string::npos)
        {
            typeAttributeFound = true;
            type = xml::Utility::attributeValue(*attribute);
        }			
    }

    if(typeAttributeFound == false)
    {
        throw xml::MissingElementSchemaException("@type", LOCATION);
    }

    this->itselfIsValid =  xml::Utility::attributeValueToBool(element, "itself");

    if(type == "innerForwardLabel")
    {
        this->forwardJumpIsValid = true;
        this->epilogueIsValid =  xml::Utility::attributeValueToBool(element, "epilogue");
    }
    else if (type == "innerBackwardLabel")
    {
        this->backwardJumpIsValid = true;
        this->prologueIsValid = xml::Utility::attributeValueToBool(element, "prologue");
    }
    else
    {
        this->forwardJumpIsValid = true;
        this->backwardJumpIsValid = true;
        this->prologueIsValid = xml::Utility::attributeValueToBool(element, "prologue");
        this->epilogueIsValid =  xml::Utility::attributeValueToBool(element, "epilogue");
    }
}

