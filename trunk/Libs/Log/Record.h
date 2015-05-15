/**
 * @file Record.h
 * Definition and implementation of the Record class.
 */

#ifndef HEADER_UGP3_LOG_RECORD
/** Defines that this file has been included */
#define HEADER_UGP3_LOG_RECORD

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include "Location.h"
#include "Level.h"
#include "Progress.h"

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
  * @class Record
  * Class to save information about an event of the application.
  * @see Formatter
  * @see Handler
  */
class Record
{
private:
    // Message of the Record
    const std::string& message;
    // Verbosity level to show about the Record
    const Level& level;
    // Location where the Record is generated
    const Location& location;
    // Structure of time (defined in time.h) to save the time of the Record
    const ::tm& time;
    // Progress when the record is saved
    const Progress* progress;
 
private:
    // Copy constructor. Not implemented.
    Record(const Record&);
    Record& operator=(const Record&);

public:
    /** 
     * Constructor of the class. Instantiates a Record object with the specified parameters and with a null progress.
     * @param message Message of the record to save
     * @param level Verbosity level of the Record
     * @param location Location where the Record is generated
     * @param time Structure time with the time of the Record
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    Record(
        const std::string& message, 
        const Level& level, 
        const Location& location, 
        const ::tm& time);

    /** 
     * Constructor of the class. Instantiates a Record object with the specified parameters.
     * @param message Message of the record to save
     * @param level Verbosity level of the Record
     * @param location Location where the Record is generated
     * @param time Structure time with the time of the Record
     * @param progress Progress when the Record is saved
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    Record(
        const std::string& message, 
        const Level& level, 
        const Location& location, 
        const ::tm& time,
        const Progress& progress);
    
public:
    /** 
     * Returns the message of the Record
     * @returns string Message of the Record
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    const std::string& getMessage() const;    
    /** 
     * Returns the verbosity level of the Record
     * @returns Level Verbosity level of the Record
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    const Level& getLevel() const;
    /** 
     * Returns the Location where the Record is produced
     * @returns Location Location where the Record is produced
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    const Location& getLocation() const;
    /** 
     * Returns the time when the Record is produced
     * @returns tm Time when the Record is produced
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    const ::tm& getTime() const;
    /** 
     * Returns the Progress when the Record is produced
     * @returns Progress Progress when the Record is produced
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    const Progress& getProgress() const;
    /** 
     * Returns if the Record has a Progress asigned or not
     * @returns bool True if the Record has Progress, false in other case
     * @throws nothing. if an exception is thrown, the execution is aborted.
     */
    bool hasProgress() const;
};

inline Record::Record(
    const std::string& message, 
    const Level& level, 
    const Location& location, 
    const ::tm& time)
 : message(message), level(level), location(location), time(time), progress(nullptr)
{
}

inline Record::Record(
    const std::string& message, 
    const Level& level, 
    const Location& location, 
    const ::tm& time,
    const Progress& progress)
 : message(message), level(level), location(location), time(time), progress(&progress)
{ }

inline const std::string& Record::getMessage() const
{
    return this->message;
}

inline const Level& Record::getLevel() const
{
    return this->level;
}

inline const Location& Record::getLocation() const
{
    return this->location;
}

inline const ::tm& Record::getTime() const
{
    return this->time;
}

// modified here previously the function returned const bool, but gave rise to a warning
// "warning: type qualifiers ignored on function return type [-Wignored-qualifiers]"
inline bool Record::hasProgress() const
{
    return this->progress != nullptr;
}

inline const Progress& Record::getProgress() const
{
    return *this->progress;
}

}

}

#endif
