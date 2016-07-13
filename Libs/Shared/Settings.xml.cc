/***********************************************************************\
|                                                                       |
| Settings.xml.cc                                                       |
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
 * @file Settings.xml.cc
 * Implementation xml related methods of the Settings class.
 * @see Settings.h
 * @see Settings.cc
 */

#include "ugp3_config.h"
#include "Settings.h"
using namespace ugp3;
using namespace std;

const string Settings::XML_NAME = "settings";

void Settings::writeXml(ostream& output) const
{
	_STACK;
	
	output << "<" << this->getXmlName() << ">" << endl;
	
	for(unsigned int i = 0; i < this->contexts.size(); i++)
	{
		this->contexts[i]->writeXml(output);
	}

	output << "</" << this->getXmlName() << ">" << endl;
}

void Settings::readXml(const xml::Element& element)
{
	_STACK;

	LOG_DEBUG << "Parsing ./" << this->getXmlName() << " ..." << ends;

	// get element name
	string elementString = element.ValueStr();
	if(elementString != this->getXmlName())
    	throw xml::SchemaException("expected element '" + this->getXmlName() + "'.", LOCATION);

  
	const xml::Element* childElement = element.FirstChildElement();
	while(childElement != nullptr)
	{
    	string childName = childElement->ValueStr();

   	 	if(childName == SettingsContext::XML_NAME)
		{
			
			unique_ptr<SettingsContext> context( new SettingsContext() );
			context->readXml(*childElement);
			
			if(this->hasContext(context->getName()) == true)
			{
				SettingsContext& existingContext = this->getContext(context->getName());
				existingContext.readXml(*childElement);
			}
			else this->addContext(std::move(context));
		}
		else throw xml::SchemaException("Expected element context.", LOCATION);
		
		childElement = childElement->NextSiblingElement();
	}
	
}

