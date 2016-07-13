/***********************************************************************\
|                                                                       |
| SelfRefParameter.h                                                    |
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

#ifndef HEADER_UGP3_CONSTRAINTS_SELFREFPARAMETER
#define HEADER_UGP3_CONSTRAINTS_SELFREFPARAMETER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Parameter.h"

namespace ugp3
{
	namespace constraints
	{
		/** A parameter that represents a reference to the instance of the macro in which the parameter is defined.
			@author Aimo Alessandro. */
		class SelfRefParameter : public StructuralParameter
		{
		private:
			/** Copy constructor. It is declared private so it's cannot be accessed.*/
			SelfRefParameter(const SelfRefParameter&);

		public:
            /** Default constructor. It is declared private so it's cannot be accessed.*/
			SelfRefParameter();

			/** Initializes a new instance of the constraints::SelfRefParameter class. */
			SelfRefParameter(const std::string& name);

		/** Gets the regex representation for this parameter
		    @return String containing a regex expression. */
		virtual const std::string getRegex() const;

            /** Serializes the instance to an xml stream. 
                 @param output The stream on which data will be output. */
			virtual void writeXml(std::ostream& output) const;

            /** Clones the instance of the parameter.
                @param outParameter A pointer to the new instance.
                @param name The name of the cloned parameter. */
			virtual void clone(Parameter*& outParameter, const std::string& name);

			/** Destructor.*/
			virtual ~SelfRefParameter();
		};
	}
}

#endif
