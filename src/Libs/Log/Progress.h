/**
 * @file Progress.h
 * Definition of the Progress class.
 * @see Progress.cc
 */

#ifndef HEADER_UGP3_LOG_PROGRESS
/** Defines that this file has been included */
#define HEADER_UGP3_LOG_PROGRESS

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <exception>

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
 * @class Progress
 * Keeps a double value between 0 and 1 to indicate the progress of a process
 */
class Progress
{
private:
    // Progress between 0 and 1 of a process
    const double progress;

public:
    /** 
     * Constructor of the class. Sets the value specified to the object.
     * @param value to set
     * @throws Any exception. Exception if the value is not between 0 and 1.
     */
    Progress(double value);
    /** 
     * Returns the double value of the actual progress
     * @returns double The double value of the actual progress
     * @throws Nothing. If an exception is thrown, the execution is aborted.
     */
    operator double() const;

public:
    /** Constant with the progress completed. Util to know if a progress is finished. */
    static const Progress START;
    static const Progress END;
};

inline Progress::operator double() const
{
    return this->progress;
}

}

}

#endif
