/***********************************************************************\
|                                                                       |
| ConsoleHandler.cc                                                     |
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
 * @file ConsoleHandler.cc
 * Implementation of the FileHandler class.
 * @see ConsoleHandler.h
 */

#include "ugp3_config.h"
#include "Environment.h"
#include "ConsoleHandler.h"
#include "Record.h"
#include "Formatter.h"
#include "XmlFormatter.h"
#include "PlainFormatter.h"
#include "BriefFormatter.h"
#include "InfoFormatter.h"

#include <ctime>

#ifdef USE_TERMIOS
    #include <termios.h>
    #include <sys/ioctl.h>
#endif

#ifdef WIN32_CONSOLE
    #include <windows.h>
#endif

using namespace ugp3::log;
using namespace std;

/** Used by the method wrap */
#define IS_WRAP_GOOD(C)		(C==' ')
/** Used by the method wrap */
#define IS_WRAP_ACCEPTABLE(C)	( C=='-'||C=='@'||C=='*'||C=='+'||C=='_' \
	||C==')'||C==']'||C=='>'||C=='/'||C=='\\'||C=='.'||C==',' )

const string ConsoleHandler::XML_SCHEMA_TYPE = "console";

ConsoleHandler::ConsoleHandler()
: console(&cout),
  fileDesc(1),
  smartWrapEnabled(false),
  progressBarsEnabled(false),
  currentColumns(80)
{
    if(this->isTTY() == true)
    {
        this->currentColumns = this->getTerminalWidth() - 1;
        this->smartWrapEnabled = true;
        this->progressBarsEnabled = true;
    }
}

int ConsoleHandler::getTerminalWidth() const
{
    int terminalColumns = 0;

    // First try, almost safe...
    if(terminalColumns <= 0 && GETENV("COLUMNS")!="")
        terminalColumns = atoi(GETENV("COLUMNS").c_str());

#ifdef TIOCGSIZE
    struct ttysize win1;
    if(!ioctl (this->fileDesc, TIOCGSIZE, &win1))
        terminalColumns = win1.ts_cols;
#endif

#ifdef TIOCGWINSZ
    struct winsize win2;
    if(!ioctl (this->fileDesc, TIOCGWINSZ, &win2))
        terminalColumns = win2.ws_col;
#endif

#ifdef WIN32_CONSOLE
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi)) {
        terminalColumns = csbi.dwSize.X;
    }
#endif

    // Last resort
    if(terminalColumns <= 0)
        terminalColumns = 79; // Safer than 80...

    return terminalColumns;
}

void ConsoleHandler::readXml(const xml::Element& element)
{
    Handler::readXml(element);

    // get the inner elements
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        const string& elementName = childElement->ValueStr();
        if(elementName == "enableSmartWrap")
        {
            this->smartWrapEnabled = Convert::toBool(childElement->FirstChild()->ValueStr());
        }
        else if(elementName == "enableProgressbars")
        {
            this->progressBarsEnabled = Convert::toBool(childElement->FirstChild()->ValueStr());
        }

        childElement = childElement->NextSiblingElement();
    }
}

void ConsoleHandler::manageProgressBar(const Record& logRecord)
{
    Assert(logRecord.hasProgress());  // If the record hasn't progrees an exception is thown

    static bool started = false;
    static char empty = ' ';
    static char full = '#';
    static clock_t lastUpdate = 0;
    clock_t now;
    int need_update;

    static const string escapeString = "\b\b"; // just a couple of backspaces

    // FIRST of all: no more than 1 update x few second
    need_update = false;
    now = time(nullptr);
    if(started==false) {
        need_update = true;
        started = true;
    }
    if(logRecord.getProgress() == Progress::START || logRecord.getProgress() == Progress::END) {
        need_update = true;
    } else if(now-lastUpdate >= 1 && this->checkProgressBars() == true) {
        need_update = true;
    } else if(now-lastUpdate >= 5 && this->checkProgressBars() == false) {
        need_update = true;
    }

    if(!need_update) {
        return;
    }
    lastUpdate = now;

    if(this->checkProgressBars()) {
        // print a dummy record
        const Record dummy(escapeString, logRecord.getLevel(), logRecord.getLocation(), logRecord.getTime());
        string dummyMessage = this->getFormatter().format(dummy);

        const size_t position = dummyMessage.find(escapeString);
        dummyMessage = dummyMessage.erase(position, escapeString.length());

        // split the formatted message
        const string& firstPart = dummyMessage.substr(0, position);
        const string& lastPart =  dummyMessage.substr(position);

        // consider only the part on the progressbar line
        const size_t firstNewLine = firstPart.find_last_of('\n');
        const size_t lastNewLine = lastPart.find_first_of('\n');
        const string& firstPartOnMessageLine = firstNewLine != string::npos ? firstPart.substr(firstNewLine, firstPart.length()) : firstPart;
        const string& lastPartOnMessageLine = lastNewLine != string::npos ? lastPart.substr(lastNewLine) : lastPart;

        *this->console << firstPart;

        *this->console << logRecord.getMessage() << " [";

        char foo[32];
        const double progress = logRecord.getProgress();
        sprintf(foo, "] %03.0f%%", 100.0*progress);
        const string& s = foo;

        const size_t maxColumns = getTerminalWidth() - s.length() - 3
                                  - firstPartOnMessageLine.length()
                                  - lastPartOnMessageLine.length()
                                  - logRecord.getMessage().length();


        for(unsigned int i = 0; i < maxColumns; i++)
        {
            if(i < progress * maxColumns)
            {
                *this->console << full;
            }
            else *this->console << empty;
        }

        *this->console << s << lastPart << "\r";

        if(logRecord.getProgress() == Progress::END)
        {
            *this->console << std::endl;
        }
    }
    else 
    {
        char foo[32];
        sprintf(foo, "... %0.3f%% complete", 100.0* logRecord.getProgress());

        const string& s = foo;
        const string& formattedMessage = this->getFormatter().format(logRecord) + s;
        this->wrap(formattedMessage);
    }

    // Goodbye!?!?!
    if(logRecord.getProgress() == Progress::END)
    {
        started = false;
    }
}

void ConsoleHandler::wrap(const string& message) const
{
    size_t chunkStart = 0;

    if(this->checkSmartWrap()) {
        while(chunkStart <= message.length())
        {
            unsigned int chunkLength = 0;
            while(message[chunkStart+chunkLength] && message[chunkStart+chunkLength] != '\n')
                ++chunkLength;

            if(!chunkLength)
            {

                *this->console << endl;
                ++chunkStart;
                continue;
            }

            --chunkLength;

            size_t startPosition = chunkStart;
            while(startPosition < chunkStart+chunkLength)
            {
                size_t breakPosition, blankPosition, candidateBreakPosition;

                if(chunkLength-(startPosition-chunkStart) < currentColumns)
                {
                    // just print it!
                    breakPosition = chunkStart+chunkLength;
                }
                else
                {
                    // seek next break position
                    
                    /* Seek 'true' blank */
                    size_t tbp = startPosition + currentColumns;
                    while(tbp > startPosition && !IS_WRAP_GOOD(message[tbp]))   
                          --tbp;
                    blankPosition = tbp;

                    /* Seek reasonable alternative */
                    size_t cbp = startPosition + currentColumns;
                    while(cbp > startPosition && !IS_WRAP_ACCEPTABLE(message[cbp]))
                          --cbp;
                    candidateBreakPosition = cbp;

                    if(blankPosition > startPosition)
                        breakPosition = blankPosition;
                    else if(candidateBreakPosition > startPosition)
                        breakPosition = candidateBreakPosition;
                    else
                        breakPosition = startPosition + currentColumns;
                }

                // print string
                for(size_t c = startPosition; c < breakPosition; ++c)
                    this->console->put(message[c]);
                if(!IS_WRAP_GOOD(message[breakPosition])) {
                    this->console->put(message[breakPosition]);
                }

                *this->console << endl;

                startPosition = breakPosition +1;
            }

            chunkStart += chunkLength +1 +1;
        }
    } else {
        // No "SmartWrap" on console... Just throw everything
        *console << message << endl;
    }
}

void ConsoleHandler::writeXmlOptions(ostream& output) const
{
    output << "    <enableSmartWrap>" << Convert::toString(this->smartWrapEnabled) << "</enableSmartWrap>" << endl;
    output << "    <enableProgressbars>"<< Convert::toString(this->progressBarsEnabled) <<"</enableProgressbars>" << endl;
}

void ConsoleHandler::publish(const Record& logRecord)
{
    Assert(logRecord.getLevel() <= this->getLevel());

    if(logRecord.hasProgress())
    {
        // A progress!
        this->manageProgressBar(logRecord);
    } 
    else
    {
        const string& formattedMessage = this->getFormatter().format(logRecord);
        this->wrap(formattedMessage);
    }
    this->console->flush();
}

void ConsoleHandler::setLevel(const Level& level)
{
    Handler::setLevel(level);

    // Info formatters are pretty special
    // Info formatters are pretty special
    if(this->level == Level::Info) {
        if(this->getFormatter().getType() == BriefFormatter::XML_SCHEMA_TYPE) {
            this->setFormatter(*new InfoFormatter);
        }
        this->enableProgressBar(true);
    } else {
        if(this->getFormatter().getType() == InfoFormatter::XML_SCHEMA_TYPE) {
            this->setFormatter(*new BriefFormatter);
        }
        this->enableProgressBar(false);
    }
}

