/***********************************************************************\
|                                                                       |
| SelfAdaptiveOperatorSelector.h |
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
 * @file SelfAdaptiveOperatorSelector.h
 * Blah
 */

#ifndef HEADER_UGP3_CORE_SELFADAPTIVEOPERATORSELECTOR
#define HEADER_UGP3_CORE_SELFADAPTIVEOPERATORSELECTOR

#include <OperatorSelector.h>
#include <SAData.h>
#include <exception>

namespace ugp3 {
namespace core {

class Statistics;

class SelfAdaptiveOperatorSelector : public SpecificOperatorSelector<SAData>
{
private:
    double getWeightFromHistogram(SAData & data);
    void updateWeight(SAData & data, double aimedValue, double inertia);
    
protected:
    virtual void prepareForSelectionsImpl();
    virtual OperatorSelector::Result selectImpl();
    
public:
    SelfAdaptiveOperatorSelector(Statistics & stats);
    virtual ~SelfAdaptiveOperatorSelector() {}
    
    virtual void step(PopulationParameters& params);
    virtual void epoch(PopulationParameters& params);
    virtual const std::string& getXmlName() const { return XML_NAME; }
    
    static const std::string XML_NAME;
};
}
}

#endif // HEADER_UGP3_CORE_SELFADAPTIVEOPERATORSELECTOR
