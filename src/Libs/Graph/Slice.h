/***********************************************************************\
|                                                                       |
| Slice.h                                                               |
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
#ifndef HEADER_UGP3_CTGRAPH_SLICE
#define HEADER_UGP3_CTGRAPH_SLICE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <vector>
#include <memory>
#include <string>

#include "InfinityString.h"
#include "CNode.h"


namespace ugp3
{

namespace ctgraph
{

/** A slice of nodes.
    @author Alessandro Salomone, Massimiliano Schillaci. */
class Slice
{
private: // static fields
    static InfinityString idCounter;

private: // fields
    std::string id;
    std::vector<CNode *> nodeSequence;

    void clear();

public:
    static const int START;
    static const int END;

public: // constructors
    /** Creates an empty slice. */
    Slice();

    /** One-node slice, most useful to minimize the class interface. */
    Slice(std::unique_ptr<CNode> node);

    ~Slice();

public: // getters
    unsigned int    getSize() const;
    CNode&          getNode(unsigned int position);
    const CNode&    getNode(unsigned int position) const;

public: // methods
    void                 spliceSlice(std::unique_ptr<Slice> additional, int position);
    void                 swapNodes(int position1, int position2);
    void                 invertSubSequence(int position1, int position2);
    std::unique_ptr<Slice> cutSlice(unsigned int position1, unsigned int position2);
    void                 attachNextAndPrev();
    void                 append(CNode& node);
    const std::string    toString();
};

inline unsigned int Slice::getSize() const
{
    return (unsigned int)this->nodeSequence.size();
}

inline void Slice::append(CNode& node)
{
    this->nodeSequence.push_back(&node);
}

inline const CNode& Slice::getNode(unsigned int position) const
{
    return *this->nodeSequence.at(position);
}

inline CNode& Slice::getNode(unsigned int position)
{
    return *this->nodeSequence.at(position);
}

}

}

#endif
