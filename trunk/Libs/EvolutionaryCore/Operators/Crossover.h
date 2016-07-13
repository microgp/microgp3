/***********************************************************************\
|                                                                       |
| CrossoverOperator.h                                                   |
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
 * @file CrossoverOperator.h
 * Definition of the CrossoverOperator class.
 * @see CrossoverOperator.cc
 */

#ifndef HEADER_UGP3_CORE_CROSSOVEROPERATOR
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_CROSSOVEROPERATOR

#include <set>
#include "GeneticOperator.h"
#include "ConstrainedTaggedGraph.h"


/**
 * ugp3 namespace
 */
namespace ugp3
{

/**
 * ugp3::core namespace
 */
namespace core
{

class CrossoverOperator : public GeneticOperator
{
protected:
    void swapSlices(
        ugp3::ctgraph::CSubGraph& subGraph1, unsigned int cutPoint11, unsigned int cutPoint12, 
        ugp3::ctgraph::CSubGraph& subGraph2, unsigned int cutPoint21, unsigned int cutPoint22) 
        const;

    /** Collects all the sub-graphs referenced by the outer labels of 
        * the nodes of the specified sub-graph. */
    void collectLinkedSubGraphs(ugp3::ctgraph::CSubGraph* rootSubGraph, std::set<ugp3::ctgraph::CSubGraph*>& subGraphs) const;
    
	/** Collects all the sub-graphs referenced by the outer labels of 
	  * the specified nodes. */
    void collectLinkedSubGraphs(const std::set<ugp3::ctgraph::CNode*>& 
        nodes, std::set<ugp3::ctgraph::CSubGraph*>& subGraphs) const 
;
    
    /** Given two individuals, randomly selects two subgraphs of the 
        * same type. */
    bool selectRandomSubgraphPair(Individual& child1, Individual& 
        child2, ugp3::ctgraph::CSubGraph*& subGraph1, 
        ugp3::ctgraph::CSubGraph*& subGraph2) const;
        
    bool swapSlices(Individual& child1, 
        ugp3::ctgraph::CSubGraph& subGraph1, 
        ugp3::ctgraph::CNode& firstNode1, unsigned int sliceSize1, 
        Individual& child2, ugp3::ctgraph::CSubGraph& subGraph2, 
        ugp3::ctgraph::CNode& firstNode2, unsigned int sliceSize2) 
        const;
        
    /** Insert selectedSubGraph and subGraphs in child 1. Remove the sub-graphs from the
     *original individual if not referenced anymore.*/
    bool transferSubGraph(const ugp3::ctgraph::CSubGraph& selectedSubGraph, std::set<ugp3::ctgraph::CSubGraph*>& subGraphs, Individual& otherChild) const;
    
    /**
     * Copy all the subgraphs referred by nodes starting at index cutPoint1,
     * up to but not including index (cutPoint1 + slice1Size) nodes of
     * selectedSubGraph1 from child1 to child 2 and vice versa.
     * If the subgraphs are referred ONLY by the nodes in the slice
     * [cutPoint1; cutPoint1 + slice1Size) of selectedSubGraph1,
     * they are removed from child 1.
     */
    bool swapSubGraphs(Individual& child1, Individual& child2,
                       const ugp3::ctgraph::CSubGraph& selectedSubGraph1,
                       const ugp3::ctgraph::CSubGraph& selectedSubGraph2,
                       unsigned int cutPoint1, 
                       unsigned int slice1Size, 
                       unsigned int cutPoint2, 
                       unsigned int slice2Size) const;

public:
    CrossoverOperator();

	/** Gets the number of parents required by the operator.
		@return The number of parents required by the operator.*/
    virtual unsigned int getParentsCardinality() const;

public: // Xml methods
    virtual bool hasParameters() const = 0;
    virtual void writeXml(std::ostream& output) const = 0;
    virtual void readXml(const xml::Element& element) = 0;
};

}

}

#endif

