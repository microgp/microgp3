/***********************************************************************\
|                                                                       |
| EnhancedPopulation.h                                                  |
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
 * @file EnhancedPopulation.h
 * Definition of the EnhancedPopulation class.
 * @see EnhancedPopulation.cc
 * @see EnhancedPopulation.xml.cc
 */

#ifndef HEADER_UGP3_CORE_ENHANCEDPOPULATION
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_ENHANCEDPOPULATION

// headers from this module
#include "IndividualPopulation.h"
#include "EnhancedIndividual.h"
#include "EnhancedPopulationParameters.h"

/** The new way in the computation of the entropy is used if defined, else the old way is used */
#define ENTROPY_PATCH //comment to let entropy computed in the old way

namespace ugp3 {
namespace core {

/**
 * @class EnhancedPopulation
 * A population that implements clone scaling
 * @see EnhancedPopulationParameters
 * @see Population
 * @see MOPopulation
 */
class EnhancedPopulation : public SpecificIndividualPopulation<EnhancedIndividual>
{
private:
    EnhancedPopulationParameters m_parameters;

protected: // virtual methods
    
    virtual void handleClone(CandidateSolution& master, CandidateSolution& clone,
                             unsigned int number, unsigned int total);
    
    // Must rescale clones
    virtual void prepareForCommit();
    
public: // constructors and destructors
    EnhancedPopulation(const EvolutionaryAlgorithm& parent);
    EnhancedPopulation(
                      std::unique_ptr< ugp3::core::EnhancedPopulationParameters > parameters, const ugp3::core::EvolutionaryAlgorithm& parent
                      );
    virtual ~EnhancedPopulation();

public: // getters and setters
    EnhancedPopulationParameters& getParameters() noexcept { return m_parameters; }
    const EnhancedPopulationParameters& getParameters() const noexcept { return m_parameters; }

public: // Xml interface
    static const std::string XML_SCHEMA_TYPE;
    
    virtual const std::string& getType() const { return XML_SCHEMA_TYPE; }
    virtual void writeInnerXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
};
        
}
}

#endif
