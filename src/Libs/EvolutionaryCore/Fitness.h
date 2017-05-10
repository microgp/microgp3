/***********************************************************************\
|                                                                       |
| Fitness.h                                                             |
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

#ifndef HEADER_UGP3_CORE_FITNESS
#define HEADER_UGP3_CORE_FITNESS

#include <vector>

// headers from other modules
#include "Exception.h"
#include "Utility.h"
#include "XMLIFace.h"
#include "IComparable.h"
#include "IString.h"

namespace ugp3
{

    namespace core
    {

        class Fitness : public xml::XMLIFace, public IComparable<Fitness>, public IString
        {
        protected: // Fields
            std::vector<double> m_values;
            std::string description;
            bool isValid;
            void clear();

        public:
            static const std::string XML_NAME;

        protected:
            static const std::string XML_CHILD_ELEMENT_VALUE;
            static const std::string XML_CHILD_ELEMENT_DESCRIPTION;

        public: // Constructors and destructors
            Fitness();
            Fitness(unsigned int parameterCount, const std::string& valuesSeparatedBySpaces = "");
            virtual ~Fitness();
            
            Fitness& operator=(const Fitness& other);

        public: // Methods
            bool getIsValid() const;
            void invalidate();
            const std::string getDescription() const;
            virtual void setDescription(const std::string& description);
            virtual void setValues(const std::vector<double>& values);
            const std::vector<double>& getValues() const;
            bool equals(const Fitness& fitness, double threshold) const;
            static Fitness parse(const std::string& value);

        public: // Xml interface
            virtual void writeXml(std::ostream& output) const;
            virtual void readXml(const xml::Element& element);
            virtual const std::string& getXmlName() const;

        public: // CSV interface
            inline void writeCSV(std::ostream& output) const;

        public: // IComparable<Fitness> interface
            virtual bool equals(const Fitness& fitness) const;
            /// Return 1 if this fitness is higher than the other
            virtual int compareTo(const Fitness& instance) const;

        public: // IString interface
            virtual const std::string toString() const;

        public: // Overloaded operators
            bool operator>=(const Fitness& fitness) const;
            bool operator<=(const Fitness& fitness) const;
        };

        inline const std::string Fitness::getDescription() const
        {
            return this->description;
        }

        inline const std::vector<double>& Fitness::getValues() const
        {
            return this->m_values;
        }

        inline const std::string& Fitness::getXmlName() const
        {
            return XML_NAME;
        }

        inline void Fitness::invalidate()
        {
            this->isValid = false;
        }

        inline bool Fitness::getIsValid() const
        {
            return this->isValid;
        }

        inline void Fitness::setDescription(const std::string& description)
        {
            this->description = description;
        }

        inline bool Fitness::operator<=(const Fitness& fitness) const
        {
            return this->compareTo(fitness) <= 0;
        }

        inline bool Fitness::operator>=(const Fitness& fitness) const
        {
            return this->compareTo(fitness) >= 0;
        }

        inline Fitness::~Fitness()
        {}

        inline void Fitness::writeCSV(std::ostream& output) const
        {
            for(unsigned int i = 0; i < this->m_values.size(); i++)
            {
                output << "," << this->m_values[i];
            }
        }
    }

}

#endif

