/***********************************************************************\
|                                                                       |
| ClassicalMigrator.h                                                   |
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

#ifndef HEADER_UGP3_CORE_CLASSICALMIGRATOR
#define HEADER_UGP3_CORE_CLASSICALMIGRATOR

#include "IMigrator.h"

#include <vector>

#include "Exceptions/ArgumentNullException.h"
    
#include "EnhancedPopulationParameters.h"
#include "MOPopulationParameters.h"

namespace ugp3 {
namespace core {
    
// forward declaration
class Population;
class Individual;

/** Simple implemenation of a strategy for the migration of individuals between populations.
 *         @author Tonda Alberto */
class ClassicalMigrator : public IMigrator
{
private:
    unsigned int interval;
    unsigned int size;
    
    double sizeMin;
    double sizeMax;
    double intervalMin;
    double intervalMax;
    
    template <class PopType>
    bool tryMigrate(std::vector<Population*>* populations);
    
    template <class PopType>
    void doMigrate(std::vector<PopType*>* populations);
    
protected:
    static const string XML_ATTRIBUTE_INTERVAL;		
    static const string XML_ATTRIBUTE_SIZE;		
public:
    static const string XML_SCHEMA_TYPE;
    
    /** Migrates the individuals between the given populations using a specific strategy.
     *             @param populations The collection of populations involved in the migration process.
     */
    
    // builder and destroyer
    ClassicalMigrator();
    ClassicalMigrator(unsigned int size, unsigned int interval);
    ~ClassicalMigrator();
    
    // function that migrates individual between different populations ("islands")	
    // NOTE we restrict migrations to Enhanced and MO populations. In case of group population, does nothing.
    // TODO adapt the algorithm for group populations.
    virtual void migrate(std::vector<Population*>* populations);
    
    // function that selects the individual to migrate
    std::vector<const Individual&> select(Population* population);
    
    // getter
    unsigned int getInterval();
    unsigned int getSize();
    
    double getIntervalMin();
    double getIntervalMax();
    double getSizeMax();
    double getSizeMin();
    
    // setter
    void setInterval(unsigned int value);
    void setSize(unsigned int value);
    
    void setIntervalMin(double value);
    void setIntervalMax(double value);
    void setSizeMax(double value);
    void setSizeMin(double value);
};

// getter
inline unsigned int ClassicalMigrator::getInterval()
{
	return this->interval;
}

inline double ClassicalMigrator::getIntervalMax()
{
	return this->intervalMax;
}

inline double ClassicalMigrator::getIntervalMin()
{
	return this->intervalMin;
}

inline unsigned int ClassicalMigrator::getSize()
{
	return this->size;
}

inline double ClassicalMigrator::getSizeMax()
{
	return this->sizeMax;
}

inline double ClassicalMigrator::getSizeMin()
{
	return this->sizeMin;
}

// setter
inline void ClassicalMigrator::setInterval(unsigned int value)
{
	this->size = value;
}

inline void ClassicalMigrator::setIntervalMax(double value)
{
	this->sizeMin = value;
}

inline void ClassicalMigrator::setIntervalMin(double value)
{
	this->sizeMin = value;
}

inline void ClassicalMigrator::setSize(unsigned int value)
{
	this->size = value;
}

inline void ClassicalMigrator::setSizeMax(double value)
{
	this->sizeMax = value;
}

inline void ClassicalMigrator::setSizeMin(double value)
{
	this->sizeMin = value;
}

    }
}


#endif

