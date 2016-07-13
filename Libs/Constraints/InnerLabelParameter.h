/***********************************************************************\
|                                                                       |
| InnerLabelParameter.h                                                 |
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

#ifndef HEADER_UGP3_CONSTRAINTS_INNERLABELPARAMETER
#define HEADER_UGP3_CONSTRAINTS_INNERLABELPARAMETER

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "StructuralParameter.h"

namespace ugp3
{
    namespace constraints
    {
        /** Represents a specific parameter.
            @author Aimo Alessandro, Salomone Alessandro */
        class InnerLabelParameter : public StructuralParameter
        {
        private:
            /** Tells if the prologue is a valid reference.*/
            bool prologueIsValid;

            /** Tells if the epilogue is a valid reference.*/
            bool epilogueIsValid;

            /** Tells if the macro itself is a valid reference.*/
            bool itselfIsValid;

            /** Tells if a backward jump is permitted.*/
            bool backwardJumpIsValid;

            /** Tells if a backward jump is permitted.*/
            bool forwardJumpIsValid;

            /** Copy constructor. It is declared private so it cannot be accessed.*/
            InnerLabelParameter(const InnerLabelParameter&);

            

        public:
            /** Default constructor. */
            InnerLabelParameter();

            /** Initializes a new instance of the constraints::InnerLabelParameter class.
                @param prologueIsValid Tells if the prologue is a valid reference.
                @param epilogueIsValid Tells if the epilogue is a valid reference.
                @param itselfIsValid Tells if the macro itself is a valid reference.
                @param backwardJumpIsValid Tells if a backward jump is permitted.
                @param forwardJumpIsValid Tells if a forward jump is permitted.*/
            InnerLabelParameter(const std::string& name, bool prologueIsValid, bool epilogueIsValid, bool itselfIsValid, bool backwardJumpIsValid, bool forwardJumpIsValid);

            /** Tells if the prologue is a valid reference.
                @return True if the prologue is a valid reference.*/
            bool getPrologueIsValid() const;

            /** Tells if the eplogue is a valid reference.
                @return True if the epilogue is a valid reference.*/
            bool getEpilogueIsValid() const;

            /** Tells if the macro itself is a valid reference.
                @return True if the macro itself is a valid reference.*/
            bool getItselfIsValid() const;

            /** Tells if a backward jump is permitted.
                @return True if a backward jump is permitted.*/
            bool getBackwardJumpIsValid() const;

            /** Tells if a forward jump is permitted.
                @return True if a forward jump is permitted.*/
            bool getForwardJumpIsValid() const;

		/** Gets the regex representation for this parameter
		    @return String containing a regex expression. */
		virtual const std::string getRegex() const;

            /** Serializes the instance to an xml stream. 
                @param output The stream on which data will be output. */
            virtual void writeXml(std::ostream& output) const;

            /** Builds the object from the xml data. 
                @param element The xml element representing the object. */
            virtual void readXml(const xml::Element& element);

            /** Clones the instance of the parameter.
                @param outParameter A pointer to the new instance.
                @param name The name of the cloned parameter. */
            virtual void clone(Parameter*& outParameter, const std::string& name);

            /** Destructor.*/
            virtual ~InnerLabelParameter();
        };
    }
}

namespace ugp3
{
    namespace constraints
    {
        inline bool InnerLabelParameter::getPrologueIsValid() const
        {
            return this->prologueIsValid;
        }

        inline bool InnerLabelParameter::getEpilogueIsValid() const
        {
            return this->epilogueIsValid;
        }

        inline bool InnerLabelParameter::getItselfIsValid() const
        {
            return this->itselfIsValid;
        }

        inline bool InnerLabelParameter::getBackwardJumpIsValid() const
        {
            return this->backwardJumpIsValid;
        }

        inline bool InnerLabelParameter::getForwardJumpIsValid() const
        {
            return this->forwardJumpIsValid;
        }
    }
}

#endif
