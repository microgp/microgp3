#include "ugp3-extractor.h"
using namespace std;
using namespace ugp3;
using namespace ugp3::constraints;
using namespace ugp3::log;
using namespace ugp3::core;
using namespace ugp3::frontend;

string  inputFileName = "";
string  outputFileName = "";
string  individualID = "";
string  groupID = "";
bool    optionAll = false;
int     level = -1;

#define PROGRAM_NAME            "ugp3-extractor"
#define OPTION_FILEINPUT        "inputFile"
#define OPTION_FILEOUTPUT       "outputFilePattern"
#define OPTION_INDIVIDUALID     "individualID"
#define OPTION_GROUPID          "groupID"
#define OPTION_ALL              "all"
#define OPTION_LEVEL            "level"
#define OPTION_HELP             "help"
#define DEFAULT_LOGFILE         "ugp3-extractor.log"
#define DEFAULT_FITNESS_FILE    "ugp3-extractor.fitness"

Handler* consoleHandler = nullptr;
Handler* fileHandler = nullptr;
ofstream* fitnessFile = nullptr;

void registerOperators()
{
	LOG_VERBOSE << "Registering genetic operators" << ends;
	Operator::registration(new ScanMutationInnerLabelOperator());
	Operator::registration(new AllopatricDifferentialOperator());
	Operator::registration(new AlterationMutationOperator());
	Operator::registration(new InsertionMutationOperator());
	Operator::registration(new OnePointCrossoverOperator());
	Operator::registration(new OnePointImpreciseCrossoverOperator());
	Operator::registration(new RandomizerOperator());
	Operator::registration(new RemovalMutationOperator());
	Operator::registration(new ReplacementMutationOperator());
	Operator::registration(new ScanMutationBitArrayOperator());
	Operator::registration(new ScanMutationConstantOperator());
	Operator::registration(new ScanMutationFloatOperator());
	Operator::registration(new ScanMutationIntegerOperator());
	Operator::registration(new SimpleDifferentialOperator());
	Operator::registration(new SingleParameterAlterationMutationOperator());
	Operator::registration(new SubGraphInsertionMutationOperator());
	Operator::registration(new SubGraphRemovalMutationOperator());
	Operator::registration(new SubGraphReplacementMutationOperator());
	Operator::registration(new SwapMutationOperator());
	Operator::registration(new TwoPointCrossoverOperator());
	Operator::registration(new TwoPointImpreciseCrossoverOperator());
	LOG_DEBUG << "Registration complete." << endl << ends;

	LOG_VERBOSE << "Registering group operators" << ends;
	Operator::registration(new GroupRandomInsertionMutationOperator());
	Operator::registration(new GroupRandomRemovalMutationOperator());
	Operator::registration(new GroupExternalInsertionMutationOperator());
	LOG_DEBUG << "Registration complete." << endl << ends;

}

void showHelp(const string& programName)
{
    consoleHandler->pushFormatter(*new PlainFormatter());
    
    LOG_INFO << "Usage: " << programName << " [options] status.xml" << endl
             << "Options: " << endl
             << "  --" OPTION_FILEOUTPUT " <file_name>" << endl
             << "  --" OPTION_ALL << endl
             << "  --" OPTION_INDIVIDUALID " <id>" << endl
             << "  --" OPTION_GROUPID " <id>" << endl
             << "  --" OPTION_LEVEL " <level_number>" << ends;

    LOG_INFO << "Description: "
             << "    Extracts one or more individuals (or groups) from an algorithm XML file, retrieving their external representation."

             << endl << ends;      
      
    consoleHandler->popFormatter();
}

bool parseArguments(unsigned int argc, char* argv[])
{
    bool exitRequired = false;
    const string& programName = argv[0];
    
    if(argc <= 1)
    {
       showHelp(programName);
       exitRequired = true;
    }
        
    for(unsigned int i = 1; i < argc && exitRequired == false; i += 2)
    {
        const string& command = argv[i];

        // the last parameter is the name of the input file
        if(i == argc - 1)
        {
            inputFileName = command;
            continue;
        }

        if(command[0] != '-' || command[1] != '-')
        {
            throw Exception("Commandline syntax error: every command should start with '--'.", LOCATION);
        } 
        
        if(command == "--" OPTION_HELP)
        {
            showHelp(programName);
            exit(EXIT_SUCCESS);
        }
        else if(command == "--" OPTION_ALL)
        {
            optionAll = true;
            i--;
            continue;
        }
        
        // all the commands need an argument
        if(i + 1 >= argc)
        {
            throw Exception("Missing commandline argument for option '" + command + "'.", LOCATION);
        }
        
        const string& value = argv[i + 1];
        if(command == "--" OPTION_FILEOUTPUT)
        {
            outputFileName = value;
        }
        else if(command == "--" OPTION_INDIVIDUALID)
        {
            individualID = value;
        }
        else if(command == "--" OPTION_GROUPID)
        {
            groupID = value;
        }
        else if(command == "--" OPTION_LEVEL)
        {
            level = Convert::toInt(value);
        }
        else throw Exception("Unknown command '" + command + "'.", LOCATION);
    }
    
    return exitRequired;
}

const Fitness& getActualFitness(const Individual& individual)
{
    // Now getFitness() always returns the actual fitness
    return individual.getFitness();
}

void saveIndividual(const Individual& individual)
{
    ostringstream fileName;
    fileName << (outputFileName != ""? outputFileName : "individual") << individual.getId();

    LOG_INFO << "Saving \"" << fileName.str() << "\"..." << ends;
    individual.toCode(fileName.str());  

    const Fitness& fitness = getActualFitness(individual);
	LOG_INFO << fitness << ends;
    *fitnessFile << fileName.str() << ", fitness=" << fitness << endl; 
}

void saveBestIndividual(const EnhancedPopulation& population)
{
    const Individual* individual = population.getBestRawIndividual();
    if(individual != nullptr)
    {
       saveIndividual(*individual);
    }
    else LOG_WARNING << "No \"best individual\" defined" << ends;
}

void saveBestIndividual(const MOPopulation& population)
{
    const Individual* individual = population.getBestIndividual();
    if(individual != nullptr)
    {
       saveIndividual(*individual);
    }
    else LOG_WARNING << "No \"best individual\" defined" << ends;
}

void saveBestIndividual(const Population& population)
{
    if(dynamic_cast<const EnhancedPopulation*>(&population) != nullptr)
    {
        saveBestIndividual((const EnhancedPopulation&)population);
    }
    else if(dynamic_cast<const MOPopulation*>(&population) != nullptr)
    {
        saveBestIndividual((const MOPopulation&)population);
    }
    else
    {
        LOG_WARNING << "No \"best individual\" defined" << ends;
    }
}

void saveGroup(const Group& group)
{
    ostringstream fileName;
    fileName << (outputFileName != ""? outputFileName : "group") << group.getId();

    LOG_INFO << "Saving \"" << fileName.str() << "\"..." << ends;
    group.toCode(fileName.str());  

    const Fitness& fitness = group.getFitness();
	LOG_INFO << fitness << ends;
    *fitnessFile << fileName.str() << ", fitness=" << fitness << endl; 
}

const Group* searchGroup(const EvolutionaryAlgorithm& algorithm, const string& id)
{
    bool groupFound = false;
    for(unsigned int i = 0; i < algorithm.getPopulationCount() && groupFound == false; i++)
      {
        LOG_VERBOSE << "Population " << i << ends;
        const Population& population = algorithm.getPopulation(i);
        
        if(population.getType()==GroupPopulation::XML_SCHEMA_TYPE)
          {
          for(unsigned k = 0; k < ((const GroupPopulation&)population).getGroupCount(); k++)
            {
            const Group& group = ((const GroupPopulation&)population).getGroup(k);
            if(group.getId() == id)
              {
                return &group;
              }
            }
          }
      }
    
    return nullptr;
}

void saveGroup(const EvolutionaryAlgorithm& algorithm)
{
    Assert(groupID.empty() == false);
    
    const Group* group = searchGroup(algorithm, groupID);
    if(group != nullptr)
    {
       saveGroup(*group);
    } 
    else
    {
        LOG_INFO << "No group with id='" << groupID << "' was found" << ends;
    }
}

void saveBestGroup(const GroupPopulation& population)
{
    const Group* group = population.getBestRawGroup();
    if(group != nullptr)
    {
       saveGroup(*group);
    }
    else
        LOG_WARNING << "No \"best group\" defined" << ends;
}

void saveAll(const EvolutionaryAlgorithm& algorithm)
{
    for(unsigned int i = 0; i < algorithm.getPopulationCount(); i++)
    {
        LOG_VERBOSE << "Population " << i << ends;
        const Population& population = algorithm.getPopulation(i);
        
        if(population.getType()==GroupPopulation::XML_SCHEMA_TYPE)
        {
            for(unsigned p = 0; p < ((const GroupPopulation&)population).getGroupCount(); p++)
            {
                const Group& group = ((const GroupPopulation&)population).getGroup(p);
                saveGroup(group);
            }
        }
        else
        {
            if (const IndividualPopulation* indPop = dynamic_cast<const IndividualPopulation*>(&population)) {
                for(unsigned p = 0; p < indPop->getIndividualCount(); p++)
                {
                    const Individual& individual = indPop->getIndividual(p);
                    saveIndividual(individual);
                }
            }
        }
    }
}

void saveLevel(const EvolutionaryAlgorithm& algorithm, unsigned int level)
{
    for(unsigned int i = 0; i < algorithm.getPopulationCount(); i++)
    {
        LOG_VERBOSE << "Population " << i << ends;
        const Population& population = algorithm.getPopulation(i);
        
        if(const MOPopulation* moPop = dynamic_cast<const MOPopulation*>(&population))
        {
            LOG_VERBOSE << "Level " << level << ends;
            for(unsigned p = 0; p < moPop->getIndividualCount(); p++)
            {
                const MOIndividual& individual = moPop->getIndividual(p);
                
				if((unsigned int) individual.getLevel() == level)
                {
                    saveIndividual(individual);
                }
            }
        }
    }
}

void saveBest(const EvolutionaryAlgorithm& algorithm)
{
    for(unsigned int i = 0; i < algorithm.getPopulationCount(); i++)
    {
        LOG_VERBOSE << "Population " << i << ends;
        
        Population& population = algorithm.getPopulation(i);
        if(population.getType()==GroupPopulation::XML_SCHEMA_TYPE)
          saveBestGroup((GroupPopulation&)population);
        else
          saveBestIndividual(population);
    }
}

const Individual* searchIndividual(const EvolutionaryAlgorithm& algorithm, const string& id)
{
    bool individualFound = false;
    for(unsigned int i = 0; i < algorithm.getPopulationCount() && individualFound == false; i++)
    {
        LOG_VERBOSE << "Population " << i << ends;
        const Population& population = algorithm.getPopulation(i);
        
        if (const IndividualPopulation* indPop = dynamic_cast<const IndividualPopulation*>(&population)) {
            for(unsigned k = 0; k < indPop->getIndividualCount(); k++)
            {
                const Individual& individual = indPop->getIndividual(k);
                if(individual.getId() == id)
                {
                    return &individual;
                }
            }
        }
    }
    
    return nullptr;
}

void saveIndividual(const EvolutionaryAlgorithm& algorithm)
{
    Assert(individualID.empty() == false);
    
    const Individual* individual = searchIndividual(algorithm, individualID);
    if(individual != nullptr)
    {
       saveIndividual(*individual);
    } 
    else
    {
        LOG_INFO << "No individual with id='" << individualID << "' was found" << ends;
    }
}

void saveDump()
{
    if(outputFileName.empty() || inputFileName.empty())
    {
        new Exception("Commandline error: not all the parameters were specified.", LOCATION);
    }

    // register genetic operators before doing everything else
    registerOperators();

    LOG_INFO << "Loading algorithm ..." << ends;
    
    consoleHandler->setLevel(Level::Error);
    bool discardFitness = false;
    unique_ptr<EvolutionaryAlgorithm> algorithm(new EvolutionaryAlgorithm());
    algorithm->fromFile(inputFileName, discardFitness);
    consoleHandler->setLevel(Level::Info);
    
    LOG_INFO << "Algorithm loaded" << ends;
    
    fitnessFile = new ofstream(DEFAULT_FITNESS_FILE);
    
    if(optionAll == true)
    {
        LOG_INFO << "Extracting all the individuals (and groups)..." << ends;
        saveAll(*algorithm);
    }
    else if(level >= 0)
    {
        LOG_INFO << "Extracting the individuals on Pareto level " << level << ends;
        saveLevel(*algorithm, level);
    }
    else if(groupID != "")
    {
        LOG_INFO << "Extracting the group \"" << groupID << "\"..." << ends;
        saveGroup(*algorithm);
    }
    else if(individualID != "")
    {
        LOG_INFO << "Extracting the individual \"" << individualID << "\"..." << ends;
        saveIndividual(*algorithm);
    }
    else
    {
        LOG_INFO << "Extracting the best individuals (or groups) of every population..." << ends;
        saveBest(*algorithm);
    }

    fitnessFile->close();
    delete fitnessFile;
    
    LOG_INFO << "Extraction complete" << ends;

    LOG_INFO << "Fitness of extracted individuals (or groups) written to file \"" << DEFAULT_FITNESS_FILE << "\"" << endl << ends;
}

void setupLogging()
{
    consoleHandler = new ConsoleHandler();
    consoleHandler->setLevel(Level::Info);
    log_.addHandler(*consoleHandler);
 
#ifndef NDEBUG   
    fileHandler = new FileHandler(DEFAULT_LOGFILE);
    fileHandler->setLevel(Level::Debug);
    log_.addHandler(*fileHandler);
#endif
}

#ifndef HAVE_CONFIG_H
#define VERSION_REVISION 0
#define VERSION_MAJOR 3
#define VERSION_MINOR 0
#define COPYRIGHT_NOTE "GPL public license"
#endif

void displayHeader()
{
    consoleHandler->pushFormatter(*new PlainFormatter());      
    
    LOG_INFO << endl << PROGRAM_NAME << " 1.0." << VERSION_REVISION << " for MicroGP++ v" 
             << VERSION_MAJOR << "." << VERSION_MINOR << ends;
    LOG_INFO
      << "Written by Alessandro Salomone <alessandro.salomone@gmail.com>" << endl
      << COPYRIGHT_NOTE
      << ends;      
    
    consoleHandler->popFormatter();
}

