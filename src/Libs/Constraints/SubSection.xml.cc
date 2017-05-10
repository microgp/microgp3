/***********************************************************************\
|                                                                       |
| SubSection.xml.cc                                                     |
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

const std::string SubSection::XML_NAME = "subSection";

void SubSection::parseMacros(const xml::Element& element)
{
    _STACK;

    LOG_DEBUG << "Parsing ./macros..." << ends;

    if(element.ValueStr() != "macros")
    {
        throw xml::SchemaException("expected element './macros'", LOCATION);
    }

	string value = xml::Utility::attributeValueToString(element, "maxOccurs");
    if(value == "unbounded" || value == "infinity")
    {
        this->maximumMacroInstances = numeric_limits<unsigned int>::max() / 2; // divided by two just to avoid overflow
    }
    else this->maximumMacroInstances = Convert::toUInt(value);
    
	this->minimumMacroInstances = xml::Utility::attributeValueToUInt(element, "minOccurs");
	
	if(this->minimumMacroInstances > maximumMacroInstances)
    {
        throw Exception("The attribute minMacroOccurs cannot be greater than maxMacroOccurs", LOCATION);
    }
    
    this->averageMacroInstances = xml::Utility::attributeValueToUInt(element, "averageOccurs");
    this->averageSigmaMacroInstances = xml::Utility::attributeValueToDouble(element, "sigma");
    
    
    if(averageSigmaMacroInstances < 0) throw Exception("Sigma attribute cannot be negative.", LOCATION);
    
    if(this->averageMacroInstances < this->minimumMacroInstances || this->averageMacroInstances > this->maximumMacroInstances)
    {
    	throw Exception("Invalid value for averageMacroInstances attribute.", LOCATION);
    }
	
    // parse child nodes
    const xml::Element* childElement = element.FirstChildElement();

    while(childElement != nullptr)
    {
        Macro* macro = new Macro(*this);
	
	try
	{
        	macro->readXml(*childElement);
        	this->addMacro(macro);
	}
	catch(const Exception& ex)
	{
		//LOG_DEBUG << "Here SubSection::parseMacros, I got this: \"" << ex.what() << "\"." << ends;
		throw;
	}

        childElement = childElement->NextSiblingElement();
    }

    if(this->macros.empty())
    {
        throw xml::MissingElementSchemaException("/constraints/section/subSection/macros/macro", LOCATION);
    }
}

void SubSection::readXml(const xml::Element& element)
{
	_STACK;

	GenericSection::readXml(element);
	
    LOG_DEBUG << "Parsing ./subSection..." << ends;

    // get element name
    string elementString = element.ValueStr();
    if(elementString != "subSection")
        throw xml::SchemaException("expected element '/constraints/sections/section/subSections/subSection'", LOCATION);


    string value = xml::Utility::attributeValueToString(element, "maxOccurs");
    if(value == "unbounded" || value == "infinity")
    {
        this->maximumInstances = numeric_limits<unsigned int>::max() / 2; // divided by two just to avoid overflow
    }
    else this->maximumInstances = Convert::toUInt(value);

    this->minimumInstances = xml::Utility::attributeValueToUInt(element, "minOccurs");
    this->maximumReferences = xml::Utility::attributeValueToUInt(element, "maxReferences");

    const char* attributeValue = element.Attribute("expand");
    if(attributeValue != nullptr)
    {
        this->expand = xml::Utility::attributeValueToBool(element, "expand");
    }

    if(this->minimumInstances > maximumInstances)
    {
        throw Exception("The attribute minOccurs cannot be greater than maxOccurs", LOCATION);
    }

    if(maximumInstances == 0)
    {
        LOG_WARNING << "The SubSection \"" << this->id << "\" will never be instantiated since its maximum instance cardinality is zero" << ends;
    }


    bool macrosFound = false;
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        string childName = childElement->ValueStr();

        if(childName == "macros")
        {
            if(macrosFound == true)
                throw xml::DuplicateElementSchemaException("/constraints/sections/section/subSections/subSection/macros", LOCATION);

            macrosFound = true;
	    try
	    {
            	this->parseMacros(*childElement);
	    }
	    catch(const Exception& ex)
	    {
		//LOG_DEBUG << "I am SubSection::readXml, I got this: \"" << ex.what() << "\"." << ends;
		throw;
	    }
        }
        else if(childName == "prologue" || childName == "epilogue" || childName == "labelFormat" || childName == "uniqueTagFormat" || childName == "commentFormat" || childName == "identifierFormat")
        {
            //DO nothing
        }
        else
        {
            throw xml::SchemaException("unexpected element \"" + childName + "\"", LOCATION); 
        }

        childElement = childElement->NextSiblingElement();
    }

    if(macrosFound == false)
    {
        throw xml::MissingElementSchemaException("/constraints/sections/section/subSections/subSection/macros", LOCATION);
    }
}

void SubSection::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::SubSection" << ends;

    output.setf(ostringstream::boolalpha);

    output 
        << "<" << this->getXmlName()
        << " " << XML_ATTRIBUTE_ID << "=\"" << this->id << "\""
        << " minOccurs=\"" << this->minimumInstances << "\""
        << " maxOccurs=\"" << this->maximumInstances << "\""
		<< " maxReferences=\"" << this->maximumReferences << "\"";
    
    if(this->expand)
    {
        output << "expand=\"true\"";
    }

    output
      << ">"
        << endl;

    if(this->commentFormat != nullptr) this->commentFormat->writeXml(output);
	if(this->labelFormat != nullptr)  this->labelFormat->writeXml(output);
	if(this->uniqueTagFormat != nullptr)  this->uniqueTagFormat->writeXml(output);
	if(this->identifierFormat != nullptr)  this->identifierFormat->writeXml(output);

    this->getPrologue().writeXml(output);
    this->getEpilogue().writeXml(output);

    if (!this->macros.empty())
    {
        output 
        	<< "<macros "
          	<< " minOccurs=\"" << this->minimumMacroInstances << "\""
        	<< " maxOccurs=\"" << this->maximumMacroInstances << "\""
        	<< " averageOccurs=\"" << this->averageMacroInstances << "\""
        	<< " sigma=\"" << this->averageSigmaMacroInstances << "\">" << endl;

        map<string, Macro*>::const_iterator iterator = this->macros.begin();
        while(iterator != this->macros.end())
        {
            Macro* macro = (*iterator).second;
            macro->writeXml(output);

            ++iterator;
        }

        output << "</macros>" << endl;
    }

    output << "</" << this->getXmlName() << ">" << endl;
}

