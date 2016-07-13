/***********************************************************************\
|                                                                       |
| SettingsContext.cc                                                    |
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
 * @file SettingsContext.h
 * Definition of the SettingsContext class.
 * @see SettingsContext.cc
 * @see SettingsContext.xml.cc
 * @see XMLIFace.h
 */

#ifndef HEADER_UGP3_SETTINGSCONTEXT
/** Defines that this file has been included */
#define HEADER_UGP3_SETTINGSCONTEXT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <memory>
#include "Option.h"

/**
 * ugp3 namespace
 */
namespace ugp3
{
	class Settings;

        /**
         * @class SettingsContext
         * Context of options. Keeps a set of options and allows to add and remove them.
         * @see Settings
         * @see xml::XMLIFace
         */
	class SettingsContext : public xml::XMLIFace
	{
	private:
                //Options in the context
		std::vector<Option*> options;
                //Name of the context
		std::string name;

    private:
        //Tags to read and write xml
        static const std::string XML_ATTRIBUTE_NAME;
	static const std::string XML_ATTRIBUTE_SEEDINGFILE;
        static const std::string POPULATION_XML_NAME;
        static const std::string POPULATIONNAMES_XML_NAME;

    public:
        /** Name of this xml element */
        static const std::string XML_NAME;

	public:
                /** 
                 * Constructor of the class.
                 * Creates an empty object.
                 */
		SettingsContext();
                /** 
                 * Constructor of the class.
                 * Creates an object with the specified name.
                 * @param name Name of the object
                 */
		SettingsContext(const std::string& name);
                /** 
                 * Destructor of the class.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		virtual ~SettingsContext();
                /** 
                 * Returns the option with the specified name.
                 * @param name Name of the option
                 * @returns Option The option with the specified name
                 * @throws Any exception. If the option doesn't exist, an exception specifying this fact is thrown.
                 */
		Option& getOption(const std::string& name);
                /** 
                 * Returns the option with the specified index.
                 * @param index Index of the option
                 * @returns Option The option with the specified name
                 * @throws Any exception. If the option doesn't exist, an exception specifying this fact is thrown.
                 */
		Option& getOption(unsigned int index);
                /** 
                 * Returns the option with the specified index.
                 * @param index Index of the option
                 * @returns Option The option with the specified name
                 * @throws Nothing. If an exception is thrown, the execution is aborted. 
                 */
		Option& getOption(unsigned int index) const;
                /** 
                 * Returns the number of options registered.
                 * @returns int The number of options.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		unsigned int getOptionCount() const;

                /** 
                 * Register a new option in the context.
                 * @param option The option to register.
                 * @throws Standard exception. ArgumenException if a nullptr pointer is passed. Exception if a repeated option.
                 */
		void addOption(std::unique_ptr< ugp3::Option > option);
                /** 
                 * Register a new option in the context.
                 * @param name of the option
                 * @param value of the option
                 * @param type of the option, by default "string"
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		void addOption(const std::string& name, const std::string& value, const std::string& type = "string");
                /** 
                 * Delete an option of the context.
                 * @param name of the option
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		void removeOption(const std::string& name);

                /** 
                 * Checks if an option exists in the context
                 * @param name of the option
                 * @returns bool True if the option exists, false in other case.
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		bool hasOption(const std::string& name);
                /** 
                 * Returns the name of the context
                 * @returns string The name of the context 
                 * @throws Nothing. If an exception is thrown, the execution is aborted.
                 */
		const std::string& getName() const;

	public: // Xml interface
		virtual void writeXml(std::ostream& output) const;
		virtual const std::string& getXmlName() const;
		virtual void readXml(const xml::Element& element);
	};


	inline SettingsContext::SettingsContext()
	{ }

	inline SettingsContext::SettingsContext(const std::string& name)
		: name(name)
	{ }

	inline Option& SettingsContext::getOption(unsigned int index)
	{
        return *this->options.at(index);
	}

	inline Option& SettingsContext::getOption(unsigned int index) const
	{
        return *this->options.at(index);
    }

	inline const std::string& SettingsContext::getName() const
	{
		return this->name;
	}

	inline void SettingsContext::addOption(const std::string& name, const std::string& value, const std::string& type)
	{
		_STACK;
		this->addOption(std::unique_ptr<Option>(new Option(name, value, type)));
	}

	inline unsigned int SettingsContext::getOptionCount() const
	{
		return (unsigned int) this->options.size();
	}

	inline const std::string& SettingsContext::getXmlName() const
	{
		return XML_NAME;
	}
}

#endif
