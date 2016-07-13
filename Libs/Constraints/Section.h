/***********************************************************************\
|                                                                       |
| Section.h                                                             |
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

#ifndef HEADER_UGP3_CONSTRAINTS_SECTION
#define HEADER_UGP3_CONSTRAINTS_SECTION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>
#include <string>
#include <sstream>
#include <exception>

#include "IContainer.h"
#include "SubSection.h"
#include "XMLIFace.h"

namespace ugp3
{
    namespace constraints
    {
        // forward declarations
        class Constraints;
        class SubSection;
        class GenericSection;
        class GenericMacro;

        /** Represents a section of the instruction library.
            @author Aimo Alessandro, Salomone Alessandro */
        class Section : public GenericSection, public IContainer<SubSection>
        {
        private:
            /** The collection of subsections contained in the section. */
            std::vector<SubSection*> subSections;

            /** Tells wether the prologue and epilogue of the section should necessarily appear or not in a program instance (even when the section instance does not contain any sub-section instance) */
            bool isCompulsoryPrologueEpilogue;

            /** Default constructor. It is declared private so it's cannot be accessed.*/
            Section();

        private:
            /** The copy constructor: defined here as private so that it cannot be invoked. */
            Section(const Section&);


            void parseSubSections(const xml::Element& element);
        
        public:
            static const std::string XML_NAME;
            
        public:
            /** Initializes a new instance of the constraints::Section class from the specified xml element. 
                @param constraints The constraints to which the section refers. */
            Section(Constraints* constraints);

            /** Initializes a new instance of the constraints::Section class. 
                @param instructionLibrary The instruction library to which the section refers.
                @param name A label that describes the section's role in the program.
                @param isCompulsoryPrologueEpilogue Tells wether the prologue and epilogue of the section should necessarily appear or not in a program instance. */
            Section(Constraints* constraints, const std::string& name, bool isCompulsoryPrologueEpilogue);

            /** Releases the resources associated to the class instance. */
            virtual ~Section();

            /** Tells wether the prologue and epilogue of the section should necessarily appear or not in a program instance. 
                @return True if the prologue and epilogue are compulsory, false otherwise.*/
            bool getIsCompulsoryPrologueEpilogue() const;

            /** Removes the specified sub-section from the section.
                @param subSection The sub-section to remove. */
            void removeSubSection(SubSection* subSection);

            /** Removes the specified sub-section from the section.
                @param index The index of the sub-section to remove. */
            void removeSubSection(unsigned int index);

            /** Adds a new sub-section to the constraints.
                @param subSection A pointer to a valid constraints::SubSection instance. */
            void addSubSection(SubSection* subSection);

            /** Searches for a macro in the specified path.
                @param path A path in the form "section_id/macro_id" or "section_id/subsection_id/macro_id". 
                @return A pointer to the requested macro or nullptr if the macro does not exist. */
            const GenericMacro* getMacroByPath(const std::string& path) const;

            /** Gets the sub-section at the specified index.
                @param index The index at which the sub-section is to be found. 
                @return A pointer to the requested constraints::SubSection instance. */
            SubSection& getSubSection(unsigned int index) const;

            /** Gets the subsection with the specified identifier.
                @param identifier The identifier of the subsection to be retrieved. 
                @return A pointer to the requested SubSection instance or nullptr if the subsection does not exist. */
            SubSection* getSubSection(const std::string& name) const;

            /** Gets the macro with the specified identifier.
                @param identifier The identifier of the macro to be retrieved. 
                @return A pointer to the requested macro instance or nullptr if the macro does not exist. */
            GenericMacro* getMacro(const std::string& id) const;

            /** Gets the sub-section at the specified index.
                @param index The index at which the sub-section is to be found. 
                @return A pointer to the requested constraints::SubSection instance. */
            SubSection* operator[](unsigned int index) const;

            /** Gets the number of sub-sections in the section.
                @return An unsigned integer indicating the number of sub-sections currently available in the section. */
            unsigned int getSubSectionCount() const;

            /** Tells wether the specified macro is contained in the section. 
                @param macro The macro to be found. 
                @return True if the macro is contained in the section, false otherwise. */
            virtual bool contains(const GenericMacro& macro) const;

            /** Tells wether the specified sub-section is contained in the section. 
                @param subSection The sub-section to be found. 
                @return True if the sub-section is contained in the section, false otherwise. */
            virtual bool contains(const SubSection& subSection) const;

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
        inline bool Section::getIsCompulsoryPrologueEpilogue() const
        {
            return this->isCompulsoryPrologueEpilogue;
        }

        inline unsigned int Section::getSubSectionCount() const
        {
            return (unsigned int)this->subSections.size();
        }
        
        inline const std::string& Section::getXmlName() const
		{
			return XML_NAME;
		}
    }
}

#endif
