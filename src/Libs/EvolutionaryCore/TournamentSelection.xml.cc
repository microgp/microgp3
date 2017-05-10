/***********************************************************************\
|                                                                       |
| TournamentSelection.xml.cc                                            |
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
#include "EvolutionaryCore.h"
using namespace std;
using namespace ugp3::core;

const string TournamentSelectionWithFitnessHole::XML_SCHEMA_TYPE = "tournamentWithFitnessHole";
const string TwoStepTournamentSelectionWithFitnessHole::XML_SCHEMA_TYPE = "twoStepTournamentWithFitnessHole";
const string TournamentSelection::XML_SCHEMA_TYPE = "tournament";

const string TournamentSelection::XML_ATTRIBUTE_TAU = "tau";
const string TournamentSelection::XML_ATTRIBUTE_TAUMAX = "tauMax";
const string TournamentSelection::XML_ATTRIBUTE_TAUMIN = "tauMin";

// meta-tau(s) are tau(s) expressed as percentage of the population
const string TournamentSelection::XML_ATTRIBUTE_METATAU = "metaTau";
const string TournamentSelection::XML_ATTRIBUTE_METATAUMAX = "metaTauMax";
const string TournamentSelection::XML_ATTRIBUTE_METATAUMIN = "metaTauMin";

const string TournamentSelectionWithFitnessHole::XML_ATTRIBUTE_FITNESSHOLE = "fitnessHole";

void TournamentSelectionWithFitnessHole::writeXml(std::ostream& output) const
{
	output << "<" << this->getXmlName()
        << " " 
        << XML_ATTRIBUTE_TYPE << "=\"" << XML_SCHEMA_TYPE << "\""
		<< " " 
		<< XML_ATTRIBUTE_TAU << "=\"" << this->getTau() << "\""
		<< " " 
		<< XML_ATTRIBUTE_TAUMIN << "=\"" << this->getTauMin() << "\""
		<< " " 
		<< XML_ATTRIBUTE_TAUMAX << "=\"" << this->getTauMax() << "\""
		<< " " 
		<< XML_ATTRIBUTE_FITNESSHOLE << "=\"" << this->fitnessHole << "\" />" << endl;
}

void TournamentSelectionWithFitnessHole::readXml(const xml::Element& element)
{
	TournamentSelection::readXml(element);
	
	this->fitnessHole = xml::Utility::attributeValueToDouble(element, XML_ATTRIBUTE_FITNESSHOLE);
}

void TournamentSelection::writeXml(std::ostream& output) const
{
	output << "<" << this->getXmlName()
        << " " 
        << XML_ATTRIBUTE_TYPE << "=\"" << XML_SCHEMA_TYPE << "\""
		<< " " 
		<< XML_ATTRIBUTE_TAU << "=\"" << this->getTau() << "\""
		<< " " 
		<< XML_ATTRIBUTE_TAUMIN << "=\"" << this->getTauMin() << "\""
		<< " " 
		<< XML_ATTRIBUTE_TAUMAX << "=\"" << this->getTauMax() << "\" />" << endl;
}

void TournamentSelection::readXml(const xml::Element& element)
{
    if(element.ValueStr() != this->getXmlName()) 
    {
        throw xml::SchemaException("expected element '/selection'", LOCATION);
    }

    // this is to verify something at the end of the method
    bool readTau = false, readTauMin = false, readTauMax = false;

    // for each tau*, either tau* or metaTau* can and MUST be specified
    if( element.Attribute( XML_ATTRIBUTE_TAU ) != nullptr )
    {
    	this->tau = xml::Utility::attributeValueToDouble(element, XML_ATTRIBUTE_TAU);
	readTau = true;
    }
    else if( element.Attribute( XML_ATTRIBUTE_METATAU ) != nullptr )
    {
	this->metaTau = xml::Utility::attributeValueToDouble(element, XML_ATTRIBUTE_METATAU);
    }
    else
	throw xml::SchemaException("selection type \"" + XML_SCHEMA_TYPE + "\" needs either attribute \"" + XML_ATTRIBUTE_TAU + "\" or attribute \"" + XML_ATTRIBUTE_METATAU + "\".", LOCATION);

    if( element.Attribute( XML_ATTRIBUTE_TAUMIN ) != nullptr )
    {
    	this->tauMin = xml::Utility::attributeValueToDouble(element, XML_ATTRIBUTE_TAUMIN);
	readTauMin = true;
    }
    else if( element.Attribute( XML_ATTRIBUTE_METATAUMIN ) != nullptr )
    {
	this->metaTauMin = xml::Utility::attributeValueToDouble(element, XML_ATTRIBUTE_METATAUMIN);
    }
    else
	throw xml::SchemaException("selection type \"" + XML_SCHEMA_TYPE + "\" needs either attribute \"" + XML_ATTRIBUTE_TAUMIN + "\" or attribute \"" + XML_ATTRIBUTE_METATAUMIN + "\".", LOCATION);

    if( element.Attribute( XML_ATTRIBUTE_TAUMAX ) != nullptr )
    {
    	this->tauMax = xml::Utility::attributeValueToDouble(element, XML_ATTRIBUTE_TAUMAX);
	readTauMax = true;
    }
    else if( element.Attribute( XML_ATTRIBUTE_METATAUMAX ) != nullptr )
    {
	this->metaTauMax = xml::Utility::attributeValueToDouble(element, XML_ATTRIBUTE_METATAUMAX);
    }
    else
	throw xml::SchemaException("selection type \"" + XML_SCHEMA_TYPE + "\" needs either attribute \"" + XML_ATTRIBUTE_TAUMAX + "\" or attribute \"" + XML_ATTRIBUTE_METATAUMAX + "\".", LOCATION);

    if( readTau != readTauMin || readTau != readTauMax )
    {
	throw xml::SchemaException("selection type \"" + XML_SCHEMA_TYPE + "\" needs all attributes expressed as \"tau/tauMin/tauMax\" (number of individuals in the tournament) or \"metaTau/metaTauMin/metaTauMax\" (percentage of the population).", LOCATION);
    }
}

