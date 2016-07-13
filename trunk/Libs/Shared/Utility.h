/***********************************************************************\
|                                                                       |
| Utility.h                                                             |
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
 * @file Utility.h
 * Definition of the Utility class.
 * @see Utility.cc
 */

#ifndef HEADER_XML_UTILITY
/** Defines that this file has been included */
#define HEADER_XML_UTILITY

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdexcept>
#include <string>
#include "tinyxml.h"
#include "Exception.h"

/**
 * xml namespace
 */
namespace xml
{
        /** Xml Element */
	typedef TiXmlElement Element;
        /** Xml Attribute */
	typedef TiXmlAttribute Attribute;
        /** Xml Document */
	typedef TiXmlDocument Document;

        /**
         * @class Utility
         * Provides methods to simplify access to xml data.
         */
	class Utility
	{
	public:
                /** 
                 * Returns a string with the hexadecimal value of the long value specified
                 * @param value Long value to transform
                 * @returns string A string representation of the long number specified transformed in hexadecimal
                 */
		static const std::string transformXmlHexValue(long value);
                /** 
                 * Returns a string with the forbidden xml characters replaced ("&"->"amp;", "<"->"lt;", ">"->"gt;", "'"->"apos;" and "\""->"quot;").
                 * @param strl String to clean
                 * @returns string A string with the forbidden characters replaced
                 */
		static const std::string transformXmlEscChar(const std::string& strl);
                /** 
                 * Throws an exception if the value is false.
                 * @param value Value to check
                 * @param message Message of the exception to throw
                 * @throws Exception. Exception with the specified message if the value is false.
                 */
		static void assertion(const bool value, const std::string& message);
                /** 
                 * Returns the bool value of the attribute of a xml element
                 * @param element Xml element with the attribute to convert
                 * @param attributeName Name of the attribute to convert
                 * @returns bool The bool conversion of the attribute
                 */
		static bool attributeValueToBool(const xml::Element& element, const std::string& attributeName);
                /** 
                 * Returns a representation of the first not null child's value of the element.
                 * @param element Xml element with the child to convert to string
                 * @returns string The string representation of the first not null child element's value.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		static const std::string elementText(const xml::Element& element);
                /** 
                 * Returns the name of the element
                 * @param element Xml element to get the name from
                 * @returns string The name of the element
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		static const std::string elementName(const xml::Element& element);
                /** 
                 * Returns the name of the attribute
                 * @param attribute Attribute to get the name from
                 * @returns string The name of the attribute
                 * @throws Any exception. Exception if the attribute name is null
                 */
		static const std::string attributeName(const xml::Attribute& attribute);
                /** 
                 * Returns the value of the attribute
                 * @param attribute Attribute to get the value from
                 * @returns string The value of the attribute
                 * @throws Any exception. Exception if the attribute value is null
                 */
		static const std::string attributeValue(const xml::Attribute& attribute);
                /** 
                 * Returns the value of the element's attribute
                 * @param element Element with the attribute
                 * @param attribute Attribute to get the value from
                 * @returns string The value of the attribute
                 * @throws Any exception. SchemaException if the attribute value is null
                 */
		static const std::string attributeValueToString(const xml::Element& element, const std::string& attributeName);
                /** 
                 * Returns the double conversion of the element's attribute
                 * @param element Element with the attribute
                 * @param attribute Attribute to get the value from
                 * @returns double The double value of the attribute
                 * @throws Any exception. 
                 */
		static double attributeValueToDouble(const xml::Element& element, const std::string& attributeName);
                /** 
                 * Returns the long conversion of the element's attribute
                 * @param element Element with the attribute
                 * @param attribute Attribute to get the value from
                 * @returns long The long value of the attribute
                 * @throws Any exception. 
                 */
		static long attributeValueToLong(const xml::Element& element, const std::string& attributeName);
                /** 
                 * Returns the unsigned long conversion of the element's attribute. ACTUALLY IT DOESN'T, IT DOES THE SAME THAT attributeValueToLong
                 * @param element Element with the attribute
                 * @param attribute Attribute to get the value from
                 * @returns (unsigned) long The unsigned long value of the attribute
                 * @throws Any exception. 
                 */
		static long attributeValueToULong(const xml::Element& element, const std::string& attributeName);
                /** 
                 * Returns the long conversion in hexadecimal of the element's attribute
                 * @param element Element with the attribute
                 * @param attribute Attribute to get the value from
                 * @returns long The hexadecimal long value of the attribute
                 * @throws Any exception. 
                 */
		static long attributeValueToHex(const xml::Element& element, const std::string& attributeName);
                /** 
                 * Returns the unsigned int conversion of the element's attribute.
                 * @param element Element with the attribute
                 * @param attribute Attribute to get the value from
                 * @returns unsigned int The unsigned int value of the attribute
                 * @throws Any exception. 
                 */
		static unsigned int attributeValueToUInt(const xml::Element& element, const std::string& attributeName);
        
                /** 
                 * Returns true iff the element has the given attribute.
                 * @param element Element 
                 * @param attribute Attribute
                 * @returns bool True iff the element has the given attribute.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
        static bool hasAttribute(const xml::Element& element, const std::string& attributeName);
	};

	
}


#endif
