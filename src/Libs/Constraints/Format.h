/***********************************************************************\
|                                                                       |
| EnvironmentParameter.h                                                |
|                                                                       |
| This file is part of MicroGP v3 (ugp3)                                |
| http://ugp3.sourceforge.net/                                          |
|                                                                       |
| Copyright (c) 2002-2016 Giovanni Squillero                            |
|                                                                       |
|-----------------------------------------------------------------------|
|                                                                       |
| This program is free software; you can redistribute it and/or modify  |
| it under the terms of the GNU General Public License as published by  |
| the Free Software Foundation, either version 3 of the License, or (at |
| your option) any later version.                                       |
|                                                                       |
| This program is distributed in the hope that it will be useful, but   |
| WITHOUT ANY WARRANTY; without even the implied warranty of            |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      |
| General Public License for more details                               |
|                                                                       |
|***********************************************************************'
| $Revision: 644 $
| $Date: 2015-02-23 14:50:30 +0100 (Mon, 23 Feb 2015) $
\***********************************************************************/

#ifndef HEADER_UGP3_CONSTRAINTS_FORMAT
#define HEADER_UGP3_CONSTRAINTS_FORMAT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include "XMLIFace.h"

namespace ugp3
{

namespace constraints
{

/** 
    @author Salomone Alessandro */
class Format : public xml::XMLIFace
{
protected:
    std::string textBefore;
    std::string textAfter;

	void clear();
	
	static const std::string XML_CHILD_ELEMENT_VALUE;
	
public:
    Format();
    Format(const std::string& textBefore, const std::string& textAfter);
	
	virtual std::string get(const std::string& value) const;

	const std::string& getTextBefore() const;
	const std::string& getTextAfter() const;
	
	virtual ~Format();
	
public: // Xml interface
	virtual void writeXml(std::ostream& output) const;
	virtual void readXml(const xml::Element& element);
};

inline Format::Format(const std::string& textBefore, const std::string& textAfter)
    : textBefore(textBefore), textAfter(textAfter)
{ }

class CommentFormat : public Format
{
public:
    static const std::string XML_NAME;
    
public:
    CommentFormat();
    CommentFormat(const std::string& textBefore, const std::string& textAfter);
    virtual const std::string& getXmlName() const;
    virtual ~CommentFormat();
};

class LabelFormat : public Format
{
public:
    static const std::string XML_NAME;
    
public:
    LabelFormat();
    LabelFormat(const std::string& textBefore, const std::string& textAfter);
    virtual std::string get(const std::string& value) const;
    virtual const std::string& getXmlName() const;
    virtual void writeXml(std::ostream& output) const;
	virtual void readXml(const xml::Element& element);
	virtual ~LabelFormat();
};

class IdentifierFormat : public Format
{
public:
    static const std::string XML_NAME;
    
public:
    IdentifierFormat();
    IdentifierFormat(const std::string& textBefore, const std::string& textAfter);
    virtual const std::string& getXmlName() const;
    virtual ~IdentifierFormat();
};

class UniqueTagFormat : public Format
{ 
public:
    static const std::string XML_NAME;
    
public:
    UniqueTagFormat();
    UniqueTagFormat(const std::string& textBefore, const std::string& textAfter);
    virtual const std::string& getXmlName() const;
    virtual ~UniqueTagFormat();
};

inline const std::string& Format::getTextBefore() const
{
	return this->textBefore;
}

inline const std::string& Format::getTextAfter() const
{
	return this->textAfter;
}

inline std::string Format::get(const std::string& value) const
{
	return textBefore + value + textAfter;
}

inline UniqueTagFormat::UniqueTagFormat()
{ }

inline UniqueTagFormat::UniqueTagFormat(const std::string& textBefore, const std::string& textAfter)
	: Format(textBefore, textAfter)
{ }

inline const std::string& UniqueTagFormat::getXmlName() const
{ 
	return XML_NAME;
}

inline IdentifierFormat::IdentifierFormat()
{ }

inline IdentifierFormat::IdentifierFormat(const std::string& textBefore, const std::string& textAfter)
	: Format(textBefore, textAfter)
{ }

inline const std::string& IdentifierFormat::getXmlName() const
{ 
	return XML_NAME;
}

inline LabelFormat::LabelFormat()
{ }

inline LabelFormat::LabelFormat(const std::string& textBefore, const std::string& textAfter)
	: Format(textBefore, textAfter)
{ }

inline const std::string& LabelFormat::getXmlName() const
{ 
	return XML_NAME;
}

inline CommentFormat::CommentFormat()
{ }

inline CommentFormat::CommentFormat(const std::string& textBefore, const std::string& textAfter)
	: Format(textBefore, textAfter)
{ }

inline const std::string& CommentFormat::getXmlName() const
{ 
	return XML_NAME;
}

}

}

#endif
