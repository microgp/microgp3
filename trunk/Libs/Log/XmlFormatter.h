/**
 * @file XmlFormatter.h
 * Definition and implementation of the XmlFormatter class.
 */

#ifndef HEADER_UGP3_LOG_XMLFORMATTER
#define HEADER_UGP3_LOG_XMLFORMATTER

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
  * @class XmlFormatter
  * Allows to write a string representation of a Record object in a xml format.
  * @see Record
  * @see Formatter
  * @see BriefFormatter
  * @see ExtendedFormatter
  * @see PlainFormatter
  */
class XmlFormatter : public Formatter
{
public:
    /** Type of the xml schema used. Usefull to know what type of the Formatter object is instantiated. */
    static const std::string XML_SCHEMA_TYPE;
    const std::string format(const Record& record) const;
    virtual const std::string& getType() const;
};

inline const std::string& XmlFormatter::getType() const
{
    return XML_SCHEMA_TYPE;
}

inline const std::string XmlFormatter::format(const Record& record) const
{
    std::ostringstream tmpStream;
    tmpStream << "<record>" << std::endl;
    tmpStream << "  <location>" << record.getLocation().getFile() << "</location>" << std::endl;
    tmpStream << "  <level>" << record.getLevel().toString() << "</level>" << std::endl;
    tmpStream << "  <message>" << record.getMessage() << "</message>" << std::endl;
    // TODO: Add information?
    tmpStream << "</record>";

    return tmpStream.str();
}

}

}

#endif
