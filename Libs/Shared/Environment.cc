/***********************************************************************\
|                                                                       |
| Environment.h                                                         |
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

#include "ugp3_config.h"
#include <iostream>

#include "Environment.h"
#include "Log.h"
#include "Debug.h"
using namespace std;

#if defined(USE_PUTENV) && defined(USE_SETENV)
#error "Can't define both USE_PUTENV and USE_SETENV"
#endif

Environment env_;

bool Environment::setEnv(string name, string value)
{
     bool alreadyPresent;

     if(variables.find(name) != variables.end()) {
	  alreadyPresent = true;
	  variables.erase(name);
     } else {
	  alreadyPresent = false;
     }

     // Set up the "virtual" environment
     if(value.empty())
	  variables.erase(name);
     else
	  variables[name] = value;

     // Ok, now let's do it
#if defined USE_PUTENV
     // FIXME the string given to putenv should not be temporary,
     // since putenv does not copy it. Here the string gets
     // destroyed at the end of the scope.
     // http://www.greenend.org.uk/rjk/tech/putenv.html
     string tempEnv;
     tempEnv.append(name.c_str());
     if(!value.empty()) {
	  // libc4,  libc5,  glibc only!
	  tempEnv.append("=");
	  tempEnv.append(value.c_str());
     }
     putenv((char *)tempEnv.c_str());
#elif defined USE_SETENV
    if(value.empty()) {
	 unsetenv(name.c_str());
    } else {
	 setenv(name.c_str(), value.c_str(), 1);
    }
#endif

    return alreadyPresent;
}

const string Environment::getEnv(string name)
{
     if(variables.find(name) != variables.end()) {
	  return variables[name];
     } else {
#ifdef USE_GETENV
	  return getenv(name.c_str())?getenv(name.c_str()):"";
#else
	  return "";
#endif
     }
}
