/**
 * @file Level.h
 * Definition of the Level class.
 * @see Level.cc
 */

#ifndef HEADER_UGP3_LOG_LEVEL
#define HEADER_UGP3_LOG_LEVEL

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Enumeration.h"
#include <exception>
#include <string>

/**
 * ugp3 namespace
 */
namespace ugp3
{

/**
 * ugp3::log namespace
 */
namespace log
{

 /**
  * @class Level
  * Enumeration of a verbosity Level. Each instance keeps an integer and a description. This class also defines the static const verbosity levels supported by the application.
  * @see Log
  */
class Level : public Enumeration
{
public:
    /** 
     * Creates an object of Level with the specified value and description. 
     * @param value Unsigned integer value of the enumeration
     * @param description of the enumeration type
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    Level(unsigned int value, const char* description);
    /** 
     * Returns the corresponding Level of the description(value parameter) specified
     * @param value Description of the enumeration type
     * @throws Any exception. std::runtime_error if there isn't a level of the specified description
     */
    static Level parse(const std::string& value);
    
public:
    /** Silent verbosity level for the Log */
    static const Level Silent;
    /** Error verbosity level for the Log */
    static const Level Error;
    /** Warning verbosity level for the Log */
    static const Level Warning;
    /** Summary verbosity level for the Log */
    static const Level Summary;
    /** Info verbosity level for the Log */
    static const Level Info;
    /** Verbose verbosity level for the Log */
    static const Level Verbose;
    /** Debug verbosity level for the Log */
    static const Level Debug;

public:
    /** 
     * Compares the integer value of this level with another one
     * @param level Level to compare with
     * @returns bool True if this Level has a lower number that the one specified, false in other case
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    bool operator<(const Level& level) const;
    /** 
     * Compares the integer value of this level with another one
     * @param level Level to compare with
     * @returns bool True if this Level has a lower or equal number that the one specified, false in other case
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    bool operator<=(const Level& level) const;
    /** 
     * Compares the integer value of this level with another one
     * @param level Level to compare with
     * @returns bool True if this Level has a higher number that the one specified, false in other case
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    bool operator>(const Level& level) const;
    /** 
     * Compares the integer value of this level with another one
     * @param level Level to compare with
     * @returns bool True if this Level has a higher or equal number that the one specified, false in other case
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    bool operator>=(const Level& level) const;
};


inline bool Level::operator<(const Level& level) const
{
    return this->value < level.value;
}


inline bool Level::operator<=(const Level& level) const
{
    return this->value <= level.value;
}


inline bool Level::operator>(const Level& level) const
{
    return this->value > level.value;
}


inline bool Level::operator>=(const Level& level) const
{
    return this->value >= level.value;
}



}

}

#endif
