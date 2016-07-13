/***********************************************************************\
|                                                                       |
| MOFitness.cc                                                          |
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
#include "MOFitness.h"
#include "Exception.h"
#include "ArgumentException.h"

using namespace ugp3;
using namespace ugp3::core;
using namespace std;

MOFitness::MOFitness()
	: ScaledFitness()
{ }


MOFitness::MOFitness(unsigned int parameterCount)
{
	this->description = "";
	this->isValid = false;
	
	_STACK;

	if(parameterCount <= 0) // 0 for added generality
	{
		throw ArgumentException("the number of the fitness parameters must be greater than zero", LOCATION);
	}

	m_values.resize(parameterCount, 0.0);
}

bool MOFitness::equals(const Fitness& fitness) const {
	
	const MOFitness* mofitness = dynamic_cast<const MOFitness*>(&fitness);
  
    if(mofitness == nullptr)
    {
        throw Exception("Fitness should be of type core::MOFitness.", LOCATION);
    }
    
	if(this->m_values.size() != mofitness->m_values.size())
		return false;
		
	unsigned int i;
	for(i = 0; i < this->m_values.size() && this->m_values[i] == mofitness->m_values[i]; i++); 
	
	if(i == this->m_values.size())
		return true;				// equal
			
	return false;					// not comparable	

}


int MOFitness::compareTo(const Fitness& instance) const {

	const MOFitness* fitness = dynamic_cast<const MOFitness*>(&instance);
  
    if(fitness == nullptr)
    {
        throw Exception("Fitness should be of type core::MOFitness.", LOCATION);
    }
    
	bool gt = false;
	bool lt = false;
	    
	for(unsigned int i = 0; i < this->m_values.size() && i < fitness->m_values.size(); i++) 
	{
		if(this->m_values[i] < fitness->m_values[i])
			lt = true;
		else if(this->m_values[i] > fitness->m_values[i])
			gt = true;
			
		if(gt && lt)
			return 0;
	}	
	
	if(gt)
		return 1;
			
	if(lt)
		return -1;

	return 0;
}

