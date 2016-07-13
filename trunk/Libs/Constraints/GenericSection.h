/***********************************************************************\
|                                                                       |
| GenericSection.h                                                      |
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

#ifndef HEADER_UGP3_CONSTRAINTS_GENERICSECTION
#define HEADER_UGP3_CONSTRAINTS_GENERICSECTION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <exception>

#include "ConstrainingElement.h"
#include "Utility.h"
#include "IValidable.h"

namespace ugp3
{
	namespace constraints
	{
		class Prologue;
		class Epilogue;
		class Constraints;

		/** Represents a generic section, a container for macros and other sections.
			@author Aimo Alessandro, Salomone Alessandro */
        class GenericSection : public ConstrainingElement, public IContainer<GenericMacro>, public IValidable
		{
		private:
			const Constraints* getConstraints() const;

			/** The copy constructor: it is declared as private so that it cannot be accessed. */
			GenericSection(const GenericSection&);

			/** Default constructor. It is declared private so it's cannot be accessed.*/
			GenericSection();

            void clear();

		protected:
			/** The prologue of the section. */
			Prologue* prologue;

			/** The epilogue of the section. */
			Epilogue* epilogue;

		public:
			/** Initializes a new instance of the constraints::GenericSection class from the given xml element.
				@param element A valid pointer to an xml element instance representing a generic section.
				@param typedefs A reference to a collection of type definitions to be filled with the definitions contained in the xml node. */

			/** Initializes a new instance of the constraints::GenericSection class.
				@param parent The parent container of this generic section.
				@param element A valid pointer to an xml element instance representing a generic section. */
			GenericSection(const GenericSection* parent);

			/** Initializes a new instance of the constraints::GenericSection class.
				@param parent The parent container of this generic section.
                @param id The idendifier of the instance. */
			GenericSection(const GenericSection* parent, const std::string& id);

			/** Gets the prologue of the generic section.
				@return A valid pointer to an instance of thie constraints::Prologue class. */
			Prologue& getPrologue() const;

			/** Gets the epilogue of the generic section.
				@return A valid pointer to an instance of thie constraints::Epilogue class. */
			Epilogue& getEpilogue() const;

			/** Sets the prologue of the generic section. It cannot be null.
				@param prologue A valid pointer to an instance of thie constraints::Prologue class. */
			void setPrologue(Prologue& prologue);

			/** Sets the epilogue of the generic section. It cannot be null.
				@param epilogue A valid pointer to an instance of thie constraints::Epilogue class. */
			void setEpilogue(Epilogue& epilogue);

            /** Serializes the instance to an xml stream.
                @param output The stream on which data will be output. */
            virtual void writeXml(std::ostream& output) const = 0;

            /** Builds the object from the xml data.
                @param element The xml element representing the object. */
            virtual void readXml(const xml::Element& element);

			/** Frees the resources associated to the instance. */
			virtual ~GenericSection();
		};
	}
}

#endif
