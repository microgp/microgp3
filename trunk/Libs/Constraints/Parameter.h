/***********************************************************************\
|                                                                       |
| Parameter.h                                                           |
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

#ifndef HEADER_UGP3_CONSTRAINTS_PARAMETER
#define HEADER_UGP3_CONSTRAINTS_PARAMETER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <exception>
#include <vector>

#include "Enumeration.h"
#include "XMLIFace.h"

namespace ugp3
{
    namespace constraints
    {
        /** Represents a parameter of the expression of a macro.
            @author Aimo Alessandro, Salomone Alessandro */
        class Parameter :  public xml::XMLIFace, public IString
        {
        private:
            /** The name of the parameter used as identifier. */
            std::string name;

            /** Copy constructor. It is declared private so it's cannot be accessed.*/
            Parameter(const Parameter&);
  
        protected:
            /** If the field Parameter::typeDefinition, that refers to another parameter is not null, the xml description will be printed using this method. */
            std::string writeXmlAsTypeDefinition() const;

            /** The parameter may refer to another parameter. 
                When a parameter is an instance of a define type, 
                its field Parameter::typeDefinition points to it, 
                otherwise the field is nullptr. 
                Cloning a parameter will make the new parameter an instance 
                of the original one. */
            const Parameter* typeDefinition;

            /** Constructor. Derived classes should use this constructor to name the parameter. 
                @param name The name of the parameter. */
            Parameter(const std::string& name);
            
        public:
            static const std::string XML_NAME;
            static const std::string XML_ATTRIBUTE_REF;
            static const std::string XML_ATTRIBUTE_NAME;
            static const std::string XML_ATTRIBUTE_TYPE;

        public:
            Parameter();

            /** Gets the name of the parameter. */
            const std::string getName() const;

            /** Clones the instance of the parameter.
                @param outParameter A pointer to the new instance.
                @param name The name of the cloned parameter. */
            virtual void clone(Parameter*& outParameter, const std::string& name) = 0;

            /** Gets a string representation of the instance. */
            virtual const std::string toString() const;

            /** Destructor. */
            virtual ~Parameter();
            
 		public: // Xml interface
			virtual const std::string& getXmlName() const;

            /** Builds the object from the xml data. 
                @param element The xml element representing the object. */
            virtual void readXml(const xml::Element& element);

	    /** Gets a regular expression for this element. It's virtual, because it's implemented
	    at the level of the classes that inherit from this one.
		@return A string, containing the regular expression for this element. */
	    virtual const std::string getRegex() const = 0;

        };
    }
}

namespace ugp3
{
    namespace constraints
    {
        inline const std::string Parameter::getName() const
        {
            return this->name;
        }
        
        inline const std::string& Parameter::getXmlName() const
	{
	    return XML_NAME;
	}
    }
}

#endif
