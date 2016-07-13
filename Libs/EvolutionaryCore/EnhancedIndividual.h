/***********************************************************************\
|                                                                       |
| EnhancedIndividual.h                                                  |
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

#ifndef HEADER_UGP3_CORE_CLONESCALEDINDIVIDUAL
#define HEADER_UGP3_CORE_CLONESCALEDINDIVIDUAL

#include "Individual.h"
#include "ScaledFitness.h"
#include "DeltaEntropy.h"

namespace ugp3
{
    
namespace core
{
    
/** An individual with delta entropy, elitism and clone scaling. */
class EnhancedIndividual : public Individual
{
protected:
    ScaledFitness m_scaledFitness;
    
public: // constructors and destructors
    EnhancedIndividual(const EnhancedIndividual& individual);
    EnhancedIndividual(const Population& population);
    EnhancedIndividual(
        unsigned long birth,
        const std::string& geneticOperator,
        const std::vector<std::string>& parents,
        const Population& population);
    virtual ~EnhancedIndividual() {}
        
public: // overridden methods
    using Individual::clone;
    virtual std::unique_ptr<Individual> clone() const;
    
    virtual const ScaledFitness& getFitness() const noexcept { return m_scaledFitness; }
    virtual ScaledFitness& getFitness() noexcept { return m_scaledFitness; }
    
    virtual void setFitnessStructure(Fitness fitness);
    
    virtual const std::string getDisplayTypeName() const;
    
public: // Xml interface
    static const std::string XML_ATTRIBUTE_VALUE;
    static const std::string XML_TYPE;
    
    virtual const std::string& getType() const { return XML_TYPE; }
    virtual void writeInnerXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
};

template <> struct TypeName<EnhancedIndividual> { static constexpr const char* name = "enhanced individual"; };

}
}

#endif
