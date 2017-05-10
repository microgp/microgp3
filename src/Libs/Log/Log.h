/***********************************************************************\
|                                                                       |
| Log.h                                                                 |
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
 * @file Log.h
 * Definition of the Log class.
 * @see Log.cc
 */

#ifndef HEADER_LOG_LOG
/** Defines that this file has been included */
#define HEADER_LOG_LOG

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// headers from standard library
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <cassert>
#include <map>
#include <type_traits>

// headers from this module
#include "Handler.h"
#include "Level.h"
#include "Location.h"
#include "Progress.h"
#include "Record.h"

// headers from other modules
#include "IString.h"
#include "XMLIFace.h"
#include "Enumeration.h"
#include "Exception.h"
#include "Exceptions/ArgumentNullException.h"
#include "Exceptions/IndexOutOfBoundsException.h"

// use these macros to write cleaner code (they refer to the global log_ instance)
/** Sets the Error level and the actual code Location in the global log_ instance of the Log class */
#define LOG_ERROR   ::log_ << ugp3::log::Level::Error   << ugp3::log::Location(LOCATION)
/** Sets the Warning level and the actual code Location in the global log_ instance of the Log class */
#define LOG_WARNING ::log_ << ugp3::log::Level::Warning << ugp3::log::Location(LOCATION)
/** Sets the Info level and the actual code Location in the global log_ instance of the Log class */
#define LOG_INFO    ::log_ << ugp3::log::Level::Info    << ugp3::log::Location(LOCATION)
/** Sets the Verbose level and the actual code Location in the global log_ instance of the Log class */
#define LOG_VERBOSE ::log_ << ugp3::log::Level::Verbose << ugp3::log::Location(LOCATION)
/** Sets the Debug level and the actual code Location in the global log_ instance of the Log class */
#define LOG_DEBUG   ::log_ << ugp3::log::Level::Debug   << ugp3::log::Location(LOCATION)


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
 * @class Log
 * Provides methods to track program activity.
 * @author Salomone Alessandro 
 */
class Log : public xml::XMLIFace
{
private:
    // A list of the opened streams on which the messages are written.
    std::vector<Handler*> handlers;

    // The stream used to store the message until it is committed on the active streams.
    std::ostringstream *temporaryStream;

    // The verbosity level of the current message.
    Level level;

    // The location of the current message.
    Location location;

    // Actual progress to show
    const Progress* progress;
    // Name of the xml element
    static const std::string XML_NAME;

    /**
     * @struct eqstr
     * Struct used to compare elements in the map 
     */ 
    struct eqstr
    {
      bool operator()(long s1, long s2) const
      {
        return s1 > s2;
      }
    };

    // Hash map: used to keep track of WARNING log messages
    std::map <long, int, eqstr> warnings;

    // Maximum number of warnings of the same kind allowed
    static const int MAX_WARNINGS;

private:
    // Flushes the current message, writing it on the active streams.
    void commit();


    // The copy constructor: it is declared as private so that it cannot be accessed.
    Log(const Log& log);

public:
    /** 
     * Constructor of the class. Creates default Log with a null progress and a info verbosity level
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    Log();
    /** 
     * Destructor of the class. Cleans memory used by the object.
     */
    ~Log();

public:
    /** 
     * Loads a xml file with log information.
     * @param fileName Name of the file to read
     * @throws Any exception.
     */
    void load(const std::string& fileName);
    /** 
     * Saves a xml file with log information.
     * @param fileName Name of the file to write
     * @throws Any exception. Exception if the file can't be accessed
     */
    void save(const std::string& fileName);
    /** 
     * Resets the attributes of the object with a null progress and a info verbosity level
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    void clear();

public:
    /** 
     * Adds a new stream to write 
     * @param handler Stream to write the messages
     * @throws Any exception. runtime_error if the handler is already inserted
     */
    void         addHandler(Handler& handler);
    /** 
     * Removes a stream
     * @param handler Stream to remove
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    void         removeHandler(Handler& handler);
    /** 
     * Returns the handler placed in the index position of the vector
     * @param index in the vector
     * @throws Any exception.
     */
    Handler&     getHandler(const unsigned int index) const;
    /** 
     * Returns the number of streams actually registered 
     * @throws Any exception.
     */
    unsigned int getHandlersCount() const;

public: // XMLIFace methods
    virtual void writeXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
    virtual const std::string& getXmlName() const;

public:
    /** 
     * Adds the float value to the stream
     * @param value Float value to add
     * @returns Log with the stream updated
     */
    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, Log&>::type operator<<(T value);
    
    /** 
     * Adds the string value to the stream.
     * @param value String value to add
     * @returns Log with the stream updated
     */
    Log& operator<<(const std::string& value);
    
    /** 
     * Adds the string value to the stream. If a null pointer is specified, "{null char*}" is added
     * @param value String value to add
     * @returns Log with the stream updated
     */
    Log& operator<<(const char* value);
    
    /** 
     * Adds the string value to the stream. If a null pointer is specified, "{null char*}" is added
     * @param value String value to add
     * @returns Log with the stream updated
     */
    Log& operator<<(const std::string* value);
    /** 
     * Adds the string representation of the IString to the stream. If a null pointer is specified, "{null}" is added
     * @param value IString to add
     * @returns Log with the stream updated
     */
    Log& operator<<(const ugp3::IString* value);
    /** 
     * Adds the string representation of the IString to the stream.
     * @param value IString to add
     * @returns Log with the stream updated
     */
    Log& operator<<(const ugp3::IString& value);
    /** 
     * Sets the actual progress
     * @param value Actual Progress to show
     * @returns Log with the progress updated
     */
    Log& operator<<(const Progress& value);
    /** 
     * 
     * @param
     * @returns Log with the stream updated
     */
    Log& operator<<(std::ostream& (*functionPointer)(std::ostream&));
    /** 
     * Sets the actual vervosity level
     * @param value Verbosity level 
     * @returns Log with the Level updated
     */
    Log& operator<<(const Level& value);
    /** 
     * Sets the actual location
     * @param value Actual location
     * @returns Log with the Location updated
     */
    Log& operator<<(const Location& value);
};


inline const std::string& Log::getXmlName() const
{
    return XML_NAME;
}


template <typename T>
inline typename std::enable_if<std::is_arithmetic<T>::value, Log&>::type Log::operator<<(T value)
{
    *this->temporaryStream << value;
    return *this;
}

inline Log& Log::operator<<(const std::string& value)
{
    *this->temporaryStream << value;
    return *this;
}

inline Log& Log::operator<<(const Progress& value)
{
    this->progress = &value;
    return *this;
}

inline Log& Log::operator<<(const char* value)
{
    if(value == nullptr)
    {
        *this->temporaryStream << "{null char*}";
    }
    else
    {
        *this->temporaryStream << value;
    }
    return *this;
}

inline Log& Log::operator<<(const std::string* value)
{
    if(value == nullptr)
    {
        *this->temporaryStream << "{null string}";
    }
    else
    {
        *this->temporaryStream << value;
    }
    return *this;
}

inline Log& Log::operator<<(const ugp3::IString* value)
{
    if(value == nullptr)
    {
        *this->temporaryStream << "{null}";
    }
    else
    {
        *this->temporaryStream << value->toString();
    }
    return *this;
}

inline Log& Log::operator<<(const ugp3::IString& value)
{
    *this->temporaryStream << value.toString();
    return *this;
}

inline Log& Log::operator<<(const Location& value)
{
    this->location = value;

    return *this;
}

inline Log& Log::operator<<(const Level& value)
{
    this->level = value;
    return *this;
}


}

}

//Global Log for showing the program activity.
extern ugp3::log::Log log_;

#endif

