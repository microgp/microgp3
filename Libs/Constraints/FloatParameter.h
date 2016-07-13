/***********************************************************************\
|                                                                       |
| FloatParameter.h                                                      |
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

#ifndef HEADER_UGP3_CONSTRAINTS_FLOATPARAMETER
#define HEADER_UGP3_CONSTRAINTS_FLOATPARAMETER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>
#include <string>

#include "RangedParameter.h"
#include "Convert.h"

namespace ugp3
{
	namespace constraints
	{
		/** Represents a specific parameter.
			@author Aimo Alessandro, Salomone Alessandro. */
		class FloatParameter : public RangedParameter
		{
		private: // fields
			/** The minimum valid value of this parameter. */
			double minValue;

			/** The maximum valid value of this parameter. */
			double maxValue;

		private: // constructors
			/** Copy constructor. It is declared private so it cannot be accessed.*/
			FloatParameter(const FloatParameter&);

		public:
            /** Default constructor. */
			FloatParameter();

			/** Initializes a new instance of the constraints::DoubleParameter class.
                @param name The name of the parameter.
				@param minValue The minimum valid value of this parameter.
				@param maxValue The maximum valid value of this parameter.*/
			FloatParameter(const std::string& name, double minValue, double maxValue);

			/** Gets a random valid value of the parameter.
				@return A string representing a random valid value. */
			virtual const std::string randomize() const;

			/** Tells if the value is valid for this parameter.
				@param value A string representing value to validate.
				@return True if the value is valid. */
			virtual bool validate(const std::string& value) const;

            /** Clones the instance of the parameter.
                @param outParameter A pointer to the new instance.
                @param name The name of the cloned parameter. */
			virtual void clone(Parameter*& outParameter, const std::string& name);

		/** Gets the regex representation for this parameter
		    @return String containing a regex expression. */
		virtual const std::string getRegex() const;

            /** Serializes the instance to an xml stream.
                @param output The stream on which data will be output. */
			virtual void writeXml(std::ostream& output) const;

            /** Builds the object from the xml data.
                @param element The xml element representing the object. */
            virtual void readXml(const xml::Element& element);

                        virtual std::string getMin() const;
			virtual std::string getMax() const ;

                        double getMinimum() const;
			double getMaximum() const ;

			virtual std::string getAt(double rangePercentage) const;
			virtual double getPosition(const std::string& value) const;
		};

		inline std::string FloatParameter::getMin() const
		{
			return Convert::toString(this->minValue);
		}

		inline std::string FloatParameter::getMax() const
		{
			return Convert::toString(this->maxValue);
		}

                inline double FloatParameter::getMaximum() const
      	        {
                return this->maxValue;
                }

               inline double FloatParameter::getMinimum() const
               {
               return this->minValue;
               }
	}
}

#endif
