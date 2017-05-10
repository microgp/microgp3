/***********************************************************************\
|                                                                       |
| GroupPopulationParameters.xml.cc                                      |
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
 * @file GroupPopulationParameters.xml.cc
 * Implementation of the xml related methods of the GroupPopulationParameters class.
 * @see GroupPopulationParameters.h
 * @see GroupPopulationParameters.cc
 */

#include "EvolutionaryCore.h"

using namespace ugp3::core;
using namespace std;

const string GroupPopulationParameters::XML_SCHEMA_TYPE = "group";
const string GroupPopulationParameters::XML_CHILDELEMENT_MAXINDPERGROUP = "maxIndPerGroup";
const string GroupPopulationParameters::XML_CHILDELEMENT_MININDPERGROUP = "minIndPerGroup";
const string GroupPopulationParameters::XML_CHILDELEMENT_MAXGROUPS = "maxGroups";
const string GroupPopulationParameters::XML_CHILDELEMENT_MINGROUPS = "minGroups";
const string GroupPopulationParameters::XML_CHILDELEMENT_GROUP_NU = "groupNu";
const string GroupPopulationParameters::XML_CHILDELEMENT_GROUP_MU = "groupMu";
const string GroupPopulationParameters::XML_CHILDELEMENT_GROUPFITNESSSHARING = "groupFitnessSharing";
const string GroupPopulationParameters::XML_CHILDELEMENT_INDIVIDUALCONTRIBUTIONSCALINGFACTOR = "individualContributionScalingFactor";

void GroupPopulationParameters::writeInternalXml(ostream& output) const
{
	_STACK;

    output
    	<< "<!-- group population specific parameters -->" << endl
    	<< "<!-- For group populations, the parameters used to create groups and individuals are the size of the groups and the number of them. The mu and nu parameters aren't used. -->" << endl

    	<< "<!-- The maximum number of individuals in a group -->" << endl
        << "<" << XML_CHILDELEMENT_MAXINDPERGROUP
        << " " << XML_ATTRIBUTE_VALUE << "=\"" << this->maxIndPerGroup << "\"/>"
        << endl

    	<< "<!-- The minimum number of individuals in a group -->" << endl
        << "<" << XML_CHILDELEMENT_MININDPERGROUP
        << " " << XML_ATTRIBUTE_VALUE << "=\"" << this->minIndPerGroup << "\"/>"
        << endl

    	<< "<!-- The number of groups created to setup the population -->" << endl
        << "<" << XML_CHILDELEMENT_GROUP_NU
        << " " << XML_ATTRIBUTE_VALUE << "=\"" << this->groupNu << "\"/>"
        << endl

    	<< "<!-- The number of groups to keep at the end of every generation -->" << endl
        << "<" << XML_CHILDELEMENT_GROUP_MU
        << " " << XML_ATTRIBUTE_VALUE << "=\"" << this->groupMu << "\"/>"
        << endl;
        
        if (this->m_groupFitnessSharingEnabled)
        {
            output
            << "<!-- (optional) use specified fitnessSharing radius during the group evolution -->" << endl
            << "<" << XML_CHILDELEMENT_GROUPFITNESSSHARING << " " 
            << XML_ATTRIBUTE_RADIUS << "=\"" << this->m_groupFitnessSharingRadius << "\" />" 
            << endl;
        }
        
        output
        << "<!-- (optional) scaling power of the individual fitness wrt to their group contributions -->" << endl
        << "<" << XML_CHILDELEMENT_INDIVIDUALCONTRIBUTIONSCALINGFACTOR << " " 
        << XML_ATTRIBUTE_VALUE << "=\"" << m_individualContributionScalingFactor << "\" />" 
        << endl;

    SpecificIndividualPopulationParameters::writeInternalXml(output);
}

void GroupPopulationParameters::readXml(const xml::Element& element)
{
	_STACK;

    SpecificIndividualPopulationParameters::readXml(element);
    
    bool minFound = false;
    bool maxFound = false;
    bool muFound = false;
    bool nuFound = false;

    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        const string& elementName = childElement->ValueStr();
        if(elementName == XML_CHILDELEMENT_MAXINDPERGROUP)
        {
            if (xml::Utility::hasAttribute(*childElement, XML_ATTRIBUTE_VALUE)) {
                this->setMaxIndividualsPerGroup(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE));
                if (!getMaxIndividualsPerGroup() > 0) {
                    throw ugp3::ArgumentException("The element " + XML_CHILDELEMENT_MAXINDPERGROUP + " must have a positive value (found: " + ugp3::Convert::toString(getMaxIndividualsPerGroup()) + ")", LOCATION);
                }
                maxFound = true;
            }
        }
        else if(elementName == XML_CHILDELEMENT_MININDPERGROUP)
        {
            if (xml::Utility::hasAttribute(*childElement, XML_ATTRIBUTE_VALUE)) {
                this->setMinIndividualsPerGroup(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE));
                minFound = true;
            }
        }
        else if(elementName == XML_CHILDELEMENT_MAXGROUPS || elementName == XML_CHILDELEMENT_MINGROUPS)
        {
            LOG_ERROR << "The " << XML_CHILDELEMENT_MAXGROUPS << " and " << XML_CHILDELEMENT_MINGROUPS << " configuration elements are deprecated." << std::ends;
            LOG_ERROR << "Please use " << XML_CHILDELEMENT_GROUP_NU << " and " << XML_CHILDELEMENT_GROUP_MU << " instead." << std::ends;
            throw exception();
        }
        else if(elementName == XML_CHILDELEMENT_GROUP_NU)
        {
            if (xml::Utility::hasAttribute(*childElement, XML_ATTRIBUTE_VALUE)) {
                this->setGroupNu(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE));
                if (!getGroupNu() > 0) {
                    throw ugp3::ArgumentException("The element " + XML_CHILDELEMENT_GROUP_NU + " must have a positive value (found: " + ugp3::Convert::toString(getGroupNu()) + ")", LOCATION);
                }
                nuFound = true;
            }
        }
        else if(elementName == XML_CHILDELEMENT_GROUP_MU)
        {
            if (xml::Utility::hasAttribute(*childElement, XML_ATTRIBUTE_VALUE)) {
                this->setGroupMu(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE));
                if (!getGroupMu() > 0) {
                    throw ugp3::ArgumentException("The element " + XML_CHILDELEMENT_GROUP_MU + " must have a positive value (found: " + ugp3::Convert::toString(getGroupMu()) + ")", LOCATION);
                }
                muFound = true;
            }
        }
        else if (elementName == XML_CHILDELEMENT_GROUPFITNESSSHARING)
        {
            try
            {
                this->setGroupFitnessSharingRadius( xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_RADIUS) );
                LOG_DEBUG   << "\"" << XML_CHILDELEMENT_GROUPFITNESSSHARING << "\" found. Group fitness Sharing enabled with radius " << this->getFitnessSharingRadius() << ends;
            }
            catch (const exception& e)
            {
                LOG_ERROR << "While parsing " << XML_CHILDELEMENT_GROUPFITNESSSHARING << ": " << e.what() << ends;
                throw;
            }
        }
        else if (elementName == XML_CHILDELEMENT_INDIVIDUALCONTRIBUTIONSCALINGFACTOR)
        {
            try
            {
                this->setIndividualContributionScalingFactor( xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_VALUE) );
                LOG_DEBUG   << "\"" << XML_CHILDELEMENT_INDIVIDUALCONTRIBUTIONSCALINGFACTOR << "\" found, with value: " << this->getIndividualContributionScalingFactor() << ends;
            }
            catch (const exception& e)
            {
                LOG_ERROR << "While parsing " << XML_CHILDELEMENT_INDIVIDUALCONTRIBUTIONSCALINGFACTOR << ": " << e.what() << ends;
                throw;
            }
        }
        
        childElement = childElement->NextSiblingElement();
    }
    
    // TODO something better than that
    // TODO detect duplicate elements
    if (!maxFound)
        throw xml::MissingElementSchemaException(XML_CHILDELEMENT_MAXINDPERGROUP, LOCATION);
    if (!minFound)
        throw xml::MissingElementSchemaException(XML_CHILDELEMENT_MININDPERGROUP, LOCATION);
    if (!muFound)
        throw xml::MissingElementSchemaException(XML_CHILDELEMENT_GROUP_MU, LOCATION);
    if (!nuFound)
        throw xml::MissingElementSchemaException(XML_CHILDELEMENT_GROUP_NU, LOCATION);
    
    // Sanity check: nu should be higher than the maximum number of individuals in a group.
    if (getNu() < getMaxIndividualsPerGroup()) {
        LOG_ERROR << "You must set " << XML_CHILDELEMENT_NU << " to a value higher than "
        << XML_CHILDELEMENT_MAXINDPERGROUP << " or else MicroGP won't be able to build big groups"
        "while setting up the population" <<  std::ends;
        throw exception();
    }
}
