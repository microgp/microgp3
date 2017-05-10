/***********************************************************************\
|                                                                       |
| IMigrator.h                                                           |
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

#ifndef HEADER_UGP3_CORE_IMIGRATOR
#define HEADER_UGP3_CORE_IMIGRATOR

#include "XMLIFace.h"

#include <vector>

#include "Exceptions/ArgumentNullException.h"

using namespace std;

namespace ugp3
{
    namespace core
    {
	// forward declaration
        class Population;
        class EvolutionaryAlgorithm;

        /** Defines a simple interface for the implemenation of a strategy for the migration of individuals between populations.
            @autor Salomone Alessandro */
        class IMigrator
        {
        public:
	    static const string XML_NAME;
            /** Migrates the individuals between the given populations using a specific strategy.
                @param populations The collection of populations involved in the migration process.
                */
            virtual void migrate(std::vector<Population*>* populations) = 0;
            virtual ~IMigrator() = 0;
	    IMigrator* instantiate(	const EvolutionaryAlgorithm& parent, const string& type);
        };
    }
}

#endif
