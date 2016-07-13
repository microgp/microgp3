/***********************************************************************\
|                                                                       |
| Evaluator.xml.cc                                                      |
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

const string Evaluator::XML_NAME = "evaluation";
const string Evaluator::XML_CHILDELEMENT_CONCURRENTEVALUATIONS = "concurrentEvaluations";
const string Evaluator::XML_CHILDELEMENT_EVALUATORINPUTPATHNAME = "evaluatorInputPathName";
const string Evaluator::XML_CHILDELEMENT_EVALUATOROUTPUTPATHNAME = "evaluatorOutputPathName";
const string Evaluator::XML_CHILDELEMENT_EVALUATORPATHNAME = "evaluatorPathName";
const string Evaluator::XML_CHILDELEMENT_REMOVETEMPFILES = "removeTempFiles";
const string Evaluator::XML_CHILDELEMENT_TOTALMILLISECONDS = "totalMilliseconds";
const string Evaluator::XML_CHILDELEMENT_CACHESIZE = "cacheSize";


void Evaluator::readXml(const xml::Element& element)
{
	_STACK;

	this->clear();

	LOG_DEBUG << "Parsing /evolutionaryAlgorithm/evaluation..." << ends;

	// get the name of the element
	if(element.ValueStr() != this->getXmlName())
	{
		throw xml::SchemaException("expected element '/evolutionaryAlgorithm/evaluation'", LOCATION);
	}

	bool filesFound = false;
	bool scriptFileFound = false;
	bool inputFileFound = false;
	bool outputFileFound = false;
	bool environmentFound = false;
	const xml::Element* childElement = element.FirstChildElement();
	while(childElement != nullptr)
	{
		string elementName = childElement->ValueStr();

        if(elementName == XML_CHILDELEMENT_CONCURRENTEVALUATIONS /*"concurrentEvaluations"*/)
        {
            m_concurrentEvaluations = xml::Utility::attributeValueToUInt(*childElement, "value");
        }
        else if(elementName == XML_CHILDELEMENT_REMOVETEMPFILES /*"removeTempFiles"*/)
        {
            m_removeTemporaryFiles = xml::Utility::attributeValueToBool(*childElement, "value");
        }
        else if(elementName == XML_CHILDELEMENT_TOTALMILLISECONDS /*"totalMilliSeconds"*/)
        {
            m_totalMilliSeconds = std::chrono::milliseconds(xml::Utility::attributeValueToUInt(*childElement, "value"));
        }
        else if(elementName == XML_CHILDELEMENT_EVALUATORPATHNAME /*"evaluatorPathName"*/)
        {
            m_scriptFile = xml::Utility::attributeValueToString(*childElement, "value");
            scriptFileFound = true;
        }
        else if(elementName == XML_CHILDELEMENT_EVALUATORINPUTPATHNAME /*"evaluatorInputPathName"*/)
        {
            m_inputFile = xml::Utility::attributeValueToString(*childElement, "value");
            inputFileFound = true;
        }
        else if(elementName == XML_CHILDELEMENT_EVALUATOROUTPUTPATHNAME /*"evaluatorOutputPathName"*/)
        {
            m_outputFile = xml::Utility::attributeValueToString(*childElement, "value");
            outputFileFound = true;
        }
		else if(elementName == "files")
		{
			if (filesFound) {
				throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/evaluation/files", LOCATION);
			}

			filesFound = true;

			LOG_DEBUG << "Parsing /evolutionaryAlgorithm/evaluation/files..." << ends;

			m_scriptFile = xml::Utility::attributeValueToString(*childElement, "script");
			m_inputFile = xml::Utility::attributeValueToString(*childElement, "input");
			m_outputFile = xml::Utility::attributeValueToString(*childElement, "output");
		}
		else if(elementName == "environment")
		{
			if(environmentFound == true)
			{
				throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/evaluation/environment", LOCATION);
			}

			environmentFound = true;

			this->parseEnvironmentVariables(*childElement);
		}
		else if (elementName == XML_CHILDELEMENT_CACHESIZE)
        {
             m_cacheSize = xml::Utility::attributeValueToUInt(*childElement, "value");
        }

		childElement = childElement->NextSiblingElement();
	}

	if (!(filesFound || (scriptFileFound && outputFileFound && inputFileFound))) {
		throw xml::MissingElementSchemaException("/evolutionaryAlgorithm/evaluation/script", LOCATION);
	}
}

void Evaluator::parseEnvironmentVariables(const xml::Element& element)
{
	_STACK;

	LOG_DEBUG << "Parsing /evolutionaryAlgorithm/evaluation/environment..." << ends;

	// get the name of the element
	if(element.ValueStr() != "environment")
		throw xml::SchemaException("expected element '/evolutionaryAlgorithm/evaluation/environment'", LOCATION);

	// get the inner elements
	const xml::Element* childElement = element.FirstChildElement();
	while(childElement != nullptr)
	{
		if(childElement->ValueStr() == "variable")
		{
			LOG_DEBUG << "Parsing /evolutionaryAlgorithm/evaluation/environment/variable..." << ends;

			string name = xml::Utility::attributeValueToString(*childElement, "name");
			string value = xml::Utility::attributeValueToString(*childElement, "value");

			//(!)20090825 this->setEnvironmentVariable(name, value);
		}
		else
		{
			throw xml::SchemaException("unexpected element \"" + childElement->ValueStr() + "\"", LOCATION);
		}

		childElement = childElement->NextSiblingElement();
	}

//(!)20090825	if(this->environmentVariables.size() == 0)
//(!)20090825		throw xml::MissingElementSchemaException("/evolutionaryAlgorithm/evaluation/environment/variable", LOCATION);
}

void Evaluator::writeXml(ostream& output) const
{
	_STACK;

    LOG_DEBUG << "Serializing object ugp3::core::Evaluator" << ends;

	output
	<< "<" << this->getXmlName() << ">" << endl
        << " <" << XML_CHILDELEMENT_CONCURRENTEVALUATIONS << " value=\"" << m_concurrentEvaluations << "\" />" << endl
        << " <" << XML_CHILDELEMENT_CACHESIZE << " value=\"" << getCacheSize() << "\" />" << endl;

        if (m_totalMilliSeconds.count() != 0) // discriminate between status.xml (totalMilliSeconds != 0) and population.settings.xml (totalMilliseconds == 0)
        {
        output
        << " <" << XML_CHILDELEMENT_TOTALMILLISECONDS << " value=\"" << m_totalMilliSeconds.count() << "\" />" << endl;
        //<< " <totalMilliSeconds value=\"" << this->totalMilliSeconds << "\" />" << endl
        //<< " <totalEvaluations value=\"" << this->totalEvaluations << "\" />" << endl;
	}

        output
        << " <" << XML_CHILDELEMENT_REMOVETEMPFILES << " value=\"" 
	<< (m_removeTemporaryFiles == 0? "false" : "true") << "\" />" << endl
        << " <" << XML_CHILDELEMENT_EVALUATORPATHNAME << " value=\"" 
	<< xml::Utility::transformXmlEscChar(m_scriptFile) << "\" />" << endl
        << " <" << XML_CHILDELEMENT_EVALUATORINPUTPATHNAME << " value=\"" 
	<< xml::Utility::transformXmlEscChar(m_inputFile) << "\" />" << endl
        << " <" << XML_CHILDELEMENT_EVALUATOROUTPUTPATHNAME << " value=\"" 
	<< xml::Utility::transformXmlEscChar(m_outputFile) << "\" />" << endl;
        //<< " <removeTempFiles value=\"" << (this->removeTemporaryFiles == 0? "false" : "true") << "\" />" << endl
        //<< " <evaluatorPathName value=\"" << xml::Utility::transformXmlEscChar(this->scriptFile) << "\" />" << endl
        //<< " <evaluatorInputPathName value=\"" << xml::Utility::transformXmlEscChar(this->inputFile) << "\" />" << endl
        //<< " <evaluatorOutputPathName value=\"" << xml::Utility::transformXmlEscChar(this->outputFile) << "\" />" << endl;

    writeInnerXml(output);

/* (!)20090825
    if(this->environmentVariables.empty() == false)
	{
		output << "<environment>" << endl;

		for(unsigned int i = 0; i < this->environmentVariables.size(); i++)
		{
			output
				<< "<variable "
				<< "name=\"" << xml::Utility::transformXmlEscChar(this->environmentVariables[i].first) << "\" "
				<< "value=\"" << xml::Utility::transformXmlEscChar(this->environmentVariables[i].second) << "\" />" << endl;
		}

		output << "</environment>" << endl;
	}
*/
	output << "</" << this->getXmlName() << ">" << endl;
}

