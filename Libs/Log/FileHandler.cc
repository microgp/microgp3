/**
 * @file FileHandler.cc
 * Implementation of the FileHandler class.
 * @see FileHandler.h
 */

#include "FileHandler.h"
#include "File.h"
#include "Record.h"
#include "Debug.h"
#include "Formatter.h"
#include "Convert.h"
using namespace std;
using namespace ugp3::log;

// TODO: an easy way to append logs to an existing file is to modify here, and open the ofstream "stream" with the std::ofstream::app flag set, adding a bool or something in the constructor; however, do we really want to do that?
FileHandler::FileHandler(const std::string& fileName)
    : stream(fileName.c_str()), //the file is opened here
     fileName(fileName),
     maxRecordsPerFile(500000),
     records(0)
{
    if(stream.is_open() == false)
    {
        throw std::runtime_error(
            "Cannot open file " + fileName + " for logging.");
    }
}

void FileHandler::publish(const Record& logRecord)
{
    Assert(logRecord.getLevel() <= this->getLevel()); //throw AssertException if the level of the record is not equal or lower to the level of the handler
    
    if(logRecord.hasProgress()) return; // the records with progress aren't published in a file

    // writes the record
    this->stream << this->getFormatter().format(logRecord) << std::endl;
    this->stream.flush();

    records++;

    // closes the file and creates a new one if split is necesary
    bool needToSplitFile = records % maxRecordsPerFile == 0;
    if(needToSplitFile)
    {
        this->stream << "-- snipped --" << std::endl;
        stream.close();
        
        const string& targetName = "old." + fileName;
        ::rename(fileName.c_str(), targetName.c_str());
      
        stream.open(fileName.c_str());

        this->stream << "--" << records << " records discarded --" << std::endl;
    }
}

void FileHandler::readXml(const xml::Element& element)
{
    Handler::readXml(element);

    // get the inner elements
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        const string& elementName = childElement->ValueStr();
        if(elementName == "fileName")
        {
               this->fileName = childElement->FirstChild()->ValueStr();
            stream.open(fileName.c_str());
        }
        else if(elementName == "recordsPerFile")
        {
             this->maxRecordsPerFile = Convert::toUInt(childElement->FirstChild()->ValueStr());
        }

        childElement = childElement->NextSiblingElement();
    }
}

FileHandler::~FileHandler()
{
    this->stream << "-- end of log --" << std::endl;
    this->stream.close();
}

void FileHandler::writeXmlOptions(std::ostream& output) const
{
    output << "    <fileName>" << fileName << "</fileName>" << endl;
    output << "    <recordsPerFile>"<< maxRecordsPerFile <<"</recordsPerFile>" << endl;
}

