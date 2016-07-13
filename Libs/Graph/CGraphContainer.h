/***********************************************************************\
|                                                                       |
| CGraphContainer.cc                                                    |
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
 * @file CGraphContainer.h
 * Definition of the CGraphContainer class.
 * @see CGraphContainer.cc
 */

#ifndef HEADER_UGP3_CTGRAPH_CGRAPHCONTAINER
/** Defines that this file has been included */
#define HEADER_UGP3_CTGRAPH_CGRAPHCONTAINER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Constraints.h"
#include "NodeContainer.h"
#include "ICloneable.h"
#include "IEquatable.h"
#include "Hashable.h"
#include <Entropy.h>

#include <iostream>
#include <vector>

/**
 * ugp3 namespace
 */
namespace ugp3
{
    /**
     * ugp3::ctgraph namespace
     */
    namespace ctgraph
    {
        // forward declarations
        class CGraph;
        class CSubGraph;
        class CNode; 
        class CGraphContainer;

        /**
         * @class CGraphContainer
         * Represents an instance of a Constraints definition and contains a collection of graphs.
         * @author Luca Motta, Alessandro Salomone.
         */
        class CGraphContainer : public NodeContainer,
            public ICloneable<CGraphContainer>,
            public IContainer<CGraph>,
            public IEquatable<CGraphContainer>,
            public MessageHolder
        {
        private:                              // fields
            // CGraphs contained in this container 
            std::vector<CGraph*> graphs;
    
    // Concatenation of the hash values of nodes, in order. When empty means it must be computed.
    mutable std::vector<hash_t> m_nodeHashSequence;

        private:                              // constructors
            // Private constructor. Not implemented and not used.
            CGraphContainer(const CGraphContainer&);

            // 
            bool attachOuterLabel(CNode& node, const constraints::OuterLabelParameter& outerLabel);

       public:
            static const std::string XML_NAME;

        public:                               // constructors and destructors
			CGraphContainer();
            virtual ~CGraphContainer();

        public:                               // methods
            /** Attaches the edges of the nodes that have no target defined (the ones that have a tag named "floating").
                 @return True if all the floating edges were restored, false otherwise. */
			bool attachFloatingEdges();

            /** Tells wether the specified node is contained in this graph-container.
                 @param element The node to be found.
                 @return True if the node is contained in the graph-container, false otherwise. */
            virtual bool contains(const CNode& element) const;

            /** Tells wether the specified graph is contained in this graph-container.
                @param element The graph to be found.
                @return True if the graph is contained in the graph-container, false otherwise. */
            virtual bool contains(const CGraph& element) const;

		public: // IEquatable interface
            virtual bool equals(const CGraphContainer& container) const;


        public: // ICloneable interface
            std::unique_ptr<CGraphContainer> clone() const;


        public:                               //getters
            /** Returns the node with the given id.
                @param id The id of the CNode to be retrieved.
                @return A valid pointer to the specified node or nullptr if the specified id is not found. */
            CNode* getNode(const std::string& id) const;

            /** Returns a random sub-graph with the given SubSection name.
                @param sectionName The name of the section corresponding to the graph in which the sub-graph is sampled.
                @param subSectionName The name of the sub-section.
                @return One of the instances of the specificed sub-section definition, or nullptr if there are no instances.*/
            CSubGraph* getRandomSubGraph(const std::string& sectionName, const std::string& subSectionName);

            /** Gets the number of graphs contained in this instance. */
            unsigned int getCGraphCount() const;

            /** Get the i-th instance of CGraph.
                @param index The index of the CGraph to be retrieved. */
            CGraph& getCGraph(unsigned int index) const;

            /** Get the graph associated to a section definition.
                @param sectionName The name of the section that the desired graph represents.
                @return The graph associated to the specified section. */
			CGraph* getCGraph(const std::string& sectionName) const;

			void addCGraph(std::unique_ptr< ugp3::ctgraph::CGraph >& graph);

			/** Sets the parent container of a specific graph.
                @param graph The graph whose parent container is to be set.
                @param container The new container for the given graph. It may be null.*/
            void setAsParent(CGraph& graph, CGraphContainer* container) const;
    
            /** Used to compute Levenshtein distance between graphs. */
            const std::vector<hash_t>& getNodeHashSequence() const;

		public: // Xml interface
			virtual void writeXml(std::ostream& output) const;
			virtual void readXml(const xml::Element& element);
			virtual const std::string& getXmlName() const;

		public: // ConstrainedElement interface
			virtual void clear();
			virtual void buildRandom();
			virtual bool validate() const;
			virtual void writeExternalRepresentation(std::ostream& stream, Relabeller& relabeller) const;

		public: // IString interface
			virtual const std::string toString() const;

		public: // Hashable interface
            virtual hash_t calculateHashCode(Purpose purpose) const;

		public: // entropy information
            virtual void computeMessage(Message& message) const;
        };
    }
}


namespace ugp3
{
    namespace ctgraph
    {
        inline unsigned int CGraphContainer::getCGraphCount() const {
            return (unsigned int) this->graphs.size();
        }

        inline CGraph& CGraphContainer::getCGraph( unsigned int i ) const {
            _STACK;
            if( i >= this->graphs.size() ) {
                throw IndexOutOfBoundsException("CGraphContainer::graphs", LOCATION);
            }

            return *this->graphs[i];
        }

		inline const std::string CGraphContainer::toString() const
		{
			return "";
		}

		inline const std::string& CGraphContainer::getXmlName() const
		{
			return XML_NAME;
		}
    }
}
#endif
