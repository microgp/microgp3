/***********************************************************************\
|                                                                       |
| IEquatable.h                                                          |
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

#ifndef HEADER_UGP3_SIGNALHANDLING
#define HEADER_UGP3_SIGNALHANDLING

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <signal.h>
#include "Log.h"


#include <signal.h>


namespace ugp3
{
   static bool SigIntRequested = false;
   static bool deferSignals = false;
   static std::string SigIntMessage;
   static int num = 0;
      
   void signalHandler(int code)
   {
   		if(code == SIGINT && num < 3)
   		{
            std::cerr << std::endl << "Caught INT signal. " << SigIntMessage << std::endl;
	   		++num;
	   		
	   		if(deferSignals == true)
	   		{
		   		SigIntRequested = true;
	   		}
	   		else 
	   		{
                signal(code, SIG_DFL);
                raise(code);
	   		}
   		} else {
            std::cerr << std::endl << "Yeuch. Caught another INT signal." << std::endl;
            signal(code, SIG_DFL);
            raise(code);
        }
   }
}
#endif

