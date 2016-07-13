/***********************************************************************\
|                                                                       |
| Tag.h                                                                 |
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
#ifndef HEADER_TAG
#define HEADER_TAG

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "string.h"
//
#include <string>
#include "Log.h"
#include "StackTrace.h"
#include "IString.h"
#include "XMLIFace.h"
#include "Utility.h"
#include "IEquatable.h"
#include <cmath>

namespace ugp3
{

/** Represents a tag used to store information.
   @author Luca Motta, Alessandro Salomone*/
class Tag : public xml::XMLIFace, public IString, public IEquatable<Tag>
{
private: // fields
    /** The name of the tag */
    std::string name;

    /** The value of the tag */
    std::string value;

    static const std::string XML_NAME;

    void clear();

private: // methods
    /** private copy constructor. It should never be used for the Tag class. */
    Tag( const Tag& );


public:
    /** Creates a Tag with the specified name and value.
        @param name The name associated with the Tag
        @param value The string associated with the Tag */
    Tag(const std::string& name, const std::string& value);
	Tag();

    /** Destroys the instance. */
    virtual ~Tag();

public: // getters and setters
    /** Gets the name of the tag. */
    const std::string& getName() const;
    /** Gets the value of the tag. */
    const std::string& getValue() const;

    /** Sets the value of the tag.
        @param value The value to be set. */
    void setValue(const std::string& value);

public: // IString interface
    virtual const std::string toString() const;
	virtual const std::string toString(double quantum) const;

public: // IEquatable<Tag> interface
    virtual bool equals(const Tag& tag) const;

public: // Xml interface
	virtual void writeXml(std::ostream& output) const;
	virtual void readXml(const xml::Element& element);
	virtual const std::string& getXmlName() const;

public: // Overloaded operators
	/** Compares two tags. This is useful when a map is used to store a set of tags.
	    Sorting is done on the name of the tag.*/
	bool operator<( const Tag& ) const;
};

inline Tag::Tag()
{ }

inline const std::string& Tag::getName() const
{
    return this->name;
}

inline void Tag::setValue(const std::string& value)
{
    this->value = value;
}

inline const std::string& Tag::getValue() const
{
    return this->value;
}

inline const std::string& Tag::getXmlName() const
{
	return XML_NAME;
}

inline bool Tag::operator<(const Tag& tag) const
{
    return this->name < tag.name;
}

inline Tag::~Tag()
{
    _STACK;

#ifndef NDEBUG
    LOG_DEBUG << "Destructor: Tag " << this << "." << std::ends;
#endif
}

}

#endif

