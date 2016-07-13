/***********************************************************************\
|                                                                       |
| SettingsContext.xml.cc                                                |
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
 * @file SettingsContext.xml.cc
 * Implementation xml related methods of the ContextSettings class.
 * @see ContextSettings.h
 * @see ContextSettings.cc
 * @see XMLIFace.h
 */

#include "ugp3_config.h"
#include "SettingsContext.h"
#include "Exceptions/SchemaException.h"
#include "Log.h"
using namespace ugp3;
using namespace std;

const string SettingsContext::XML_ATTRIBUTE_NAME = "name";
const string SettingsContext::XML_ATTRIBUTE_SEEDINGFILE = "seedingFile";
const string SettingsContext::XML_NAME = "context";
const string SettingsContext::POPULATION_XML_NAME = "population";
const string SettingsContext::POPULATIONNAMES_XML_NAME = "populationNames";

void SettingsContext::readXml(const xml::Element& element)
{
	_STACK;

	LOG_DEBUG << "Parsing ./" << this->getXmlName() << " ..." << ends;

	// get element name
	string elementString = element.ValueStr();
	if(elementString != this->getXmlName())
    	throw xml::SchemaException("expected element '" + this->getXmlName() + "'.", LOCATION);


	this->name = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_NAME);

    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        string childName = childElement->ValueStr();

        if(childName == Option::XML_NAME)
        {
            // new block of code
            string optionName = xml::Utility::attributeValueToString((*childElement), XML_ATTRIBUTE_NAME);
            LOG_DEBUG << "Option name is " << optionName << ends;

            if( optionName == Option::XML_POPULATIONS_NAME )
            {
                // temporary
                this->removeOption("population");
                this->removeOption("populationNames");
		this->removeOption("populationSeedingFiles"); // added 2016-04-10

                // if an option is called "populations", we treat it differently
                unique_ptr<Option> populationFiles( new Option() );
                unique_ptr<Option> populationNames( new Option() );
                unique_ptr<Option> populationSeedingFiles( new Option() ); // added 2016-04-10

                // two different kind of options (population names and population files)
                populationFiles->setName("population");
                populationFiles->setType("string");

                populationNames->setName("populationNames");
                populationNames->setType("string");
		
		// added 2016-04-10
		populationSeedingFiles->setName("populationSeedingFiles");
		populationSeedingFiles->setType("string");
		
		// TODO here we should add new 'options' for the population, e.g. the file with the
		//	names of the individuals we are trying to assimilate before starting

                bool firstName = true;

                const xml::Element* populationsChildElement = childElement->FirstChildElement();
                while(populationsChildElement != nullptr)
                {
                    if(firstName == true)
                    {
                        firstName = false;
                        populationFiles->setValue(xml::Utility::attributeValueToString((*populationsChildElement), "value"));
                        populationNames->setValue(xml::Utility::attributeValueToString((*populationsChildElement), XML_ATTRIBUTE_NAME));
                    }
                    else
                    {
                        populationFiles->setValue(populationFiles->getValue() + ";" + xml::Utility::attributeValueToString((*populationsChildElement), "value"));
                        populationNames->setValue(populationNames->getValue() + ";" + xml::Utility::attributeValueToString((*populationsChildElement), XML_ATTRIBUTE_NAME));
                    }
		    // in any case, check if the optional tag is set
		    if( xml::Utility::hasAttribute((*populationsChildElement), XML_ATTRIBUTE_SEEDINGFILE) )
		    {
			if(populationSeedingFiles->getValue().length() > 0) // some attributes of this kind have already been read
			{
				populationSeedingFiles->setValue(populationSeedingFiles->getValue() + ";" + 
									xml::Utility::attributeValueToString((*populationsChildElement), 
													XML_ATTRIBUTE_SEEDINGFILE));
			}
			else
			{
				// first attribute read
				populationSeedingFiles->setValue(xml::Utility::attributeValueToString((*populationsChildElement), 
													XML_ATTRIBUTE_SEEDINGFILE));
			}
		    } 
		    else // there is no tag
		    {
			if(populationSeedingFiles->getValue().length() > 0) // some attributes of this kind have already been read
			{
				populationSeedingFiles->setValue(populationSeedingFiles->getValue() + ";" + " ");
			}
			else
			{
				// first population read
				populationSeedingFiles->setValue(" ");
			}
		    }

                    populationsChildElement = populationsChildElement->NextSiblingElement();
                }

                this->addOption(std::move(populationFiles));
                this->addOption(std::move(populationNames));
		this->addOption(std::move(populationSeedingFiles)); // added 2016-04-10

            } // end of new block
            else
            {
                unique_ptr<Option> option( new Option() );

                option->readXml(*childElement);
                LOG_DEBUG << "Reading " << this->name << "/" << option->getName() << ends;

                if(this->hasOption(option->getName()) == true)
                {
                    Option& existingOption = this->getOption(option->getName());
                    existingOption.readXml(*childElement);
                }
                else this->addOption(std::move(option));
            }// end of "populations" if

        } // end of "option" if
        else throw xml::SchemaException("Expected element option.", LOCATION);

        childElement = childElement->NextSiblingElement();
    }
}

void SettingsContext::writeXml(ostream& output) const
{
	_STACK;

	output << "<" << this->getXmlName() << " "
	<< XML_ATTRIBUTE_NAME << "=\"" << this->name << "\">" << endl;

	for(unsigned int i = 0; i < this->options.size(); i++)
	{
	    if( this->options[i]->getName() != "population" && this->options[i]->getName() != "populationNames" )
	    {
            this->options[i]->writeXml(output);
	    }
	    else if( this->options[i]->getName() == "population" )
	    {
            output << "<!-- For each population, its name and the file where the population parameters are defined. -->" << endl
                   << "<option name =\"populations\">" << endl;

            vector<std::string> populationFiles = this->getOption(i).toList();
            vector<std::string> populationNames = this->getOption(i+1).toList();

            for(unsigned int i = 0; i < populationNames.size() && i < populationFiles.size(); i++)
            {
                output << "<population name=\"" << populationNames[i] << "\" value=\"" << populationFiles[i] << "\" />" << endl;
            }

            output << "</option>" << endl;
        }
	}

	output << "</" << this->getXmlName() << ">" << endl;
}

