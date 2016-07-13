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

#ifndef HEADER_UGP3_CONSTRAINTS_ENVIRONMENTPARAMETER
#define HEADER_UGP3_CONSTRAINTS_ENVIRONMENTPARAMETER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>
#include <string>

#include "DataParameter.h"

namespace ugp3
{
    namespace constraints
    {
        /** Represents a parameter whose value is given by an environment variable.
            @author Aimo Alessandro, Salomone Alessandro */
        class EnvironmentParameter : public DataParameter
        {
        private: // fields
            std::string environmentVariableName;

        private: // constructors
            /** Copy constructor. It is declared private so it's cannot be accessed.*/
            EnvironmentParameter(const EnvironmentParameter&);
        
        protected:
            static const std::string XML_ATTRIBUTE_VARIABLE;
            
        public:
            /** Default constructor. */
            EnvironmentParameter();

            /** Constructor. Initializes a new instance of the constraints::EnvironmentParameter class. 
                @param name The name of the parameter.
                @param constants The name of the environment variable associated to this parameter. */
            EnvironmentParameter(const std::string& name, const std::string& environmentVariableName);

            /** Gets a valid value for the parameter. Actually, it always returns the value of the environment variable.
                @return A string representing a random valid value. */
            virtual const std::string randomize() const;

            /** Tells if the value is valid for this parameter.
                @param value A string representing value to validate.
                @return True if the value is valid. */
            virtual bool validate(const std::string& value) const;

            /** Serializes the instance to an xml stream. 
                @param output The stream on which data will be output. */
            virtual void writeXml(std::ostream& output) const;

            /** Builds the object from the xml data. 
                @param element The xml element representing the object. */
            virtual void readXml(const xml::Element& element);

            /** Clones the instance of the parameter.
                @param outParameter A pointer to the new instance.
                @param name The name of the cloned parameter. */
            virtual void clone(Parameter*& outParameter, const std::string& name);

		/** Gets the regex representation for this parameter
		    @return String containing a regex expression. */
		virtual const std::string getRegex() const;

            /** Destructor. */
            virtual ~EnvironmentParameter();
        };
    }
}

#endif
