/**
 * @file Formatter.cc
 * Implementation of the Formatter class.
 * @see Formatter.h
 */

#include "Log.h"
#include "BriefFormatter.h"
#include "ExtendedFormatter.h"
#include "PlainFormatter.h"
#include "InfoFormatter.h"
#include "XmlFormatter.h"
using namespace ugp3::log;
using namespace std;

const string Formatter::XML_NAME = "formatter";

const string ExtendedFormatter::XML_SCHEMA_TYPE = "extended";
const string PlainFormatter::XML_SCHEMA_TYPE = "plain";
const string XmlFormatter::XML_SCHEMA_TYPE = "xml";

unique_ptr<Formatter> Formatter::instantiate(const std::string& type)
{
    unique_ptr<Formatter> formatter;

    if(type == BriefFormatter::XML_SCHEMA_TYPE)
    {
        formatter = unique_ptr<Formatter>(new BriefFormatter());
    }
    /**
    else if(type == InfoFormatter::XML_SCHEMA_TYPE)
    {
        LOG_WARNING << "The InfoFormatter is intended for internal use only" << ends;
        formatter = unique_ptr<Formatter>(new InfoFormatter());
    }
    */
    else if(type == ExtendedFormatter::XML_SCHEMA_TYPE)
    {
        formatter = unique_ptr<Formatter>(new ExtendedFormatter());
    }
    else if(type == PlainFormatter::XML_SCHEMA_TYPE)
    {
        formatter = unique_ptr<Formatter>(new PlainFormatter());
    }
    else if(type == XmlFormatter::XML_SCHEMA_TYPE)
    {
        formatter = unique_ptr<Formatter>(new XmlFormatter());
    }
    
    if(formatter.get() == nullptr)
    {
        throw Exception("Formatter type " + type + " unknown.", LOCATION);
    }

    return formatter;
}

void Formatter::writeXml(std::ostream& output) const
{
    output << "  <" << XML_NAME << " type='" << this->getType() << "'/>" << endl;
}

void Formatter::readXml(const xml::Element& element)
{
    // do nothing
}

