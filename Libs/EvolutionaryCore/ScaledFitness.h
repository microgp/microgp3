/***********************************************************************\
|                                                                       |
| ScaledFitness.h                                                     |
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

#ifndef HEADER_UGP3_CORE_CLONESCALEDFITNESS
#define HEADER_UGP3_CORE_CLONESCALEDFITNESS

#include "Fitness.h"

namespace ugp3
{
    
namespace core
{

class ScaledFitness : public Fitness
{
public:
    typedef enum {
        CLONE_SCALING = 0,
        SHARING_SCALING,
        CONTRIBUTION_SCALING,
        SCALING_CAUSE_COUNT // For iteration
    } ScalingCause;
    
    struct Scaling {
        std::vector<double> factors = {1};
        void fromString(const std::string& str);
    };
    
private:
    Scaling m_scalingFactors[SCALING_CAUSE_COUNT];
    std::string m_descriptions[SCALING_CAUSE_COUNT];
    unsigned int m_cloneCount = 0;
    
    // We keep the unscaled values here to be able to rescale them later
    // The scaled values are kept in the m_values vector from Fitness
    std::vector<double> m_rawValues;
    std::string m_rawDescription;
    
    // Copy the raw values to the scaled values and apply the current scaling factors.
    void rescale();
    
    void redescribe();

public:
	ScaledFitness(unsigned int parameterCount);
    ScaledFitness();

	unsigned int getCloneCount() const { return m_cloneCount; }
	void setCloneCount(unsigned int cloneCount) { m_cloneCount = cloneCount; }
    
    /**
     * When set, this scaling factor will be applied to the stored fitness values.
     * TODO explain component-wise
     * .... this scaling is the same for all fitness components.
     */
    void setScalingFactor(ScalingCause cause, double factor);
    void setComponentWiseScaling(ScalingCause cause, const std::vector<double>& factors);
    
    const std::vector<double>& getScaling(ScalingCause cause) const { return m_scalingFactors[cause].factors; }   
    
    /**
     * When set, these descriptions will be appended to the original description.
     */
    const std::string& getAdditionalDescription(ScalingCause cause) const { return m_descriptions[cause]; }   
    void setAdditionalDescription(ScalingCause cause, const std::string& description);
    
    /**
     * When a new description is set, additional descriptions of
     * the various scalings applied are appended to the end.
     */
    virtual void setDescription(const std::string& description);
    
    /**
     * When new values are set, the stored scaling factors will be applied to them.
     * You should give unscaled values to this function.
     */
    virtual void setValues(const std::vector< double >& values);
	
public: // Xml interface
    static const std::string XML_NAME;
    static const std::string XML_ATTRIBUTE_CLONECOUNT;
    static const std::string XML_ATTRIBUTE_SCALING_FACTOR[SCALING_CAUSE_COUNT];
    static const std::string XML_ATTRIBUTE_DESCRIPTION[SCALING_CAUSE_COUNT];
    
	virtual const std::string& getXmlName() const { return XML_NAME; }
	virtual void readXml(const xml::Element& element);
	virtual void writeXml(std::ostream& output) const;
};

std::ostream& operator<<(std::ostream& stream, const ScaledFitness::Scaling& scaling);

}
}

#endif
