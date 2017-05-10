/***********************************************************************\
|                                                                       |
| Distances.h |
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
 * @file Distances.h
 * Blah
 */

#ifndef HEADER_UGP3_DISTANCES
#define HEADER_UGP3_DISTANCES

#include <vector>
#include <algorithm>
#include <Entropy.h>

namespace ugp3 {

class Distances
{
public:
    // https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance
    template<class T>
    static unsigned int levenshtein(const T &s1, const T & s2)
    {
        const size_t len1 = s1.size(), len2 = s2.size();
        std::vector<unsigned int> col(len2+1), prevCol(len2+1);
        
        for (unsigned int i = 0; i < prevCol.size(); i++)
            prevCol[i] = i;
        for (unsigned int i = 0; i < len1; i++) {
            col[0] = i+1;
            for (unsigned int j = 0; j < len2; j++)
                col[j+1] = std::min( std::min(prevCol[1 + j] + 1, col[j] + 1),
                                     prevCol[j] + (s1[i]==s2[j] ? 0 : 1) );
                col.swap(prevCol);
        }
        return prevCol[len2];
    }
    
    template<class T>
    static unsigned int hamming(const T &s1, const T &s2)
    {
        const size_t minSize = std::min(s1.size(), s2.size());
        unsigned int distance = 0;
        for (unsigned int i = 0; i < minSize; ++i) {
            if (s1[i] != s2[i])
                ++distance;
        }
        return distance + (std::max(s1.size(), s2.size()) - minSize);
    }
    
    static unsigned int entropic(const Message& startingPointMessage, const Message& endPointMessage)
    {
        // first, compute the entropy of both individuals
        ugp3::Message totalMessage;
        totalMessage += startingPointMessage;
        totalMessage += endPointMessage;
        
        // entropic distance is the the difference between the two
        return 2*totalMessage.getSize() - startingPointMessage.getSize() - endPointMessage.getSize();
    }
};

}

#endif // HEADER_UGP3_DISTANCES
