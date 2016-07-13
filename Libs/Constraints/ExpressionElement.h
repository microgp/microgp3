/***********************************************************************\
|                                                                       |
| ExpressionElement.h                                                   |
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

#ifndef HEADER_UGP3_CONSTRAINTS_EXPRESSIONELEMENT
#define HEADER_UGP3_CONSTRAINTS_EXPRESSIONELEMENT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>

#include "Parameter.h"

namespace ugp3
{
	namespace constraints
	{
		/** Represents an element of the parametric code of a macro.
			@author Aimo Alessandro, Salomone Alessandro. */
		class ExpressionElement
		{
		private:
			/** If it's not null this element is a parameter. */
			const Parameter* parameter;

			/** If parameter is null this element is a string. */
			const std::string* characters;

			/** Copy constructor. It is declared private so it's cannot be accessed.*/
			ExpressionElement(const ExpressionElement&);

			/** Default constructor. It is declared private so it's cannot be accessed.*/
			ExpressionElement();

		public:
			/** Constructor. Initializes a new instance of the constraints::ExpressionElement class. 
				@param parameter This expressionElement is a parameter.*/
			ExpressionElement(const Parameter& parameter);

			/** Constructor. Initializes a new instance of the constraints::ExpressionElement class. 
				@param characters This expressionElement is a string.*/
			ExpressionElement(const std::string& characters);

			/** Gets a string representing this element. 
				@return A string representing this element. */
			const std::string* getCharacters() const; 

			/** Gets a parameter representing this element. 
				@return A parameter representing this element. */
			const Parameter* getParameter()  const;
			
			/** Gets a regular expression, representing this element.
				@return A string containing a regex for this element. */
			const std::string getRegex() const;

			/** Destructor.*/
			~ExpressionElement();
		};
	}
}

namespace ugp3
{
	namespace constraints
	{
		inline const std::string* ExpressionElement::getCharacters() const
		{
			return this->characters;
		}

		inline const Parameter* ExpressionElement::getParameter() const
		{
			return this->parameter;
		}
	}
}

#endif
