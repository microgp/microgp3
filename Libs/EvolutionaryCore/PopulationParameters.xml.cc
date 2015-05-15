/**
 * @file PopulationParameters.xml.cc
 * Implementation of the xml related methods of the PopulationParameters class.
 * @see PopulationParameters.h
 * @see PopulationParameters.cc
 */

#include "EvolutionaryCore.h"
using namespace ugp3::core;
using namespace std;

const string PopulationParameters::XML_NAME = "parameters";
const string PopulationParameters::XML_ATTRIBUTE_TYPE = "type";
const string PopulationParameters::XML_ATTRIBUTE_VALUE = "value";
const string PopulationParameters::XML_CHILDELEMENT_MU = "mu";
const string PopulationParameters::XML_CHILDELEMENT_LAMBDA = "lambda";
const string PopulationParameters::XML_CHILDELEMENT_DIFFEVODW = "diffevodw";
const string PopulationParameters::XML_CHILDELEMENT_DIFFEVOCP = "diffevocp";
const string PopulationParameters::XML_CHILDELEMENT_NU = "nu";
const string PopulationParameters::XML_CHILDELEMENT_INERTIA = "inertia";
const string PopulationParameters::XML_CHILDELEMENT_FITNESSPARAMETERS = "fitnessParameters";
const string PopulationParameters::XML_CHILDELEMENT_MAXIMUMAGE = "maximumAge";
const string PopulationParameters::XML_CHILDELEMENT_SIGMA = "sigma";
const string PopulationParameters::XML_CHILDELEMENT_INVALIDATEFITNESS = "invalidateFitnessAfterGeneration";
const string PopulationParameters::XML_CHILDELEMENT_MAXIMUMGENERATIONS = "maximumGenerations";
const string PopulationParameters::XML_CHILDELEMENT_MAXIMUMEVALUATIONS = "maximumEvaluations";
const string PopulationParameters::XML_CHILDELEMENT_ELITE = "eliteSize";
const string PopulationParameters::XML_CHILDELEMENT_STEADYSTATEGENERATIONS = "maximumSteadyStateGenerations";
const string PopulationParameters::XML_CHILDELEMENT_MAXFITNESS = "maximumFitness";

const string PopulationParameters::XML_CHILDELEMENT_MAXTIME = "maximumTime";
const string PopulationParameters::XML_ATTRIBUTE_HOURS = "hours";
const string PopulationParameters::XML_ATTRIBUTE_MINUTES = "minutes";
const string PopulationParameters::XML_ATTRIBUTE_SECONDS = "seconds";

const string PopulationParameters::XML_CHILDELEMENT_FITNESSSHARING = "fitnessSharing";
const string PopulationParameters::XML_ATTRIBUTE_DISTANCETYPE = "distance";
const string PopulationParameters::XML_ATTRIBUTE_RADIUS = "radius";
const string PopulationParameters::XML_CHILDELEMENT_CLONESCALINGFACTOR = "cloneScalingFactor";

const string PopulationParameters::XML_CHILDELEMENT_EVALUATOR = "evaluator";

unique_ptr<PopulationParameters> PopulationParameters::instantiate(const xml::Element& element) throw(exception)
{
    _STACK;

    if(element.ValueStr() != PopulationParameters::XML_NAME)
    {
        throw xml::SchemaException("expected element 'parameters' (found '" + element.ValueStr() + "').", LOCATION);
    }

    try
    {
        const string& type = xml::Utility::attributeValueToString(element, PopulationParameters::XML_ATTRIBUTE_TYPE);

        unique_ptr<PopulationParameters> parameters = PopulationParameters::instantiate(type);

        parameters->readXml(element);

        return parameters;
     }
     catch(const Exception& e)
     {
    LOG_DEBUG << "Yes, I got the exception \"" << e.what() << "\"." << ends;
    throw e;
     }
}


void PopulationParameters::writeInternalXml(std::ostream& output) const
{
	output
        << "<!-- the maximum size of the population -->" << endl
        << "<" << XML_CHILDELEMENT_MU
        << " " << XML_ATTRIBUTE_VALUE << "=\"" << this->mu << "\"/>"
        << endl
        << "<!-- the initial size of the population -->" << endl
        << "<" << XML_CHILDELEMENT_NU
        << " " << XML_ATTRIBUTE_VALUE <<"=\"" << this->nu << "\"/>"
        << endl
        << "<!-- the numbers of genetic operators applied at every step of the evolution -->" << endl
        << "<" << XML_CHILDELEMENT_LAMBDA
        << " " << XML_ATTRIBUTE_VALUE <<"=\"" << this->lambda << "\"/>"
        << endl
        << "<!-- the inertia for the self-adaptating parameters [0,1] -->" << endl
        << "<" << XML_CHILDELEMENT_INERTIA
        << " " << XML_ATTRIBUTE_VALUE <<"=\"" << this->inertia << "\"/>"
        << endl
        << "<!-- the number of dimensions of the fitness -->" << endl
        << "<" << XML_CHILDELEMENT_FITNESSPARAMETERS
        << " " << XML_ATTRIBUTE_VALUE <<"=\"" << this->fitnessParametersCardinality << "\"/>"
        << endl
        << "<!-- the maximum age of the individuals -->" << endl
        << "<" << XML_CHILDELEMENT_MAXIMUMAGE
        << " " << XML_ATTRIBUTE_VALUE <<"=\"" << this->maximumAge << "\"/>"
        << endl
        << "<!-- differential evolution crossover probability -->" << endl
        << "<" << XML_CHILDELEMENT_DIFFEVOCP
        << " " << XML_ATTRIBUTE_VALUE <<"=\"" << this->diffevocp << "\"/>"
        << endl
        << "<!-- differential evolution differential weight -->" << endl
        << "<" << XML_CHILDELEMENT_DIFFEVODW
        << " " << XML_ATTRIBUTE_VALUE <<"=\"" << this->diffevodw << "\"/>"
        << endl
        << "<!-- the strength of the mutation operators (0,1) -->" << endl
        << "<" << XML_CHILDELEMENT_SIGMA
        << " " << XML_ATTRIBUTE_VALUE <<"=\"" << this->sigma << "\"/>"
        << endl
        << "<!-- when set to true, the fitness of all the individuals of the population" << endl
        << "     is discarded at every step so that in the next step it is re-evaluated -->" << endl
        << "<" << XML_CHILDELEMENT_INVALIDATEFITNESS
        << " " << XML_ATTRIBUTE_VALUE <<"=\"" << (this->invalidateFitnessAfterGeneration == 0? false:true) << "\"/>"
        << endl
        << "<!-- the definition of the constraints of the problem -->" << endl
        << "<" << ugp3::constraints::Constraints::XML_NAME
        << " " << XML_ATTRIBUTE_VALUE << "=\""
        << (this->constraints != nullptr? this->constraints->getFilePath() : "") << "\"/>"
        << endl
        << "<!-- elite size: the eliteSize best individuals do not age at each generational step. -->" << endl
        << "<" << XML_CHILDELEMENT_ELITE
        << " " << XML_ATTRIBUTE_VALUE << "=\"" << this->eliteCardinality << "\"/>"
        << endl
        << "<!-- end of enhanced population specific parameters -->" << endl;
        
        if (this->maximumFitnessStop)
        {
            output
            << "<!-- (optional) the maximum value of the fitness -->" << endl
            << "<" << XML_CHILDELEMENT_MAXFITNESS
            << " " << XML_ATTRIBUTE_VALUE <<"=\"";
            for (unsigned int i = 0; i < this->maximumFitness.size(); i++)
            {
                output << this->maximumFitness[i] << (i < this->maximumFitness.size() - 1? " " : "");
            }
            output << "\"/>" << endl;
        }
        
        if (this->SteadyStateGenerationsStop)
        {
            output
            << "<!-- (optional) if the best fitness value does not change for maximumSteadyStateGenerations generations, the evolutions stops -->" << endl
            << "<" << XML_CHILDELEMENT_STEADYSTATEGENERATIONS
            << " " << XML_ATTRIBUTE_VALUE <<"=\"" << this->maximumSteadyStateGenerations << "\"/>"
            << endl;
        }
        
        if (this->maximumTimeStop)
        {
            output
            << "<!-- (optional) after a time expressed in hours:minutes:seconds, the evolution stops -->" << endl
            << "<" << XML_CHILDELEMENT_MAXTIME
            << " seconds=\"" << this->maximumTime.count() << "\" />"
            << endl;
        }
        
        if (this->fitnessSharingEnabled )
        {
            output
            << "<!-- (optional) use specified fitnessSharing distance during the evolution -->" << endl
            << "<" << XML_CHILDELEMENT_FITNESSSHARING << " " 
            << XML_ATTRIBUTE_DISTANCETYPE << "=\"" << this->fitnessSharingDistance << "\" "
            << XML_ATTRIBUTE_RADIUS << "=\"" << this->fitnessSharingRadius << "\" />" 
            << endl;
        }
        
        if(this->maximumGenerationsStop)
        {
            output
            << "<!-- (optional) the maximum number of generations -->" << endl
            << "<" << XML_CHILDELEMENT_MAXIMUMGENERATIONS
            << " " << XML_ATTRIBUTE_VALUE <<"=\"" << this->maximumGenerations << "\"/>"
            << endl;
        }
        
        if(this->maximumEvaluationsStop)
        {
            output
            << "<!-- (optional) the maximum number of individuals that can be evaluated -->" << endl
            << "<" << XML_CHILDELEMENT_MAXIMUMEVALUATIONS
            << " " << XML_ATTRIBUTE_VALUE <<"=\"" << this->maximumEvaluations << "\"/>"
            << endl;
        }
    output
      << "<!-- enhanced population specific parameters -->" << endl
      << "<!-- clone scaling factor: the fitness of each clone of a given individual is multiplied by this factor (0,1). If set to 0, clones are discarded. -->" << endl
      << "<" << XML_CHILDELEMENT_CLONESCALINGFACTOR
      << " " << XML_ATTRIBUTE_VALUE << "=\"" << this->cloneScalingFactor << "\"/>"
      << endl;
        
        output
        << "<!-- parents selector parameters -->" << endl;
        this->selector->writeXml(output);
        output
        << "<!-- evaluator parameters -->" << endl;
        getEvaluator().writeXml(output);
        output
        << "<!-- operator statistics -->" << endl;
        this->activations->writeXml(output);
}

void PopulationParameters::writeXml(std::ostream& output) const
{
    _STACK;
    
    output
    << "<" << XML_NAME
    << " " << XML_ATTRIBUTE_TYPE << "=\"" << this->getType() << "\""
    << ">" << endl;
    
    this->writeInternalXml(output);
    
    output
    << "</" << XML_NAME << ">" << endl;
}

void PopulationParameters::readXml(const xml::Element& element)
{
    _STACK;
    
    bool evaluatorFound = false;
    
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        const string& elementName = childElement->ValueStr();
        LOG_DEBUG << "elementName is \"" << elementName << "\"..." << ends;
        if(elementName == CandidateSelection::XML_NAME)
        {
            this->selector = CandidateSelection::instantiate(*childElement);
            this->selector->readXml(*childElement);
        }
        else if(elementName == XML_CHILDELEMENT_MU)
        {
            try
            {
                this->setMu(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch(const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if(elementName == XML_CHILDELEMENT_LAMBDA)
        {
            try
            {
                this->setLambda(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch(const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if(elementName == XML_CHILDELEMENT_NU)
        {
            try
            {
                this->setNu(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch(const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if(elementName == XML_CHILDELEMENT_INERTIA)
        {
            try
            {
                this->setInertia(xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch(const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if(elementName == XML_CHILDELEMENT_DIFFEVOCP)
        {
            try
            {
                this->setDiffEvoCP(xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch(const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if(elementName == XML_CHILDELEMENT_DIFFEVODW)
        {
            try
            {
                this->setDiffEvoDW(xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch(const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if(elementName == XML_CHILDELEMENT_MAXIMUMAGE)
        {
            try
            {
                this->setMaximumAge(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch(const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
            
        }
        else if(elementName == XML_CHILDELEMENT_FITNESSPARAMETERS)
        {
            try
            {
                this->setFitnessParametersCardinality(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch(const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if(elementName == XML_CHILDELEMENT_SIGMA)
        {
            try
            {
                this->setSigma(xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_VALUE));
                this->setOriginalSigma(this->getSigma());
            }
            catch(const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if(elementName == XML_CHILDELEMENT_MAXIMUMGENERATIONS)
        {
            try
            {
                this->setMaximumGenerations(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch(const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if(elementName == XML_CHILDELEMENT_MAXIMUMEVALUATIONS)
        {
            try
            {
                this->setMaximumEvaluations(xml::Utility::attributeValueToULong(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch(const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if(elementName == getEvaluator().getXmlName())
        {
            try
            {
                evaluatorFound = true;
                getEvaluator().readXml(*childElement);
            }
            catch(const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if(elementName == XML_CHILDELEMENT_INVALIDATEFITNESS)
        {
            try
            {
                this->setInvalidateFitnessAfterGeneration(xml::Utility::attributeValueToBool(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch(const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if(elementName == ugp3::constraints::Constraints::XML_NAME)
        {
            const string& constraintsFile =
            xml::Utility::attributeValueToString(*childElement, XML_ATTRIBUTE_VALUE);
            
            try
            {
                // TODO Load constraints relatively to the population file
                ugp3::constraints::Constraints* tempConstraints = ugp3::constraints::Constraints::fromFile(constraintsFile).release();
                this->setConstraints(*tempConstraints);
            }
            catch(const Exception& e)
            {
                LOG_ERROR << "Unable to load constraints '" << constraintsFile << "' specified in the population parameters file" << ends;
                throw;
            }
        }
        else if(elementName == "operatorsStatistics") // Old name
        {
            throw Exception("The element \"operatorsStatistics\" is now called "
            "\"operators\" and has been reworked to be more user-friendly. "
            "Please consult MicroGP's documentation and update your file.", LOCATION);
        }
        else if(elementName == Statistics::XML_NAME)
        {
            this->activations = unique_ptr<Statistics>(new Statistics(population));
            this->activations->readXml(*childElement);
        }
        
        else if(elementName == LocalSearchOperator::XML_NAME )
        {
            LOG_DEBUG << "Found details of \"" << LocalSearchOperator::XML_NAME << "\". Trying to read XML..." << ends;
            
            // check if local search is among registered operators
            GeneticOperator* localSearch = dynamic_cast<GeneticOperator*>(Operator::getOperator(LocalSearchOperator::XML_NAME));
            if( localSearch != nullptr )
            {
                // call localSearch->readXml(*childElement) on the element
                localSearch->readXml(*childElement);
            }
            
            // it's not perfect, but it might work...
        }
        else if (elementName == XML_CHILDELEMENT_ELITE)
        {
            try
            {
                this->setEliteCardinality(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch (const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if (elementName == XML_CHILDELEMENT_MAXFITNESS)
        {
            try
            {
                this->setMaximumFitness(Fitness::parse(xml::Utility::attributeValueToString(*childElement, XML_ATTRIBUTE_VALUE)));
            }
            catch (const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if (elementName == XML_CHILDELEMENT_STEADYSTATEGENERATIONS)
        {
            try
            {
                this->setMaximumSteadyStateGenerations(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch (const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        else if (elementName == XML_CHILDELEMENT_MAXTIME)
        {
            try
            {
                chrono::hours hours(0);
                chrono::minutes minutes(0);
                chrono::seconds seconds(0);
                if (xml::Utility::hasAttribute(*childElement, XML_ATTRIBUTE_HOURS))
                {
                    hours = chrono::hours(xml::Utility::attributeValueToUInt(*childElement, "hours"));
                }
                if (xml::Utility::hasAttribute(*childElement, XML_ATTRIBUTE_MINUTES))
                {
                    minutes = chrono::minutes(xml::Utility::attributeValueToUInt(*childElement, "minutes"));
                }
                if (xml::Utility::hasAttribute(*childElement, XML_ATTRIBUTE_SECONDS))
                {
                    seconds = chrono::seconds(xml::Utility::attributeValueToUInt(*childElement, "seconds"));
                }

                if (hours + minutes + seconds <= chrono::seconds(0))
        {
                    throw ArgumentException("no hours/minutes/seconds specified.", LOCATION);
        }

                this->setMaximumTime(hours + minutes + seconds);
                LOG_DEBUG << "maximumTime element found and set to " << this->getMaximumTime().count() << ends;
            }
            catch (const exception& e)
            {
                LOG_ERROR << "While parsing " << XML_CHILDELEMENT_MAXTIME << ": " << e.what() << ends;
                throw;
            }
        }
    else if (elementName == XML_CHILDELEMENT_FITNESSSHARING)
    {
            try
            {
        this->setFitnessSharingDistance( xml::Utility::attributeValueToString(*childElement, XML_ATTRIBUTE_DISTANCETYPE) );
        this->setFitnessSharingRadius( xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_RADIUS) );
        LOG_DEBUG   << "\"" << XML_CHILDELEMENT_FITNESSSHARING << "\" found. Fitness Sharing enabled of type \"" 
                << this->getFitnessSharingDistance() 
                << "\" with radius " << this->getFitnessSharingRadius() << ends;
            }
            catch (const exception& e)
            {
                LOG_ERROR << "While parsing " << XML_CHILDELEMENT_FITNESSSHARING << ": " << e.what() << ends;
                throw;
            }
    }
    else if (elementName == XML_CHILDELEMENT_CLONESCALINGFACTOR)
        {
            try
            {
                this->setCloneScalingFactor(xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_VALUE));
            }
            catch (const exception& e)
            {
                LOG_ERROR << "While parsing population data: " << e.what() << ends;
                throw;
            }
        }
        
        childElement = childElement->NextSiblingElement();
    }
    
    if(evaluatorFound == false)
    {
        throw xml::MissingElementSchemaException("/evolutionaryAlgorithm/populations/evaluation", LOCATION);
    }
}
