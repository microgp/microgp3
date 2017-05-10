/***********************************************************************\
|                                                                       |
| Utility.cc                                                            |
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

/**
 * @file Utility.cc
 * Implementation of the Utility class.
 * @see Utility.h
 */

#include "ugp3_config.h"
#include <stdexcept>
#include <string>
#include "tinyxml.h"
#include "Log.h"
#include "StackTrace.h"
#include "Exceptions/SchemaException.h"
#include "Utility.h"
#include "Convert.h"
using namespace xml;
using namespace ugp3;


void Utility::assertion(const bool value, const std::string& message)
{
	if(value == false) throw Exception(message, LOCATION);
}

unsigned int Utility::attributeValueToUInt(const xml::Element& element, const std::string& attributeName)
{
	_STACK;

	std::string attributeValue = attributeValueToString(element, attributeName);

	return Convert::toUInt(attributeValue);
}

long Utility::attributeValueToULong(const xml::Element& element, const std::string& attributeName)
{
    _STACK;

	std::string attributeValue = attributeValueToString(element, attributeName);

	return Convert::toLong(attributeValue);
}

const std::string Utility::elementName(const xml::Element& element)
{
	return element.ValueStr();
}

const std::string Utility::transformXmlHexValue(long value)
{
	_STACK;

	std::ostringstream s;
        // writes the hexadecimal representation of value in the s stream
	s << std::hex << value;

	std::string stringValue = s.str();

	// xml requires that number of digits must be even
	std::string::size_type size = stringValue.length();
	if (size % 2 != 0)
	{
		stringValue = "0" + stringValue;
	}


	return stringValue;
}

const std::string Utility::elementText(const xml::Element& element)
{
	_STACK;

	const TiXmlNode* childElement = element.FirstChild();
	while(childElement != nullptr)
	{
		if(childElement->ToText() != nullptr)
		{
			return childElement->ToText()->ValueStr();
		}

		childElement = childElement->NextSiblingElement();
	}

	return "";
}

const std::string Utility::attributeName(const xml::Attribute& attribute)
{
	_STACK;

	const char* attributeName = attribute.Name();
	Utility::assertion(attributeName != nullptr, "Attribute name is null.");


	return std::string(attributeName);
}

const std::string Utility::attributeValue(const xml::Attribute& attribute)
{
	_STACK;

	const char* attributeValue = attribute.Value();
	
	if(attributeValue == nullptr)
		throw Exception("The attribute \"" + Utility::attributeName(attribute) + "\" is null.", LOCATION);


	return std::string(attributeValue);
}

const std::string Utility::attributeValueToString(const xml::Element& element, const std::string& attributeName)
{
	_STACK;

	LOG_DEBUG << "Parsing attribute \"" << attributeName << "\" ..." << std::ends;
	const char* attributeValue = element.Attribute(attributeName);
	
	
	if(attributeValue == nullptr)
	{
		throw SchemaException("Attribute \"" + attributeName + "\" not found in element \"" + element.ValueStr() + "\".", LOCATION);
	}

	const std::string result = attributeValue;

	return result;
}

double Utility::attributeValueToDouble(const xml::Element& element, const std::string& attributeName)
{
	_STACK;

	std::string attributeValue = attributeValueToString(element, attributeName);

	return Convert::toDouble(attributeValue);
}

long Utility::attributeValueToLong(const xml::Element& element, const std::string& attributeName)
{
	_STACK;

	std::string attributeValue = attributeValueToString(element, attributeName);

	return Convert::toLong(attributeValue);
}

long Utility::attributeValueToHex(const xml::Element& element, const std::string& attributeName)
{
	_STACK;

	std::string attributeValue = attributeValueToString(element, attributeName);

	return Convert::toLong(attributeValue, Base::Hexadecimal);
}

bool Utility::attributeValueToBool(const xml::Element& element, const std::string& attributeName)
{
	_STACK;

	std::string attributeValue = attributeValueToString(element, attributeName);

	return Convert::toBool(attributeValue);
}

// Use TiXml implementation for the next method
class TiAdapter: private TiXmlBase {
public:
    // Change visibility
    using TiXmlBase::PutString;
};

const std::string Utility::transformXmlEscChar(const std::string& strl)
{
	_STACK;
    
    std::string outString;
    
    TiAdapter::PutString(strl, &outString);

	return outString;
}

bool Utility::hasAttribute(const Element& element, const std::string& attributeName)
{
    return element.Attribute(attributeName);
}
