/***********************************************************************\
|                                                                       |
| File.h                                                                |
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
 * @file File.h
 * Definition of the File class.
 * @see File.cc
 */

#ifndef HEADER_UGP3_FILE
/** Defines that this file has been included */
#define HEADER_UGP3_FILE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <vector>

/**
 * ugp3 namespace
 */
namespace ugp3
{
    /**
     * @class File
     * Provides some methods to manage files.
     */
    class File
    {
    public:
        /** 
         * Returns if a file exists
         * @param name of the file
         * @returns bool True if the file exists, false in other case
         */
        static bool exists(const std::string& name);
        /** 
         * Returns the extension of the file
         * @param name of the file
         * @returns string The extension of the file
         */
        static std::string getExtension(const std::string& name);
        /** 
         * Appends the actual time at the end of the name but before the extension 
         * @param name String in which append the token
         * @returns string The resulting string
         */
        static std::string appendTimeToName(const std::string& name);
        
        /** 
         * Creates a filename using a printf with format and token as 
         * argument 
         * @param format Format string (as printf)
         * @param token String to use in %s
         * @returns string The resulting string
         */
        static std::string formatToName(const std::string& format, const std::string& token);
        
        /** 
         * Appends the token at the end of the name but before the extension 
         * @param name String in which append the token
         * @param token String to append
         * @returns string The resulting string
         */
        static std::string appendStringToName(const std::string& name, const std::string& token);
        /** 
         * Removes a file
         * @param name of the file to remove
         * @returns bool True if the file has been removed, false in other case
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
        static bool remove(const std::string& name);
        /** 
         * Renames a file
         * @param oldName Name of the file to rename
         * @param newName New name for the file
         * @returns bool True if the file has been renamed, false in other case
         * @throws nothing. if an exception is thrown, the execution is aborted.
         */
        static bool rename(const std::string& oldName, const std::string& newName);
        /**
         * Gets the list of file names with a given pattern
         * @param pattern Pattern (usually something like "name*" or "*.txt" or "*name*.txt"
         * @returns std::vector<std::string> list of filenames
         */
        static std::vector<std::string> getList(const std::string& pattern);
        /**
         * Creates a directory. If the directory already exists, does nothing.
         * @param name Name of the directory
         * @throws std::exception if the directory can not be created.
         */
        static void createDirectory(const std::string& name);
        /**
         * Put a platform-depently-inclined-slash between the two string.
         * @param dir Directory name.
         * @param name File name.
         * @return dir/name or dir\name depending on the platform.
         */
        static std::string concat(const std::string& dir, const std::string& name);
    };
}

#endif
