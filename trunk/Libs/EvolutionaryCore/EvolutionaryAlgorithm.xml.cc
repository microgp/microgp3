/***********************************************************************\
|                                                                       |
| EvolutionaryAlgorithm.xml.cc                                          |
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
| $Revision: 656 $
| $Date: 2015-04-15 14:16:06 +0200 (Wed, 15 Apr 2015) $
\***********************************************************************/

#include "ugp3_config.h"
#include "EvolutionaryCore.h"

// needed to obtain version information
#include "Info.h" 

#include <iomanip>

using namespace std;
using namespace ugp3;
using namespace ugp3::core;

const string EvolutionaryAlgorithm::XML_CHILD_ELEMENT_POPULATIONS = "populations";
const string EvolutionaryAlgorithm::XML_ATTRIBUTE_STEP = "step";
const string EvolutionaryAlgorithm::XML_ATTRIBUTE_ELAPSEDTIME_PREVIOUSRUNS = "elapsedTimePreviousRuns";
const string EvolutionaryAlgorithm::XML_ATTRIBUTE_ELAPSEDTIME_CURRENTRUN = "elapsedTimeCurrentRun";
const string EvolutionaryAlgorithm::XML_ATTRIBUTE_OUTPUTFILE = "outputPathName";
const string EvolutionaryAlgorithm::XML_ATTRIBUTE_OVERWRITEOUTPUT = "overwriteOutput";
const string EvolutionaryAlgorithm::XML_ATTRIBUTE_STATISTICSFILE = "statisticsPathName";
const string EvolutionaryAlgorithm::XML_ATTRIBUTE_DATETIME = "dateTime";
const string EvolutionaryAlgorithm::XML_STEP = "step";
const string EvolutionaryAlgorithm::XML_STEPS = "steps";
const string EvolutionaryAlgorithm::XML_ATTRIBUTE_RANDOMSTATE = "randomState";
const string EvolutionaryAlgorithm::XML_ATTRIBUTE_RANDOMTYPE = "randomType";
const string EvolutionaryAlgorithm::XML_NAME = "evolutionaryAlgorithm";
const string EvolutionaryAlgorithm::XML_NAMESPACE = "http://www.cad.polito.it/ugp3/schemas/evolutionary-algorithm";
const string EvolutionaryAlgorithm::XML_VERSION = "version";

void EvolutionaryAlgorithm::readXml(const xml::Element& element)
{
	_STACK;

    LOG_DEBUG << "Building evolutionary algorithm from xml element..." << ends;
    LOG_DEBUG << "Parsing /evolutionaryAlgorithm" << ends;

    // get the name of the element
    if(element.ValueStr() != this->getXmlName())
    {
        throw xml::SchemaException("expected element './" + this->getXmlName()+ "', found '" + element.ValueStr() + "'.", LOCATION);
    }

    // get the attributes
    this->algorithmStep = xml::Utility::attributeValueToUInt(element, XML_ATTRIBUTE_STEP);
    m_elapsedTimePreviousRuns = chrono::seconds(xml::Utility::attributeValueToUInt(element, XML_ATTRIBUTE_ELAPSEDTIME_PREVIOUSRUNS));
    if (xml::Utility::hasAttribute(element, XML_ATTRIBUTE_ELAPSEDTIME_CURRENTRUN)) {
        m_elapsedTimePreviousRuns += chrono::seconds(xml::Utility::attributeValueToUInt(element, XML_ATTRIBUTE_ELAPSEDTIME_CURRENTRUN));
    }
    this->outputPathName = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_OUTPUTFILE);
    this->statisticsPathName = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_STATISTICSFILE);
    this->overwriteOutput = xml::Utility::attributeValueToBool(element, XML_ATTRIBUTE_OVERWRITEOUTPUT);

    // important attribute: version of ugp3 recorded inside the status file (it's optional, so it could be absent)
    string statusVersion;
    try
    {
        statusVersion = xml::Utility::attributeValueToString(element, XML_VERSION);
        
        // compare current version of ugp3 with the version inside the 
        if( statusVersion.compare( ugp3::frontend::Info::getVersion() ) != 0 )
        {
            LOG_WARNING << 	"The status file was produced by ugp3 version \"" << statusVersion << "\". The current version of ugp3 is "
            "\"" << ugp3::frontend::Info::getVersion() << "\". Status files from a different version might not be fully "
            "compatible, and trying to read them could lead to a crash." << ends;
            
            string filename = "UpdateXML_" + statusVersion + "_to_" + ugp3::frontend::Info::getVersion() + ".xsl";
            
            LOG_WARNING << "If your MicroGP distribution contains an XSLT stylesheet named " << filename
            << ", you can apply this stylesheet to your status file to remove this warning." << ends;
            LOG_WARNING << "Example command for Unix-like systems: " << ends;
            LOG_WARNING << "$ xsltproc \"" << filename << "\" status.xml -o updated-status.xml" << ends;
        }
        else
        {
            LOG_DEBUG << "Status file produced by the current version of ugp3, \"" << statusVersion << "\"." << ends;
        }
    }
    catch( Exception& ex )
    {
        LOG_WARNING << 	"No information on ugp3 version was found inside the status file. Old status files might not be fully compatible "
        "with the current version of ugp3, and trying to read them could lead to a crash." << ends;
    }
    
    const string& randomState = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_RANDOMSTATE);
    string randomType;
    try {
        randomType = xml::Utility::attributeValueToString(element, XML_ATTRIBUTE_RANDOMTYPE);
    } 
    catch (Exception& ex) 
    {
        randomType = Random::RANDOM_TYPE_RAND48; // Old versions always used this
    }

    if (randomType == Random::getType()) 
    {
        Random::setStatus(randomState);
	LOG_INFO << "Recovering random generator state..." << ends;
        LOG_VERBOSE << "Random generator state: " << Random::getStatus() << ends;
    } 
    else 
    {
        LOG_WARNING << "The random generator used to generate this file (" 
        << (randomType.empty() ? "unspecified" : randomType)
        << ") does not match the implementation in this version of MicroGP ("
        << Random::getType() << "). For this run, we will seed our random generator"
        " with part of the saved seed, instead of a full recovery." << ends;

        hash_t seed = Hashable::djbHash(Hashable::startValue, randomState);
        LOG_INFO << "Generator seed from incompatible state: " << seed << ends;
        Random::seed(seed);
    }
    
    // get the inner elements
    bool populationsFound = false;
    bool evaluationFound = false;
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        string elementName = childElement->ValueStr();
        if(elementName == "evaluation")
        {
            if(evaluationFound == true)
            {
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/evaluation", LOCATION);
            }

            evaluationFound = true;
        }

        childElement = childElement->NextSiblingElement();
    }

    // the populations must be parsed AFTER the evaluator
    // (the population will try to access the evaluator field of its parent EvolutionaryAlgorithm instance)
    childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        string elementName = childElement->ValueStr();
        if(elementName == XML_CHILD_ELEMENT_POPULATIONS)
        {
            if(populationsFound == true)
            {
                throw xml::DuplicateElementSchemaException("/evolutionaryAlgorithm/populations", LOCATION);
            }

            populationsFound = true;

            this->parsePopulations(*childElement);
        }

        childElement = childElement->NextSiblingElement();
    }

    Individual::setIndividualCounter(xml::Utility::attributeValueToString(element, "infinityStringIndividual"));

    ctgraph::CNode::setInfinityString(xml::Utility::attributeValueToString(element, "infinityStringNode"));

    CandidateSolution::setAllopatricTagCounter(xml::Utility::attributeValueToString(element, "infinityStringIndividualAllopatricTag"));
    
    if (xml::Utility::hasAttribute(element, "infinityStringGroup")) 
    {
        Group::setGroupCounter(xml::Utility::attributeValueToString(element, "infinityStringGroup"));
    }

    if(populationsFound == false)
    {
        throw xml::MissingElementSchemaException("/evolutionaryAlgorithm/populations", LOCATION);
    }

}

void EvolutionaryAlgorithm::fromFile(const string& xmlFileName, bool discardFitness)
{
    _STACK;

    xml::Document algorithmFile;
    algorithmFile.LoadFile(xmlFileName);
    
	this->readXml(*algorithmFile.RootElement());

#ifndef NDEBUG
	this->save("debug." + xmlFileName);
#endif

    if (discardFitness)
    {
        LOG_INFO << "Invalidating individuals' fitness..." << ends;
        for(unsigned int p = 0; p < this->getPopulationCount(); p++)
        {
		Population& population = this->getPopulation(p);
		population.discardFitnessValues();

		MOPopulation *moPopulation = dynamic_cast<MOPopulation *>(&population);
		if(moPopulation != nullptr)
			moPopulation->setFirstRecovery(true);

		// now that the cache is stored inside the status file, however, to
		// force re-evaluation we have to delete all values in the cache;
		// or, more appropriately, only the values of individuals in the current
		// population. The latter option, however, is way more complicated to
		// implement...and we may assume that forcing re-evaluation means that
		// the fitness function somehow changed.
		Evaluator& evaluator = population.getParameters().getEvaluator();
		
		// now, the base class "Evaluator" has no visibility on its cache; we need to typecast it
		// to the correct inherited class, that actually includes cache information
		if( dynamic_cast<EvaluatorCommon<Individual>*>( &evaluator) != nullptr )
		{
			LOG_INFO << "Clearing cache for individual population #" << p << "..." << ends;
			EvaluatorCommon<Individual>* evaluatorCommon = dynamic_cast<EvaluatorCommon<Individual>*>( &evaluator );
			evaluatorCommon->clearCache();
		}
		else if( dynamic_cast<EvaluatorCommon<Group>*>( &evaluator) != nullptr )
		{
			LOG_INFO << "Clearing cache for group population #" << p << "..." << ends;
			EvaluatorCommon<Group>* evaluatorCommon = dynamic_cast<EvaluatorCommon<Group>*>( &evaluator );
			evaluatorCommon->clearCache();
		}
		
        }
	
	
    }
}

void EvolutionaryAlgorithm::parsePopulations(const xml::Element& element)
{
    _STACK;

    LOG_DEBUG << "Parsing /evolutionaryAlgorithm/populations" << ends;

    // get the name of the element
    if(element.ValueStr() != XML_CHILD_ELEMENT_POPULATIONS)
    {
        throw xml::SchemaException("expected element '/evolutionaryAlgorithm/populations'", LOCATION);
    }

    // get the inner elements
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        string elementName = childElement->ValueStr();
        if(elementName == Population::XML_NAME)
        {
            unique_ptr<Population> population = Population::instantiate(*childElement, *this);
            this->populations.push_back(population.release());
        }
        else
        {
            throw xml::SchemaException("unexpected element \"" + elementName + "\"", LOCATION);
        }

        childElement = childElement->NextSiblingElement();
    }

    if(this->populations.size() == 0)
    {
        throw xml::MissingElementSchemaException("/evolutionaryAlgorithm/populations", LOCATION);
    }
}

void EvolutionaryAlgorithm::writeXml(ostream& output) const
{
    _STACK;

    LOG_DEBUG << "Serializing object ugp3::core::EvolutionaryAlgorithm" << ends;

    // get the time
    ::time_t timeNow;
    ::time(&timeNow);
    ::tm* timeinfo = localtime(&timeNow);

    ostringstream timeString;
    timeString << setfill('0');
    timeString << timeinfo->tm_year + 1900 << "-"
        << setw(2) << timeinfo->tm_mon + 1 << "-"
        << setw(2) << timeinfo->tm_mday << "T"
        << setw(2) << timeinfo->tm_hour << ":"
        << setw(2) << timeinfo->tm_min << ":"
        << setw(2) << timeinfo->tm_sec;

    // Store doubles with enough digits to ensure lossless round-trip
    output << std::setprecision(std::numeric_limits<double>::digits10 + 2);

    output
        << "<?xml version='1.0' encoding='utf-8' ?>" << endl
        << "<" << this->getXmlName()
        << " " << XML_VERSION         << "='" << ugp3::frontend::Info::getVersion() << "'" << endl
        << " " << XML_ATTRIBUTE_STEP            << "='" << this->algorithmStep << "'" << endl
        << " " << XML_ATTRIBUTE_ELAPSEDTIME_CURRENTRUN << "='" << m_elapsedTimeThisRun.count() << "'" << endl
        << " " << XML_ATTRIBUTE_ELAPSEDTIME_PREVIOUSRUNS << "='" << m_elapsedTimePreviousRuns.count() << "'" << endl
        << " " << XML_ATTRIBUTE_DATETIME        << "='" << timeString.str() << "'" << endl
        << " " << XML_ATTRIBUTE_OUTPUTFILE      << "='" << this->outputPathName << "'" << endl
        << " " << XML_ATTRIBUTE_OVERWRITEOUTPUT << "='" << this->overwriteOutput << "'" << endl
        << " " << XML_ATTRIBUTE_STATISTICSFILE  << "='" << this->statisticsPathName << "'" << endl
        << " " << XML_ATTRIBUTE_RANDOMTYPE      << "='" << Random::getType() << "'" << endl
        << " " << XML_ATTRIBUTE_RANDOMSTATE     << "='" << Random::getStatus() << "'" << endl
        << " infinityStringIndividual" << "='" << Individual::getIndividualCounter() << "'" << endl
        << " infinityStringIndividualAllopatricTag" << "='" << CandidateSolution::allopatricTagCounter << "'" << endl
        << " infinityStringNode" << "='" << ctgraph::CNode::getInfinityString() << "'" << endl
        << " infinityStringGroup" << "='" << Group::getGroupCounter() << "'" << endl
        << " xmlns" << "='" << XML_NAMESPACE << "'" << endl
        << " xmlns:xsi" << "='" << "http://www.w3.org/2001/XMLSchema-instance'" << endl
        << " xsi:schemaLocation" << "='" << XML_NAMESPACE << " " << XML_NAMESPACE << ".xsd'"
        << ">" << endl;

    LOG_DEBUG << "Current random seed: " << Random::getStatus()
	      << "; session calls: " << Random::getTotalCalls() << ends;

    if(this->populations.size() > 0)
    {
        output << "<" << XML_CHILD_ELEMENT_POPULATIONS << ">" << endl;

        for(unsigned int i = 0; i < this->populations.size(); i++)
        {
            this->populations[i]->writeXml(output);
        }

        output << "</" << XML_CHILD_ELEMENT_POPULATIONS << ">" << endl;
    }

    output << "</" << this->getXmlName() << ">" << endl;
}


