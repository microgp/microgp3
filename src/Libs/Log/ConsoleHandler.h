/***********************************************************************\
|                                                                       |
| ConsoleHandler.h                                                      |
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
 * @file ConsoleHandler.h
 * Definition of the ConsoleHandler class.
 * @see ConsoleHanler.cc
 */

#ifndef HEADER_UGP3_LOG_CONSOLEHANDLER
/** Defines that this file has been included */
#define HEADER_UGP3_LOG_CONSOLEHANDLER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fstream>
#include <map>
#include <locale>
#include <unistd.h>

#include "Handler.h"
#include "Debug.h"
#include "Convert.h"

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif


/**
 * ugp3 namespace
 */
namespace ugp3
{

/**
 * ugp3::log namespace
 */
namespace log
{

/**
* @class ConsoleHandler 
* Manages a stream to write information about the execution of the application.
* @see Handler
* @see FileHandler
* @see MailHandler
*/
class ConsoleHandler : public Handler
{
private:
    // Stream where write
    std::ostream* console;
    // File descriptor
    int fileDesc;
    // Sets if SmartWrap is enabled
    bool smartWrapEnabled;
    // Sets if progress bars are showed
    bool progressBarsEnabled;
    // Columns of the output
    unsigned int currentColumns;

private:
    // 
    void wrap(const std::string& message) const;
    // Show the progress bar
    void manageProgressBar(const Record& logRecord);
    
    // Returns if the output is a TTY
    bool isTTY() const;
    // Saves the actual cursor position. NOT IMPLEMENTED
    void saveCursorPosition() const;
    // Restores the cursor position. NOT IMPLEMENTED
    void restoreCursorPosition() const;
    // NOT IMPLEMENTED
    void blankLine() const;

protected: // Handler methods
     virtual void writeXmlOptions(std::ostream& output) const;

public:
    /** 
     * Constructor of the class. Creates a new ConsoleHandler with enabled wraps and progress bars
     * @throws Any exception
     */
    ConsoleHandler();

    // Progress bar...
    bool checkProgressBars() const;
    void enableProgressBar(bool value);

    /** 
     * Returns if smart wrap is enabled
     * @returns bool True if smart wrap is enabled, false in other case
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    bool checkSmartWrap() const;
    /** 
     * Sets the smart wrap enable value to the value specified. If the stream is a TTY, the smart wrap is disabled.
     * @param value True if the intention is to enable the smart wrap, false in other case
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    void enableSmartWrap(bool value);
    /** 
     * Returns the width of the console
     * @returns int The number of columns of the console
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    int getTerminalWidth() const;
    
    /** 
     * Sets a new output stream for the class (e.g. an ostringstream). This function has been created as an attempt
     * to re-use the logging classes in a GUI, without modifying the existing stuff.
     * @param console The new ostream* the text will be sent to.
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    void setConsole(std::ostream* console);

    /** Type of the schema to use with this kind of xml elements **/
    static const std::string XML_SCHEMA_TYPE;

    // Handler methods
    virtual void publish(const Record& logRecord);
    virtual const std::string& getType() const;
    virtual void readXml(const xml::Element& element);
    virtual void setLevel(const Level& level);
};


inline const std::string& ConsoleHandler::getType() const
{
    return XML_SCHEMA_TYPE;
}

inline bool ConsoleHandler::checkProgressBars() const
{
    return this->progressBarsEnabled && this->isTTY();
}

inline void ConsoleHandler::enableProgressBar(bool value)
{
    this->progressBarsEnabled = value;
}

inline bool ConsoleHandler::checkSmartWrap() const
{
    return this->smartWrapEnabled && this->isTTY();
}

inline void ConsoleHandler::enableSmartWrap(bool value)
{
    this->smartWrapEnabled = value;
}

inline void ConsoleHandler::setConsole(std::ostream* console)
{
	// TODO maybe make it throw an exception?
	if( console != nullptr)
		this->console = console;
}

inline bool ConsoleHandler::isTTY() const
{
#ifdef _WIN32
    return _isatty(this->fileDesc);
#else
    return isatty(this->fileDesc);
#endif
}

}

}

#endif

