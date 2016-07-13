/***********************************************************************\
|                                                                       |
| Constraints.h                                                         |
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

#ifndef HEADER_UGP3_CONSTRAINTS_CONSTRAINTS
#define HEADER_UGP3_CONSTRAINTS_CONSTRAINTS

// standard headers
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <limits>
#include <ostream>
#include <memory>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// external headers
#include "Log.h"
#include "Random.h"
#include "Exception.h"
#include "IContainer.h"
#include "IValidable.h"
#include "XMLIFace.h"
#include "Utility.h"
#include "StackTrace.h"
#include "Convert.h"
#include "Base.h"
#include "Exceptions/SchemaException.h"
#include "Exceptions/ArgumentOutOfRangeException.h"
#include "Exceptions/NullReferenceException.h"
#include "Exceptions/IndexOutOfBoundsException.h"

// module headers
#include "BitArrayParameter.h"
#include "CombinatorialParameter.h"
#include "ConstantParameter.h"
#include "ConstrainingElement.h"
#include "DataParameter.h"
#include "EnvironmentParameter.h"
#include "Epilogue.h"
#include "Expression.h"
#include "ExpressionElement.h"
#include "FloatParameter.h"
#include "GenericMacro.h"
#include "GenericSection.h"
#include "InnerLabelParameter.h"
#include "IntegerParameter.h"
#include "IValidator.h"
#include "Macro.h"
#include "OuterLabelParameter.h"
#include "Parameter.h"
#include "Prologue.h"
#include "RangedParameter.h"
#include "Section.h"
#include "SelfRefParameter.h"
#include "StructuralParameter.h"
#include "SubSection.h"
#include "UniqueTagParameter.h"
// TODO add here new types of parameters
//#include "MacroParameter.h"


namespace ugp3
{
    namespace constraints
    {
        // forward declarations
        class Section;
        class GenericMacro;

        /** Represents the constraints on the coding of a problem.
            @author Aimo Alessandro, Salomone Alessandro. */
        class Constraints : public GenericSection, public IContainer<Section>
        {
        private:
            /** The sections contained in the constraints. */
            std::vector<Section*> sections;

            /** The type definitions contained in the constraints. */
            std::vector<Parameter*>* typedefs;
            
            std::string filePath;

            /** The copy constructor: defined here as private so that it cannot be invoked. */
            Constraints(const Constraints&);

            void parseTypeDefinitions(const xml::Element& element);
            void parseSections(const xml::Element& element);
            void clear();
        
       
        private:
            static const std::string XML_NAMESPACE;
            
        public:
            static const std::string XML_NAME;
            
        public:
            /** Creates a new constraints definition from the specified xml element. 
                @param element A valid pointer to an xml element instance representing an instruction library. */
            Constraints(xml::Element* element);
            
            Constraints(const std::string& name);

            /** Creates an empty constraints definition. */
            Constraints();

            /** Releases the resources associated to the class instance. */
            virtual ~Constraints();

            /** Creates a new constraints definition from the specified xml file.*/
            static std::unique_ptr<Constraints> fromFile(const std::string& fileName);

            /** Serializes the instance of the instruction library on the specified file.
                @param xmlFile A std::string containing the path and the name of the target file. */
            void save(const std::string& xmlFile);

            /** Removes the specified section from the library.
                @param index The index of the section to remove. */
            void removeSection(Section* section);

            /** Removes the specified section from the library.
                @param index The index of the section to remove. */
            void removeSection(unsigned int index);

            /** Adds a new section to the library.
                @param section A pointer to a valid constraints::Section instance. */
            void addSection(Section* section);

            /** Gets the section at the specified index.
                @param index The index at which the section is to be found. 
                @return A pointer to the requested constraints::Section instance. */
            Section& getSection(unsigned int index) const;

            /** Gets the section with the specified identifier.
                @param identifier The identifier of the section to be retrieved. 
                @return A pointer to the requested constraints::Section instance or nullptr if the section does not exist. */
            const Section* getSection(const std::string& identifier) const;

            /** Gets the section at the specified index.
                @param index The index at which the section is to be found. 
                @return A pointer to the requested constraints::Section instance. */
            Section* operator[](unsigned int index) const;

            /** Gets the number of sections in the library.
                @return An unsigned integer that describes the number of sections currently available in the constraints. */
            unsigned int getSectionCount() const;
            
            /** Gets a collection of type definitions. 
                @return A valid std::vector pointer of the user defined types of the constraints. */
            const std::vector<Parameter*>* getTypeDefs() const;

            /** Tells wether the specified macro is contained in the constraints. 
                @param macro The macro to be found. 
                @return True if the macro is contained in the constraints, false otherwise. */
            virtual bool contains(const GenericMacro& macro) const;

            /** Tells wether the specified section is contained in the constraints. 
                @param section The section to be found. 
                @return True if the section is contained in the constraints, false otherwise. */
            virtual bool contains(const Section& section) const;

            /** Searches for a macro in the specified path.
                @param path A path in the form "constraints_id/macro_id" or "constraints_id/section_id/macro_id" or "constraints_id/section_id/subsection_id/macro_id". 
                @return A pointer to the requested macro or nullptr if the macro does not exist. */
            const GenericMacro* getMacroByPath(const std::string& path) const;
            
            const std::string& getFilePath() const;

            /** Validates the instance. 
                @return True if the instance is valid, false otherwise. */
            bool validate() const;
            
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
        inline unsigned int Constraints::getSectionCount() const
        {
            return (unsigned int)this->sections.size();
        }
        
        inline const std::string& Constraints::getXmlName() const
		{
			return XML_NAME;
		}
		
		inline const std::string& Constraints::getFilePath() const
		{
			return this->filePath;
		}
    }
}

#endif
