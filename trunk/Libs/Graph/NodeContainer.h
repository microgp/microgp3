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
#ifndef HEADER_UGP3_CTGRAPH_NODECONTAINER
#define HEADER_UGP3_CTGRAPH_NODECONTAINER

#include <memory>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//#include "CNode.h" // needed, since there are std::unique_ptr<CNode> declared
#include "Constraints.h"
#include "ConstrainedElement.h"
#include "Debug.h"
#include "Hashable.h"
#include "IContainer.h"
#include "IValidable.h"
#include "IString.h"

namespace ugp3
{
    namespace ctgraph
    {
	class CNode;

        class NodeContainer : 
			public IContainer<CNode>, 
			public ConstrainedElement,
			public IString,
			public Hashable
        {

        protected: // fields
		std::unique_ptr<CNode> prologue;
		std::unique_ptr<CNode> epilogue;
        public:
		NodeContainer();
		NodeContainer(const ugp3::constraints::ConstrainingElement& element);

		virtual CNode* getNode(const std::string& nodeId) const = 0;

		virtual CNode& getPrologue() const;
		virtual CNode& getEpilogue() const;

		virtual void setPrologue(std::unique_ptr< ugp3::ctgraph::CNode >& value);
		virtual void setEpilogue(std::unique_ptr< ugp3::ctgraph::CNode >& value);
		virtual void setPrologue(CNode* node);
		virtual void setEpilogue(CNode* node);

		void setAsParent(CNode* node, NodeContainer* newParent) const;

		virtual ~NodeContainer();
        };
    }
}

namespace ugp3
{
    namespace ctgraph
    {
        inline NodeContainer::NodeContainer()
            : ConstrainedElement()
        { }

		inline NodeContainer::NodeContainer(const ugp3::constraints::ConstrainingElement& element)
			 : ConstrainedElement(element)
		{ }

        inline NodeContainer::~NodeContainer()
        { }

        inline CNode& NodeContainer::getEpilogue() const
        {
            Assert(this->epilogue.get() != nullptr);
            return *this->epilogue;
        }

        inline CNode& NodeContainer::getPrologue() const
        {
            Assert(this->prologue.get() != nullptr);
            return *this->prologue;
        }

		
    }
}

#endif
