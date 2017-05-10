/***********************************************************************\
|                                                                       |
| CloneRecord.h                                                         |
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
#ifndef HEADER_UGP3_CORE_CLONERECORD
#define HEADER_UGP3_CORE_CLONERECORD

// NOTE file kept in tree for the following piece of documentation, should be moved somewhere else
// TODO maybe move it to ScaledFitness? or the wiki?

    /** A reference to the (best) clone individual.
        In a population of 4 individuals A, B, C, D, ordered by 
        best fitness, if A, C and D are clones, A is the master 
        clone of C and D.*/
//    IndividualType* masterClone;

    /** All data in this object are referred to individual.*/
//    IndividualType* individual;

    /** It represents the number of clones between individual and
        masterClone + 1. In a population of 4 individuals A, B, C, 
        D, ordered by best fitness, if A, C and D are clones:
    - A has numberOfClones = 0;
    - C has numberOfClones = 1;
    - D has numberOfClones = 2;*/
//    unsigned int numberOfClones;
    

#endif
