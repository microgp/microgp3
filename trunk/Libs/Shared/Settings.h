/***********************************************************************\
|                                                                       |
| Settings.h                                                            |
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
 * @file Settings.h
 * Definition of the Settings class.
 * @see Settings.cc
 * @see Settings.xml.cc
 * @see XMLIFace.h
 */

#ifndef HEADER_UGP3_SETTINGS
/** Defines that this file has been included */
#define HEADER_UGP3_SETTINGS

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits>
#include <fstream>
#include <memory>

#include "StackTrace.h"
#include "Log.h"
#include "Utility.h"
#include "Random.h"
#include "XMLIFace.h"
#include "Exceptions/SchemaException.h"
#include "Convert.h"
#include "SettingsContext.h"


/**
 * ugp3 namespace
 */
namespace ugp3
{
    /**
     * @class Settings
     * Manages the settings of ugp
     * @see SettingsContext
     * @see xml::XMLIFace
     */
    class Settings : public xml::XMLIFace
    {
    private:
        //Vector of sets of options related by a context name
        std::vector<SettingsContext*> contexts;

    public:
        /** Name of this xml element */
        static const std::string XML_NAME;

    public:
        /** 
         * Returns the set of options with the specified name.
         * @param name Name of the SettingsContext
         * @returns SettingsContext The set of options with the specified name
         * @throws Any exception. If the SettingsContext doesn't exist, an exception specifying this fact is thrown.
         */
    	SettingsContext& getContext(const std::string& name);
        /** 
         * Returns the option of a context.
         * @param contextName Name of the set of options 
         * @param optName Name of the option
         * @returns Option The option corresponding to the paremeters
         * @throws Any exception. If there is not a SettingsContext with this name or the option doesn't exist, an exception specifying this fact is thrown.
         */
    	Option& getOption(const std::string& contextName, const std::string& optionName);
        /** 
         * Checks if a context exists in the settings
         * @param name of the context
         * @returns bool True if the context exists, false in other case.
         * @throws Nothing. If an exception is thrown, the execution is aborted.
         */
    	bool hasContext(const std::string& name);
        /** 
         * Register a new context in the settings.
         * @param context to add
         * @throws Standard exception. Exception if repeated context.
         */
    	void addContext(std::unique_ptr< ugp3::SettingsContext > context);
        /** 
         * Register a new context in the settings.
         * @param name of the context to add
         * @throws Standard exception. Exception if repeated context.
         */
    	void addContext(const std::string& name);
        /** 
         * Removes a context of the settings -- NOT IMPLEMENTED.
         * @param name of the context to remove
         * @throws Standard exception.
         */
    	void removeContext(const std::string& name); //added by AT

        /** 
         * Destructor of the class.
         * @throws Nothing. If an exception is thrown, the execution is aborted.
         */
    	virtual ~Settings();


    public:
        /** 
         * Constructor of the class.
         * @throws Nothing. If an exception is thrown, the execution is aborted.
         */
        Settings();
        /** 
         * Reads and loads the settings from a xml file
         * @param xmlFile of the xml file
         * @throws Standard exception.
         */
        void load(const std::string& xmlFile);
        /** 
         * Saves the settings in a xml file.
         * @param xmlFile of the xml file
         * @throws Standard exception. Exception if can not access the file.
         */
        void save(const std::string& xmlFile);
        /** 
         * Remoes all the contexts of the settings.
         * @throws Nothing. If an exception is thrown, the execution is aborted.
         */
        void clear();

    public:// Xml interface
		virtual void writeXml(std::ostream& output) const;
		void readXml(const xml::Element& element);
		virtual const std::string& getXmlName() const;
    };
}

namespace ugp3
{
	inline const std::string& Settings::getXmlName() const
	{
		return XML_NAME;
	}

	inline void Settings::addContext(const std::string& name)
	{
		_STACK;
		this->addContext(std::unique_ptr< SettingsContext >(new SettingsContext(name)));
	}

	inline Option& Settings::getOption(const std::string& contextName, const std::string& optionName)
	{
		_STACK;

		return this->getContext(contextName).getOption(optionName);
	}

	inline Settings::~Settings()
	{ }
}

#endif

