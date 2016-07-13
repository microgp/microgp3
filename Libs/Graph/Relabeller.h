/***********************************************************************\
|                                                                       |
| Relabeller.h |
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
 * @file Relabeller.h
 * A class that can translate ugp3 internal labels while translating
 * genotype to phenotype so that if a and b are genotype-equal, then they
 * are phenotype equal.
 */

#ifndef HEADER_UGP3_CTGRAPH_RELABELLER
#define HEADER_UGP3_CTGRAPH_RELABELLER

#include <string>
#include <unordered_map>
#include <InfinityString.h>

namespace ugp3 {
namespace ctgraph {

class Relabeller
{
public:
    Relabeller() {};
    
    // Do not allow copies or moves
    Relabeller(const Relabeller&) = delete;
    Relabeller(Relabeller&&) = delete;
    Relabeller& operator=(const Relabeller&) = delete;
    Relabeller& operator=(Relabeller&&) = delete;
    
    virtual const std::string& translate(const std::string& label) = 0;
};

class IdentityRelabeller: public Relabeller
{
public:
    virtual const std::string& translate(const std::string& label) {
        return label;
    }
};

class NormalizingRelabeller: public Relabeller
{
private:
    std::unordered_map<std::string, std::string> m_translations;
    InfinityString m_counter;
    
public:
    virtual const std::string& translate(const std::string& label);
};

}
}

#endif // HEADER_UGP3_CTGRAPH_RELABELLER
