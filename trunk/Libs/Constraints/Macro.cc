/***********************************************************************\
|                                                                       |
| Macro.cc                                                              |
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

#include "ugp3_config.h"
#include "Debug.h"
#include "Constraints.h"
using namespace std;
using namespace ugp3::constraints;


Macro::Macro(SubSection& parent, const string& id, vector<ExpressionElement*> *expressionElements, vector<Parameter*>* parameters)
	: GenericMacro(parent, id, expressionElements, parameters),
	weight(1.0)
{ }	

void Macro::setWeight(double weight)
{
	_STACK;

	if(weight < 0.0)
	{
		throw ArgumentOutOfRangeException("weight", LOCATION);
	}

	this->weight = weight;
}

Macro::~Macro()
{ }

Macro::Macro(SubSection& parent, const string& id)
	: GenericMacro(parent, id),
	weight(1.0)
{ 
	_STACK;
}

Macro::Macro(SubSection& parent)
	: GenericMacro(parent),
	weight(1.0)
{
	_STACK;
}

void Macro::clear()
{
	this->weight = 1.0;
}

const SubSection* Macro::getSubSection() const
{
    const SubSection* parent = dynamic_cast<const SubSection*>(this->parent);
    
    Assert(parent != nullptr);

	return parent;
}

bool Macro::validate() const
{
    if(this->parent == nullptr)
    {
        return false;
    }

    const SubSection* subSection = this->getSubSection();
    const Section* parentSection = dynamic_cast<const Section*>(subSection->getParent());
    if(parentSection == nullptr)
    {
        return false;
    }

    const Constraints* constraints = dynamic_cast<const Constraints*>(parentSection->getParent());
    if(constraints == nullptr)
    {
        return false;
    }

    for(unsigned p = 0; p < this->parameters.size(); p++)
    {
        const Parameter* parameter = this->parameters[p];

        // ckeck the target of the outer labels
        const OuterLabelParameter* olParam = dynamic_cast<const OuterLabelParameter*>(parameter);
        if(olParam == nullptr)
        {
            continue;
        }
            
        bool found = false;
        for(unsigned s = 0; s < constraints->getSectionCount(); s++)
        {
           const Section& section = constraints->getSection(s);
            for(unsigned ssIndex = 0; ssIndex < section.getSubSectionCount(); ssIndex++)
            {
                SubSection& ssection = section.getSubSection(ssIndex);

                if(olParam->validate(section.getId() + " " + ssection.getId()))
                {
                    found = true;

                    if(ssection.getExpand() == true)
                    {
                        if(&section != this->getParent()->getParent())
                        {
 throw Exception("When inlining a sub-section, the targets of its outer-labels can only be inside the same section.",    LOCATION);
                        }
                    }
                    
                }
            }
        }

        if(found == false)
        {
            LOG_ERROR << "Parameter " << olParam << ": outer label's reference doesn't refer to a valid section/subsection pair" << ends;
            return false;
        }
    }

    return true;
}

