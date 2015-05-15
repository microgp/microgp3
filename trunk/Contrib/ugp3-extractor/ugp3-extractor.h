#ifndef HEADER_UGP3_EXTRACTOR
#define HEADER_UGP3_EXTRACTOR

#include <iostream>
#include <memory>
#include <sstream>
#include <exception>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ConsoleHandler.h"
#include "FileHandler.h"
#include "PlainFormatter.h"
#include "BriefFormatter.h"
#include "InfoFormatter.h"
#include "EvolutionaryCore.h"
#include "Convert.h"
#include "Program.h"

void setupLogging();
void displayHeader();
bool parseArguments(unsigned int argc, char* argv[]);
void saveDump();
void registerOperators();

#endif
