/***********************************************************************\
|                                                                       |
| MOPopulationParameters.h                                              |
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
#ifndef HEADER_UGP3_CORE_MOPOPULATIONPARAMETERS
#define HEADER_UGP3_CORE_MOPOPULATIONPARAMETERS

#include "IndividualPopulationParameters.h"
#include "MOIndividual.h"
#include "FitnessEvaluator.h"

namespace ugp3
{

namespace core
{
    
class MOPopulation;

class MOPopulationParameters : public SpecificIndividualPopulationParameters<MOIndividual>
{
private:
    FitnessEvaluator m_evaluator;
    
public:
    static const std::string XML_SCHEMA_TYPE;
   
public:
    MOPopulationParameters(MOPopulation* population);
    virtual ~MOPopulationParameters();
    
    virtual const std::string& getType() const;
    
    virtual FitnessEvaluator& getEvaluator() noexcept { return m_evaluator; }
    virtual const FitnessEvaluator& getEvaluator() const noexcept { return m_evaluator; }
};

inline MOPopulationParameters::~MOPopulationParameters()
{
    
}

inline const std::string& MOPopulationParameters::getType() const
{
    return XML_SCHEMA_TYPE;
}

}

}

#endif
