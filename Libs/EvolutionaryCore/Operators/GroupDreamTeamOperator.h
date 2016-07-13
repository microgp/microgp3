/***********************************************************************\
|                                                                       |
| GroupDreamTeamOperator.h |
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
 * @file GroupDreamTeamOperator.h
 * Build new groups by selecting the best/most diverse individuals
 */

#ifndef HEADER_UGP3_CORE_GROUPDREAMTEAMOPERATOR
#define HEADER_UGP3_CORE_GROUPDREAMTEAMOPERATOR

#include <GroupOperator.h>

namespace ugp3 {
namespace core {

class GroupDreamTeamOperator : public GroupOperator
{
public:
    virtual void generate(const std::vector<Group*>& parents, std::vector<Group*>& outChildren, GroupPopulation* population) const;
    virtual unsigned int getParentsCardinality() const { return 0; }
    
    virtual bool hasParameters() const { return false; }
 
    virtual Category getCategory() { return DEFAULT_ON; }
    
    virtual const std::string getAcronym() const { return "G.DT"; }
    virtual const std::string getName() const { return "groupDreamTeam"; }
    virtual const std::string getDescription() const {
        return getName() + " builds new groups by selecting the best and most diverse individuals.";
    }
    
    virtual void readXml(const xml::Element& element) {}
    virtual void writeXml(std::ostream& output) const {}
};
}
}

#endif // HEADER_UGP3_CORE_GROUPDREAMTEAMOPERATOR
