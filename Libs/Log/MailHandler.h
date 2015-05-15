/**
 * @file MailHandler.h
 * Definition and implementation of the MailHandler class.
 */
#ifndef HEADER_UGP3_LOG_MAILHANDLER
/** Defines that this file has been included */
#define HEADER_UGP3_LOG_MAILHANDLER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ostream>
#include <vector>
#include <string>

#include "Handler.h"
#include "Record.h"
#include "Formatter.h"

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
* @class MailHandler 
* Keeps a set of records and send by mail it when reach a maximum number 
* @see Handler
* @see ConsoleHandler
* @see FileHandler
*/
class MailHandler : public Handler
{
private:
    // Recipient of the mail
    std::string recipient;
    // Server to connect with to send the mail
    std::string smtpServer;
    // Sender of the mail
    static const std::string sender;
    // Maximum number of the records to send in a message
    unsigned int maxRecordsPerMessage;
    // Maximum number of minutes 
    unsigned int maxMinutes;
    // Records to send
    std::vector<std::string> records;

    // Sends the records to the mail
    void sendRecords();

    // Handler methods
    virtual void writeXmlOptions(std::ostream& output) const;
   
public:
    /** 
     * Constructor of the class. Creates a new MailHandler with 10000 of maximun records to write per file and 60 of maximum minutes.
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    MailHandler();
    
    /** Type of the schema to use with this kind of xml elements **/
    static const std::string XML_SCHEMA_TYPE;

    // Handler methods
    virtual void publish(const Record& logRecord);
    virtual const std::string& getType() const;

    /** 
     * Destructor of the class. Sends the residuary records
     */
    virtual ~MailHandler();
};

inline const std::string& MailHandler::getType() const
{
    return XML_SCHEMA_TYPE;
}

inline MailHandler::MailHandler()
    : maxRecordsPerMessage(10000), maxMinutes(60)
{ }

inline void MailHandler::publish(const Record& logRecord)
{
    Assert(logRecord.getLevel() <= this->getLevel()); //throw AssertException if the level of the record is not equal or lower to the level of the handler

    // Gets the message to write
    const std::string& message = this->getFormatter().format(logRecord) + "\n";
    
    this->records.push_back(message);
    
    // If the number of records is the maximum, then they are sent
    if(this->records.size() >= maxRecordsPerMessage)
    {
        this->sendRecords();
        this->records.clear();
    }
}

inline void MailHandler::writeXmlOptions(std::ostream& output) const
{

}

inline void MailHandler::sendRecords()
{
    // open connection to smtpServer
    
    // send records
    
    // close connection
}

inline MailHandler::~MailHandler()
{
    this->sendRecords();
}

}

}

#endif

