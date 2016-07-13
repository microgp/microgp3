/***********************************************************************\
|                                                                       |
| EnhancedPopulationParameters.h                                        |
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
#ifndef HEADER_UGP3_CORE_ENHANCEDPOPULATIONPARAMETERS
#define HEADER_UGP3_CORE_ENHANCEDPOPULATIONPARAMETERS

// headers from this module
#include "TournamentSelection.h"
#include "IndividualPopulationParameters.h"
#include "EnhancedIndividual.h"
#include "FitnessEvaluator.h"

// forward declaration for friend class
namespace Ui
{
	class MicroGPMainWindow;
}

namespace ugp3 {
namespace core {
    
class EnhancedPopulation;
        
class EnhancedPopulationParameters : public SpecificIndividualPopulationParameters<EnhancedIndividual>
{
private:
    FitnessEvaluator m_evaluator;
    
public:
    EnhancedPopulationParameters(EnhancedPopulation* population);
    
public:
    virtual FitnessEvaluator& getEvaluator() { return m_evaluator; }
    virtual const FitnessEvaluator& getEvaluator() const { return m_evaluator; }
    
public: //Xml interface
    static const std::string XML_SCHEMA_TYPE;
    
    virtual const std::string& getType() const { return XML_SCHEMA_TYPE; }
    virtual void writeInternalXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
    
public: // friend class, used for the graphical user interface
    friend class Ui::MicroGPMainWindow;
};

}
}

#endif
