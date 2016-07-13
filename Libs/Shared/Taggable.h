/***********************************************************************\
|                                                                       |
| Taggable.h                                                            |
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
#ifndef HEADER_UGP3_TAGGABLE
#define HEADER_UGP3_TAGGABLE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <map>
#include <string>

#include "Tag.h"
#include "ITaggable.h"
#include "XMLIFace.h"
#include "Exceptions/SchemaException.h"

namespace ugp3
{

/** Represents a taggable object.
    @author Luca Motta. */
class Taggable : public xml::XMLIFace
{
private:
    /** the vector of Tag associated to the taggable object. */
    std::map<std::string, Tag*> tags;

protected:  
    void clear();

public:
    /** Constuctor. Initializes a new instance of the taggable class. */
    Taggable();

    /// Virtual Distructor. Deletes the instance of this class.
    virtual ~Taggable();

    /** Adds a Tag to the object.
        @param tag The Tag to be added. */
    bool addTag(Tag*);

    /** Adds new Tag to the internal tag list.
        @name name The name of the tag
        @value value The value of the tag. */
    void addTag(const std::string& name, const std::string& value);
    
    /** Removes a Tag from the list.
        @param name The name of the Tag to be removed */
    bool removeTag(const std::string& name);
    
    /** Returns the pointer of the specified Tag
        @param name The name of the Tag to be retrieved. */
    Tag& getTag(const std::string& name) const;

    /** Returns an indexed Tag
        @param index The index of the Tag to be retrieved */
    Tag* getTag(unsigned int) const;
    
    bool containsTag(const std::string& tagName) const;

    bool contains(const Tag& tag) const;

    /// returns the number of Tag in this instance of ITaggable
    unsigned int getTagCount() const;

	public: // Xml interface
    virtual void writeXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
};

inline unsigned int Taggable::getTagCount() const
{
    return (unsigned int)tags.size();
}

inline bool Taggable::containsTag(const std::string& tagName) const
{
    return tags.find(tagName) != tags.end();
}

}

#endif

