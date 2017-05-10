/**
 * @file Handler.h
 * Definition of the Handler class.
 * @see Handler.cc
 */

#ifndef HEADER_UGP3_LOG_HANDLER
/** Defines that this file has been included */
#define HEADER_UGP3_LOG_HANDLER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <exception>
#include <memory>
#include "XMLIFace.h"
#include "Level.h"

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

class Formatter;
class Record;

/**
* @class Handler 
* Manages a stream to write information about the execution of the application.
* @see xml::XMLIFace
* @see ConsoleHandler
* @see FileHandler
* @see MailHandler
*/
class Handler : public xml::XMLIFace
{
private:
    // Formatter to use with the information
    const Formatter* formatter;
    const Formatter* formatterSaved;

protected:
    // Verbosity level of this handler
    Level level;
    /** 
     * Writes the options of this handler in the output stream
     * @param output The stream where write the options
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    virtual void writeXmlOptions(std::ostream& output) const = 0;
    
public:
    /** 
     * Constructor of the class. Instantiates a Handler object with info level and brief formatter.
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    Handler();
    /** 
     * Destructor of the class. Cleans memory
     */
    virtual ~Handler();
    
public:
    /** 
     * Sets the verbosity level of the handler
     * @param level Verbosity level to set
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    virtual void     setLevel(const Level& level);
    /** 
     * Sets the formatter to use with the information
     * @param formatter Formatter to use
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    void             setFormatter(const Formatter& formatter);

    /*
    * Rudimentary stack (only 1 position)!
    */
    void             pushFormatter(const Formatter& formatter);
    void             popFormatter(void);

    /** 
     * Returns if the record is loggable with this handler or not
     * @param record to check
     * @returns bool True if the record has the same or lower level than the handler (it means that the record has to be handled), false in other case
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    bool             isLoggable(const Record& logRecord) const;
    /** 
     * Returns if the verbosity level of the handler
     * @returns Level The verbosity level of the handler
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    const Level&     getLevel() const;
    /** 
     * Returns if the formatter of the handler
     * @returns Formatter The formatter of the handler
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    const Formatter& getFormatter() const;

public:
    /** Name of this xml element */
    static const std::string& XML_NAME;
    /** 
     * Creates a (sub)object of Handler. 
     * @param type Type of the handler
     * @returns Pointer to a Formatter instance 
     * @throws Any exception. Exception if the type is unknown.
     */
    static std::unique_ptr<Handler> instantiate(const std::string& type);

public: //XMLIFace interface
    virtual void writeXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
    virtual const std::string& getXmlName() const;
  
public:  
    /** 
     * Writes the record specified in the stream handled by the handler
     * @param record Information to write
     * @throws Any exception.
     */
    virtual void publish(const Record& logRecord) = 0;
    /** 
     * Returns the type of the handler
     * @returns string The type of the handler
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    virtual const std::string& getType() const = 0;
};

inline const std::string& Handler::getXmlName() const
{
    return XML_NAME;
}

}

}

#endif

