/***********************************************************************\
|                                                                       |
| SubSection.h                                                          |
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

#ifndef HEADER_UGP3_CONSTRAINTS_SUBSECTION
#define HEADER_UGP3_CONSTRAINTS_SUBSECTION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <map>
#include <vector>
#include <string>
#include <exception>
#include <limits>

#include "IContainer.h"
#include "GenericSection.h"
#include "Exceptions/NullReferenceException.h"
#include "XMLIFace.h"

namespace ugp3
{
	
namespace constraints
{
	
// forward declarations
class Section;
class Macro;
class GenericMacro;

/** Represents a sub-section of the constraints.
	@author Aimo Alessandro, Salomone Alessandro */
class SubSection : public GenericSection
{
private:
    /** The collection of macros contained in the sub-section. */
    std::map<std::string, Macro*> macros;
    
    std::size_t maximumMacroInstances = std::numeric_limits<size_t>::max();
    std::size_t minimumMacroInstances = 0;
    std::size_t averageMacroInstances = 1;
    double averageSigmaMacroInstances = 1;
    std::size_t maximumInstances = std::numeric_limits<size_t>::max();
    std::size_t minimumInstances = 0;
    std::size_t maximumReferences = 1;
    bool expand = false;

private:
    /** The copy constructor: declared as private so that nobody can invoke it. */
    SubSection(const SubSection&);

    void parseMacros(const xml::Element& element);
    SubSection(Section* section, const std::string& id);
	
	static const std::string XML_NAME;
public:
    /** Initializes a new instance of the constraints::Section class from the specified xml element.
    	@param section The section to which the subSection refers.*/
    SubSection(Section* section);

    /** Initializes a new instance of the constraints::Section class.
    	@param section The section to which the subSection refers.
    	@param minimumInstances The minimum number of instances of the sub-section allowed in a section instance.
    	@param maximumInstances The maximum number of instances of the sub-section allowed in a section instance. */
    SubSection(Section* section, const std::string& id, unsigned int minimumInstances = 1, unsigned int maximumInstances = std::numeric_limits<unsigned int>::max());

    /** Frees the data associated to the instance. */
    virtual ~SubSection();

    /** Adds a new macro to the sub-section.
    	@param macro A pointer to a valid constraints::Macro instance. Once bound to the constraints::SubSection instance, the lifetime of constraints::Macro object is managed by the sub-section itself. */
    void addMacro(Macro* macro);

    /** Gets the number of macros in the sub-section.
    	@return An unsigned integer that describes the number of macros currently available in the sub-section. */
    std::size_t getMacroCount() const;

    /** Gets the macro with the specified identifier.
    	@param id The identifier of the macro to retrieve. 
    	@return A pointer to the requested constraints::Macro instance or nullptr if the macro is not found. */
    Macro* getMacro(const std::string& id) const;
    Macro& getMacro(unsigned int i) const;


    /** Gets a random macro based on the weight of the macros in the sub-section. */
    Macro* getRandomMacro() const;

    /** Gets the maximum number of macros that can be contained in an instance of a sub-section. */
    std::size_t getMaximumMacroInstances() const { return maximumMacroInstances; }

    /** Gets the minimum number of macros that can be contained in an instance of a sub-section. */
    std::size_t getMinimumMacroInstances() const { return minimumMacroInstances; }

    /** Sets the minimum number of macros that can be contained in an instance of a sub-section. */
    void setMaximumMacroInstances(std::size_t value);

    /** Sets the minimum number of macros that can be contained in an instance of a sub-section. */
    void setMinimumMacroInstances(std::size_t value);

    /** Gets the maximum number of instances of the sub-section that can appear in a section. */
    std::size_t getMaximumInstances() const { return maximumInstances; }

    /** Gets the minimum number of instances of the sub-section that can appear in a section. */
    std::size_t getMinimumInstances() const { return minimumInstances; }
    double getSigma() const;
    std::size_t getAverageMacroInstances() const { return averageMacroInstances; }

    /** Sets the maximum number of instances of the sub-section that can appear in a section. */
    void setMaximumInstances(std::size_t value);

    /** Sets the minimum number of instances of the sub-section that can appear in a section. */
    void setMinimumInstances(std::size_t value);

    /** Tells wether the specified macro is contained in the sub-section.
        @param macro The macro to be found. 
        @return True if the macro is contained in the sub-section, false otherwise. */
    virtual bool contains(const GenericMacro& macro) const;

    std::size_t getMaximumReferences() const { return maximumReferences; }

    void setMaximumReferences(std::size_t value);

    /** Validates the instance.
        @return True if the instance is valid, false otherwise. */
    bool validate() const;

    bool getExpand() const;

public: // Xml interface
    virtual void writeXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
    virtual const std::string& getXmlName() const;
};

inline void SubSection::setMaximumReferences(std::size_t value)
{
    this->maximumReferences = value;
}

inline std::size_t SubSection::getMacroCount() const
{
    return this->macros.size();
}

inline bool SubSection::getExpand() const
{
    return this->expand;
}

inline const std::string& SubSection::getXmlName() const
{
    return XML_NAME;
}

inline void SubSection::setMaximumMacroInstances(std::size_t value)
{
    if(value < this->minimumMacroInstances)
        throw ArgumentException("", LOCATION);

    this->maximumMacroInstances = value;
}

inline double SubSection::getSigma() const
{
    return this->averageSigmaMacroInstances;
}

inline void SubSection::setMinimumMacroInstances(std::size_t value)
{
    if(value > this->maximumMacroInstances)
        throw ArgumentException("", LOCATION);

    this->minimumMacroInstances = value;
}

inline void SubSection::setMaximumInstances(std::size_t value)
{
    if(value < this->minimumInstances)
        throw ArgumentException("", LOCATION);

    this->maximumInstances = value;
}

inline void SubSection::setMinimumInstances(std::size_t value)
{
    if(value > this->maximumInstances)
        throw ArgumentException("", LOCATION);

    this->minimumInstances = value;
}

}

}


#endif
