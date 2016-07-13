/***********************************************************************\
|                                                                       |
| RangedParameter.h                                                     |
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

#ifndef HEADER_UGP3_CONSTRAINTS_RANGEDPARAMETER
#define HEADER_UGP3_CONSTRAINTS_RANGEDPARAMETER

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
		/** Represents a specific parameter.
			@author Aimo Alessandro, Salomone Alessandro. */
		class RangedParameter : public DataParameter
		{
		private: // constructors
			/** Copy constructor. It is declared private so it cannot be accessed.*/
			RangedParameter(const RangedParameter&);

		public:
            /** Default constructor. */
			RangedParameter();

			/** Initializes a new instance of the constraints::DoubleParameter class. 
                @param name The name of the parameter. */
			RangedParameter(const std::string& name);
			
			virtual std::string getMin() const = 0;
			virtual std::string getMax() const = 0;
			
			virtual std::string getAt(double rangePercentage) const = 0;
			virtual double getPosition(const std::string& value) const = 0;
		};
		
		inline RangedParameter::RangedParameter(const std::string& name)
			: DataParameter(name)
		{ }
		
		inline RangedParameter::RangedParameter()
			: DataParameter()
		{ }
	}
}

#endif
