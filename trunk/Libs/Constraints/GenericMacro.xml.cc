/***********************************************************************\
|                                                                       |
| GenericMacro.xml.cc                                                   |
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

void GenericMacro::parseParameters(const xml::Element& element)
{
    _STACK;

    if(element.ValueStr() != "parameters")
    {
        throw xml::SchemaException("expected element ./parameters", LOCATION);
    }

    const xml::Element* paramElement = element.FirstChildElement();
    while(paramElement != nullptr)
    {
        string paramName = paramElement->ValueStr();

        if(paramName != "item")
        {
            throw xml::SchemaException("expected element ./parameters/item", LOCATION);
        }

        // parse the <parameter> tag
        Parameter* parameter = nullptr;
	string parameterID; 
	try
	{
        	parameterID  = ConstrainingElement::parseParameter(paramElement, parameter);
	}
	catch(const Exception& ex)
	{
		//LOG_DEBUG << "I am genericMacro::parseParameters and I got this exception: \"" << ex.what() << "\"." << ends;
		throw;
	}

        // check if there are other parameters with the same identifier
        if(this->parametersMap.find(parameterID) != this->parametersMap.end())
        {
            throw xml::SchemaException("A parameter with the specified id \"" + parameterID + "\" already exists.", LOCATION);
        }
        this->parametersMap[parameterID] = parameter;

        // save the parameter in the list of parameters
        this->parameters.push_back(parameter);

        // move to the next xml element
        paramElement = paramElement->NextSiblingElement();
    }
}

void GenericMacro::readXml(const xml::Element& element)
{
	_STACK;

	this->clear();

    try
    {
    	ConstrainingElement::readXml(element);
    }
    catch(const Exception& e)
    {
	//LOG_DEBUG << "I am genericMacro::readXml and I got this exception: \"" << e.what() << "\"." << ends;
	throw;
    }

    bool parametersFound = false;
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        string elementName = childElement->ValueStr();

        if(elementName == "parameters")
        {
            if(parametersFound == true)
                throw xml::DuplicateElementSchemaException("./parameters", LOCATION);

            parametersFound = true;

	    try
            {
            	this->parseParameters(*childElement);
 	    }
	    catch(const Exception& ex)
	    {
		//LOG_DEBUG << "I am genericMacro::readXml and I got this exception: \"" << ex.what() << "\"." << ends;
		throw;
	    }
        }
        else if (elementName != "expression" && elementName != "labelFormat" && elementName != "commentFormat" && elementName != "uniqueTagFormat" && elementName != "identifierFormat")
        {
            throw xml::SchemaException("unexpected element \"" + elementName + "\"", LOCATION);
        }

        childElement = childElement->NextSiblingElement();
    }

    bool expressionFound = false;
    childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        string elementName = childElement->ValueStr();

        if(elementName == "expression")
        {
            if(expressionFound == true)
                throw xml::DuplicateElementSchemaException("./expression", LOCATION);

            expressionFound = true;

            this->expression = unique_ptr<Expression>( new Expression(this) );
            this->expression->readXml(*childElement);
        }
        else if(elementName != "parameters" && elementName != "labelFormat" && elementName != "commentFormat" && elementName != "uniqueTagFormat"&& elementName != "identifierFormat")
        {
            throw xml::SchemaException("Schema error: unexpected element \"" + elementName + "\"", LOCATION);
        }


        childElement = childElement->NextSiblingElement();
    }


    if(expressionFound == false)
    {
    	if(this->parameters.size() > 0)
    	{
    		LOG_WARNING << "The generic macro " << this << " does not have an expression but does have some parameters that won't never be used" << ends;
    	}

    	vector<ExpressionElement*> elements;
    	this->expression = unique_ptr<Expression>( new Expression(this, &elements) );
    }

}

void GenericMacro::writeXml(std::ostream& output) const
{
	_STACK;

	output << "<" << this->getXmlName() << " id=\"" << this->getId() << "\">" << endl;
	output << this->getInnerXmlDescription();
	output << "</" << this->getXmlName() << ">" << endl;
}

const string GenericMacro::getInnerXmlDescription() const
{
    _STACK;

    ostringstream output;
    if(this->commentFormat != nullptr) this->commentFormat->writeXml(output);
    if(this->labelFormat != nullptr) this->labelFormat->writeXml(output);
    if(this->uniqueTagFormat != nullptr) this->uniqueTagFormat->writeXml(output);
    if(this->identifierFormat != nullptr) this->identifierFormat->writeXml(output);


    this->getExpression().writeXml(output);

    if(this->parameters.size() > 0)
    {
        output << "<parameters>" << endl;

        for(unsigned int j = 0; j < this->parameters.size(); j++)
        {
            this->parameters[j]->writeXml(output);
        }

        output << "</parameters>" << endl;
    }

    return output.str();
}

