/**
 * @file Location.h
 * Definition of the Location class.
 * @see Location.cc
 */

#ifndef HEADER_UGP3_LOG_LOCATION
/** Defines that this file has been included */
#define HEADER_UGP3_LOG_LOCATION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
 * @class Location
 * Provides additional information on the location in the code of the log message.
 * @see Log
 * @author Salomone Alessandro
 */
class Location
{
private:
    // The name of the file where the message in which the logger is invoked.
    std::string file;

    // The name of the function or method in which the logger is invoked.
    std::string function;

    // The line in the file where the logger is invoked.
    unsigned int line;

public:
    /**
     * Creates a copy of the specified instance.
     * @param location A reference to a valid instance of a Log::Location class. 
     */
    Location(const Location& location);

    /** 
     * Initializes a new instance of the Location class with the specified attributes.
     * @param function The name of the function or method in which the message is written.
     * @param file The name of the file where the message in which the logger is invoked.
     * @param line The line in the file where the message is written.
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    Location(
        const std::string& file = "unknownFile", 
        const std::string& function = "unknownFunction", 
        unsigned int line = 0);

    /**
     * Gets the line in the file where the logger is invoked.
     * @return An unsigned integer indicating the line of code.
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    unsigned int getLine() const;

    /**
     * Returns the name of the function or method in which the logger is invoked.
     * @return string The name of the function or method.
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    const std::string& getFunction() const;

    /**
     * Returns the name of the file where the message in which the logger is invoked.
     * @return string The name of the file.
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    const std::string& getFile() const;

    /**
     * Returns a Location with the values of the Location specified
     * @param Location with the values to copy 
     * @return Location with the values copied
     */
    Location& operator=(const Location& location);
};

inline unsigned int Location::getLine() const
{
    return this->line;
}

inline const std::string& Location::getFunction() const
{
    return this->function;
}

inline const std::string& Location::getFile() const
{
    return this->file;
}

}

}


#endif
