/**
 * @file PopulationParameters.cc
 * Implementation of the PopulationParameters class.
 * @see PopulationParameters.h
 * @see PopulationParameters.xml.cc
 */

#include "EvolutionaryCore.h"
using namespace ugp3::core;
using namespace std;

map<string, const ugp3::constraints::Constraints*> PopulationParameters::constraintsDefinitions;

PopulationParameters::PopulationParameters(Population* population)
: population(population),
constraints(nullptr),
mu(0),
lambda(0),
nu(0),
diffevodw(0.0),
diffevocp(0.0),
inertia(0.0),
sigma(0.0),
maximumAge(0),
maximumGenerations(0),
maximumGenerationsStop(false),
fitnessParametersCardinality(0),
activations(new Statistics(population)),
selector(new TournamentSelection()),
dumpBeforeEvaluation(false),
invalidateFitnessAfterGeneration(false),
maximumEvaluations(0),
maximumEvaluationsStop(false),
eliteCardinality(0),
maximumFitnessStop(false),
SteadyStateGenerationsStop(false),
maximumSteadyStateGenerations(0),
maximumTimeStop(false),
fitnessSharingEnabled(false),
fitnessSharingRadius(0),
cloneScalingFactor(0.0)
{
    
}

PopulationParameters::~PopulationParameters()
{

}

void PopulationParameters::clearConstraintsCache()
{
    map<string, const ugp3::constraints::Constraints*>::iterator iterator = constraintsDefinitions.begin();
    for (; iterator != constraintsDefinitions.end(); ++iterator) {
        delete (*iterator).second;
    }
}

void PopulationParameters::save(const string& fileName) const
{
    _STACK;

    ofstream output;
    output.open(fileName.c_str());
    if (output.is_open() == false)
    {
        throw Exception("Cannot access file \"" + fileName + "\"", LOCATION);
    }

    output << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl;
    this->writeXml(output);
    output.close();
}

unique_ptr<PopulationParameters> PopulationParameters::instantiate(const string& type) throw(exception)
{
    _STACK;

    PopulationParameters* parameters = NULL;

    if (type == EnhancedPopulationParameters::XML_SCHEMA_TYPE) {
        parameters = new EnhancedPopulationParameters(nullptr);
    } else if (type == MOPopulationParameters::XML_SCHEMA_TYPE) {
        parameters = new MOPopulationParameters(nullptr);
    } else if (type == GroupPopulationParameters::XML_SCHEMA_TYPE) {
        parameters = new GroupPopulationParameters(nullptr);
    }
    // ADD HERE: a new 'if' block for each new population

    if (parameters == NULL) {
        throw Exception("The parameters type '" + type + "' is unknown.", LOCATION);
    }

    return unique_ptr<PopulationParameters> (parameters);
}

std::unique_ptr<PopulationParameters> PopulationParameters::fromFile(const std::string& fileName) throw(exception)
{
    _STACK;

    xml::Document parametersFile;
    
    // possible exceptions here, if file does not exist or problems during PopulationParameters::instantiate()
    try
    {
        parametersFile.LoadFile(fileName);
        std::unique_ptr<PopulationParameters> populationParameters = PopulationParameters::instantiate(*parametersFile.RootElement());
        return populationParameters;
    }
    catch(const Exception& e)
    {
        //LOG_DEBUG << "I am PopulationParameters::fromFile and I got this exception: \"" << e.what() << "\"." << ends;
        throw e;
    } 
    
}
