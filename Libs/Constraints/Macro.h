/***********************************************************************\
|                                                                       |
| Macro.h                                                               |
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

#ifndef HEADER_UGP3_CONSTRAINTS_MACRO
#define HEADER_UGP3_CONSTRAINTS_MACRO

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>
#include <string>
#include <sstream>

#include "GenericMacro.h"
#include "Parameter.h"
#include "XMLIFace.h"
#include "IValidable.h"

namespace ugp3
{
	namespace constraints
	{
		// forward declaration
		class SubSection;

		/** Represents the core class of the constraints.
			@author Aimo Alessandro, Salomone Alessandro */
		class Macro : public GenericMacro, public IValidable
		{
		private:
			/** The probability of the occurrence of the macro after the initial generation stage of a program. */
			double weight;

			/** The copy constructor: it is declared as private so that it cannot be accessed. */
			Macro(const Macro&);

			/** Default constructor. It is declared private so it's cannot be accessed.*/
			Macro();
			
			void clear();

			static const std::string XML_NAME;
		public:
			/** Initializes a new instance of the constraints::Macro class. 
				@param parent The sub-section that contains the macro. */
			Macro(SubSection& parent, const std::string& id);

			/** Initializes a new instance of the constraints::Macro class. 
				@param parent The sub-section that contains the macro.
				@param element A valid pointer to an xml element instance representing a macro. */
			Macro(SubSection& parent);

			/** Initializes a new instance of the constraints::Macro class. 
				@param parent The sub-section that contains the macro.
                @param id The identifier of the macro.
				@param elements An ordered collection of expression elements that describe the expression of the macro.
				@param parameters The collection of parameters of the macro. */
			Macro(SubSection& parent, const std::string& id, std::vector<ExpressionElement*> *elements, std::vector<Parameter*>* parameters);

			/** Releases the resources associated to the class instance. */
			virtual ~Macro();

            /** Gets the parent container of the macro. Since not all the macros are directly contained in a sub-section, the method may return nullptr. */
			const SubSection* getSubSection() const;

            /** Validates the instance. 
                @return True if the instance is valid, false otherwise. */
            virtual bool validate() const;

			/** Gets the probability of the occurrance of the macro after the initial generation stage of a program.
				@return A valure representing the probability. */
			double getWeight() const;

			/** Sets the probability of the macro.
				@param probability A floating point number representing the probability of the macro .*/
			void setWeight(double weight);

		public: // Xml interface
			virtual void writeXml(std::ostream& output) const;
			virtual void readXml(const xml::Element& element);
			virtual const std::string& getXmlName() const;
		};
	}
}

namespace ugp3
{
	namespace constraints
	{		
		inline double Macro::getWeight() const
		{
			return this->weight;
		}
		
		inline const std::string& Macro::getXmlName() const
		{
			return XML_NAME;
		}
	}
}

#endif
