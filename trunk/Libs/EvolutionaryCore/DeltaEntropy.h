/***********************************************************************\
|                                                                       |
| DeltaEntropy.h                                                        |
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

#ifndef HEADER_UGP3_CORE_DELTAENTROPY
#define HEADER_UGP3_CORE_DELTAENTROPY

// headers from shared module
#include "IComparable.h"


namespace ugp3
{
	namespace core
	{
		class DeltaEntropy : public IComparable<DeltaEntropy>
		{
		private:
			double value;

		public:
			DeltaEntropy(double value);
			virtual bool equals(const DeltaEntropy& other) const;
			virtual int compareTo(const DeltaEntropy& other) const;
			double getValue() const;
			void setValue(double value);

			virtual ~DeltaEntropy();
		};
	}
}

namespace ugp3
{
	namespace core
	{
		inline double DeltaEntropy::getValue() const
		{
			return this->value;
		}
	}
}

#endif
