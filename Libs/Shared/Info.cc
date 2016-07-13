/***********************************************************************\
|                                                                       |
| Info.cc                                                               |
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
 * @file Info.cc
 * Implementation of the Info class.
 * @see Info.h
 */

#include "ugp3_config.h"

#include <string>
#include <sstream>
#include "Info.h"
#include "Log.h"
using namespace ugp3::frontend;
using namespace std;

#ifndef UGP3_VERSION_NAME
/** Name of this ugp version */
#define UGP3_VERSION_NAME "Unknown Soldier"
#endif

#ifndef UGP3_VERSION
/** Version of ugp */
#define UGP3_VERSION ""
#endif

#ifndef UGP3_SUBVERSION
/** Subversion of the compilation */
#define UGP3_SUBVERSION ""
#endif

#ifndef UGP3_REVISION
/** Revision of this compilation */
#define UGP3_REVISION ""
#endif

// initialization of the static members of the Program class
const std::string Info::name = "ugp3 (MicroGP++)";
const std::string Info::description = "Yet another multi-purpose extensible self-adaptive evolutionary algorithm";

std::string Info::getCompileInfo(void)
{
     char            a[8], b[8], c[8], d[16];

     sscanf(__DATE__ " " __TIME__, "%s %s %s %s", a, b, c, d);

     std::ostringstream cinfo;
     cinfo << b << "-" << a << "-" << c << " " << d;
#if defined(__GNUG__) && defined(__VERSION__)
     cinfo << " with GNU g++ " __VERSION__;
#elif defined(_MSC_FULL_VER)
     cinfo << " with Microsoft VisualC++ [" << _MSC_FULL_VER << "]";
#endif

     return cinfo.str();
}

std::string Info::getVersion(void)
{
	std::ostringstream version;

	if(!*UGP3_VERSION) {
	    version << "3";
	} else if(!*UGP3_SUBVERSION) {
	    version << "3." UGP3_VERSION;
	} else if(!*UGP3_REVISION) {
	    version << "3." UGP3_VERSION "." UGP3_SUBVERSION;
	} else {
	    version << "3." UGP3_VERSION "." UGP3_SUBVERSION "_" UGP3_REVISION;
	}

    return version.str();
}

std::string Info::getTagline(void)
{
	std::ostringstream tagline;

    tagline << name << " v" << getVersion() << " \"" UGP3_VERSION_NAME "\"";
    return tagline.str();
}

std::string Info::getSystemType(void)
{
    std::ostringstream systemType;

     systemType << "System is ";
#if defined(__AMD64__) || defined(__AMD64) || defined(__amd64__) || defined(__amd64)
     systemType << "amd64";
#elif defined(__IA64__) || defined(__IA64) || defined(__ia64__) || defined(__ia64)
     systemType << "ia64";
#elif defined(X86_64) || defined(__X86_64) || defined(__X86_64__)
     systemType << "x86-64";
#elif defined(__LP64__) || defined(__LP64)
     systemType << "x64";
#elif defined(__AMD32__) || defined(__AMD32)
     systemType << "amd32";
#elif defined(__ia32__) || defined(__ia32)
     systemType << "ia32";
#elif defined(i386) || defined(__i386) || defined(__i386__)
     systemType << "i386";
#elif defined(i486) || defined(__i486) || defined(__i486__)
     systemType << "i486";
#elif defined(i586) || defined(__i586) || defined(__i586__)
     systemType << "i586";
#elif defined(i686) || defined(__i686) || defined(__i686__)
     systemType << "i686";
#elif defined(_M_IX86)
	 systemType << "x86";
#elif defined(sparc) || defined(__sparc) || defined(__sparc__)
     systemType << "sparc";
#elif defined(mc68000) || defined(__mc68000) || defined(__mc68000__)
     systemType << "mc68000";
#elif defined(_IBMR2) || defined(___IBMR2) || defined(___IBMR2__)
     systemType << "ibm";
#elif defined(_POWER) || defined(___POWER) || defined(___POWER__) || defined(_M_PPC)
     systemType << "powerpc";
#elif defined(alpha) || defined(__alpha) || defined(__alpha__)
     systemType << "alpha";
#else
     systemType << "unknown";
#endif

#if defined(_win32) || defined(_WIN32) || defined(__win32) || defined(__WIN32) || defined(_win32_) || defined(_WIN32_) || defined(__win32__) || defined(__WIN32__)
     systemType << "/win32";
#elif defined(__APPLE__)
    systemType << "/macos";
#elif defined(_gnu_linux) || defined(_GNU_LINUX) || defined(__gnu_linux) || defined(__GNU_LINUX) || defined(_gnu_linux_) || defined(_GNU_LINUX_) || defined(__gnu_linux__) || defined(__GNU_LINUX__)
    systemType << "/gnu-linux";
#elif defined(_linux) || defined(_LINUX) || defined(__linux) || defined(__LINUX) || defined(_linux_) || defined(_LINUX_) || defined(__linux__) || defined(__LINUX__)
     systemType << "/linux";
#elif defined(_unix) || defined(_UNIX) || defined(__unix) || defined(__UNIX) || defined(_unix_) || defined(_UNIX_) || defined(__unix__) || defined(__UNIX__)
     systemType << "/unix";
#endif
     systemType << ", ";

    if(sizeof(void*) == sizeof(long) && sizeof(long) > sizeof(int)) {
      systemType << "normal " << sizeof(void*)*8 << "-bit architecture (int is " << sizeof(int) << " bytes, long is " << sizeof(long) << " bytes)";
    } else if(sizeof(long) > sizeof(int)) {
      systemType << "looked like a " << sizeof(void*)*8 << "-bit architecture (int is " << sizeof(int) << " bytes, long is " << sizeof(long) << " bytes)";
    }  else if(sizeof(size_t) < sizeof(void*) && sizeof(int)==2) {
      systemType << "oldish 16-bit  architecture (int is " << sizeof(int) << " bytes, long is " << sizeof(long) << " bytes)";
    } else if(sizeof(void*) == sizeof(int)) {
      systemType << "usual " << sizeof(void*)*8 << "-bit architecture (int is " << sizeof(int) << " bytes, long is " << sizeof(long) << " bytes)";
    } else {
      systemType << "uknown architecture";
    }

    return systemType.str();
}
