/***********************************************************************\
|                                                                       |
| MicroGPSettings.h                                                     |
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
| the Free Software Foundation, either version 3 of the License, or     |
| (at your option) any later version.                                   |
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
 * @file MicroGPSettings.h
 * Definition of the MicroGPSettings class.
 * @see MicroGPSettings.cc
 * @see Settings.h
 */

#ifndef HEADER_UGP3_MICROGPSETTINGS
/** Defines that this file has been included */
#define HEADER_UGP3_MICROGPSETTINGS

#include "Settings.h"
#include "Argument.h"

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
                 * @class MicroGPSettings
                 * Manages the settings of ugp
                 * @see Settings
                 */
		class MicroGPSettings : public Settings
		{
		public:
                  /** 
                   * Constructor of the class. It does nothing.
                   * @throws nothing. if an exception is thrown, the execution is aborted.
                   */
	          MicroGPSettings();
                  /** 
                   * Initilizes all the setting contexts and its options with descriptions
                   */
	          void initialize();
                  /** 
                   * Initilizes all the setting contexts and its options with descriptions
                   */
	          void initializeSettingsFile();
  
                  /** 
                   * Destructor of the class. It does nothing.
                   * @throws nothing. if an exception is thrown, the execution is aborted.
                   */
	          virtual ~MicroGPSettings();
		};

		inline MicroGPSettings::MicroGPSettings()
		{ }

		inline MicroGPSettings::~MicroGPSettings()
		{ }
	}
}

#endif
