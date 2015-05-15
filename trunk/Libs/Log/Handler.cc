/**
 * @file Handler.cc
 * Implementation of the Handler class.
 * @see Handler.h
 */
#include "Handler.h"
#include "ConsoleHandler.h"
#include "MailHandler.h"
#include "FileHandler.h"
#include "BriefFormatter.h"
#include "InfoFormatter.h"
#include "Record.h"

using namespace ugp3::log;
using namespace std;

const string& Handler::XML_NAME = "handler";
const string FileHandler::XML_SCHEMA_TYPE = "file";
const string MailHandler::XML_SCHEMA_TYPE = "mail";


Handler::Handler()
    :  	formatter(new BriefFormatter()),
	level(Level::Info)
{ 
    formatterSaved = nullptr;
}

const Formatter& Handler::getFormatter() const
{
    return *this->formatter;
}

void Handler::setFormatter(const Formatter& formatter)
{
    delete this->formatter;
    this->formatter = &formatter;
}

void Handler::pushFormatter(const Formatter& formatter)
{
    Assert(formatterSaved == nullptr);
    this->formatterSaved = this->formatter;
    this->formatter = &formatter;
}

void Handler::popFormatter(void)
{
    Assert(formatterSaved != nullptr);
    delete this->formatter;
    this->formatter = this->formatterSaved;
    this->formatterSaved = nullptr;
}

void Handler::setLevel(const Level& level)
{
    this->level = level;

}

const Level& Handler::getLevel() const
{
    return this->level;
}

Handler::~Handler()
{ 
    delete this->formatter;
}

bool Handler::isLoggable(const Record& logRecord) const
{
    return logRecord.getLevel() <= this->level;
}

void Handler::writeXml(std::ostream& output) const
{
    output << "  <" << XML_NAME << " type='" << this->getType() << "' level='" << this->level.toString() << "'>" << endl;

    this->formatter->writeXml(output);
    this->writeXmlOptions(output);

    output << "  </" << XML_NAME << ">" << endl;
}

void Handler::readXml(const xml::Element& element)
{
    if(element.ValueStr() != XML_NAME) 
    {
        throw std::runtime_error("expected element " + XML_NAME);
    }

    const string& levelString = xml::Utility::attributeValueToString(element, "level");
    this->level = Level::parse(levelString);

    // get the inner elements
    const xml::Element* childElement = element.FirstChildElement();
    while(childElement != nullptr)
    {
        const string& elementName = childElement->ValueStr();
        if(elementName == Formatter::XML_NAME)
        {
            const string& type = xml::Utility::attributeValueToString(*childElement, "type");
            this->setFormatter( *Formatter::instantiate(type).release() );
            ((Formatter*)this->formatter)->readXml(*childElement);
        }

        childElement = childElement->NextSiblingElement();
    }
}

unique_ptr<Handler> Handler::instantiate(const std::string& type)
{
    unique_ptr<Handler> handler;

    if(type == ConsoleHandler::XML_SCHEMA_TYPE)
    {
        handler = unique_ptr<Handler>(new ConsoleHandler());
    }
    else if(type == FileHandler::XML_SCHEMA_TYPE)
    {
        handler = unique_ptr<Handler>(new FileHandler());
    }
    else if(type == MailHandler::XML_SCHEMA_TYPE)
    {
        handler = unique_ptr<Handler>(new MailHandler());
    }
    
    if(handler.get() == nullptr)
    {
        throw Exception("Handler type " + type + " unknown.", LOCATION);
    }

    return handler;
}
