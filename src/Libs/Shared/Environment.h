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

#ifndef HEADER_COMPAT_ENVIRONMENT
#define HEADER_COMPAT_ENVIRONMENT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <iostream>
#include <map>

#include "Convert.h"

#define GETENV(X) ::env_.getEnv(X)
#define SETENV(X, Y) ::env_.setEnv(X, Y)

/**	Simple wrapper for ENVIRONMENT functions
@author Giovanni Squillero */
class Environment
{
private:
    std::map<std::string, std::string> variables;
#ifdef HAVE_PUTENV
    std::map<std::string, std::string> putEnvParam;
#endif

public:
    Environment() { };

    ~Environment() { };

    bool setEnv(std::string name, std::string value);
    bool unsetEnv(std::string name) { return setEnv(name, nullptr); };
    const std::string getEnv(std::string name);
};

extern Environment env_;
#endif
