/***********************************************************************\
|                                                                       |
| OuterLabelParameter.h                                                 |
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

#ifndef HEADER_UGP3_CONSTRAINTS_OUTERLABELPARAMETER
#define HEADER_UGP3_CONSTRAINTS_OUTERLABELPARAMETER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>
#include <string>

#include "StructuralParameter.h"
#include "IValidator.h"

namespace ugp3
{

namespace constraints
{

/** Represents a specific parameter.
	@author Aimo Alessandro, Salomone Alessandro. */
class OuterLabelParameter : public StructuralParameter, public IValidator<std::string>
{
private:
	/** A vector of strings representing the names of the subsections that the label can refer. */
	std::vector<std::string> subSectionNames;

	/** Copy constructor. It is declared private so it's cannot be accessed. */
	OuterLabelParameter(const OuterLabelParameter&);

public:
    
	/** Default constructor. */
	OuterLabelParameter();

	/** Initializes a new instance of the constraints::OuterLabelParameter class.
        @param name The name of the parameter.
		@param subSectionNames A vector of strings representing the names of the valid targets in the form "section subsection" */
	OuterLabelParameter(const std::string& name, const std::vector<std::string>* subSectionNames);

	/** Gets a random name of a valid subSection.
		@param value A string representing value to validate.
		@return A string representing a name of a valid section. */
	const std::string randomize() const;

    /** Clones the instance of the parameter.
        @param outParameter A pointer to the new instance.
        @param name The name of the cloned parameter. */
	virtual void clone(Parameter*& outParameter, const std::string& name);

    /** Tells if the value is valid for this parameter.
		@param value A string representing value to validate.
		@return True if the value is valid. */
	bool validate(const std::string& value) const;

	/** Gets the regex representation for this parameter
	    @return String containing a regex expression. */
	virtual const std::string getRegex() const;

	/** Destructor. */
	virtual ~OuterLabelParameter();

public: // Xml interface
	virtual void writeXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
};

}

}

#endif
