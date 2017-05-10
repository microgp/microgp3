/***********************************************************************\
|                                                                       |
| CandidateSelection.xml.cc                                            |
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
 * @file CandidateSelection.xml.cc
 * Implementation of the xml related methods of the CandidateSelection class.
 * @see CandidateSelection.h
 */

#include "ugp3_config.h"
#include "EvolutionaryCore.h"
using namespace std;
using namespace ugp3::core;
using namespace std;

const string CandidateSelection::XML_NAME = "selection";
const string CandidateSelection::XML_ATTRIBUTE_TYPE = "type";

unique_ptr<CandidateSelection> CandidateSelection::instantiate(const xml::Element& element)
{
    if(element.ValueStr() != XML_NAME)
    {
        throw xml::SchemaException("expected element 'selection' (found '" + element.ValueStr() + "').", LOCATION);
    }
    
    const string& type = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_TYPE);
    
    CandidateSelection* selection = nullptr;
    if(type == TournamentSelection::XML_SCHEMA_TYPE)
    {
        selection = new TournamentSelection();
    }
    else if(type == TournamentSelectionWithFitnessHole::XML_SCHEMA_TYPE)
    {
        selection = new TournamentSelectionWithFitnessHole();
    }
    else if(type == TwoStepTournamentSelectionWithFitnessHole::XML_SCHEMA_TYPE)
    {
        selection = new TwoStepTournamentSelectionWithFitnessHole();
    }
    else if(type == RankingSelection::XML_SCHEMA_TYPE)
    {
        selection = new RankingSelection();
    }
    // insert here new types of individual selection 

    if(selection == nullptr)
    {
        throw Exception("The selection type '" + type + "' is unknown.", LOCATION);
    }
    
    selection->readXml(element);
    
    return unique_ptr<CandidateSelection> (selection);
}
