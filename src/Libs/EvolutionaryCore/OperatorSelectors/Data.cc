/***********************************************************************\
|                                                                       |
| Data.h                                                                |
|                                                                       |
| This file is part of MicroGP v3 (ugp3)                                |
| https://github.com/squillero/microgp3                                 |
|                                                                       |
| Copyright (c) 2006-2016 Giovanni Squillero                            |
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
 * @file Data.h
 * Definition and implementation of the Data template class.
 */

#include "Data.h"
#include "GeneticOperator.h"
#include "GroupOperator.h"
#include "Performance.h"
#include "make_unique.h"

/**
 * ugp3 namespace
 */
namespace ugp3
{
    
/**
 * ugp3::core namespace
 */
namespace core
{


Data::Data()
{ }

const std::string& Data::getXmlName() const
{
    return XML_NAME;
}

unsigned int Data::getPerformance(const Performance& level) const
{
    return this->performance[level.toValue()];
}

void Data::incrementPerformance(const Performance& level, unsigned int performance)
{
    this->performance[level.toValue()] += performance;
}

Operator* Data::getOperator() const
{
    return this->_operator;
}

void Data::cleanOperatorReference()
{
    this->_operator = nullptr;
}

void Data::clear()
{
    this->_operator = nullptr;
    this->enabled = true;
    this->operatorName = "";
    this->failedCalls = 0;
}

unsigned int Data::getFailedCalls() const
{
    return this->failedCalls;
}

const std::string& Data::getOperatorName() const
{
    return this->operatorName;
}

Data::Data(Operator& op)
: _operator(&op),
operatorName(op.getName())
{
    this->cleanPerformanceData();

    LOG_DEBUG << "Constructed a new Data (2) for " << this->operatorName << std::ends;

}

Data::Data(const std::string& operatorName)
: operatorName(operatorName)
{
    if(operatorName.empty())
    {
        throw ArgumentException("The parameter 'operatorName' cannot be an empty string", LOCATION);
    }

    this->cleanPerformanceData();


    LOG_DEBUG << "Constructed a new Data for " << this->operatorName << std::ends;
}

Data::~Data()
{
    deleteCallData();
}

void Data::setOperator(Operator* op)
{
    _STACK;

    if(op == nullptr)
    {
        throw ArgumentNullException("op", LOCATION);
    }

    if(this->_operator != nullptr)
    {
        throw Exception("An operator has been already associated to these statistics", LOCATION);
    }

    this->_operator = op;
}


void Data::cleanPerformanceData()
{
    LOG_VERBOSE << "CLEANING of Operator : " << this->operatorName <<  std::ends;

    performance.clear();
    performance.resize(Performance::PerformanceLevelCount, 0);
}

CallData* Data::newCallData()
{
    callData.push_back(make_unique<CallData>());
    return callData.back().get();
}

const std::vector<std::unique_ptr<CallData>>& Data::getCallData() const
{
    return this->callData;
}

void Data::deleteCallData()
{
    nCallsLastGen = 0;
    for (auto& cd: callData) {
        if (cd->getValidChildrenCount() > 0) {
            ++nCallsLastGen;
        }
    }
    callData.clear();
}

// Xml related implementation //////////

using namespace std;

const string Data::XML_NAME = "operator";
const string Data::XML_ATTRIBUTE_REF = "ref";
const string Data::XML_ATTRIBUTE_ENABLED = "enabled";
const string Data::XML_ATTRIBUTE_EXPLICIT_ENABLED = "explicitEnabled";
const string Data::XML_CHILD_ELEMENT_PERFORMANCE = "performance";
const string Data::XML_CHILD_ELEMENT_PARAMETERS = "operatorParameters";
const string Data::XML_CHILD_ELEMENT_FAILURES = "failures";
const string Data::XML_ATTRIBUTE_IS_PSEUDO_DEACTIVATED = "isPseudoDeactivated";
const string Data::XML_ATTRIBUTE_TOKENS = "tokens";
const string Data::XML_ATTRIBUTE_GENERATIONS_SINCE_LAST_TOKEN = "generationsSinceLastToken";


void Data::readXml(const xml::Element& element)
{
    _STACK;

    this->clear();
    this->cleanPerformanceData();

    const string operatorName = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_REF);

    this->operatorName = operatorName;
    LOG_DEBUG << "Loading statistics for operator \"" << operatorName << "\"" << ends;
    
    if (xml::Utility::hasAttribute(element, XML_ATTRIBUTE_ENABLED)) {
        enabled = xml::Utility::attributeValueToBool(element, XML_ATTRIBUTE_ENABLED);
    }
    
    if (xml::Utility::hasAttribute(element, XML_ATTRIBUTE_EXPLICIT_ENABLED)) {
        explicitEnabled = xml::Utility::attributeValueToBool(element, XML_ATTRIBUTE_EXPLICIT_ENABLED);
    } else {
        // First time reading this file: the enabling from before is explicit
        explicitEnabled = true;
        if (explicitEnabled && !enabled) {
            LOG_VERBOSE << "Disabling operator \"" << operatorName << "\" (blacklisted)" << ends;
        }
    }
    
    this->_operator = Operator::getOperator(operatorName);
    if (!this->_operator) {
        LOG_WARNING << "Disabling operator \"" << operatorName << "\" (not implemented)" << ends;
        this->enabled = false;
    }
    
    bool performanceFound = false;
    bool failureFound = false;
    bool parametersFound = false;

    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        string elementName = childElement->ValueStr();
        if(elementName == XML_CHILD_ELEMENT_PERFORMANCE)
        {
            if (performanceFound == true)
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/operators/operator/performance", LOCATION);

            performanceFound = true;

            this->performance[Performance::VeryBad.toValue()] = xml::Utility::attributeValueToUInt(*childElement, "veryBad");
            this->performance[Performance::Bad.toValue()] = xml::Utility::attributeValueToUInt(*childElement, "bad");
            this->performance[Performance::Normal.toValue()] = xml::Utility::attributeValueToUInt(*childElement, "normal");
            this->performance[Performance::Good.toValue()] = xml::Utility::attributeValueToUInt(*childElement, "good");
            this->performance[Performance::VeryGood.toValue()] = xml::Utility::attributeValueToUInt(*childElement, "veryGood");

            this->failedCalls = xml::Utility::attributeValueToUInt(*childElement, "failures");
        }
        else if(elementName == XML_CHILD_ELEMENT_FAILURES)
        {
            if (failureFound == true)
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/operators/operator/failures", LOCATION);

            failureFound = true;
            // looks like an ugly patch!?!?!?!
            if (this->_operator!=nullptr) // the ugp3-extractor don't instantiate the operators
            {
                this->_operator->readXml(*childElement);
            }
        }
        else if(elementName == XML_CHILD_ELEMENT_PARAMETERS)
        {
            if (parametersFound == true)
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations/population/operators/operator/operatorParameters", LOCATION);

            parametersFound = true;
            // looks like an ugly patch!?!?!?!
            if (this->_operator!=nullptr) // the ugp3-extractor don't instantiate the operators
            {
                this->_operator->readXml(*childElement);
            }
        }

        childElement = childElement->NextSiblingElement();
    }
}

void Data::writeXml(ostream& output) const
{

    LOG_DEBUG << "Serializing object ugp3::core::Data" << ends;

    output
        << "<" << XML_NAME
        << " " << XML_ATTRIBUTE_REF << "=\"" << xml::Utility::transformXmlEscChar(this->operatorName) << "\""
        << " " << XML_ATTRIBUTE_ENABLED << "=\"" << this->enabled << "\""
        << " " << XML_ATTRIBUTE_EXPLICIT_ENABLED << "=\"" << this->explicitEnabled << "\""
        << ">" << endl;


    // Writes the additional information of the operator
    if(this->_operator->hasParameters())
      {
      output<< "  <" << XML_CHILD_ELEMENT_PARAMETERS << ">" << endl;
      this->_operator->writeXml(output);
      output<< "  </" << XML_CHILD_ELEMENT_PARAMETERS << ">" << endl;
      }

    // print the performance if the operator was called at least once
    if(this->failedCalls > 0
        || this->getPerformance(Performance::VeryGood) > 0
        || this->getPerformance(Performance::Good) > 0
        || this->getPerformance(Performance::Normal) > 0
        || this->getPerformance(Performance::Bad) > 0
        || this->getPerformance(Performance::VeryBad) > 0)
    {
        output
            << "  <" << XML_CHILD_ELEMENT_PERFORMANCE << " "
            << "veryGood=\"" << this->getPerformance(Performance::VeryGood) << "\" "
            << "good=\"" <<     this->getPerformance(Performance::Good) << "\" "
            << "normal=\"" <<   this->getPerformance(Performance::Normal) << "\" "
            << "bad=\"" <<      this->getPerformance(Performance::Bad) << "\" "
            << "veryBad=\"" <<  this->getPerformance(Performance::VeryBad) << "\" "
            << "failures=\"" << this->failedCalls << "\"/>"
            << endl;
    }
    
    output
        << "  <" << XML_CHILD_ELEMENT_FAILURES << " "
        << XML_ATTRIBUTE_IS_PSEUDO_DEACTIVATED << "=\"" << isPseudoDeactivated << "\" "
        << XML_ATTRIBUTE_TOKENS << "=\"" << tokens << "\" "
        << XML_ATTRIBUTE_GENERATIONS_SINCE_LAST_TOKEN << "=\"" << generationsSinceLastToken << "\" "
        << "/>" << endl;
    
    writeInnerXml(output);

    output << "</" << XML_NAME << ">" << endl;

    LOG_DEBUG << "Serialized object ugp3::core::Data" << ends;
}

void Data::writeInnerXml(ostream& output) const
{
}

void Data::dumpStatistics(ostream& output) const
{
    output << "," << getNumberCallsLastGeneration();
}

void Data::dumpStatisticsHeader(const string& name, ostream& output) const
{
    output << "," << name << "_OpUse_" << getOperatorName();
}


}

}

