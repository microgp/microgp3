/***********************************************************************\
|                                                                       |
| Random.h                                                              |
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
 * @file Random.h
 * Definition of the Random class.
 * @see Random.cc
 */

#ifndef HEADER_UGP3_RANDOM
/** Defines that this file has been included */
#define HEADER_UGP3_RANDOM

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/**
 * @def USE_MERSENNE_TWISTER
 * Tag to define the Mersenne Twister as the random numbers generator
 */
#ifdef USE_MERSENNE_TWISTER
#include <random>
#else
#include "drand48.h"
#endif

#include <sstream>
#include <stdexcept>
#include <algorithm>

/**
 * ugp3 namespace
 */
namespace ugp3
{
	
/**
 * @class Random
 * Static class to generate random numbers
 */
class Random
{
private:	
#ifdef USE_MERSENNE_TWISTER
    // Used to generate the numbers in Mersenne Twister
    static std::mt19937 mtEngine;
#else
    static randbuf              a;
    static randbuf              rand48buf;
#endif
        // Number of times that this class generate a random number
	static unsigned long 		totalCalls;
        // Constructor of the class definded as private. This class is static.
	Random();
        // Constructor of the class definded as private. This class is static.
	Random(const Random& random);

public:
    
    static constexpr const char* RANDOM_TYPE_MERSENNE_TWISTER = "Mersenne Twister";
    static constexpr const char* RANDOM_TYPE_RAND48 = "rand48";
    static std::string getType() {
#ifdef USE_MERSENNE_TWISTER
        return RANDOM_TYPE_MERSENNE_TWISTER;
#else
        return RANDOM_TYPE_RAND48;
#endif
    }
        /**
         * Sets the seed for the generator
         * @param value Seed to set
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
	static void 				seed(long value);
        /**
         * Returns a random double number
         * @returns double The generated number
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
	static double 				nextDouble();
        /**
         * Returns a random double number between a minimum and a maximum
         * @minimum Minimum value that for the number
         * @maximum Maximum value that for the number
         * @returns double The generated number
         * @throws Any exception
         */
	static double 				nextDouble(double minimum, double maximum);
        /**
         * Returns a normal double number
         * @param sigma
         * @returns double The generated number
         */
	static double 				nextNormal(const double sigma);
    
        /**
         * Returns a double number ~ Beta(alpha, beta)
         * @param sigma
         * @returns double The generated number
         */
    static double               nextBeta(double alpha, double beta);
    
        /**
         * Returns a random long number between a minimum and a maximum
         * @minimum Minimum value that for the number
         * @maximum Maximum value that for the number
         * @returns long The generated number
         * @throws Any exception
         */
	static long 				nextSInteger(signed long minimum, signed long maximum);
        /**
         * Returns a random unsigned long number between a minimum and a maximum
         * @minimum Minimum value that for the number
         * @maximum Maximum value that for the number
         * @returns unsigned long The generated number
         * @throws Any exception
         */
        static unsigned long 		nextUInteger(unsigned long minimum, unsigned long maximum);
        /**
         * Shuffles in place the given range.
         * @start The start of the range to shuffle (included)
         * @end The end of the range to shuffle (excluded)
         */
        template <class RandomAccessIterator>
        static void shuffle(RandomAccessIterator begin, RandomAccessIterator end) {
            std::random_shuffle(begin, end, [] (int max) {
                return nextUInteger(0, max - 1);
            });
        }
        
        /**
         * Returns the current generator seed
         * @returns string The current seed
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
        static const std::string 	getStatus();
        /**
         * Returns the current generator seed
         * @returns string The current seed
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
        static void 				setStatus(const std::string& status);
        /**
         * Returns the number of times that a random number was generated by this class
         * @returns unsigned long Number of generated numbers
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
        static unsigned long 		getTotalCalls();
};

inline unsigned long Random::getTotalCalls()
{
	return totalCalls;
}

}

#endif
