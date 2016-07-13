/***********************************************************************\
|                                                                       |
| Expression.h                                                          |
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

#ifndef HEADER_UGP3_CONSTRAINTS_EXPRESSION
#define HEADER_UGP3_CONSTRAINTS_EXPRESSION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <vector>

#include "Utility.h"
#include "XMLIFace.h"

namespace ugp3
{
    namespace constraints
    {
        // forward declaration
        class ExpressionElement;
        class ConstrainingElement;
        class GenericMacro;

        /** Represents the parametric code of a macro.
            @author Aimo Alessandro, Salomone Alessandro. */
        class Expression : public xml::XMLIFace
        {
        private: // Fields
            /** The elements of the expression. */
            std::vector<ExpressionElement*> elements;

            /** The macro containing this expression. */
            const GenericMacro *parent;
            
            void clear();
            static const std::string XML_NAME;

        public: // Constructors and destructors
            Expression(const GenericMacro *parent);

            /** Initializes a new instance of the constraints::Expression class. 
                @param parent The macro containing this expression.
                @param elements The elements of the expression.*/
            Expression(const GenericMacro *parent, const std::vector<ExpressionElement*>* elements);

			/** Destructor.*/
            ~Expression();
       
       public: // Getters and setters
 			/** Gets The elements of the expression. 
                @return The elements of the expression. */
            const std::vector<ExpressionElement*>& getElements() const;
		
		/** Gets the regex for the whole expression, building on the regex for the parameters and fixed parts.
		    @return A const string containing the regex for the expression.*/
		const std::string getRegex() const;
 
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
        inline const std::vector<ExpressionElement*>& Expression::getElements() const
        {
            return elements;
        }
        
        inline const std::string& Expression::getXmlName() const
		{
			return XML_NAME;
		}
    }
}

#endif
