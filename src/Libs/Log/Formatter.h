/**
 * @file Formatter.h
 * Definition of the Formatter class.
 * @see Formatter.cc
 * @see XMLIFace.h
 */

#ifndef HEADER_UGP3_LOG_FORMATTER
/** Defines that this file has been included */
#define HEADER_UGP3_LOG_FORMATTER

#include <memory>
#include <string>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "XMLIFace.h"
#include "Record.h"

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
  * @class Formatter
  * Allows to instantiate a sub-Formatter class: brief, extended, plain or xml
  * @see BriefFormatter
  * @see ExtendedFormatter
  * @see PlainFormatter
  * @see XmlFormatter
  * @see xml::XMLIFace
  */
class Formatter : public xml::XMLIFace
{
public:
    /** Name of this xml element. */
    static const std::string XML_NAME;
    /** 
     * Creates a (sub)object of Formatter. 
     * @param type Type of format
     * @returns Pointer to a Formatter instance 
     * @throws Any exception. Exception if the type is unknown or the method can't instantiate the object.
     */
    static std::unique_ptr<Formatter> instantiate(const std::string& type);

public:
    /** 
     * Returns a string with the Record information
     * @param record Record of an event
     * @returns string Information of the record
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual const std::string format(const Record& record) const = 0;
    /** 
     * Returns the format type
     * @returns string The format type
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    virtual const std::string& getType() const = 0;
    /** 
     * Destructor of the class 
     * Clean if necessary.
     */
    virtual ~Formatter();

public: //XMLIFace interface
    virtual const std::string& getXmlName() const;
    virtual void writeXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
};

inline Formatter::~Formatter()
{ }

inline const std::string& Formatter::getXmlName() const
{
    return XML_NAME;
}

}

}

#endif
