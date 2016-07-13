/***********************************************************************\
|                                                                       |
| Option.h                                                              |
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
| the Free Software Foundation, either version 3 of the License, or     |
| (at your option) any later version.                                   |
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
 * @file Option.h
 * Definition of the Option class.
 * @see Option.cc
 * @see Option.xml.cc
 * @see XMLIFace.h
 * @see IString.h
 */

#ifndef HEADER_UGP3_OPTION
/** Defines that this file has been included */
#define HEADER_UGP3_OPTION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>
#include "XMLIFace.h"
#include "IString.h"
#include "Utility.h"
#include "Convert.h"

/**
 * ugp3 namespace
 */
namespace ugp3
{
        /**
         * @class Option
         * Option of ugp. Keep the name, value, type and descryption of an option of ugp. Provides methods to handle this option.
         * @see xml::XMLIFace
         * @see IString
         */
	class Option : public xml::XMLIFace, public IString
	{
	private:
		std::string name; //Name of the option.
		std::string value; //Value of the option.
		std::string type; //Type of the option. Allowed values: "string", "integer", "boolean" and "float".
		std::string description; //Description of the option.

	private:
                //Validates the type and value of the option. Returns true if both are valids.
		bool validate();

	private:
        //Tags for xml handling.
        static const std::string XML_ATTRIBUTE_NAME;
        static const std::string XML_ATTRIBUTE_VALUE;
        static const std::string XML_ATTRIBUTE_TYPE;

    public:
        //Tags for xml handling.
        static const std::string XML_NAME;
        static const std::string XML_POPULATIONS_NAME;

	public:
                /** 
                 * Constructor of the class. Sets the name, value and type of the option.
                 * @param name Name of the option.
                 * @param value Value of the option.
                 * @param type Type of the option. Allowed values: "string", "integer", "boolean" and "float".
                 */
		Option(
		    const std::string& name,
		    const std::string& value,
		    const std::string& type = "string");

                /** 
                 * Constructor of the class. Creates an empty option.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		Option();
                /** 
                 * Destructor of the class.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		~Option();

	public:
                /** 
                 * Returns the name of the option.
                 * @returns string Name of the option.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		const std::string& getName() const;
                /** 
                 * Returns the value of the option.
                 * @returns string Value of the option.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		const std::string& getValue() const;
                /** 
                 * Returns the type of the option.
                 * @returns string Type of the option.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		const std::string& getType() const;
                /** 
                 * Returns the description of the option.
                 * @returns string Description of the option.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		const std::string& getDescription() const;

                /** 
                 * Sets the value of the option.
                 * @param Value of the option.
                 * @throws Any exception 
                 */
		void setValue(const std::string&);
                /** 
                 * Sets the name of the option.
                 * @param Name of the option.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		void setName(const std::string&);
                /** 
                 * Sets the type of the option.
                 * @param Type of the option.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		void setType(const std::string&);
                /** 
                 * Sets the description of the option.
                 * @param Description of the option.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		void setDescription(const std::string& value);

                /** 
                 * Returns the value of the option converted to long.
                 * @returns long Value of the option.
                 * @throws Any exception
                 */
		long toLong() const;
                /** 
                 * Returns the value of the option converted to unsigned int.
                 * @returns unsigned int Value of the option.
                 * @throws Any exception
                 */
		unsigned int toUInt() const;
                /** 
                 * Returns the value of the option converted to bool.
                 * @returns bool Value of the option.
                 * @throws Any exception
                 */
		bool toBool() const;
                /** 
                 * Returns the value of the option converted to double.
                 * @returns double Value of the option.
                 * @throws Any exception
                 */
		double toDouble() const;
                /** 
                 * Returns a vector in which the elements are the value of the option splitted by the character ';'.
                 * @returns vector Value of the option splitted by the character ';'.
                 * @throws Any exception
                 */
		std::vector<std::string> toList() const;

	public: // Xml interface
		virtual void writeXml(std::ostream& output) const;
		virtual const std::string& getXmlName() const;
		virtual void readXml(const xml::Element& element);

	public: // IString interface
		virtual const std::string toString() const;
	};


	inline long Option::toLong() const
	{
		return Convert::toLong(this->value);
	}

	inline unsigned int Option::toUInt() const
	{
		return Convert::toUInt(this->value);
	}

	inline bool Option::toBool() const
	{
		return Convert::toBool(this->value);
	}

	inline double Option::toDouble() const
	{
		return Convert::toDouble(this->value);
	}

	inline const std::string& Option::getDescription() const
	{
	    return this->description;
	}

	inline void Option::setDescription(const std::string& value)
	{
	    this->description = value;
	}

	inline void Option::setName(const std::string& value)
	{
        this->name = value;
    }

    inline void Option::setType(const std::string& value)
	{
        this->type = value;
    }

	inline Option::~Option()
	{ }

	inline void Option::setValue(const std::string& value)
	{
		_STACK;

		this->value = value;

		if(this->validate() == false)
		{
			throw Exception("value attribute is not valid for option '"
			    + this->name + "'.", LOCATION);
		}
	}

	inline const std::string Option::toString() const
	{
		return this->value;
	}

	inline const std::string& Option::getName() const
	{
		return this->name;
	}

	inline const std::string& Option::getType() const
	{
		return this->type;
	}

	inline const std::string& Option::getValue() const
	{
		return this->value;
	}

	inline Option::Option()
	  : type("string")
	{ }

	inline const std::string& Option::getXmlName() const
	{
		return XML_NAME;
	}
}

#endif
