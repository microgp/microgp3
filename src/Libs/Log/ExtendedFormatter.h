/**
 * @file ExtendedFormatter.h
 * Definition and implementation of the ExtendedFormatter class.
 */

#ifndef HEADER_UGP3_LOG_EXTENDEDFORMATTER
/** Defines that this file has been included */
#define HEADER_UGP3_LOG_EXTENDEDFORMATTER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Formatter.h"
#include <sstream>

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
  * @class ExtendedFormatter
  * Allows to write a string representation of a Record object in a extended format.
  * @see Record
  * @see Formatter
  * @see BriefFormatter
  * @see PlainFormatter
  * @see XmlFormatter
  */
class ExtendedFormatter : public Formatter
{
public:
    /** Type of the xml schema used. Usefull to know what type of the Formatter object is instantiated. */
    static const std::string XML_SCHEMA_TYPE;
    const std::string format(const Record& record) const;
    virtual const std::string& getType() const;
};


inline const std::string& ExtendedFormatter::getType() const
{
    return XML_SCHEMA_TYPE;
}

inline const std::string ExtendedFormatter::format(const Record& record) const
{
    std::ostringstream tmpStream;
    
	tmpStream << std::setfill('0');
	tmpStream 
    	<< "[" 
    	<< std::setw(4) << record.getTime().tm_year + 1900 << "-" 
    	<< std::setw(2) << record.getTime().tm_mon + 1 << "-" 
    	<< std::setw(2) << record.getTime().tm_mday << " " 
    	<< std::setw(2) << record.getTime().tm_hour << ":" 
    	<< std::setw(2) << record.getTime().tm_min << ":" 
    	<< std::setw(2) << record.getTime().tm_sec << "] ";
	
	tmpStream << std::setfill(' ');
	tmpStream 
	    << record.getLocation().getFunction() << "@" 
	    << record.getLocation().getFile() << ":" 
	    << record.getLocation().getLine();
	tmpStream << " " << record.getLevel().toString() << "::" << std::endl << "    ";
	tmpStream << record.getMessage();
	
	return tmpStream.str();
}

}

}

#endif
