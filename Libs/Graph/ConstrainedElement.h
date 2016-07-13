/***********************************************************************\ 
|                                                                       |
| ConstrainedElement.h                                                  |
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
#ifndef HEADER_UGP3_CTGRAPH_CONSTRAINEDELEMENT
#define HEADER_UGP3_CTGRAPH_CONSTRAINEDELEMENT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <memory>
#include "Constraints.h"
#include "Relabeller.h"

namespace ugp3
{
	namespace ctgraph
	{
		class ConstrainedElement : public IValidable
		{
		private:
			ugp3::constraints::ConstrainingElement* constrain;

        protected:
            static const std::string XML_ATTRIBUTE_CONSTRAINTSREF;
            
		public:
			ConstrainedElement();
			ConstrainedElement(const ugp3::constraints::ConstrainingElement& element);
			virtual ~ConstrainedElement();

        public:
			virtual void clear() = 0;
			virtual void buildRandom() = 0;
			virtual void writeExternalRepresentation(std::ostream& stream, Relabeller& relabeller) const = 0;

			void setConstrain(const ugp3::constraints::ConstrainingElement& constrain);
			const ugp3::constraints::ConstrainingElement* getConstrain() const;
		};
		
		inline ConstrainedElement::ConstrainedElement()
			: constrain(nullptr)
		{ }

		inline ConstrainedElement::ConstrainedElement(const ugp3::constraints::ConstrainingElement& element)
			: constrain((ugp3::constraints::ConstrainingElement*)&element)
		{ }

		inline ConstrainedElement::~ConstrainedElement()
		{ }

		inline void ConstrainedElement::setConstrain(const ugp3::constraints::ConstrainingElement& constrain)
		{
			this->constrain = (ugp3::constraints::ConstrainingElement*)&constrain;
		}

		inline const ugp3::constraints::ConstrainingElement* ConstrainedElement::getConstrain() const
		{
			return this->constrain;
		}
	}
}

#endif
