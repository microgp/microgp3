/***********************************************************************\
|                                                                       |
| ScaledFitness.cc                                                    |
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

#include "ScaledFitness.h"

#include "Debug.h"
#include "Log.h"

#include <cmath>
#include <algorithm>

ugp3::core::ScaledFitness::ScaledFitness()
{
}

ugp3::core::ScaledFitness::ScaledFitness(unsigned int parameterCount)
:Fitness(parameterCount)
{
}

void ugp3::core::ScaledFitness::setScalingFactor(ScalingCause cause, double factor)
{
    m_scalingFactors[cause].factors = {factor};
    rescale();
}

void ugp3::core::ScaledFitness::setComponentWiseScaling(ugp3::core::ScaledFitness::ScalingCause cause, const std::vector< double >& factors)
{
    m_scalingFactors[cause].factors = factors;
    rescale();
}


void ugp3::core::ScaledFitness::setValues(const std::vector< double >& values)
{
    m_rawValues = values;
    rescale();
}

void ugp3::core::ScaledFitness::setAdditionalDescription(ugp3::core::ScaledFitness::ScalingCause cause, const std::string& description)
{
    m_descriptions[cause] = description;
    redescribe();
}

void ugp3::core::ScaledFitness::setDescription(const std::string& description)
{
    m_rawDescription = description;
    redescribe();
}

void ugp3::core::ScaledFitness::rescale()
{
    std::vector<double> newValues(m_rawValues);
    
    for (std::size_t i = 0; i < newValues.size(); ++i) {
        for (int j = 0; j < SCALING_CAUSE_COUNT; ++j) {
            newValues[i] *= m_scalingFactors[j].factors[std::min(i, m_scalingFactors[j].factors.size() - 1)];
        }
    }
    
    Fitness::setValues(newValues);
}

void ugp3::core::ScaledFitness::redescribe()
{
    std::string desc = m_rawDescription;
    
    for (int i = 0; i < SCALING_CAUSE_COUNT; ++i) {
        desc += m_descriptions[i];
    }
    
    Fitness::setDescription(desc);
}

std::ostream& ugp3::core::operator<<(std::ostream& stream, const ugp3::core::ScaledFitness::Scaling& scaling)
{
    for (double factor: scaling.factors) {
        stream << factor;
    }
    return stream;
}

void ugp3::core::ScaledFitness::Scaling::fromString(const std::string& str)
{
    factors.clear();
    std::istringstream stream(str);
    double factor;
    while (stream >> factor) {
        factors.push_back(factor);
    }
}


