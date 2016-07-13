/***********************************************************************\
|                                                                       |
| UniqueIdCounter.cc                                                    |
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

#ifndef _UNIQUEIDCOUNTER_H
#define _UNIQUEIDCOUNTER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "InfinityString.h"
#include "IString.h"

namespace ugp3
{

 /**
    Class UniqueIdCounter

    This class represents a unique ID. The unicity is granted by a static member
    and the number of unique ID depends on the implementation of HugeNumber.

    \date
    @author Luca Motta
    @version 1
   */
    class UniqueIdCounter : public IString
    {
        static InfinityString idCounter;

     protected:
         std::string id;

     public:
      /**
         UniqueIdCounter. Makes a new instance of idCounter with a unique identifier of type
        */
      UniqueIdCounter();

  /**
     Get the ID represented by this UniqueIdCounter
    */
  inline const InfinityString getId() const;

  /**
     Try resuming with a given state. Returns false if already resumed

     @param state The desired state
    */
  bool resume( std::vector<char>& );

  const std::string toString() const;
  /**
     Compare the ID of this instance with another UniqueIdCounter

     @param uniqueId The right object to be compared
    */
  /**
     Compare the ID of this instance with a HugeNumber

     @param uniqueId The HugeNumber to be compared with this object
    */

  virtual ~UniqueIdCounter();
 };
}

namespace ugp3{
 inline const std::string UniqueIdCounter::toString() const{
  return id;
 }

}

#endif // _UNIQUEIDCOUNTER_H
