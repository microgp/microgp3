/***********************************************************************\
|                                                                       |
| Log.cc                                                                |
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
 * @file Log.cc
 * Implementation of the Log class.
 * @see Log.h
 */

#include "ugp3_config.h"
#include "Log.h"
#include <stdexcept>
using namespace std;
using namespace ugp3;
using namespace ugp3::log;

const std::string Log::XML_NAME = "logger";
const int Log::MAX_WARNINGS = 5;

/** Global Log for showing the program activity. */
Log log_;


Log::Log()
     : temporaryStream(new ostringstream()),
       level(Level::Info),
       progress(nullptr)
{ }

Log::~Log()
{
    for(unsigned int i = 0; i < this->handlers.size(); i++)
    {
        delete this->handlers[i];
        this->handlers[i] = nullptr;
    }

    delete temporaryStream;
}

void Log::load(const string& fileName)
{
    xml::Document logSettingsFile;
    logSettingsFile.LoadFile(fileName);

	this->clear();
	this->readXml(*logSettingsFile.RootElement());
}

void Log::save(const string& fileName)
{
    ofstream output;
    output.open(fileName.c_str());
    if(output.is_open() == false)
    {
        throw Exception("Cannot access file \"" + fileName + "\"", LOCATION);
    }

    output << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl;
    this->writeXml(output);
    output.close();
}

void Log::writeXml(std::ostream& output) const
{
    output << "<" << XML_NAME << ">" << endl;

    for(unsigned int i = 0; i < this->handlers.size(); i++)
    {
        this->handlers[i]->writeXml(output);
    }

    output << "</" << XML_NAME << ">" << endl;
}

void Log::readXml(const xml::Element& element)
{
    if(element.ValueStr() != XML_NAME)
    {
        throw std::runtime_error("expected element " + XML_NAME);
    }

    // get the inner elements
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        const string& elementName = childElement->ValueStr();
        if(elementName == Handler::XML_NAME)
        {
            const string& type = xml::Utility::attributeValueToString(*childElement, "type");
            unique_ptr<Handler> handler = Handler::instantiate(type);
            handler->readXml(*childElement);
            this->handlers.push_back(handler.release());
        }

        childElement = childElement->NextSiblingElement();
    }
}

void Log::clear()
{
    this->level = Level::Info;
    this->location = Location();
    this->progress = nullptr;

    delete this->temporaryStream;
    this->temporaryStream = new std::ostringstream();

    for(unsigned int i = 0; i < this->handlers.size(); i++)
    {
        delete this->handlers[i];
    }

    this->handlers.clear();
}

void Log::commit()
{
    bool messageSuppressed = false;
    //bool lastWarning = false;

    // Gets the current message to show
    std::string message = this->temporaryStream->str();
    message.resize(message.size() -1);

    if(this->level == Level::Warning)
    {
        // if we get a warning, we need to check whether we're going to show it
        // get the hash value for the warning message
        std::locale loc;

        // search for the warning message in the hash map
        const collate<char>& coll = use_facet<collate<char> >(loc);
        long warningHash = coll.hash(message.data(), message.data()+message.length());
        std::map <long, int, eqstr>::iterator it = this->warnings.find( warningHash );

        if( it == this->warnings.end() )
        {
            // new element, starting from one
            this->warnings[ warningHash ] = 0;
        }

        // now update the map
        ++this->warnings[ warningHash ];
        if( this->warnings[ warningHash ] == Log::MAX_WARNINGS ) 
        {
            message += " (further warnings of this type will be ignored)";
        } else if( this->warnings[ warningHash ] > Log::MAX_WARNINGS ) 
        {
            messageSuppressed = true;
        }
    }

    if(messageSuppressed == false)
    {
        // Gets the actual time
        ::time_t timeNow;
        ::time(&timeNow);
        ::tm* timeInfo = ::localtime(&timeNow);

        // Creates the record with the information to show
        const Record* record = nullptr;
        if(this->progress != nullptr)
        {
            record = new Record(message, this->level, this->location, *timeInfo, *this->progress);
        }
        else
        {
            record = new Record(message, this->level, this->location, *timeInfo);
        }

        // Reports the message on the various handlers
        for(unsigned int i = 0; i < this->handlers.size(); i++)
        {
            if(this->handlers[i]->isLoggable(*record))
            {
                this->handlers[i]->publish(*record);
            }
        }
        delete record;
    }

    // Resets the parameters for a new message
    this->level = Level::Info;
    this->location = Location();
    this->progress = nullptr;

    delete this->temporaryStream;
    this->temporaryStream = new std::ostringstream();
}

void Log::addHandler(Handler& handler)
{
    for(unsigned int i = 0; i < this->handlers.size(); i++)
    {
        if(&handler == this->handlers[i])
        {
            throw runtime_error("Handler already inserted.");
        }
    }

    this->handlers.push_back(&handler);
}

void Log::removeHandler(Handler& handler)
{
    for(unsigned int i = 0; i < this->handlers.size(); i++)
    {
        if(&handler == this->handlers[i])
        {
            this->handlers.erase(this->handlers.begin() + i);
        }
    }
}

Handler& Log::getHandler(const unsigned int index) const
{
    return *this->handlers.at(index);
}

unsigned int Log::getHandlersCount() const
{
    return (unsigned int) this->handlers.size();
}

Log& Log::operator<<(ostream& (*functionPointer)(ostream&))
{
#ifdef NO_LOGGING
this->temporaryStream->str(string(""));
	return *this;
#endif

    ((*functionPointer)(*this->temporaryStream));

    if(this->temporaryStream->str().back() == '\0')
    {
    //the call to 'functionPointer' just added a '\0' to the end of the string
        this->commit();
    }

    return *this;
}

