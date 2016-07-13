/***********************************************************************\
|                                                                       |
| Info.h                                                                |
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
 * @file Info.h
 * Definition of the Info class.
 * @see Info.cc
 */

#ifndef HEADER_UGP3_FRONTEND_INFO
/** Defines that this file has been included */
#define HEADER_UGP3_FRONTEND_INFO

#include "Version.h"

/**
 * ugp3 namespace
 */
namespace ugp3
{

/**
 * ugp3::frontend namespace
 */
namespace frontend
{

/**
 * @class Program
 * Static class that keeps information about the application
 */
class Info {
  public:
    /** 
     * Returns information about the compilation
     * @returns string Information about the compilation
     */
    static std::string getCompileInfo(void);
    /** 
     * Returns the version of the application
     * @returns string The version of the application
     */
    static std::string getVersion(void);
    /** 
     * Returns the name and version of the application to use like a tag in order to show it
     * @returns string Name and version of the application
     */
    static std::string getTagline(void);
    /** 
     * Returns information about the platform and operative system where the application is running
     * @returns string Information about the platform
     */
    static std::string getSystemType(void);

  public:
    /** Name of this application */
    static const std::string name;
    /** Description of this application */
    static const std::string description;
    /** Authors of this application */
    static const std::string authors;

  public:
    /** 
     * Shows information about the copyright of the application. NOT IMPLEMENTED.
     */
    static void displayCopyright(void);
};

}

}

#endif


