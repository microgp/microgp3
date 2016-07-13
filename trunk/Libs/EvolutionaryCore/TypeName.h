/***********************************************************************\
|                                                                       |
| TypeName.h |
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
| $Revision: 652 $
| $Date: 2015-03-09 15:02:22 +0100 (Mon, 09 Mar 2015) $
\***********************************************************************/

/**
 * @file TypeName.h
 * Display meaningful type information in templated functions.
 */

#ifndef HEADER_UGP3_CORE_TYPENAME
#define HEADER_UGP3_CORE_TYPENAME

namespace ugp3 {
namespace core {
    
template <typename T> struct TypeName      { static constexpr auto name = "<unknown type>"; };
template <typename T> struct TypeName<T*>  { static constexpr auto name = TypeName<T>::name; };
template <typename T> struct TypeName<T*&> { static constexpr auto name = TypeName<T>::name; };

}
}

#endif // HEADER_UGP3_CORE_TYPENAME
