/***********************************************************************\
|                                                                       |
| MOIndividual.h                                                        |
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

#ifndef MOINDIVIDUAL_H_
#define MOINDIVIDUAL_H_

#include "MOFitness.h"
#include "Individual.h"

namespace ugp3
{

namespace core
{

class MOIndividual : public Individual
{
private:
    
    /** The placeholder of the individual.
     *             It stores the position of the individual in the population and
     *             is used to perform clone scaling. */
    unsigned int m_placeholder;
    
    int m_level;		// # of the level the individual belongs to
    int m_previousLevel;
    double m_strength;
    
    MOFitness m_fitness;
    
public:
    MOIndividual(const MOIndividual& individual);
    MOIndividual(const Population& population);
    MOIndividual(
        unsigned long birth,
        const std::string& geneticOperator,
        const std::vector<std::string>& parents,
        const Population& population);
    virtual ~MOIndividual() {}
    
    using Individual::clone;
    virtual std::unique_ptr<Individual> clone() const;
    
    int getLevel() const { return m_level; }
    int getPreviousLevel() const { return m_previousLevel; }
    void setLevel(int level) { m_level = level; }
    void setPreviousLevel(int previousLevel) { m_previousLevel = previousLevel; }
    
    double getPerceivedStrength() const { return m_strength; }
    void setPerceivedStrength(double strength) { m_strength = strength; }
    
    unsigned int getPlaceholder() const { return m_placeholder; }
    void setPlaceholder(unsigned int place) { m_placeholder = place; }
    
    virtual MOFitness& getFitness() { return m_fitness; }
    virtual const MOFitness& getFitness() const { return m_fitness; }
    
    virtual const std::string getDisplayTypeName() const;
    
public: // Xml Interface
    static const std::string XML_TYPE;
    static const std::string XML_ATTRIBUTE_LEVEL;
    static const std::string XML_ATTRIBUTE_CURRENT;
    static const std::string XML_ATTRIBUTE_PREVIOUS;
    static const std::string XML_ATTRIBUTE_VALUE;
    static const std::string XML_CHILD_ELEMENT_PLACEHOLDER;
    
    virtual const std::string& getType() const { return XML_TYPE; }
    virtual void writeInnerXml(std::ostream& output) const;
    virtual void readXml(const xml::Element& element);
};

template <> struct TypeName<MOIndividual> { static constexpr const char* name = "MO individual"; };

}
}

#endif /*MOINDIVIDUAL_H_*/


