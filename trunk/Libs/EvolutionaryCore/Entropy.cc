/***********************************************************************\
|                                                                       |
| Entropy.cc                                                            |
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

#include "ugp3_config.h"
#include "EvolutionaryCore.h"

using namespace ugp3;
using namespace std;

void Message::extractSymbols(const vector< hash_t >& sequence)
{
    unsigned int maxTuples = 3;
    
    for (size_t i = 0; i < sequence.size(); ++i) {
        // Add the symbol of the hash with its position
        *this += Hashable::djbHash(sequence[i], i);
    }
    
    // Then, add the groups of two and three consecutive hashes
    for(size_t maxOffset = 1; maxOffset < maxTuples; maxOffset++)
    for(size_t i = 0; (i + maxOffset) < sequence.size(); i++) {
        hash_t symbol = sequence[i];

        // we consider the maxOffset values starting from i; e.g. if maxOffset
        // is 2, we'll obtain an hash of values i, i+1, i+2 (thus considering
        // a set of three values)
        for (size_t j = i + 1; j <= i + maxOffset; j++)
            symbol = Hashable::djbHash(symbol, sequence[j]);

        *this += symbol;
    }
}

void Message::operator+=(const hash_t symbol)
{
    m_isValid = false;
    
	// if the symbol is not present in the hash map
    if(m_symbols.find(symbol) == m_symbols.end())
		// total number of symbols of that kind = 1
        m_symbols[symbol] = 1;
    else
		// increment the number of symbols of that kind
        ++m_symbols[symbol];
}

void Message::operator-=(const hash_t symbol)
{
    m_isValid = false;
    
    auto s = m_symbols.find(symbol);
    if (s != m_symbols.end()) {
        --s->second;
        if (s->second == 0)
            m_symbols.erase(s);
    }
    else
        throw Exception("Cannot remove symbol from total message",LOCATION);
}

void Message::operator+=(const Message& other)
{
    m_isValid = false;
    
    for (auto it = other.m_symbols.begin(); it != other.m_symbols.end(); ++it) {
        auto s = m_symbols.find(it->first);
        if (s != m_symbols.end()) {
            s->second += it->second;
        } else {
            m_symbols.insert(*it);
        }
    }
}

void Message::operator-=(const Message& other)
{
    m_isValid = false;
    
    for (auto it = other.m_symbols.begin(); it != other.m_symbols.end(); ++it) {
        auto s = m_symbols.find(it->first);
        Assert(s != m_symbols.end());
        Assert(s->second >= it->second);
        s->second -= it->second;
        if (s->second == 0) {
            m_symbols.erase(s);
        }
    }
}

void Message::reset()
{
    m_symbols.clear();
    m_isValid = true;
    m_entropy = 0; // Entropy of an empty message
}

double Message::getEntropy() const
{
    if (!m_isValid) {
        m_isValid = true;
        m_entropy = 0;
        
        unsigned int totSymbols = 0;
        for (auto iterator = m_symbols.begin(); iterator != m_symbols.end(); ++iterator) {
            totSymbols += iterator->second;
        }
        if (totSymbols != 0) {
            for (auto iterator = m_symbols.begin(); iterator != m_symbols.end(); ++iterator) {
                Assert(iterator->second != 0);
                double p = iterator->second / (double)totSymbols;
                m_entropy += - p * ::log(p);
            }
        }
    }
    
    return m_entropy;
}

std::unordered_map<hash_t, unsigned int> Message::getMessageMap()
{
	return this->m_symbols;
}

const Message& MessageHolder::getMessage() const
{
    if (!m_isMessageValid) {
        m_message.reset();
        computeMessage(m_message);
        m_isMessageValid = true;
    }
    
    return m_message;
}


#if 0
void Message::operator+=(const ctgraph::CGraphContainer &container)
{
    m_isValid = false;
    
    for(unsigned int t = 0; t < container.getCGraphCount(); t++)
        *this += container.getCGraph(t);

    *this += container.getPrologue();
    *this += container.getEpilogue();
}

void Message::operator+=(const ctgraph::CSubGraph &subgraph)
{
    m_isValid = false;
    
    for(ctgraph::CNode* cursor = &subgraph.getPrologue();
        cursor != nullptr;
        cursor = cursor->getNext())
    {
        *this += *cursor;
    }

}

void Message::operator+=(const ctgraph::CGraph &graph)
{
    m_isValid = false;
    
    for(unsigned int t = 0; t < graph.getSubGraphCount(); t++)
    {
        *this += graph.getSubGraph(t);
    }

    *this += graph.getPrologue();
    *this += graph.getEpilogue();
}

void Message::operator+=(const ctgraph::CNode &node)
{
    m_isValid = false;
    
    *this += node.getHashCode(Hashable::ENTROPY);
}

void Message::operator-=(const ctgraph::CGraphContainer &container)
{
    m_isValid = false;
    
    for(unsigned int t = 0; t < container.getCGraphCount(); t++)
        *this -= container.getCGraph(t);

    *this -= container.getPrologue();
    *this -= container.getEpilogue();
}

void Message::operator-=(const ctgraph::CSubGraph &subgraph)
{
    m_isValid = false;
    
    for(ctgraph::CNode* cursor = &subgraph.getPrologue();
        cursor != nullptr;
        cursor = cursor->getNext())
    {
        *this -= *cursor;
    }

}

void Message::operator-=(const ctgraph::CGraph &graph)
{
    m_isValid = false;
    
    for(unsigned int t = 0; t < graph.getSubGraphCount(); t++)
    {
        *this -= graph.getSubGraph(t);
    }

    *this -= graph.getPrologue();
    *this -= graph.getEpilogue();
}

void Message::operator-=(const ctgraph::CNode &node)
{
    m_isValid = false;
    
    *this -= node.getHashCode(Hashable::ENTROPY);
}
#endif

