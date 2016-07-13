/***********************************************************************\
|                                                                       |
| OuterLabelParameter.xml.cc                                            |
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
#include "Constraints.h"
using namespace std;
using namespace ugp3::constraints;

void OuterLabelParameter::writeXml(ostream& output) const
{
	_STACK;

    LOG_DEBUG << "Serializing object ugp3::constraints::OuterLabelParameter" << ends;

	if(this->typeDefinition == nullptr)
	{
		output 
            << "<" << this->getXmlName() << " xsi:type=\"outerLabel\" " 
            << XML_ATTRIBUTE_NAME << "=\"" << xml::Utility::transformXmlEscChar(this->getName()) << "\"";
		
	  

        output << ">" << endl;

		for(unsigned int i = 0 ; i < this->subSectionNames.size() ; i++)
		{
			string path = this->subSectionNames[i];

			istringstream stream;
			stream.str(path);
			string sectionId;
			stream >> sectionId;
			string subSectionId;
			stream >> subSectionId;

			output 
                << "<ref" 
                << " "
                << "section=\"" << xml::Utility::transformXmlEscChar(sectionId) << "\"" 
                << " "
                << "subSection=\"" <<  xml::Utility::transformXmlEscChar(subSectionId) << "\""
                << "/>" << endl;
		}

		output << "</" << this->getXmlName() << ">" << endl;
	}
	else output << this->writeXmlAsTypeDefinition();
}

void OuterLabelParameter::readXml(const xml::Element& element)
{
    _STACK;

    Parameter::readXml(element);

    this->subSectionNames.clear();

 

    const xml::TiXmlNode* childNode = element.FirstChild();
    while(childNode != nullptr)
    {
        if(childNode->ToElement() != nullptr)
        {
            const xml::Element* childElement = childNode->ToElement();
            string childName = childElement->ValueStr();

            if(childName != "ref")
                throw xml::SchemaException("expected element ./item/ref", LOCATION);

            string sectionName = 
                xml::Utility::attributeValueToString(*childElement, "section") + " " + 
                xml::Utility::attributeValueToString(*childElement, "subSection");

           

            this->subSectionNames.push_back(sectionName);
        }

        childNode = element.IterateChildren(childNode);
    }

    if(this->subSectionNames.empty())
        throw xml::MissingElementSchemaException("./item/ref", LOCATION);
}

