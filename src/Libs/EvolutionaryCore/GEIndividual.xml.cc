/***********************************************************************\
|                                                                       |
| GEIndividual.xml.cc                                                   |
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
 * @file GEIndividual.xml.cc
 * Implementation of the GEIndividual class xml related methods.
 * @see GEIndividual.h
 * @see GEIndividual.cc
 */

#include "EvolutionaryCore.h"

using namespace ugp3::core;

const string GEIndividual::XML_TYPE = "group";


void GEIndividual::readXml(const xml::Element& element)
{
    Individual::readXml(element);
    
    const xml::Element* childElement = element.FirstChildElement();
    while (childElement) {
        string elementName = childElement->ValueStr();
        if (elementName == ScaledFitness::XML_NAME) {
            m_fitness.readXml(*childElement);    
        }
        
        childElement = childElement->NextSiblingElement();
    }
}

void GEIndividual::writeInnerXml(ostream& output) const
{
    Individual::writeInnerXml(output);
}