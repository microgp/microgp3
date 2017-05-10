/***********************************************************************\
|                                                                       |
| BitArrayParameter.cc                                                  |
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
#include "Constraints.h"
using namespace std;
using namespace ugp3::constraints;

BitArrayParameter::BitArrayParameter()
: DataParameter(),
base(Base::Binary)
{ }

BitArrayParameter::BitArrayParameter(const string& name, const string& pattern, const ugp3::Base& base)
	: DataParameter(name),
    pattern(pattern),
    base(base)
{ 
	_STACK;
	
    if(pattern.empty())
    {
        throw ArgumentException("The size of the bit array must be at least 1.", LOCATION);
    }

    unsigned int size = (unsigned int)pattern.length();
    if(base == Base::Hexadecimal && size % 4 != 0)
    {
        throw ArgumentException("In hexaecimal mode, the pattern should have a length that is a multiple of 4.", LOCATION);
    }
    else if(base == Base::Octal && size % 3 != 0)
    {   
        throw ArgumentException("In octal mode, the pattern should have a length that is a multiple of 3.", LOCATION);
    }
    else if(base == Base::Decimal)
    {
        throw ArgumentException("A byte array accepts only hex, oct or bin representations.", LOCATION);
    }
    
    for(unsigned int i = size; i > 0; i--)
    {
        if(pattern[i - 1] != '0' && pattern[i - 1] != '1' && pattern[i - 1] != '-')
        {
           throw ArgumentException("The pattern is not a valid", LOCATION);
        }
    }
}

BitArrayParameter::~BitArrayParameter()
{}

const string BitArrayParameter::randomize() const
{
	_STACK;
	
    string value = "";

    if(this->base == Base::Binary)
    {
        for(size_t i = 0; i < this->pattern.length(); i++)
        {
            if(pattern[i] != '-')
            {
                value += pattern[i];
            }
            else
            {
                value += getRandomBit();
            }
        }  
    }
    else if(this->base == Base::Hexadecimal)
    {
        assert(this->pattern.length() % 4 == 0); 

        // start from LSB
        for(int i = (int)this->pattern.length() - 1; i >= 3; i -= 4)
        {
            // 4 bits
            unsigned int val4Bits = 0;
            for(int k = 0; k < 4; k++)
            {
                if(pattern[i - k] == '1')
                {
                    unsigned int p = 1 << k;
                    val4Bits += p;
                }
                else if(pattern[i - k] == '-')
                {
                    const char bit = getRandomBit();
                    if(bit == '1')
                    {
                        unsigned int p = 1 << k;
                        val4Bits += p;
                    }
                }
            }

            value = Convert::toString(val4Bits, Base::Hexadecimal) + value;
        }
    }
    else if(this->base == Base::Octal)
    {
       assert(this->pattern.length() % 3 == 0); 

        // start from LSB
        for(int i = (int)this->pattern.length() - 1; i > 0; i -= 3)
        {
            // 3 bits
            unsigned int val3Bits = 0;
            for(int k = 0; k < 3; k++)
            {
                if(pattern[i - k] == '1')
                {
                    unsigned int p = 0x1 << k;
                    val3Bits += p;
                }
                else if(pattern[i - k] == '-')
                {
                    const char bit = getRandomBit();
                    if(bit == '1')
                    {
                        unsigned int p = 0x1 << k;
                        val3Bits += p;
                    }
                }
            }

            value = Convert::toString(val3Bits, Base::Octal) + value;
        }
    }

    return value;
}

char BitArrayParameter::getRandomBit() const
{
    if (!initNull && Random::nextDouble() < 0.5) {
        return '1';
    }
    
    return '0';   
}

bool BitArrayParameter::validate(const string& value) const
{
    _STACK;
    
    try
    {
        string result = "";
        if(this->base == Base::Binary)
        {
            if(value.length() != this->pattern.length()) return false;
            result = value; 
        }
        else if(this->base == Base::Hexadecimal)
        {
            for(unsigned int i = 0; i < value.length(); i++)
            {
                //string element = &value.at(i);
                // substr is used to avoid problems with the const casting
                string element = value.substr(i,1);
                long int val = Convert::toLong(element, Base::Hexadecimal);
                string val4bits = "";
                
                for(int k = 0; k < 4; k++)
                {
                    val4bits = Convert::toString((val >> k) & 0x1) + val4bits;
                }
                
                result += val4bits;
            }
        }
        else if(this->base == Base::Octal)
        {
            for(unsigned int i = 0; i < value.length(); i++)
            {
                //string element = &value.at(i);
                // substr is used to avoid problems with the const casting
                string element = value.substr(i,1);
                
                long int val = Convert::toLong(element, Base::Octal);
                string val3bits = "";
                
                for(int k = 0; k < 3; k++)
                {
                    val3bits = Convert::toString((val >> k) & 0x1) + val3bits;
                }
                
                result += val3bits;
            }
        }
        else {
            // FIXME this will fail only in debug mode
            Assert(false);
        }
        
        for(unsigned int i = 0; i < result.length(); i++)
        {
            if(this->pattern[i] != '-' && result[i] != this->pattern[i])
            {
                return false;
            }
        }
        
    }
    catch(...)
    {
        return false;
    }
    
    return true;
}

void BitArrayParameter::clone(Parameter*& outParameter, const string& name)
{
    _STACK;
    
    BitArrayParameter* parameter = new BitArrayParameter(name, this->pattern, this->base);
    parameter->typeDefinition = (this->typeDefinition != nullptr? this->typeDefinition : this);
    
    outParameter = parameter;
}

const string BitArrayParameter::getRegex() const
{
	// depending on the base, this might assume different forms
	if( this->base == Base::Binary )
	{
		return "([01]+)";
	}
	else if( this->base == Base::Octal )
	{
		return "([0-7]+)";
	}
	else if( this->base == Base::Hexadecimal )
	{
		return "([0-9A-Fa-f]+)";
	}
	else
	{
		LOG_ERROR << "Cannot find Base for BitArrayParameter \"" << this->getName() << "\": cannot compute regex." << ends;
		return "";
	}
}
