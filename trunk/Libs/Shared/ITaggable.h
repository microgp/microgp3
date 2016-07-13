/***********************************************************************\
|                                                                       |
| ITaggable.h                                                           |
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
#ifndef HEADER_UGP3_ITAGGABLE
#define HEADER_UGP3_ITAGGABLE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <map>
#include <string>

#include "Tag.h"
#include "IContainer.h"

namespace ugp3
{
    /** Represents a taggable object.
        @author Alessandro Aimo, Luca Motta, Alessandro Salomone. */
    class ITaggable : IContainer<Tag>
    {
    public:
        /// Virtual Distructor. Delete an instance of this class.
        virtual ~ITaggable();

        /** Adds a Tag to the internal tag list.
            @param tag The Tag to be added. */
        virtual bool addTag(Tag* tag) = 0;

        /** Adds new Tag to the internal tag list.
            @name name The name of the tag.
            @value value The value of the tag. */
        virtual void addTag(const std::string& name, const std::string& value) = 0;
       
        /** Removes a Tag from the list.
            @param name The name of the Tag to be removed */
        virtual bool removeTag(const std::string& name) = 0;
     
        /** Returns the pointer of the specified Tag
            @param name The name of the Tag to be retrieved. */
        virtual Tag& getTag(const std::string& name) const = 0;

        /** Returns an indexed Tag.
            @param index The index of the Tag to be retrieved */
        virtual Tag* getTag(unsigned int index) const = 0;
        
        /** Tells if a tag with the specified name exists in this container. */
        virtual bool containsTag(const std::string& name) const = 0;

        /* Gets the number of tags in this instance. */
        virtual unsigned int getTagCount() const = 0;
    };

    inline ITaggable::~ITaggable()
    { }
}

#endif
