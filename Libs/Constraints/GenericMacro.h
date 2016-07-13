/***********************************************************************\
|                                                                       |
| GenericMacro.h                                                        |
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

#ifndef HEADER_UGP3_CONSTRAINTS_GENERICMACRO
#define HEADER_UGP3_CONSTRAINTS_GENERICMACRO

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <map>
#include <sstream>
#include <stdexcept>

#include "Debug.h"
#include "Parameter.h"
#include "DataParameter.h"
#include "OuterLabelParameter.h"
#include "InnerLabelParameter.h"
#include "IntegerParameter.h"
#include "FloatParameter.h"
#include "ConstantParameter.h"
#include "Expression.h"
#include "ConstrainingElement.h"
#include "UniqueTagParameter.h"
#include "SelfRefParameter.h"
#include "Exceptions/NullReferenceException.h"

namespace ugp3
{
    namespace constraints
    {
        //forward declarations
        class Expression;
        class ExpressionElement;
        class GenericSection;
        class SubSection;

        /** This is the base class of Macro, Prologue and Epilogue.
        @author Aimo Alessandro, Salomone Alessandro */
        class GenericMacro : public ConstrainingElement
        {
        protected:
            /** The vector of code parameters. */
            std::vector<Parameter*> parameters;
            std::map<std::string, Parameter*> parametersMap;

            /** The string representing the code assigned to this macro. */
            std::unique_ptr<Expression> expression;

            void parseParameters(const xml::Element& element);
            const std::string getInnerXmlDescription() const;

			void clear();
        private:
            /** Copy constructor. It is declared private so it's cannot be accessed.*/
            GenericMacro(const GenericMacro&);

            /** Default constructor. It is declared private so it's cannot be accessed.*/
            GenericMacro();


        public:
            /** Constructor. Initializes a new instance of the constraints::GenericMacro class. 
                @param parent The genericSection containing this expression.*/
            GenericMacro(const GenericSection& parent, const std::string& id);

            /** Constructor. Initializes a new instance of the constraints::GenericMacro class. 
                @param parent The genericSection containing this expression. */
            GenericMacro(const GenericSection& parent);

            /** Constructor. Initializes a new instance of the constraints::GenericMacro class. 
                @param parent The genericSection containing this expression.
                @param expressionElements A valid pointer to a vector of expressionElements.
                @param parameters A valid pointer to a vector of parameters. */
            GenericMacro(const GenericSection& parent, const std::string& id, std::vector<ExpressionElement*> *expressionElements, std::vector<Parameter*>* parameters);

            /** Gets a specific code parameter.
                @param index A number representing the index of the desired parameter.
                @return A pointer to an constraints::Parameter instance.*/	
            Parameter& getParameter(unsigned int index) const;

            /** Gets a parameter.
                @param name The name of the parameter to be retrieved.
                @return A pointer to an constraints::Parameter instance.*/	
            Parameter* getParameter(const std::string& name) const;

            /** Gets the number of parameters contained in this macro.
                @return The number of parameters contained in this macro.*/
            unsigned int getParameterCount() const;

            /** Gets a specific code parameter.
                @param index A number representing the index of a specific parameter.
                @return A pointer to an constraints::Parameter instance.*/
            const Parameter* operator[](unsigned int index) const;

            /** Gets a string representing the full path from the constraints to this macro. */
            const std::string getPath() const;

            /** Gets the code expression represented by this macro.
                @return A valid constraints::Expression pointer.*/
            const Expression& getExpression() const;

			virtual void readXml(const xml::Element& element);
			virtual void writeXml(std::ostream& output) const;
			
            /** Destructor.*/
            virtual ~GenericMacro();
        };
    }
}
namespace ugp3
{
    namespace constraints
    {
        inline const Expression& GenericMacro::getExpression() const
        {
        	Assert(this->expression.get() != nullptr);
        	
            return *this->expression.get();
        }

        inline unsigned int GenericMacro::getParameterCount() const
        {
            return (unsigned int)this->parametersMap.size();
        }

        inline Parameter* GenericMacro::getParameter(const std::string& name) const
        {
            std::map<std::string, Parameter*>::const_iterator iterator = this->parametersMap.find(name);

            return iterator != this->parametersMap.end()? iterator->second : nullptr;
        }
    }
}

#endif
