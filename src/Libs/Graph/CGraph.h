/***********************************************************************\
|                                                                       |
| CGraph.h                                                              |
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
#ifndef HEADER_UGP3_CTGRAPH_CGRAPH
#define HEADER_UGP3_CTGRAPH_CGRAPH

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// other module headers
#include "TaggedGraph.h"
#include "Constraints.h"
// shared headers
#include "Utility.h"
#include "IContainer.h"
#include "Hashable.h"
#include "ICloneable.h"
// this module headers
#include "CGraphContainer.h"
#include "NodeContainer.h"
#include "ConstrainedElement.h"
// standard headers
#include <vector>
#include <string>

namespace ugp3
{
	namespace ctgraph
	{
		// forward declarations
		class CNode;
		class CSubGraph;
		class CGraphContainer;

		/** The constrained tagged graph.
			@author Luca Motta, Alessandro Salomone */
		class CGraph :
			public tgraph::Graph,
			public NodeContainer,
			public IContainer<CSubGraph>,
			public IEquatable<CGraph>,
            public MessageHolder
		{
		private: // fields
			IContainer<CGraph>* parentContainer;
			std::vector<CSubGraph*> subGraphs;

        private:
            static const std::string XML_ATTRIBUTE_CONSTRAINTSREF;

		private: // constructors
			CGraph();
			CGraph(const CGraph&);
			friend void CGraphContainer::setAsParent(CGraph& graph, CGraphContainer* newParent) const;

			bool attachSubGraphNoSizeCheck(CSubGraph& subGraph);
			bool detachSubGraphNoSizeCheck(CSubGraph& subGraph);
			void setParentContainer(IContainer<CGraph>* parentContainer);

		public: // constructors and destructors
			/** Builds a new constrained graph.
			    @param parentContainer The parent container of this instance. */
			CGraph(IContainer<CGraph>& parentContainer);

			/** Destructor. */
			virtual ~CGraph();

		public: // methods
			/** Inserts a new random sub-graph into this graph.
			@param subSection The type of the sub-graph to be built.
			@return True if the sub-graph was successfully created and inserted, false otherwise. */
			bool insertSubGraph(const constraints::SubSection& subSection);

			/** Attaches the specified sub-graph to this graph.
			@param subGraph The graph to attach.
			@return True if the sub-graph was successfully attached, false otherwise. */
			bool attachSubGraph(CSubGraph& subGraph);

			/** Detaches the specified sub-graph of this graph.
			@param subGraph The graph to detach.
			@return True if the sub-graph was successfully detached, false otherwise.
			The sub-graph is not deleted. */
			bool detachSubGraph(CSubGraph& subGraph);

			/** Replaces a sub-graph of this graph with another sub-graph.
			@param originalSubGraph The sub-graph to be replaced.
			@param newSubGraph The sub-graph that will replace the original one.
			@return True if the replacement was successfull, false otherwise. */
			bool replaceSubGraph(CSubGraph* originalSubGraph, CSubGraph* newSubGraph);

			/** Sets the parent container of a specific sub-graph.
			@param subGraph The sub-graph whose parent container is to be set.
			@param newParent The new pareht graph for the given sub-graph. It may be null.
			FIXME why isn't this a method of CSubGraph? */
			void setAsParent(CSubGraph* subGraph, CGraph* newParent) const;

			/** Tries to restore the target of the floating edges (the ones that have a tag named "floating") of all the nodes of the graph.
			@return True if all the floating edges were restored, false otherwise. */
			bool attachFloatingEdges();

			/** Tells wether the specified node is contained in this graph.
			@param node The node to be found.
			@return True if the node is contained in the graph, false otherwise. */
			virtual bool contains(const CNode& node) const;

			/** Tells wether the specified sub-graph is contained in this graph.
			@param subGraph The sub-graph to be found.
			@return True if the sub-graph is contained in the graph, false otherwise. */
			virtual bool contains(const CSubGraph& subGraph) const;

		public: // getters and setters
			/** Searches for a node with given identifier.
			@param id A string representing the id of the node.
			@return A valid pointer to a CNode instance or nullptr if no node with the specified id cannot be found. */
			CNode* getNode(const std::string& id) const;

			/** Returns the parent container of this instance. */
			IContainer<CGraph>* getParentContainer() const;

			/** Gets the number of sub-graphs contained in this graph. */
			unsigned int getSubGraphCount() const;

			/** Gets the sub-graph at the given position.
			@param index The index of the sub-graph in the collection of sub-graphs. */
			CSubGraph& getSubGraph(unsigned int index) const;

			/** Gets a collection of sub-graphs referring to a specific type.
			@param subSectionName The name of the type of the sub-graphs to retrieve. */
			std::vector<CSubGraph*> getSubGraphs(const std::string& subSectionName) const;

			const constraints::Section& getSection() const;

		public: // ICloneable interface
			virtual std::unique_ptr<CGraph> clone() const;

		public: // IEquatable interface
			virtual bool equals(const CGraph& graph) const;

		public: // Hashale interface
			virtual hash_t calculateHashCode(Purpose purpose) const;

		public: // Xml interface
			virtual void writeXml(std::ostream& output) const;
			virtual void readXml(const xml::Element& element);

		public: // ConstrainedElement interface
			virtual void clear();
			virtual void buildRandom();
			virtual bool validate() const;
			virtual void writeExternalRepresentation(std::ostream& stream, Relabeller& relabeller) const;

		public: // IString interface
			virtual const std::string toString() const;

		public: // entropy information
            virtual void computeMessage(Message& message) const;
		};
	}
}


namespace ugp3
{
	namespace ctgraph
	{

		inline const constraints::Section& CGraph::getSection() const
		{
			if(this->getConstrain() != nullptr)
			{
				return *(const constraints::Section*)this->getConstrain();
			}
			else throw Exception("", LOCATION);
		}


		inline unsigned int CGraph::getSubGraphCount() const {
			return (unsigned int)this->subGraphs.size();
		}

		inline void CGraph::setParentContainer(IContainer<CGraph>* parentContainer) {
			this->parentContainer = parentContainer;
		}
	}
}
#endif
