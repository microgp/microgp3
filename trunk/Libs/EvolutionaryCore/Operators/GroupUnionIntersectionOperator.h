/***********************************************************************\
|                                                                       |
| GroupUnionIntersectionOperator.h |
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

/**
 * @file GroupUnionIntersectionOperator.h
 * Blah
 */

#ifndef HEADER_UGP3_CORE_GROUPUNIONINTERSECTIONOPERATOR
#define HEADER_UGP3_CORE_GROUPUNIONINTERSECTIONOPERATOR

#include <Operators/GroupCrossover.h>

namespace ugp3 {
namespace core {

class GroupUnionIntersectionOperator : public GroupCrossoverOperator
{
protected:
    virtual bool doSwaps(unsigned int min, unsigned int max, double sigma,
                         Group* child1, Group* child2,
                         std::vector< GEIndividual* >& swappable1, std::vector< GEIndividual* >& swappable2) const;
    
public:
    virtual const std::string getName() const;
    virtual const std::string getAcronym() const;
    virtual const std::string getDescription() const;
 
    virtual Category getCategory() { return DEFAULT_ON; }
    
public: // XML Methods
    virtual void readXml(const xml::Element& element) {}
    virtual void writeXml(std::ostream& output) const {}
    virtual bool hasParameters() const { return false; }
};

}
}

#endif // HEADER_UGP3_CORE_GROUPUNIONINTERSECTIONOPERATOR
