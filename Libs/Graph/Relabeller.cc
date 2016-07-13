/***********************************************************************\
|                                                                       |
| Relabeller.cc |
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
 * @file Relabeller.cc
 *
 */

#include "Relabeller.h"

const std::string& ugp3::ctgraph::NormalizingRelabeller::translate(const std::string& label)
{
    auto translation = m_translations.find(label);
    if (translation == m_translations.end()) {
        std::tie(translation, std::ignore) = m_translations.insert(std::make_pair(label, m_counter.toString()));
        ++m_counter;
    }
    return translation->second;
}
