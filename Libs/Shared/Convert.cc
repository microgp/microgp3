/***********************************************************************\
|                                                                       |
| Convert.cc                                                            |
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
 * @file Convert.cc
 * Implementation of the Convert class.
 * @see Convert.h
 */

#include "ugp3_config.h"
#include <sstream>
#include <limits>
#include <bitset>
#include <cassert>

#include "Exception.h"
#include "Convert.h"
#include "StackTrace.h"
using namespace ugp3;
using namespace std;

long int Convert::toLong(const string& value, Base base)
{
    _STACK;


	if(value == "infinity") return numeric_limits<long int>::max();
	
    long int result = 0;
    if(base == Base::Binary)
    {
        for(unsigned int i = 0 ; i < value.size() ; i++)
        {
            result = result << 1;
            if(value[i]=='1')
            {
                result = result + 1;
            }
            else if(value[i] != '0')
            {
                throw Exception("conversion failed", LOCATION);
            }
        }
    }
    else
    {
        istringstream stream;
        stream.str(value);

        if(base == Base::Decimal)
        {
            stream >> result;
        }
        else if(base == Base::Hexadecimal)
        {
            stream >> hex >> result;
        }
        else if(base == Base::Octal)
        {
            stream >> oct >> result;
        }

        if(stream.fail())
        {
            throw Exception("conversion failed", LOCATION);
        }
    }

    return result;
}

unsigned int Convert::toUInt(const string& value, Base base)
{
    _STACK;


	if(value == "infinity") return numeric_limits<unsigned int>::max();
	
    unsigned int result = 0;
    if(base == Base::Binary)
    {
        for(unsigned int i = 0 ; i < value.size() ; i++)
        {
            result = result << 1;
            if(value[i]=='1')
            {
                result = result + 1;
            }
            else if(value[i] != '0')
            {
                throw Exception("Cannot convert value '" + value + "' to an unsigned integer.", LOCATION);
            }
        }
    }
    else
    {
        istringstream stream;
        stream.str(value);

        if(base == Base::Decimal)
        {
            stream >> result;
        }
        else if(base == Base::Hexadecimal)
        {
            stream >> hex >> result;
        }
        else if(base == Base::Octal)
        {
            stream >> oct >> result;
        }

        if(stream.fail())
        {
            throw Exception("Cannot convert value '" + value + "' to an unsigned integer.", LOCATION);
        }
    }

    return result;
}

unsigned long Convert::toULong(const string& value)
{
    _STACK;

	if(value == "infinity") return numeric_limits<long>::max();

    istringstream stream;
    stream.str(value);

    unsigned long result = 0;
    stream >> result;

    if(stream.fail())
    {
        throw Exception("conversion failed", LOCATION);
    }

    return result;
}

int Convert::toInt(const string& value)
{
    _STACK;

    if(value == "infinity") return numeric_limits<int>::max();
	
    istringstream stream;
    stream.str(value);

    int result = 0;
    stream >> result;

    if(stream.fail())
    {
        throw Exception("conversion failed", LOCATION);
    }

    return result;
}

vector<int> Convert::toIntVector(const string& value, const string& delimiters)
{
    _STACK;

    vector<int> result;

    // parse the string
    vector<string> tokens = Convert::toStringVector(value, delimiters);

    // for each token, convert it into an integer and push it into the vector<int>
    for(unsigned int t = 0; t < tokens.size(); t++)
    {
	int r = Convert::toInt( tokens[t] );
	result.push_back( r );
    }

    return result;
}

vector<string> Convert::toStringVector(const string& value, const string& delimiters)
{
    _STACK;

    vector<string> result;

    // Skip delimiters at beginning.
    string::size_type lastPos = value.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = value.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        result.push_back(value.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = value.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = value.find_first_of(delimiters, lastPos);
    }

    return result;
}

bool Convert::toBool(const string& value)
{
    _STACK;

    if(value == "0" || value == "false")
    {
        return false;
    }
    else if(value == "1" || value == "true")
    {
        return true;
    }
    else 
    {
        throw Exception("Cannot convert string '" + value + "' to boolean value.", LOCATION);
    }
}

double Convert::toDouble(const string& value)
{
    _STACK;

	if(value == "infinity") return numeric_limits<double>::max();
	
    istringstream stream;
    stream.str(value);

    double result = 0;
    stream >> result;

    if(stream.fail())
    {
        throw Exception("conversion failed", LOCATION);
    }

    return result;
}

float Convert::toFloat(const string& value)
{
    _STACK;

	if(value == "infinity") return numeric_limits<float>::max();
	
    istringstream stream;
    stream.str(value);

    float result = 0;
    stream >> result;

    if(stream.fail())
    {
        throw Exception("conversion failed", LOCATION);
    }

    return result;
}
const string Convert::toString(double value)
{
    _STACK;

    ostringstream stream;
    stream.precision(15);
    stream << value;

    return stream.str();
}

const string Convert::toString(bool value)
{
    _STACK;

    ostringstream stream;
    stream << boolalpha << value;

    return stream.str();
}

const string Convert::toString(int value)
{
    _STACK;

    ostringstream stream;
    stream << value;

    return stream.str();
}
const string Convert::toString(long int value)
{
    _STACK;

    ostringstream stream;
    stream << value;

    return stream.str();
}
const string Convert::toString(unsigned int value, Base base)
{
    _STACK;

    std::string result = "";

    if(base == Base::Binary)
    {
        throw Exception("not implemented", LOCATION);
    }
    else
    {
        ostringstream stream;

        if(base == Base::Decimal)
        {
            stream << value;
        }
        else if(base == Base::Hexadecimal)
        {
            stream << hex << value;
        }
        else if(base == Base::Octal)
        {
            stream << oct << value;
        }

        result = stream.str();
    }

    return result;
}
const string Convert::toString(unsigned long int value, Base base)
{
    _STACK;

    std::string result = "";

    if(base == Base::Binary)
    {
        throw Exception("not implemented", LOCATION);
    }
    else
    {
        ostringstream stream;

        if(base == Base::Decimal)
        {
            stream << value;
        }
        else if(base == Base::Hexadecimal)
        {
            stream << hex << value;
        }
        else if(base == Base::Octal)
        {
            stream << oct << value;
        }

        result = stream.str();
    }

    return result;
}

const string Convert::toString(float value)
{
    _STACK;

    ostringstream stream;
    stream << value;

    return stream.str();
}

const string Convert::toString(vector<int> value)
{
    _STACK;

    ostringstream stream;
    stream << value[0];
    for(unsigned int i = 1; i < value.size(); i++)
    	stream << value[i];

    return stream.str();
}

const string Convert::toString(vector<string> value)
{
    _STACK;

    ostringstream stream;
    stream << value[0];
    for(unsigned int i = 1; i < value.size(); i++)
    	stream << value[i] << " ";

    return stream.str();
}

const string Convert::toBitString(std::string value, Base base)
{
    _STACK;

    std::string result = "";

    if(base == Base::Decimal)
    {
        throw Exception("Base Decimal not implemented.", LOCATION);
    }
    else if(base == Base::Binary)
    {
	// binary string is already a bit string
	return value;
    }
    else
    {
        ostringstream stream;

        if(base == Base::Hexadecimal)
        {
            	for(unsigned int i = 0; i < value.size(); i++)
	    	{
			if( value[i] >= '0' && value[i] <= '9' )
			{
				std::bitset<4> binary( value[i] - 48 );
				stream << binary;
			}
			else if( value[i] >= 'a' && value[i] <= 'f')
			{
				std::bitset<4> binary( value[i] - 87 );
				stream << binary;
			}
			else
				throw Exception("Hexadecimal values range from '0' to 'e', value was " + toString(value[i]), LOCATION);
		}
        }
        else if(base == Base::Octal)
        {
            	for(unsigned int i = 0; i < value.size(); i++)
	    	{
			if( value[i] >= '0' && value[i] <= '7' )
			{
				std::bitset<3> binary( value[i] - 48);
				stream << binary;
			}
			else
				throw Exception("Octal values range from '0' to '7', value was " + toString(value[i]), LOCATION);
		}
        }

        result = stream.str();
    }

    return result;
}

const string Convert::fromBitString(std::string value, Base base)
{
    _STACK;

    std::string result = "";

    if(base == Base::Decimal)
    {
        throw Exception("Base Decimal not implemented.", LOCATION);
    }
    else if(base == Base::Binary)
    {
	// a bit string is already binary
	return value;
    }
    else
    {
        ostringstream stream;

        if(base == Base::Hexadecimal)
        {
		assert( value.length() % 4 == 0);

		// start from LSB
		for(int i = (int)value.length() - 1; i >= 3; i -= 4)
		{
		    // 4 bits
		    unsigned int val4Bits = 0;
		    for(int k = 0; k < 4; k++)
		    {
			if(value[i - k] == '1')
			{
			    unsigned int p = 1 << k;
			    val4Bits += p;
			}
		    }

		    stream << Convert::toString(val4Bits, Base::Hexadecimal);
		}
        }
        else if(base == Base::Octal)
        {
		assert( value.length() % 3 == 0);

		// start from LSB
		for(int i = (int)value.length() - 1; i > 0; i -= 3)
		{
		    // 3 bits
		    unsigned int val3Bits = 0;
		    for(int k = 0; k < 3; k++)
		    {
			if(value[i - k] == '1')
			{
			    unsigned int p = 0x1 << k;
			    val3Bits += p;
			}
		    }

		    stream << Convert::toString(val3Bits, Base::Octal);
		}
        }

        result = stream.str();
    }

    return result;
}

