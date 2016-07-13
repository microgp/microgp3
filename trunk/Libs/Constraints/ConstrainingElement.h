/***********************************************************************\
|                                                                       |
| ConstrainingElement.h                                                 |
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

#ifndef HEADER_UGP3_CONSTRAINTS_CONSTRAININGELEMENT
#define HEADER_UGP3_CONSTRAINTS_CONSTRAININGELEMENT

#include "Utility.h"
#include "IString.h"
#include "Format.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

namespace ugp3
{
    namespace constraints
    {
        class GenericSection;
        class Parameter;

        /** The base class of the constraints object model.
            @author Aimo Alessandro, Salomone Alessandro. */
        class ConstrainingElement : public IString, public xml::XMLIFace
        {
        private:
             void clear();

            /** The copy constructor: defined as private so that it cannot be accessed. */
            ConstrainingElement(const ConstrainingElement&);
           
            /** Initializes a new instance of the ConstrainingElement class.*/
            ConstrainingElement();

        protected:
            /** The unique identifier of the instance. */
            std::string id;

            /** The format of the language comments. */
            CommentFormat* commentFormat;

            /** The format of the labels. */
            LabelFormat* labelFormat;

            /** The format of the unique tag. */
            UniqueTagFormat* uniqueTagFormat;

            /** The format of the identifiers. */
            IdentifierFormat* identifierFormat;

            /** The parent section that contains the element. */
            const GenericSection* parent;

            const std::string parseParameter(const xml::Element* element, Parameter*& parameter);
        
        public:
            static const std::string XML_ATTRIBUTE_ID;
            
        public:  // constructors and destructors
            /** Initializes a new instance of the ConstrainingElement class with the specified parent container.
                @param parent The parent generic section container. 
                @param id The identifier of the instance. */
            ConstrainingElement(const GenericSection& parent, const std::string& id);

            /** Initializes a new instance of the ConstrainingElement class from the given xml element.
                @param parent The parent generic section container.
                @param element A valid pointer to an xml element instance representing an instruction library element. */
            ConstrainingElement(const GenericSection& parent);

            /** Frees the resources associated to the instance. */
            virtual ~ConstrainingElement();

        public:   // setters
            /** Sets the format of the label.
                @param labelFormat A string specifiying the format of the label. */
            void setLabelFormat(LabelFormat* labelFormat);

            /** Sets the format of the identifiers.
                @param labelFormat A string specifiying the format of the identifiers. */
            void setIdentifierFormat(IdentifierFormat* identifierFormat);

            /** Sets the format of the language comments. 
                @param commentFormat A string specifiying the format of the comments. */
            void setCommentFormat(CommentFormat* commentFormat);
            
            /** Sets the format of the unique tag.
                @param uniqueTagFormat A string specifiying the format of the unique tag. */
            void setUniqueTagFormat(UniqueTagFormat* uniqueTagFormat);

        public:   // getters
            /** Gets the format of the language comments.
                @return A string specifiying the format of the comments. */
            const CommentFormat* getCommentFormat() const;
 
            /** Gets the format of the label.
                @return A string specifiying the format of the label. */
            const LabelFormat* getLabelFormat() const;

            /** Gets the format of the unique tag.
                @return A string specifiying the format of the unique tag. */
            const UniqueTagFormat* getUniqueTagFormat() const;

            /** Gets the format of the identifiers.
                @return A string specifiying the format of the identifiers. */
            const IdentifierFormat* getIdentifierFormat() const;

            /** Gets the parent container of the element.
                @return A pointer to the parent container. */
            const GenericSection* getParent() const;
            
            /** Gets the name of the instance. 
                @return A std::string that describes the instance. */
            const std::string& getId() const;

        public:  // overridden methods
            /** Gets the name of the parameter. */
            virtual const std::string toString() const;

		public: // Xml interface
            virtual void readXml(const xml::Element& element);
        }; 
    }
}

namespace ugp3
{
    namespace constraints
    {
        inline const std::string& ConstrainingElement::getId() const
        {
            return this->id;
        }

        inline const GenericSection* ConstrainingElement::getParent() const
        {
            return this->parent;
        }

        inline const std::string ConstrainingElement::toString() const
        {
            return "{" + this->id + "}";
        }
    }
}

#endif
