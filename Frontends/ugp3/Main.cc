/***********************************************************************\
|                                                                       |
| Main.cc                                                               |
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
 * @file Main.cc
 * Implements the main method.
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "ugp3_config.h"
#include "Program.h"

/* Conditional inclusions, needed to compile successfully under both Linux and Win32 systems */
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

/* Protos */
static inline void Banner(void);
static inline void Date(void);
static inline void Lock(void);
static inline void Unlock(void);

/* Lock */
#ifdef USE_FAKE_LOCK

#define LOCK_FILE "ugp3.lok"

static inline void Lock(void)
{
    FILE *F;
    
    if (F = fopen(LOCK_FILE, "r")) 
    {
        fclose(F);
        Banner();
        Date();
        cerr << "ERROR:: Found an existing \"" << LOCK_FILE << "\"" << endl << ends;
        Date();
        cerr << "INFO:: Another copy may be running in the same directory" << endl << ends;
        exit(EXIT_FAILURE);
    }
    
    F = fopen(LOCK_FILE, "w");
       
    if(!F) 
    {
        Banner();
        Date();
        cerr << "ERROR:: Can't create \"" << LOCK_FILE << "\"" << endl << ends;
        Date();
        cerr << "INFO:: Check write permissions in the current directory" << endl << ends;
        exit(EXIT_FAILURE);
    } 
    else 
    {
        fclose(F);
    }
}

static inline void Unlock(void)
{   
    unlink(LOCK_FILE);
}

#else

#define LOCK_FILE "ugp3.pid"
int LockFD;

static inline void Lock(void)
{
    char buf[256];
    
    /* 
     * NOTE: not all systems support open's O_CLOEXEC (which was only standardized in SUSv4)
     */
    LockFD = open(LOCK_FILE, O_RDWR | O_CREAT | O_CLOEXEC, S_IRUSR | S_IWUSR);
    if (LockFD == -1) 
    {
        Banner();
        Date();
        cerr << "ERROR:: Can't create PID file \"" << LOCK_FILE << "\"" << endl << ends;
        Date();
        cerr << "INFO:: Check write permissions in the current directory" << endl << ends;
        exit(EXIT_FAILURE);
    }

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;

    if (fcntl(LockFD, F_SETLK, &fl) == -1) 
    {
        Banner();
        Date();
        cerr << "ERROR:: Can't lock PID file \"" << LOCK_FILE << "\"" << endl << ends;
        Date();
        cerr << "INFO:: Another copy may be running in the same directory" << endl << ends;
        exit(EXIT_FAILURE);
    }

    if (ftruncate(LockFD, 0) != 0) {
        Date();
        cerr << "ERROR:: Can't truncate PID file \"" << LOCK_FILE << "\"" << endl << ends;
        exit(EXIT_FAILURE);
    }
    snprintf(buf, 256, "%ld\n", (long)getpid());
    if (write(LockFD, buf, strlen(buf)) == -1) {
        Date();
        cerr << "ERROR:: Can't write to PID file \"" << LOCK_FILE << "\"" << endl << ends;
        exit(EXIT_FAILURE);
    }

}

static inline void Unlock(void)
{   
    close(LockFD);
    unlink(LOCK_FILE);
}

#endif


/** 
 * Initialites and starts the program execution with the arguments and configuration stablished.
 * @param argumentCount Numbers of arguments.
 * @param arguments Arguments.
 * @see ugp3::frontend::Argument
 * @see ugp3::frontend::Program
 */
// The two-argument signature for main has int as the first argument. GCC 4.3 rigorously enforces this.
int main(int argc, char* argv[])
{
    _STACK;

#if defined(_DEBUG) && defined(_MSC_VER)
    // Enable memory leak detection in Microsoft Visual Studio
    // (memory leaks are reported in the debug output window)
    _CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Change the value of this field to break
    // the execution at the n-th memory allocation
    // (set it to -1 to desable breakpoint triggering)
    // (see http://msdn2.microsoft.com/en-us/library/x98tx3cf(VS.80).aspx for more information)
    _crtBreakAlloc = -1;
#endif

    Lock();
    int r = ugp3::frontend::Program::start(argc, argv);
    Unlock();

    return r;
}

static inline void Banner(void)
{
    cerr << ugp3::frontend::Info::getTagline() << endl
    << ugp3::frontend::Info::description << endl
    << COPYRIGHT_NOTE << endl
    << "This is free software, and you are welcome to redistribute it under certain" << endl << "conditions (use option \"--" << ugp3::frontend::Argument::License << "\" for details)"
    << endl << endl << ends;
}

static inline void Date(void)
{
    struct tm *now;
    time_t raw;
    
    time(&raw);
    now = localtime(&raw);
    cerr << "[" << setw(2) << now->tm_hour << ":" << setw(2) << now->tm_min << ":" << setw(2) << now->tm_sec << "] ";   
}

