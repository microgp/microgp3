/***********************************************************************\
|                                                                       |
| Epilogue.h                                                            |
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

#ifndef HEADER_UGP3_CONSTRAINTS_EPILOGUE
#define HEADER_UGP3_CONSTRAINTS_EPILOGUE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <exception>
#include <vector>

#include "GenericMacro.h"
#include "XMLIFace.h"

namespace ugp3
{
    namespace constraints
    {
        // forward declarations
        class GenericSection;
        class Expression;
        class Parameter;
        class ExpressionElement;

        /** Represents the last macro of a generic section.
            @author Aimo Alessandro, Salomone Alessandro */
        class Epilogue : public GenericMacro
        {
        private:
            /** The copy constructor: declared as private so that it cannot be accessed. */
            Epilogue(const Epilogue&);

            /** Default constructor. It is declared private so it's cannot be accessed.*/
            Epilogue();

        public:
            static const std::string XML_NAME;

        public:
            /** Initializes a new instance of the constraints::Prologue class.
                @param parent The generic section containing the prologue.
                @param id An identifier for the instance.*/
            Epilogue(GenericSection& parent, const std::string& id);

            /** Initializes a new instance of the constraints::Prologue class from the specified xml element.
                @param parent The generic section containing the prologue.
                @param element A valid pointer to an xml element instance representing an epilogue. */
            Epilogue(GenericSection& parent);

            /** Initializes a new instance of the constraints::Prologue class.
                @param parent The generic section containing the prologue.
                @param expressionElements An ordered collection of expression elements that describe the expression of the prologue.
                @param parameters The collection of parameters of the prologue. */
            Epilogue(GenericSection& parent, const std::string& id, std::vector<ExpressionElement*>* expressionElements, std::vector<Parameter*>* parameters);

            /** Releases the resources associated to the class instance. */
            virtual ~Epilogue();

		public: // Xml interface
			virtual const std::string& getXmlName() const;
        };
    }
}

namespace ugp3
{
	namespace constraints
	{
		inline const std::string& Epilogue::getXmlName() const
		{
			return XML_NAME;
		}
	}
}

#endif
