/***********************************************************************\
|                                                                       |
| Fitness.cc                                                            |
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

#include "ugp3_config.h"
#include "Fitness.h"
#include "ArgumentException.h"
#include "Convert.h"
#include <Debug.h>

#include <sstream>
#include <cmath>
#include <algorithm>

using namespace ugp3;
using namespace ugp3::core;
using namespace std;

Fitness::Fitness()
: description(""),
isValid(false)
{ }

Fitness::Fitness(unsigned int parameterCount, const string& valuesSeparatedBySpaces)
: Fitness()
{
    _STACK;
    
    if (parameterCount == 0) {
        throw ArgumentException("the number of the fitness parameters cannot be zero", LOCATION);
    }
    
    if (valuesSeparatedBySpaces.empty()) {
        m_values.resize(parameterCount, 0.0);
    } else {
        // TODO factorize with next function
        // convert string to vector of doubles
        std::istringstream lineStream(valuesSeparatedBySpaces);
        
        // parse the fitness values
        for (unsigned int f = 0; f < parameterCount; f++)
        {
            double value = 0;
            lineStream >> value;
            if (lineStream.fail() == true)
                throw Exception("Bad fitness format.", LOCATION);
            m_values.push_back(value);
        }
        isValid = true;
    }
}

Fitness& Fitness::operator=(const Fitness& other)
{
    Assert(other.getIsValid());
    setDescription(other.getDescription());
    setValues(other.getValues());
    return *this;
}


void Fitness::clear()
{
    this->m_values.clear();
    this->description = "";
    this->isValid = false;
}

Fitness Fitness::parse(const std::string& value)
{
    istringstream stream(value);
    
    Fitness fitness;
    vector<double> values;
    while(stream.eof() == false)
    {
        string token;
        stream >> token;
        if(stream.fail() == false)
        {
            try
            {
                double value = Convert::toDouble(token);
                values.push_back(value);
            }
            catch(const exception& e)
            {
                fitness.setDescription(value);
                break;
            }
        }
        else break;
    }
    
    if(values.empty())
    {
        throw Exception("Fitness format is not valid", LOCATION);
    }
    else fitness.setValues(values);
    
    return fitness;
}

const std::string Fitness::toString() const
{
    std::ostringstream stream;
    
    stream << "{";
    
    for(unsigned int i = 0; i < this->m_values.size(); i++)
    {
        stream << this->m_values[i];
        if(i < this->m_values.size() - 1) stream << " ";
    }
    
    if(this->description != "")
        stream << "; \"" << this->description << "\"";
    
    stream << "}";
    
    if(this->isValid == false)
        stream << " Invalid fitness";
    
    return stream.str();
}

bool Fitness::equals(const Fitness& fitness) const
{
    return this->compareTo(fitness) == 0;
}

bool Fitness::equals(const Fitness& fitness, double threshold) const
{
    _STACK;
    
    if(this->m_values.size() != fitness.m_values.size())
        return false;
    
    for(unsigned int i = 0 ; i < this->m_values.size() ; i++)
    {
        double maximum = std::max<double>(this->m_values[i], fitness.m_values[i]);
        
        if(this->m_values[i] - fitness.m_values[i] > threshold * maximum)
            return false;
    }
    
    return true;
}

void Fitness::setValues(const std::vector<double>& values)
{
    Assert(std::none_of(values.begin(), values.end(), [] (double v) {
        return std::isnan(v);
    }));
    this->m_values = values;
    this->isValid = true;
}

int Fitness::compareTo(const Fitness& fitness) const
{
    Assert(getIsValid());
    Assert(fitness.getIsValid());
    
    for (unsigned int i = 0; i < this->m_values.size() && i < fitness.m_values.size(); i++)
    {
        if (this->m_values[i]  > fitness.m_values[i]) {
            return 1;
        }
        else if (this->m_values[i] < fitness.m_values[i]) {
            return -1;
        }
    }
    
    return 0;
}


