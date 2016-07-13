/***********************************************************************\
|                                                                       |
| Program.cc                                                            |
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
| $Revision: 657 $
| $Date: 2015-04-23 10:32:25 +0200 (Thu, 23 Apr 2015) $
\***********************************************************************/

/**
 * @file Program.cc
 * Implementation of the Program class.
 * @see Program.h
 * @see Program.Run.cc
 */

#include "ugp3_config.h"

#include "Program.h"
using namespace ugp3::frontend;
using namespace std;

#include "ConsoleHandler.h"
#include "FileHandler.h"
#include "BriefFormatter.h"
#include "ExtendedFormatter.h"
#include "PlainFormatter.h"
using namespace ugp3::log;

#include "EnhancedPopulation.h"
#include "MOPopulation.h"
#include "Operators/AlterationMutation.h"
#include "Operators/InsertionMutation.h"
#include "Operators/OnePointCrossover.h"
#include "Operators/OnePointImpreciseCrossover.h"
#include "Operators/RemovalMutation.h"
#include "Operators/ReplacementMutation.h"
#include "Operators/ScanMutationBitArray.h"
#include "Operators/ScanMutationConstant.h"
#include "Operators/ScanMutationFloat.h"
#include "Operators/ScanMutationInnerLabel.h"
#include "Operators/ScanMutationInteger.h"
#include "Operators/SingleParameterAlterationMutation.h"
#include "Operators/SubGraphInsertionMutation.h"
#include "Operators/SubGraphRemovalMutation.h"
#include "Operators/SubGraphReplacementMutation.h"
#include "Operators/TwoPointCrossover.h"
#include "Operators/TwoPointImpreciseCrossover.h"
#include "Operators/UniformCrossover.h"
using namespace ugp3::core;

#ifdef UGP3_USE_LUA
#include <lua.hpp>
#endif

// initialization of the static members of the Program class
MicroGPSettings Program::settings;
const string Program::defaultSettingsFile = "./ugp3.settings.xml";
const string Program::defaultLoggingFile = "./ugp3.logging.xml";

ConsoleHandler* consoleHandler = nullptr;


Program::Program()
{}

void Program::setupLogger()
{
    _STACK;

    // checking data stored in settings
    Assert( Program::settings.hasContext("logging") != false );

    // logging context found: analyzing options
    for (unsigned int i = 0; i < Program::settings.getContext("logging").getOptionCount(); i++)
    {
        // options parsing
        std::string buffer, level, format;
        size_t pos;

        buffer = Program::settings.getContext("logging").getOption(i).getValue();
        pos = buffer.find(";");

        level = buffer.substr(0, pos);
        format = buffer.substr(pos+2);

        // for each option, create a handler
        if ( Program::settings.getContext("logging").getOption(i).getName() == "std::cout" ) // add other names for console
        {
            // console handler should already exist
            consoleHandler->setFormatter(*Formatter::instantiate(format).release());
            // level must be set AFTER formatter
            consoleHandler->setLevel( Level::parse(level) );
        } else
        {
            Program::addLogHandler(
                                  Program::settings.getContext("logging").getOption(i).getName(),
                                  level,
                                  format);

        }

    }
}


int Program::start(unsigned int argumentCount, char** arguments)
{
    _STACK;
    bool deprecatedRecovery = false;

    // If we are in debug mode, we let exceptions crash the program: it is easier to debug with GDB.
#ifdef NDEBUG
    try
    {
#endif
        // initialize console
        consoleHandler = new ConsoleHandler();
        ::log_.addHandler(*consoleHandler); //::log_ is a global instance of a Log class
        //consoleHandler->enableSmartWrap(true);
        consoleHandler->setLevel(Level::Info);

        // initialize settings
        Program::settings.initialize();

        if (File::exists(Program::defaultSettingsFile) == true)
        {
            Program::settings.load(Program::defaultSettingsFile);
            if (settings.getOption("recovery", Argument::RecoveryInputAlgorithm).getValue().empty() == false)
            {
                deprecatedRecovery = true;
            }
        }

        // parse the command line arguments
        bool exitProgramRequired = Program::parseArguments(argumentCount, arguments);

        // create the log handlers specified in settings file
	// this function has been moved after parseArguments, because a settings file could be specified on the command line
        Program::setupLogger();

        if (exitProgramRequired == true)
            return EXIT_SUCCESS;

        // display a startup message (program name, copyright, etc...)
        Program::displayStartupMessage();

#ifndef NDEBUG
        LOG_WARNING << "All internal coherency asserts enabled: performance will be significantly impaired." << std::ends;
#endif

        // set some environmental variables
        SETENV("UGP3_VERSION", Info::getVersion());
        SETENV("UGP3_TAGLINE", Info::getTagline());

	// at this point, we either have the default settings file, or a settings file specified by command line
	// inside the context "program", option "settingsFile"
	if( File::exists( Program::settings.getOption("program", Argument::SettingsFile).getValue() ) == false )
	{
		LOG_ERROR 	<< "Settings file \"" 
				<< Program::settings.getOption("program", Argument::SettingsFile).getValue()
				<< "\" not found in current folder." << ends;
		throw Exception(Argument::SettingsFile, LOCATION); 
	}

        // Some Checks

        if (deprecatedRecovery)
        {
            LOG_WARNING << "Setting's parameter \"" << Argument::RecoveryInputAlgorithm << "\" is deprecated. Use the command-line option instead." << ends;
        }

        //LOG_DEBUG << "Value of " << Program::settings.getOption("evolution", "population").getName()
        //          << " is " << Program::settings.getOption("evolution", "population").getValue() << ends;

        // enter the execution stage
        Program::run();

        ::log_.clear();
        Program::settings.clear();
#ifdef NDEBUG
    } catch (const Exception& e)
    // catch ugp3 exceptions
    {
        displayStartupMessage();
        // show the error and exit with failure
        LOG_ERROR
        << e.what() << endl << e.getStackTrace()
        << ends;
        return EXIT_FAILURE;
    } catch (const exception& e)
    // catch C++ exceptions
    {
        displayStartupMessage();
        // show the error and exit with failure
        LOG_ERROR
        << "Caught standard exception: "
        << e.what();

// Stack trace is not SO meaningful here...
//#ifdef USE_STACK_TRACE
//        LOG_ERROR << endl << ugp3::StackTrace::getTrace();
//#endif

        LOG_ERROR << ends;

#ifndef NDEBUG
#if defined(_MSC_VER)
        LOG_ERROR << "Invoking debug..." << endl << ends;
        _CrtDbgBreak();
#else
        abort();
#endif
#endif
        return EXIT_FAILURE;
    }
#endif

    return EXIT_SUCCESS;
}

bool Program::parseArguments(
                            unsigned int argumentCount,
                            char** arguments)
{
    _STACK;

    bool exitProgramRequired = false;

    if (argumentCount == 0 || arguments == nullptr) return exitProgramRequired;

    for(unsigned int index = 0; index < argumentCount && exitProgramRequired == false; index++)
    {
        if (index == 0)
        {
            // the first element is the name of the program: we add a special context with the name
            // used for ugp3, and the name of the settings file (initially set to the default value)
            settings.addContext(
                unique_ptr<SettingsContext>(new SettingsContext("program"))
            );
            
            settings.getContext("program").addOption(unique_ptr<Option>(
                new Option("pathName", arguments[index], "string"))
            );
            
            settings.getContext("program").addOption(unique_ptr<Option>(
                new Option(Argument::SettingsFile, Program::defaultSettingsFile, "string"))
            );
            
            continue;
        }
        
        
        if (arguments[index][0] != '-' || arguments[index][1] != '-')
        {
            throw invalid_argument(
                "Invalid command syntax: "
                "every command should start with \"--\".");
        } 
        else
        {
            // get the string without the leading "--"
            string argument = &arguments[index][2];
            
            // a file containing the settings has been specified
            // the settings will be loaded from the file
            if( argument.compare(Argument::SettingsFile) == 0 )
            {
                if(index + 1 < argumentCount)
                {
                    // load settings from the specified file
                    Program::settings.load(arguments[index+1]);
                    
                    // also, alterate the special option to preserve the name of the new settings file
                    // inside the "program" context
                    settings.getOption("program", Argument::SettingsFile).setValue(arguments[index+1]);
                    
                    // increase index
                    index++;
                } 
                else
                {
                    throw Exception(Argument::SettingsFile, LOCATION);
                }
            } 
            else if (argument == Argument::License)
            {
                Program::displayLicense();
                exitProgramRequired = true;
            } 
            else if (argument == Argument::Operators)
            {
                Program::displayOperators();
                exitProgramRequired = true;
            }
            else if (argument == Argument::Help)
            {
                Program::displayStartupMessage();
                
                if (index + 1 < argumentCount)
                {
                    index++;
                    const string& helpArgument = arguments[index];
                    if (helpArgument[0] != ' ' && helpArgument[1] != ' ')
                    {
                        const string& optionName = helpArgument;
                        
                        SettingsContext* context = nullptr;
                        if (settings.getContext("recovery").hasOption(optionName))
                        {
                            context = &settings.getContext("recovery");
                        } else if (settings.getContext("evolution").hasOption(optionName))
                        {
                            context = &settings.getContext("evolution");
                        }
                        /** Commented by Fran Navas. 11/07/11. This context is not defined in the MicroGPSettings::initialize method, what made an execution error.
                         * else if(settings.getContext("evaluation").hasOption(optionName))
                         * {
                         *    context = &settings.getContext("evaluation");
                    }
                    */
                        if (context != nullptr)
                        {
                            const string& optionDescription =
                            context->getOption(optionName).getDescription();
                            
                            LOG_INFO << "Option '" << optionName << "': " <<
                            optionDescription << ends;
                        } else throw Exception("Option '" + optionName + "' not found", LOCATION);
                    } else
                        Program::displayArgumentsUsage();
                    
                } else
                    Program::displayArgumentsUsage();
                
                exitProgramRequired = true;
            } 
            else if (argument == Argument::Version)
            {
                Program::displayStartupMessage();
                Program::displayVersion();
                exitProgramRequired = true;
            }
            else if (argument == Argument::Debug)
            {
                consoleHandler->setLevel(Level::Debug);
            }
            else if (argument == Argument::Verbose)
            {
                consoleHandler->setLevel(Level::Verbose);
            } 
            else if (argument == Argument::Info)
            {
                consoleHandler->setLevel(Level::Info);
            } 
            else if (argument == Argument::Warning)
            {
                consoleHandler->setLevel(Level::Warning);
            } 
            else if (argument == Argument::Error)
            {
                consoleHandler->setLevel(Level::Error);
            } 
            else if (argument == Argument::Silent)
            {
                consoleHandler->setLevel(Level::Silent);
            } 
            else if (argument == Argument::Log)
            {
                if (index + 3 < argumentCount)
                    // the '--log' command must be followed by 3 parameters
                {
                    Program::addLogHandler(
                        arguments[index + 1],
                        arguments[index + 2],
                        arguments[index + 3]);
                    
                    index += 3;
                } else throw Exception(Argument::Log, LOCATION);
            } 
            else if (argument == Argument::FitnessRemoveTempFiles)
            {
                if (index + 1 < argumentCount)
                {
                    settings.getContext("evaluation").getOption(Argument::FitnessRemoveTempFiles).setValue(arguments[index+1]);
                    index++;
                } else throw Exception(
                    Argument::FitnessRemoveTempFiles, LOCATION);
            } 
            else if (argument == Argument::further)
            {
                if (index + 1 < argumentCount)
                {
                    settings.getOption("evolution", Argument::further)
                    .setValue(arguments[index + 1]);
                    
                    index++;
                } else throw Exception(Argument::further, LOCATION);
            }
            else if (argument == Argument::AllOperatorsEnabled)
            {
                if (settings.getOption("evolution", Argument::AllOperatorsDisabled).toBool()) {
                    throw Exception("Can't have both --" + Argument::AllOperatorsDisabled
                        + " and --" + Argument::AllOperatorsEnabled, LOCATION);
                }
                settings.getOption("evolution", Argument::AllOperatorsEnabled).setValue("true");
            }
            else if (argument == Argument::AllOperatorsDisabled)
            {
                if (settings.getOption("evolution", Argument::AllOperatorsEnabled).toBool()) {
                    throw Exception("Can't have both --" + Argument::AllOperatorsDisabled
                        + " and --" + Argument::AllOperatorsEnabled, LOCATION);
                }
                settings.getOption("evolution", Argument::AllOperatorsDisabled).setValue("true");
            }
            else if (argument == Argument::RandomSeed)
            {
                if (index + 1 < argumentCount)
                {
                    settings.getOption("evolution", Argument::RandomSeed)
                    .setValue(arguments[index + 1]);
                    
                    index++;
                } else throw Exception(Argument::RandomSeed, LOCATION);
            }
            else if (argument == Argument::StatisticsPathName)
            {
                if (index + 1 < argumentCount)
                {
                    settings.getOption("evolution", Argument::StatisticsPathName)
                    .setValue(arguments[index + 1]);
                    
                    index++;
                } else throw Exception(Argument::StatisticsPathName, LOCATION);
            } 
            else if (argument == Argument::FitnessConcurrentEvaluations)
            {
                if (index + 1 < argumentCount)
                {
                    settings.getOption(
                        "evaluation",
                        Argument::FitnessConcurrentEvaluations)
                    .setValue(arguments[index + 1]);
                    
                    index++;
                } else throw Exception(Argument::FitnessConcurrentEvaluations, LOCATION);
            } 
            else if (argument == Argument::FitnessEvaluator)
            {
                if (index + 1 < argumentCount)
                {
                    settings.getContext("evaluation")
                    .getOption(Argument::FitnessEvaluator)
                    .setValue(arguments[index + 1]);
                    
                    index++;
                } else throw Exception(Argument::FitnessEvaluator, LOCATION);
            } 
            else if (argument == Argument::FitnessEvaluatorInput)
            {
                if (index + 1 < argumentCount)
                {
                    settings.getContext("evaluation")
                    .getOption(Argument::FitnessEvaluatorInput)
                    .setValue(arguments[index + 1]);
                    
                    index++;
                } else throw Exception(Argument::FitnessEvaluatorInput, LOCATION);
            } 
            else if (argument == Argument::FitnessEvaluatorOutput)
            {
                if (index + 1 < argumentCount)
                {
                    settings.getContext("evaluation").getOption(Argument::FitnessEvaluatorOutput).setValue(arguments[index + 1]);
                    
                    index++;
                } else throw Exception(Argument::FitnessEvaluatorOutput, LOCATION);
            } 
            else if (argument == Argument::RecoveryOutputAlgorithm)
            {
                if (index + 1 < argumentCount)
                {
                    settings.getOption(
                        "recovery",
                        Argument::RecoveryOutputAlgorithm)
                    .setValue(arguments[index + 1]);
                    
                    index++;
                } else throw Exception(Argument::RecoveryOutputAlgorithm, LOCATION);
            } 
            else if (argument == Argument::Merge)
            {
                if (index + 2 < argumentCount)
                {
                    // append the option, do not overwrite it
                    Option& option = settings.getOption("evolution", Argument::Merge);
                    
                    option.setValue(option.getValue() + ";" + arguments[index + 1] + " " + arguments[index + 2]);
                    
                    index += 2;
                } else throw Exception(Argument::Merge, LOCATION);
            } 
            else if (argument == Argument::Population)
            {
                if (index + 1 < argumentCount)
                {
                    // append the option, do not overwrite it
                    Option& option = settings.getOption("evolution", Argument::Population);
                    
                    option.setValue(option.getValue() + ";" + arguments[index + 1]);
                    
                    index++;
                } else throw Exception(Argument::Population, LOCATION);
            } 
            else if (argument == Argument::RecoveryInputAlgorithm)
            {
                if (index + 1 < argumentCount)
                {
                    settings.getOption("recovery", Argument::RecoveryInputAlgorithm).setValue(arguments[index + 1]);
                    
                    index++;
                } else throw Exception(Argument::RecoveryInputAlgorithm, LOCATION);
            } 
            else if (argument == Argument::RecoveryDiscardFitness)
            {
                if (index + 1 < argumentCount)
                {
                    settings.getOption(
                        "recovery",
                        Argument::RecoveryDiscardFitness)
                    .setValue(arguments[index + 1]);
                    
                    index++;
                } else throw Exception(Argument::RecoveryDiscardFitness, LOCATION);
            } 
            else
            {
                string message = "Unrecognized command \"" + argument + "\"";
                
                throw runtime_error(message.c_str());
            }
        }
    }
    
    return exitProgramRequired;
}

void Program::addLogHandler(const string& fileName, const string& level, const string& format)
{
    _STACK;
    
    Handler* handler = new FileHandler(fileName);
    handler->setLevel(Level::parse(level));
    handler->setFormatter(*Formatter::instantiate(format).release());
    
    ::log_.addHandler(*handler);
}

void Program::displayVersion()
{
    _STACK;
    
    consoleHandler->pushFormatter(*new PlainFormatter());      
    consoleHandler->setLevel(Level::Info);
    
    Program::registerOperators();
    
    ::log_
    << "Compiled on " << Info::getCompileInfo()
#ifdef UGP3_USE_LUA
    #ifdef LUAJIT_VERSION
    << ", with Lua support (" << LUA_VERSION << " with " << LUAJIT_VERSION << ")." << endl
    #else
    << ", with Lua support (" << LUA_VERSION << " without JIT)." << endl
    #endif
#else
    << ", without Lua support." << endl
#endif
    << std::endl
    << Info::getSystemType() << std::ends;
    
    ::log_ << std::endl << "Available operators:";
    for (unsigned int t=0; t<Operator::getOperatorCount(); ++t)
    {
        ::log_ << " " << Operator::getOperator(t)->getName();
    }
    ::log_ << std::ends;
    
#ifndef NDEBUG
    ::log_ << std::endl << "WARNING: All internal coherency asserts enabled: performance may be significantly impaired (define macro \"NDEBUG\" to disable them)." << std::endl << std::ends;
#endif
    
    
    consoleHandler->popFormatter();
}

void Program::displayStartupMessage()
{
    _STACK;
    
    static bool alreadyShown = false;
    
    if (alreadyShown)
        return;
    
    alreadyShown = true;
    
    consoleHandler->pushFormatter(*new PlainFormatter());      
    
    ::log_ << ugp3::log::Level::Error << Info::getTagline() << endl
    << Info::description << endl
    << COPYRIGHT_NOTE << endl
    << "This is free software, and you are welcome to redistribute it under certain conditions (use option \"--" << Argument::License << "\" for details)"
    << endl << ends;
    
    // ::log_ << "For any problem or suggestion, please mail to <" << PACKAGE_BUGREPORT << ">" << ends;
    
    consoleHandler->popFormatter();
}

void Program::displayLicense()
{
    _STACK;
    
    // displayStartupMessage();
    
    consoleHandler->pushFormatter(*new PlainFormatter());      
    
    ::log_
    << Info::getTagline() << endl
    << Info::description << endl
    << COPYRIGHT_NOTE << endl
    << endl
    
    << "This program is free software: you can redistribute it and/or modify "
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation, either version 3 of the License, or "
    "(at your option) any later version. " << endl << endl
    
    << "This program is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
    "GNU General Public License for more details. " << endl << endl
    
    << "You should have received a copy of the GNU General Public License "
    "along with this program.  If not, see <http://www.gnu.org/licenses/>. "
    << ends;
    
    consoleHandler->popFormatter();
}

void Program::printOption(const ugp3::Option& defaultOption)
{
    ostringstream stream1;
    stream1 << "    --" << defaultOption.getName() << " <" << defaultOption.getType() << ">" ;
    
    ostringstream stream2;
    stream2 << " [default: '" << defaultOption.getValue() << "']" << endl;
    
    
    ::log_  << stream1.str();
    
    int spaces = consoleHandler->getTerminalWidth() - stream2.str().length() - stream1.str().length();
    for (int i = 0; i < spaces; i++)
    {
        ::log_ << " ";
    }
    
    ::log_ << stream2.str();
    
    
}

void Program::displayArgumentsUsage()
{
    _STACK;
    
    MicroGPSettings defaults;
    defaults.initialize();
    
    consoleHandler->pushFormatter(*new PlainFormatter());      
    
    ::log_
    << "Usage: " << settings.getContext("program").getOption("pathName") << " [ options ]" << endl
    << endl
    << "General information:" << endl
    << "    --license, --version, --help" << endl
    << endl
    << "Control program verbosty (* = default):" << endl
    << "    --debug, --verbose, --info*, --warning, --error, --silent" << endl
    << endl
    << "Exploit results from previous runs:" << endl
    << "    --recoveryInput <status.xml>" << endl
    << "    --population <population.xml>" << endl
    << "    --merge <destination> <source>" << endl
    << endl
    << "Common tweaks:" << endl
    << "    --further <num>" << endl
    << "    --" << Argument::AllOperatorsEnabled << " | --" << Argument::AllOperatorsDisabled << endl
    << "    --log <file> <level> <format>" << endl
    << "    --statisticsPathName <file.csv>" << endl
    << "    --randomSeed <num> [0 = seconds from The Epoch]" << endl
    << ends;
    
    ::log_ 
    << "NOTEZ BIEN: The behavior of MicroGP is controlled by configuration files, not command line options. "
    << "Options (not listed here) can be used to override specific parameters in the current run. "
    << "Go to <http://ugp3.sf.net/> to find out more." << endl << ends;
    
    consoleHandler->popFormatter();
}

void Program::displayOperators()
{
    consoleHandler->pushFormatter(*new PlainFormatter());      
    ::log_
    << Info::getTagline() << endl
    << Info::description << endl
    << COPYRIGHT_NOTE << endl
    << endl
    
    << "This is the list of available operators for the current version:" 
    << endl << ends;
    
    Program::registerOperators();
    
    for(unsigned int i = 0; i < Operator::getOperatorCount(); i++)
    {
        Operator* op = Operator::getOperator(i);
        ::log_ << op->getDescription() << endl << ends;
    }
    
    consoleHandler->popFormatter();
    
    Operator::unregisterAll();
}
