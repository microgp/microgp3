/***********************************************************************\
|                                                                       |
| Random.cc                                                             |
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
| $Revision: 645 $
| $Date: 2015-02-23 16:22:09 +0100 (Mon, 23 Feb 2015) $
\***********************************************************************/

/**
 * @file Random.cc
 * Implementation of the Random class.
 * @see Random.h
 */

#include "ugp3_config.h"
#include <stdexcept>
#include <cmath>
#include <cassert>
#include <algorithm>
using namespace std;

#include "Random.h"
using namespace ugp3;

unsigned long Random::totalCalls = 0;

#ifdef USE_MERSENNE_TWISTER
std::mt19937 Random::mtEngine;
#endif

#include <beta_distribution.h>

double Random::nextNormal(const double sigma)
{
	totalCalls++;

    // FIXME candidate implementation using the standard library, but I'm not sure what the parameter means.
    // return std::normal_distribution<double>(0, sigma)(mtEngine);
    
	static double V1, V2, S;
	static int phase = 0;
	double X;

	if(phase == 0)
	{
		do
		{
			double U1;
			double U2;
			do
			{
			 U1 = (double)Random::nextDouble();
			 U2 = (double)Random::nextDouble();
			}while(U1 == 1 || U1 == 0 || U2 == 1 || U2 == 0);


			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
		}
		while(S >= 1 || S == 0);

		X = V1 * sigma* sqrt(-2 * log(S) / S);
	}
	else X = V2 * sigma * sqrt(-2 * log(S) / S);

	phase = 1 - phase;

	return X;
}

double Random::nextBeta(double alpha, double beta)
{
#ifdef USE_MERSENNE_TWISTER
    return sftrabbit::beta_distribution<double>(alpha, beta)(mtEngine);
#else
    // TODO implement beta distribution by hand
    return 0;
#endif
}

double Random::nextDouble(double minimum, double maximum)
{
	totalCalls++;

	if(minimum > maximum)
		throw std::invalid_argument("Random::nextDouble: The parameter \"minimum\" cannot be greater than \"maximum\"");

    if(minimum == maximum)
        return minimum;

#ifdef USE_MERSENNE_TWISTER
    return std::uniform_real_distribution<double>(minimum, maximum)(mtEngine);
#else
	return minimum + Random::nextDouble() * (maximum - minimum);
#endif
}

double Random::nextDouble()
{
	totalCalls++;

#ifdef USE_MERSENNE_TWISTER
    return std::uniform_real_distribution<double>(0, 1)(mtEngine);
#else
	return rand_drand48();
#endif
}

void Random::seed(long value)
{
#ifdef USE_MERSENNE_TWISTER
	mtEngine.seed(value);
#else
	rand_srand48(value);
#endif
}

const std::string Random::getStatus()
{
#ifdef USE_MERSENNE_TWISTER
    std::ostringstream stream;
    stream << mtEngine;
    return stream.str();
#else
	return drand48_getStatus();
#endif
}

void Random::setStatus(const std::string& status)
{
#ifdef USE_MERSENNE_TWISTER
    std::istringstream stream(status);
    stream >> mtEngine;
#else
	drand48_setStatus(status);
#endif
}

unsigned long  Random::nextUInteger(unsigned long  minimum, unsigned long  maximum)
{
	totalCalls++;
    
    assert(minimum <= maximum);

    if(minimum == maximum)
        return minimum;

#ifdef USE_MERSENNE_TWISTER
    return std::uniform_int_distribution<unsigned long>(minimum, maximum)(mtEngine);
#else
    unsigned long int range = maximum - minimum + 1;
    if(range == 0)
    // overflow
    {
        range = range - 1;
    }

	unsigned long int randomValue = 0;
	unsigned long int mask = 0xff;
	for(unsigned int i=0; i<sizeof(long int) ; i++)
	{
		unsigned long int temp = rand_lrand48();
		temp = temp & mask;
		temp = temp << 8*i;
		randomValue = randomValue | temp;
	}

	return minimum + randomValue % range;
#endif
}

long Random::nextSInteger(long minimum, long maximum)
{
	totalCalls++;

	assert(minimum <= maximum);

    if(minimum == maximum)
        return minimum;

#ifdef USE_MERSENNE_TWISTER
    return std::uniform_int_distribution<long>(minimum, maximum)(mtEngine);
#else
    unsigned long int range = maximum - minimum + 1;
    if(range == 0)
    // overflow
    {
       range = range - 1;
    }

	long int randomValue = 0;
	long int mask = 0xff;

	for(unsigned int i=0; i<sizeof(long int) ; i++)
	{
		long int temp = rand_lrand48();
		temp = temp & mask;

		temp = temp << 8*i;
		randomValue = randomValue | temp;
	}


	long int res;
	long int ret;

	res = randomValue % range;
	ret = res + minimum;

	return ret;
#endif
}

