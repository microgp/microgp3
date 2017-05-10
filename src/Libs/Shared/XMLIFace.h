/***********************************************************************\
|                                                                       |
| XMLIFace.h                                                            |
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

/**
 * @file XMLIFace.h
 * Definition of the XMLIFace class.
 * @see XMLIFace.cc
 */

#ifndef HEADER_XML_XMLIFACE
/** Defines that this file has been included */
#define HEADER_XML_XMLIFACE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ostream>
#include <string>
#include <exception>

#include "Utility.h"

/**
 * xml namespace
 */
namespace xml
{
    /**
     * @class XMLIFace
     * Abstract class; interface to write and read xml elements. 
     */
    class XMLIFace
    {
    public:
        /**
         * Destructor of the class. By default it does nothing.
         */
        virtual ~XMLIFace();

        /**
         * Serializes the instance to an xml stream. 
         * @param output The stream on which data will be outputted.
         */
        virtual void writeXml(std::ostream& output) const = 0;
        		
	/**
         * Builds the object from the xml data. 
         * @param element The xml element representing the object.
         */
        virtual void readXml(const xml::Element& element) = 0;
        
        /**
         * Returns the name of the xml element that represent the object. 
         * @returns string Name of the xml element
         */
        virtual const std::string& getXmlName() const = 0;
    };
}


#endif
