/***********************************************************************\
|                                                                       |
| OperatorToolbox.h                                                     |
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

#ifndef HEADER_UGP3_CORE_OPERATORTOOLBOX
#define HEADER_UGP3_CORE_OPERATORTOOLBOX

// headers from of modules
#include "ConstrainedTaggedGraph.h"
#include "Constraints.h"

namespace ugp3
{
    namespace core
    {
        class OperatorToolbox
        {
        private:
            ugp3::ctgraph::CGraphContainer& container;
            OperatorToolbox(const OperatorToolbox& operatorToolbox);
			OperatorToolbox& operator=(const OperatorToolbox& toolbox);

        public: // constructors and destructors
            OperatorToolbox(ugp3::ctgraph::CGraphContainer& container);
            virtual ~OperatorToolbox();
	
	public: // deterministic navigation
		std::vector<ctgraph::CNode*> getAllNodes() const;
		std::vector<ctgraph::CNode*> getMutableNodes() const;
		std::vector<ctgraph::CNode*> getNodesWithParameter() const;
		template <class T>
		std::vector<ctgraph::CNode*> getNodesWithParameter() const;
		template <class T>
		std::vector<T*> getParameters() const;

	public: // random navigation
            ctgraph::CGraph* getRandomGraph() const;
            ctgraph::CSubGraph* getRandomSubGraph(const ctgraph::CGraph& sectionGraph);
            ctgraph::CNode* getRandomNode(const ctgraph::CGraph& sectionGraph, bool includePrologue = false, bool includeEpilogue = false);
            static ctgraph::CNode* getRandomNode(const ctgraph::CSubGraph& subGraph, bool includePrologue = false, bool includeEpilogue = false);
            ctgraph::CSubGraph* getRandomSubGraph(const std::string& section,  const std::string& subSection) const;

        public: // random node insert
            bool insertRandomNode();
            bool insertRandomNode(ctgraph::CGraph& sectionGraph);
            bool insertRandomNode(ctgraph::CSubGraph& subGraph);

        public: // random node remove
            bool removeRandomNode();
            bool removeRandomNode(ctgraph::CGraph& sectionGraph);
            bool removeRandomNode(ctgraph::CSubGraph& subGraph);

        public: // random subSection insert
            bool insertRandomSubSection();
            bool insertRandomSubSection(ctgraph::CGraph& sectionGraph);

        public: // random subSection remove
            bool removeRandomSubSection();
            bool removeRandomSubSection(ctgraph::CGraph& sectionGraph);
        };

	// in C++, the implementation of a template method must be in the same file where the declaration is
	// return all nodes that contain a given parameter
	template <class T>
	inline std::vector<ctgraph::CNode*> OperatorToolbox::getNodesWithParameter() const
	{
		std::vector<ctgraph::CNode*> allNodes;

		// add global prologue
		bool found = false;
		for(unsigned int p = 0; p < this->container.getPrologue().getGenericMacro().getParameterCount() && !found; p++)
		{
			// if at least one parameter is of the specified type
			if( dynamic_cast<T*>( &this->container.getPrologue().getGenericMacro().getParameter(p) ) != nullptr)
			{
				allNodes.push_back( &this->container.getPrologue() );
				found = true;
			}
		}

		// for each section
		for(unsigned int s = 0; s < this->container.getCGraphCount() ; s++)
		{	
			// pointer to current section
			ctgraph::CGraph* section = &this->container.getCGraph(s);

			// add section prologue
			found = false;
			for(unsigned int p = 0; p < section->getPrologue().getGenericMacro().getParameterCount() && !found; p++)
			if( dynamic_cast<T*>(&section->getPrologue().getGenericMacro().getParameter(p) ) != nullptr )
			{
				allNodes.push_back( &section->getPrologue() );
				found = true;
			}

			// for each subsection
			for(unsigned int ss = 0; ss < section->getSubGraphCount() ; ss++)
			{
				// pointer to current subsection
				ctgraph::CSubGraph* subsection = &section->getSubGraph(ss);

				// add all the nodes in the subsection (prologue and epilogue included)
				ctgraph::CNode* node = &subsection->getPrologue(); 
				while(node != nullptr)
				{
					found = false;
					for(unsigned int p = 0; p < node->getGenericMacro().getParameterCount() && !found; p++)
					if( dynamic_cast<T*>(&node->getGenericMacro().getParameter(p) ) != nullptr )
					{
						allNodes.push_back( node );
						found = true;
					}

					node = node->getNext();
				}
			}

			// add section epilogue
			found = false;
			for(unsigned int p = 0; p < section->getEpilogue().getGenericMacro().getParameterCount() && !found; p++)
			if( dynamic_cast<T*>(&section->getEpilogue().getGenericMacro().getParameter(p) ) != nullptr )
			{
				allNodes.push_back( &section->getEpilogue() );
				found = true;
			}
		}

		// add global epilogue
		found = false;
		for(unsigned int p = 0; p < this->container.getEpilogue().getGenericMacro().getParameterCount() && !found; p++)
		if( dynamic_cast<T*>(&this->container.getEpilogue().getGenericMacro().getParameter(p) ) != nullptr )
		{
			allNodes.push_back( &this->container.getEpilogue() );
			found = true;
		}

		LOG_DEBUG 	<< "OperatorToolbox::getAllNodes collected a total of " << allNodes.size() 
				<< " nodes containing a parameter of the specified type " << std::ends;

		// return the vector
		return allNodes; 
	}

	// return all parameters of a given type in an individual
	template <class T>
	inline std::vector<T*> OperatorToolbox::getParameters() const
	{
		std::vector<T*> parameters;

		// add parameters of the global prologue
		for(unsigned int p = 0; p < this->container.getPrologue().getGenericMacro().getParameterCount(); p++)
		{
			// if one parameter is of the specified type, add it
			T* parameter = nullptr;
			if( (parameter = dynamic_cast<T*>( &this->container.getPrologue().getGenericMacro().getParameter(p) )) != nullptr)
			{
				parameters.push_back( parameter );
			}
		}

		// for each section
		for(unsigned int s = 0; s < this->container.getCGraphCount() ; s++)
		{	
			// pointer to current section
			ctgraph::CGraph* section = &this->container.getCGraph(s);

			// add parameters in the section prologue
			for(unsigned int p = 0; p < section->getPrologue().getGenericMacro().getParameterCount(); p++)
			{
				// if one parameter is of the specified type, add it
				T* parameter = nullptr;
				if( (parameter = dynamic_cast<T*>(&section->getPrologue().getGenericMacro().getParameter(p) )) != nullptr )
				{
					parameters.push_back( parameter );
				}
			}

			// for each subsection
			for(unsigned int ss = 0; ss < section->getSubGraphCount() ; ss++)
			{
				// pointer to current subsection
				ctgraph::CSubGraph* subsection = &section->getSubGraph(ss);

				// add all the parameters of nodes in the subsection (prologue and epilogue included)
				ctgraph::CNode* node = &subsection->getPrologue(); 
				while(node != nullptr)
				{
					for(unsigned int p = 0; p < node->getGenericMacro().getParameterCount(); p++)
					{
						T* parameter = nullptr;
						if( (parameter = dynamic_cast<T*>(&node->getGenericMacro().getParameter(p) )) != nullptr )
						{
							parameters.push_back( parameter );
						}
					}
					node = node->getNext();
				}
			}

			// add section epilogue
			for(unsigned int p = 0; p < section->getEpilogue().getGenericMacro().getParameterCount(); p++)
			{
				T* parameter = nullptr;
				if( (parameter = dynamic_cast<T*>(&section->getEpilogue().getGenericMacro().getParameter(p) )) != nullptr )
				{
					parameters.push_back( parameter );
				}
			}
		}

		// add global epilogue
		for(unsigned int p = 0; p < this->container.getEpilogue().getGenericMacro().getParameterCount(); p++)
		{
			T* parameter = nullptr;
			if( (parameter = dynamic_cast<T*>(&this->container.getEpilogue().getGenericMacro().getParameter(p) )) != nullptr )
			{
				parameters.push_back( parameter );
			}
		}

		LOG_DEBUG 	<< "OperatorToolbox::getParameters collected a total of " << parameters.size() 
				<< " parameters of the specified type " << std::ends;

		// return the vector
		return parameters;
	}

    } // end of namespace core
} // end of namespace ugp3
#endif
