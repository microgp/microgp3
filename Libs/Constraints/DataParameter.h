/***********************************************************************\
|                                                                       |
| DataParameter.h                                                       |
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

#ifndef HEADER_UGP3_CONSTRAINTS_DATAPARAMETER
#define HEADER_UGP3_CONSTRAINTS_DATAPARAMETER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Enumeration.h"
#include "Parameter.h"
#include "IValidator.h"

namespace ugp3
{
    namespace constraints
    {
        /** Represents a specific parameter.
            @author Aimo Alessandro, Salomone Alessandro */
        class DataParameter : public Parameter, public IValidator<std::string>
        {
        private:
            /** Copy constructor. It is declared private so it cannot be accessed.*/
            DataParameter(const DataParameter&);

            
        protected:
            /** Constructor. Initializes a new instance of the constraints::DataParameter class. */
            DataParameter(const std::string& name);

        public:
            /** Default constructor. */
            DataParameter();

            /** Gets a random valid value.
                @return A string representing a random valid value. */
            virtual const std::string randomize() const = 0;

            /** Tells if the value is valid for this parameter.
                @param value A string representing value to validate.
                @return True if the value is valid. */
            virtual bool validate(const std::string& value) const= 0;

           

            /** Clones the instance of the parameter.
                @param outParameter A pointer to the new instance.
                @param name The name of the cloned parameter. */
            virtual void clone(Parameter*& outParameter, const std::string& name) = 0;

            /** Destructor. */
            virtual ~DataParameter();
            
        public: // Xml interface 
             /** Serializes the instance to an xml stream. 
                @param output The stream on which data will be output. */
            virtual void writeXml(std::ostream& output) const = 0;
        };
    }
}

#endif
