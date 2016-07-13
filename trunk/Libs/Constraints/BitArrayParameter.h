/***********************************************************************\
|                                                                       |
| BitArrayParameter.h                                                   |
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

#ifndef HEADER_UGP3_CONSTRAINTS_BITARRAYPARAMETER
#define HEADER_UGP3_CONSTRAINTS_BITARRAYPARAMETER

#include <vector>
#include <string>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "DataParameter.h"
#include "Base.h"

namespace ugp3
{
	namespace constraints
	{
		/** Represents a string of bits.
			@author Aimo Alessandro, Salomone Alessandro. */
		class BitArrayParameter : public DataParameter
		{
		private: // fields
		    std::string pattern;
		    Base base;
            bool initNull = false;

		private: // constructors
			/** Copy constructor. It is declared private so it cannot be accessed.*/
			BitArrayParameter(const BitArrayParameter&);

            char getRandomBit() const;
            
        private:
            static const std::string XML_ATTRIBUTE_PATTERN;
            static const std::string XML_ATTRIBUTE_BASE;
            static const std::string XML_ATTRIBUTE_INIT;


		public:
            /** Default constructor. */
            BitArrayParameter();

			/** Initializes a new instance of the constraints::BitArrayParameter class. 
                @param name The name of the parameter.
                @param mask An bit patter that determines the form of the array.
                @param base The base used to represent the array. */
            BitArrayParameter(const std::string& name, const std::string& pattern, const Base& base = Base::Binary);
			
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
			
			const std::string& getPattern() const;

		/** Gets the base for this bit array; it can be hexadecimal, octal or binary.
		    @return Base::Hexadecimal, Base::Octal or Base::Binary constant. */
			const Base getBase() const;
		
		/** Gets the regex representation for this parameter
		    @return String containing a regex expression. */
		virtual const std::string getRegex() const;
			
            virtual ~BitArrayParameter();
            
		public: // Xml interface
		    /** Serializes the instance to an xml stream. 
			@param output The stream on which data will be output. */
			virtual void writeXml(std::ostream& output) const;

		    /** Builds the object from the xml data. 
			@param element The xml element representing the object. */
			virtual void readXml(const xml::Element& element);
		
		
		};

		
		inline const std::string& BitArrayParameter::getPattern() const
		{
		    return this->pattern;
		}
		
		inline const Base BitArrayParameter::getBase() const
		{
		    return this->base;
		}
	}
}

#endif
