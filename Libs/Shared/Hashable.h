/***********************************************************************\
|                                                                       |
| Hashable.h                                                            |
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

#ifndef HEADER_UGP3_IHASHABLE
#define HEADER_UGP3_IHASHABLE

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

typedef unsigned long int hash_t;

#include <string>
#include "Debug.h"

namespace ugp3
{

    class Hashable
    {
    public:
        typedef enum {
            PURPOSE_FIRST = 0,
            GENOTYPE = 0, ///< Hash for genotype-equality testing
            ENTROPY = 1,  ///< Hash for entropy computation (with real value quantization)
            PURPOSE_COUNT = 2,
        } Purpose;
        
        Hashable() {};
        virtual ~Hashable() {};
        
        hash_t getHashCode(Purpose purpose = GENOTYPE) const {
            Assert(PURPOSE_FIRST <= purpose && purpose < PURPOSE_COUNT);
            if (!m_hashValid[purpose]) {
                m_hashValue[purpose] = calculateHashCode(purpose);
                m_hashValid[purpose] = true;
            } else {
                // FIXME Disabled because it is slow and at some point it worked well
                // Assert(m_hashValue[purpose] == calculateHashCode(purpose));
            }
            
            return m_hashValue[purpose];
        }
        
        void invalidateHashCodes() {
            for (int p = PURPOSE_FIRST; p < PURPOSE_COUNT; ++p) {
                m_hashValid[p] = false;
            }
        }
        
        // Mostly DJB Hash...
        static constexpr hash_t startValue = 5381;
        
        static hash_t djbHash(hash_t link, const std::string& data) {
            hash_t hash = link;
            for (int c: data) {
                hash = ((hash << 5) + hash) ^ c;
            }
            return hash;
        }
        
        static hash_t djbHash(hash_t link, hash_t hashCode2) {
            hash_t hash = link;
            for (std::size_t t = 0; t < sizeof(hash_t); t++)
            {
                hash = ((hash << 5) + hash) ^ ( hashCode2  & 0xff );
                hashCode2 >>= 8;
            }
            return hash;
        }
        
    protected:
        virtual hash_t calculateHashCode(Purpose purpose) const = 0;
        
    private:
        mutable hash_t m_hashValue[PURPOSE_COUNT] = {startValue};
        mutable bool m_hashValid[PURPOSE_COUNT] = {false};
    };
}

#endif
