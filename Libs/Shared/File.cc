/***********************************************************************\
|                                                                       |
| File.cc                                                               |
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
 * @file File.cc
 * Implementation of the File class.
 * @see File.h
 */

#include "ugp3_config.h"
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <iostream>

#include <sys/stat.h>

#include <dirent.h>
#include <string.h>

#include "File.h"
#include "Exception.h"
#include <FileException.h>

#ifdef WINDOWS
#include <windows.h>
#include <direct.h>
#endif

using namespace ugp3;
using namespace std;

bool File::exists(const std::string& name)
{
	// trying to open the file is not good practice; the function is thus modified to use "stat"
	// the only problem might be the cross-compiling...but it should work
	struct stat stFileInfo;
	int fileNotFound;
	
	fileNotFound = stat(name.c_str(), &stFileInfo);
	
	// stat returns 0 if the file exists, otherwise an integer
	if( fileNotFound )
		return false;
	else
		return true;
	/*
	ifstream stream;
	stream.open(name.c_str());

	bool exists = stream.is_open() == true;

	stream.close();

	return exists;
	*/
}

string File::getExtension(const string& name)
{
	string::size_type position = name.find_last_of(".");
	if(position == string::npos)
	{
		return "";
	}

	return name.substr(position, name.size()-1);
}

string File::appendTimeToName(const string& name)
{
	::time_t timeNow;
	::time(&timeNow);
	::tm* timeinfo = localtime(&timeNow);

	ostringstream timeString;

	timeString << setfill('0');
	timeString << "_" << timeinfo->tm_year + 1900 << "-"
		<< setw(2) << timeinfo->tm_mon + 1 << "-"
		<< setw(2) << timeinfo->tm_mday << ","
		<< setw(2) << timeinfo->tm_hour << "-"
		<< setw(2) << timeinfo->tm_min << "-"
		<< setw(2) << timeinfo->tm_sec;
	
	return File::appendStringToName(name, timeString.str());
}

string File::formatToName(const string& format, const string& token)
{
	char* buffer;

	buffer = (char *)malloc(sizeof(char) * (format.length()+token.length()) );
	sprintf(buffer, format.c_str(), token.c_str());

	string newBuffer(buffer);

	free(buffer);

	return newBuffer;

	/*
	char buffer[format.length()+token.length()];
	sprintf(buffer, format.c_str(), token.c_str());
	return string(buffer);
	*/
}

string File::appendStringToName(const string& name, const string& token)
{
	// search for the last '.' and append the string after the character ...
	string::size_type position = name.find_last_of(".");
	if(position == string::npos)
	{
		position = name.size();
	}
	
	// ... but avoid strings like "./", "..", ".\" ...
	if(position + 1 < name.size())
	{
		if(name[position + 1] == '.' || name[position + 1] == '/' || name[position + 1] == '\\')
		{
			position = name.size();
		}
	}

	// ... and avoid strings like "/.", "..", "\."
	if(position > 0)
	{
		if(name[position - 1] == '.' || name[position - 1] == '/' || name[position - 1] == '\\')
		{
			position = name.size();
		}
	}

	string fileName = name;

	return fileName.insert(position, token);
}

bool File::remove(const std::string& name)
{
         // call to std::remove
	 return ::remove(name.c_str()) != -1;
}

bool File::rename(const std::string& oldName, const std::string& newName)
{
    // call to std::rename
    return ::rename(oldName.c_str(), newName.c_str()) != 0; //TODO: I think it should be compared with -1. NAVAS.
}

vector<string> File::getList(const string& pattern)
{
	vector<string> fileList;
	
	// TODO: cross-platform method to find a list of files with a certain pattern

#ifdef WINDOWS
// Windows TODO: everything, it is not tested...it should list files with a pattern
	HANDLE dir;
	WIN32_FIND_DATA file_data;

	if ((dir = FindFirstFile(pattern.c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return fileList; /* No files found */

	do 
	{
		const string file_name = file_data.cFileName;
		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (file_name[0] == '.')
			continue;

		if (is_directory)
			continue;

		fileList.push_back(file_name);
	}
	while (FindNextFile(dir, &file_data));

	FindClose(dir);
#else
// Unix, Mac... TODO: almost complete, BUT it still does not work as a proper "ls"; it should take into account the "*" at the
// beginning and at the end of a pattern, in some way. But it's complicated.
	DIR *dir;
	class dirent *ent;
	class stat st;

	// isolate directory name from pattern
	string directoryName;
	string filePattern;

	// path is absolute, so find the first '/' and use that as a pattern; the rest as a directory
	unsigned int p = pattern.size() - 1;
	while( p > 0 && (pattern[p] != '/' || pattern[p-1] == '\\') ) p--;
	
	if( p != 0 )
	{
		directoryName = pattern.substr(0, p);
		filePattern = pattern.substr(p+1, pattern.size() - p);
	}
	else
	{
		directoryName = ".";
		filePattern = pattern;
	}
	
	// remove all '*' from pattern
	string filePatternTemp;
	vector<string> filePatternClean;
	for(unsigned int i = 0; i < filePattern.size(); i++)
	{
		if( filePattern[i] != '*' )
		{
			filePatternTemp += filePattern[i];
		}
		else
		{
			// when we meet a '*', create a new string
			filePatternClean.push_back(filePatternTemp);
			filePatternTemp.clear();
		}
	}
	
	/* debugging
	cout 	<< "Directory name is \"" << directoryName 
		<< "\"; file pattern is:";
	for(unsigned int i = 0; i < filePatternClean.size(); i++)
		cout << " \"" << filePatternClean[i] << "\"";
	cout << endl;
	*/

	dir = opendir( directoryName.c_str() );
	
	if( !dir )
	{
		cerr << "Cannot open directory \"" << directoryName << "\"." << endl;
		return fileList;
	}

	while((ent = readdir(dir)) != nullptr)
	{
		const string file_name = ent->d_name;
		const string full_file_name = directoryName + "/" + file_name;

		if( file_name[0] != '.' && stat(full_file_name.c_str(), &st) != -1 && (st.st_mode & S_IFDIR) == 0 )
		{
			unsigned int previous_position = 0;
			bool string_compliant = true;
			
			for(unsigned int i = 0; i < filePatternClean.size() && string_compliant; i++)
			{
				// don't check empty strings, something like (*match*) would produce "", "match", ""
				if( filePatternClean[i].length() != 0)
				{
					// look for pattern, starting from the last pattern found (order is important)
					auto position = file_name.find( filePatternClean[i].c_str(), previous_position );
					
					if( position == string::npos ) string_compliant = false;
					
					previous_position = position + filePatternClean[i].length() - 1;
				}
			}
			
			if( string_compliant == true )
				fileList.push_back(full_file_name);
		}
		/*
		if(file_name[0] == '.')
			continue;

		if(stat(full_file_name.c_str(), &st) == -1)
			continue;

		const bool is_directory = (st.st_mode & S_IFDIR) != 0;

		if(is_directory)
			continue;
		*/
	}
	closedir(dir);
#endif
	/* debugging
	cout << "File list retrieved is:";
	for(unsigned int i = 0; i < fileList.size(); i++)
		cout << " " << fileList[i];
	cout << endl;
	*/

	return fileList;
} 

void File::createDirectory(const string& name)
{
    struct stat st;
#ifdef WINDOWS
    bool created = _mkdir(name.c_str()) == 0;
#else
    bool created = mkdir(name.c_str(), 0700) == 0;
#endif
    if (!created && !(errno == EEXIST && stat(name.c_str(), &st) == 0 && S_ISDIR(st.st_mode))) {
        throw new FileException(name, "Impossible to create directory.", LOCATION);
    }
}

std::string File::concat(const string& dir, const string& name)
{
#ifdef WINDOWS
    return dir + "\\" + name;
#else
    return dir + "/" + name;
#endif
}
