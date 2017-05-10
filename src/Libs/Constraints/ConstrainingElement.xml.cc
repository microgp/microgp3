/***********************************************************************\
|                                                                       |
| ConstrainingElement.xml.cc                                            |
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

const std::string ConstrainingElement::XML_ATTRIBUTE_ID = "id";

void ConstrainingElement::readXml(const xml::Element& element)
{
    _STACK;

    this->clear();

    // get the attribute id (it should always exist)
    this->id = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_ID);
    bool commentFormatFound = false;
    bool labelFormatFound = false;
    bool uniqueTagFormatFound = false;
    bool identifierFormatFound = false;

    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        const string& elementName = childElement->ValueStr();
        if(elementName == CommentFormat::XML_NAME)
        {
            if(commentFormatFound == true)
                throw xml::DuplicateElementSchemaException("./commentFormat", LOCATION);

            commentFormatFound = true;
            this->commentFormat = new CommentFormat();
            this->commentFormat->readXml(*childElement);
        }
        else if(elementName == LabelFormat::XML_NAME)
        {
            if(labelFormatFound == true)
                throw xml::DuplicateElementSchemaException("./labelFormat", LOCATION);

            labelFormatFound = true;
            this->labelFormat = new LabelFormat();
            this->labelFormat->readXml(*childElement);
        }
        else if(elementName == IdentifierFormat::XML_NAME)
        {
            if(identifierFormatFound == true)
                throw xml::DuplicateElementSchemaException("./identifierFormat", LOCATION);

            identifierFormatFound = true;
            this->identifierFormat = new IdentifierFormat();
            this->identifierFormat->readXml(*childElement);
        }
        else if(elementName == UniqueTagFormat::XML_NAME)
        {
            if(uniqueTagFormatFound == true)
                throw xml::DuplicateElementSchemaException("./uniqueTagFormat", LOCATION);

            uniqueTagFormatFound = true;
            this->uniqueTagFormat = new UniqueTagFormat();
            this->uniqueTagFormat->readXml(*childElement);
        }
        

        childElement = childElement->NextSiblingElement();
    }
}

const string ConstrainingElement::parseParameter(const xml::Element* element, Parameter*& parameter)
{
    _STACK;

    if(element == nullptr)
    {
        throw ArgumentNullException("element", LOCATION);
    }

    if(element->ValueStr() != Parameter::XML_NAME)
    {
        throw xml::SchemaException("expected element '" + this->getXmlName() + "' (found '" + element->ValueStr() + "').", LOCATION);
    }

    const xml::Element& e = *element;
    string name = xml::Utility::attributeValueToString(e, Parameter::XML_ATTRIBUTE_NAME);
    LOG_DEBUG << "Building parameter \"" << name << "\"..." << ends;

    // get the "type" attribute
    bool typeAttributeFound = false;
    string type = "";
    for(const xml::Attribute* attribute = e.FirstAttribute(); attribute != nullptr; attribute = attribute->Next())
    {
        string attributeName = attribute->Name();

        string::size_type position = attributeName.find(Parameter::XML_ATTRIBUTE_TYPE);
        if(position != string::npos)
        {
            typeAttributeFound = true;
            type = xml::Utility::attributeValue(*attribute);
        }			
    }

    if(typeAttributeFound == false)
    {
        throw xml::MissingElementSchemaException("/constraints/typeDefinitions/item/@type", LOCATION);
    }

    LOG_DEBUG << "Parameter's type is \"" << type << "\"" << ends;
    bool isFromTypeDefinition = false;
    if(type == "integer")
    {
        parameter = new IntegerParameter();
    }
    else if(type == "environment")
    {
        parameter = new EnvironmentParameter();
    }
    else if(type == "float")
    {
       parameter = new FloatParameter();
    }
    else if(type == "bitArray")
    {
         parameter = new BitArrayParameter();
    }
    else if(type == "constant")
    {
        parameter = new ConstantParameter();
    }
    else if(type == "combinatorial")
    {
        parameter = new CombinatorialParameter();
    }
    else if(type == "outerLabel")
    {
        parameter = new OuterLabelParameter();
    }
    else if(type == "innerGenericLabel" || type == "innerForwardLabel" || type == "innerBackwardLabel")
    {
        parameter = new InnerLabelParameter();
    }
    else if(type == "selfRef")
    {
        parameter = new SelfRefParameter();
    }
    else if(type == "uniqueTag")
    {
        parameter = new UniqueTagParameter();
    }
    else if(type == "definedType")
    {
        isFromTypeDefinition = true;
        const string& typeName = xml::Utility::attributeValueToString(e, "ref");


        const GenericSection* parent = this->getParent();
        for(; parent->getParent() != nullptr; parent = parent->getParent());
        const Constraints* constraints = static_cast<const Constraints*>(parent);

        const vector<Parameter*>& typeDefs = *constraints->getTypeDefs();


        bool parameterFound = false;
        for(unsigned int i = 0; i < typeDefs.size(); i++)
        {
            if(typeName == typeDefs[i]->getName())
            {
                typeDefs[i]->clone(parameter, name);
                LOG_DEBUG << "Created parameter \"" << name << "\" from type definiton \"" << typeName << "\"" << ends;
                parameterFound = true;
                break;
            }
        }

        if(parameterFound == false)
        {
            throw xml::SchemaException("the foreign key ./parameters/item/@ref (@ref = \"" + typeName + "\") does not match the primary key /constraints/typeDefinitions/item/@name", LOCATION);
        }
    }  
    else
    {
        throw xml::SchemaException("unexpected value " + type + " for attribute ./item/@type", LOCATION);
    }

    if(isFromTypeDefinition == false)
    {
	try
	{
        	parameter->readXml(e);
	}
	catch(const Exception& ex)
	{
		//LOG_DEBUG << "I am constraingElement and I got this exception: \"" << ex.what() << "\"." << ends;
		throw;
	}
    }

    return name;
}

