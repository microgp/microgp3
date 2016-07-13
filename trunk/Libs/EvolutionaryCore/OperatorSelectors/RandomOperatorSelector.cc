/***********************************************************************\
|                                                                       |
| RandomOperatorSelector.cc |
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
 * @file RandomOperatorSelector.cc
 *
 */

#include "RandomOperatorSelector.h"
#include "Statistics.h"

namespace ugp3 {
namespace core {
    
const std::string RandomOperatorSelector::XML_NAME = "random";

RandomOperatorSelector::RandomOperatorSelector(ugp3::core::Statistics& stats)
:SpecificOperatorSelector(stats)
{
}

ugp3::core::OperatorSelector::Result RandomOperatorSelector::selectImpl()
{
    std::vector<Data*> selectable;
    for (unsigned int i = 0; i < m_stats.getDataCount(); ++i) {
        Data& data = m_stats.getData(i);
        if (data.getSelectable()) {
            selectable.push_back(&data);
        }
    }
    auto i = ugp3::Random::nextUInteger(0, selectable.size() - 1);
    return Result(selectable[i], selectable[i]->getOperator(), 0);
}

void RandomOperatorSelector::prepareForSelectionsImpl()
{
}

void RandomOperatorSelector::epoch(PopulationParameters& params)
{

}

void RandomOperatorSelector::step(PopulationParameters& params)
{

}


}
}