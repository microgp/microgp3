/**
 * @file FileHandler.h
 * Definition of the FileHandler class.
 * @see FileHandler.cc
 */

#ifndef HEADER_UGP3_LOG_FILEHANDLER
/** Defines that this file has been included */
#define HEADER_UGP3_LOG_FILEHANDLER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ostream>
#include <fstream>
#include "Handler.h"


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
* @class FileHandler 
* Manages a file stream to write information about the execution of the application.
* @see Handler
* @see ConsoleHandler
* @see MailHandler
*/
class FileHandler : public Handler
{
private:
    // Stream where write
    std::ofstream stream;
    // Name of the file to write
    std::string fileName;
    // Maximum number of records to save in a file. If exceeded, a new file is written.
    unsigned int maxRecordsPerFile;
    // Number of records written
    unsigned long records;

protected: // Handler methods
     virtual void writeXmlOptions(std::ostream& output) const;

public:
    /** 
     * Constructor of the class. Creates a new FileHandler with 500000 of maximun records to write per file.
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    FileHandler();
    /** 
     * Constructor of the class. Creates a new FileHandler with the name specified and 500000 of maximun records to write per file.
     * @param fileName Name of the file to write.
     * @throws Any exception. runtime_error if the file can't be opened.
     */
    FileHandler(const std::string& fileName);
    
    /** Type of the schema to use with this kind of xml elements **/
    static const std::string XML_SCHEMA_TYPE;

    // Handler methods
    virtual void publish(const Record& logRecord);
    virtual const std::string& getType() const;
    virtual void readXml(const xml::Element& element);
    
    /** 
     * Destructor of the class. Writes "-- end of log --" in the file and close it.
     */
    virtual ~FileHandler();
};

inline const std::string& FileHandler::getType() const
{
    return XML_SCHEMA_TYPE;
}

inline FileHandler::FileHandler()
: maxRecordsPerFile(500000), records(0)
{ }

}

}

#endif

