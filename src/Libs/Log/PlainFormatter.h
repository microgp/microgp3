/**
 * @file PlainFormatter.h
 * Definition and implementation of the PlainFormatter class.
 */

#ifndef HEADER_UGP3_LOG_PLAINFORMATTER
/** Defines that this file has been included */
#define HEADER_UGP3_LOG_PLAINFORMATTER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
  * @class PlainFormatter
  * Allows to write a string representation of a Record object in a plain format.
  * @see Record
  * @see Formatter
  * @see BriefFormatter
  * @see ExtendedFormatter
  * @see XmlFormatter
  */
class PlainFormatter : public Formatter
{
public:
    /** Type of the xml schema used. Usefull to know what type of the Formatter object is instantiated. */
    static const std::string XML_SCHEMA_TYPE;
    const std::string format(const Record& record) const;
    virtual const std::string& getType() const;
};

inline const std::string& PlainFormatter::getType() const
{
    return XML_SCHEMA_TYPE;
}

inline const std::string PlainFormatter::format(const Record& record) const
{
    return record.getMessage();
}

}

}

#endif
