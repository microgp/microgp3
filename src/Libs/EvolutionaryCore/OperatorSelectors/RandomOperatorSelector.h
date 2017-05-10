/***********************************************************************\
|                                                                       |
| RandomOperatorSelector.h |
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
 * @file RandomOperatorSelector.h
 * Blah
 */

#ifndef HEADER_RANDOMOPERATORSELECTOR
#define HEADER_RANDOMOPERATORSELECTOR

#include <OperatorSelector.h>

namespace ugp3 {
namespace core {
    
/**
 * Uses roulette-wheel selection with constant and equal coefficients
 * (no self adaptation).
 */
class RandomOperatorSelector : public SpecificOperatorSelector<Data>
{
protected:
    virtual void prepareForSelectionsImpl();
    virtual Result selectImpl();
    
public:
    RandomOperatorSelector(Statistics& stats);
    virtual ~RandomOperatorSelector() {}
    
    virtual void epoch(PopulationParameters& params);
    virtual void step(PopulationParameters& params);
    virtual const std::string& getXmlName() const { return XML_NAME; }
    
    static const std::string XML_NAME;
};

}
}

#endif // HEADER_RANDOMOPERATORSELECTOR
