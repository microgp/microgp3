/***********************************************************************\
|                                                                       |
| Entropy.h                                                             |
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
| $Revision: 648 $
| $Date: 2015-02-26 00:41:17 +0100 (Thu, 26 Feb 2015) $
\***********************************************************************/

#ifndef HEADER_UGP3_ENTROPY
#define HEADER_UGP3_ENTROPY

#include <unordered_map>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// headers from shared module

#include "Hashable.h"

// headers of other modules
#include "Log.h"

namespace ugp3
{
    
class Message
{
public:
    /**
     * Extract from a sequence of entropic hash values the following symbols:
     *  1.  Unique hash values with their position in the sequence
     *  2.  Groups of two consecutive hash values
     *  3.  Groups of three consecutive hash values
     * and add these symbols to this message.
     */
    void extractSymbols(const std::vector<hash_t>& sequence);
    
    /**
     * Modify the internal set of symbols.
     */
    void operator+=(const Message& other);
    void operator+=(const hash_t symbol);
    
    void operator-=(const Message& other);
    void operator-=(const hash_t symbol);
    
#if 0
    void operator+=(const ctgraph::CGraphContainer &container);
    void operator+=(const ctgraph::CGraph &graph);
    void operator+=(const ctgraph::CSubGraph &subgraph);
    void operator+=(const ctgraph::CNode &node);
    
    void operator-=(const ctgraph::CGraphContainer &container);
    void operator-=(const ctgraph::CGraph &graph);
    void operator-=(const ctgraph::CSubGraph &subgraph);
    void operator-=(const ctgraph::CNode &node);
#endif
    
    /**
     * Return the computed entropy.
     */
    double getEntropy() const;
    
    /**
     * Erase the internal set of symbols.
     */
    void reset();
    
    /**
     * FIXME: Number of different symbols, or total number of symbols?
     */
    size_t getSize() const { return m_symbols.size(); }
	
    /**
     * Return the map of symbols
     */
    std::unordered_map<hash_t, unsigned int> getMessageMap();
    
private:
    // Indicate whether the stored entropy is valid.
    mutable bool m_isValid = true;
    
    // Computed value of the entropy of this message.
    mutable double m_entropy = 0;
    
    // Internal set of symbols
    std::unordered_map<hash_t, unsigned int> m_symbols;
};

class MessageHolder
{
private:
    mutable bool m_isMessageValid = false;
    mutable Message m_message;
    
public:
    
    /**
     * Subclasses must implement this method to compute their entropic message
     * by adding symbols to the given blank message.
     */
    virtual void computeMessage(Message& message) const = 0;
    
    const Message& getMessage() const;
    
    void invalidateMessage() const { m_isMessageValid = false; }
};

}

#endif
