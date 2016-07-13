/***********************************************************************\
|                                                                       |
| Lineage.h                                                             |
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

#ifndef HEADER_UGP3_CORE_LINEAGE
#define HEADER_UGP3_CORE_LINEAGE

#include <string>
#include <vector>

// headers from other modules
#include "XMLIFace.h"
#include <SchemaException.h>
#include "CallData.h"

namespace ugp3
{

namespace core
{

class CandidateSolution;

/**
 * This class is used:
 *  -   During a generation, to store pointers to parents
 *      of the generated candidates, and a pointer to the
 *      statistical data about the operator call.
 * 
 *  -   From one generation to another, to store the names
 *      of the parents of the individual, and the name of
 *      the operator that generated it.
 * 
 * Only the names are written and read from the XML dumps,
 * all other pointers are reset at the end of the generation.
 */
class Lineage: public xml::XMLIFace
{
private:
    void parseParents(const xml::Element& element);

private: // static fields
    static const std::string XML_CHILD_OPERATOR;
    static const std::string XML_CHILD_PARENTS;
    static const std::string XML_CHILD_PARENT;
    static const std::string XML_ATTRIBUTE_REF;
    
protected:
    // Fields that will be written to XML
    std::string m_operatorName;
    /*
     * NOTE Since group ids and individual ids can collide,
     * this list of ids means nothing on its own. Whether
     * those refer to groups or individuals depends on the type
     * of this Lineage.
     */
    std::vector<std::string> m_parentNames;
    
    // Temporary fields for the generation
    CallData* m_callData = nullptr;

protected:
    Lineage();
    virtual void clear();
    
public: // static fields
    static const std::string XML_NAME;

public: // constructors and destructors
    Lineage(const Lineage& lineage) = delete;
	virtual ~Lineage() {};

public: // methods
	const std::string& getOperatorName() const { return m_operatorName; }
	const std::vector<std::string>& getParentNames() const { return m_parentNames; }
    
    void setCallData(CallData * callData) { m_callData = callData; }
    CallData * getCallData() const { return m_callData; }
    
    /**
     * This method should be called by the candidate when
     * moving to the next generation.
     * Reset pointers to statistics and parents, as those may be dead now.
     */
    virtual void step();

public: // Xml interface
	virtual void writeXml(std::ostream& output) const;
	virtual void readXml(const xml::Element& element);
	virtual const std::string& getXmlName() const { return XML_NAME; }
};

template <class T>
class SpecificLineage: public Lineage
{
private:
    std::vector<T*> m_parents;
    
protected:
    virtual void clear() {
        Lineage::clear();
        m_parents.clear();
    }
   
public:
    SpecificLineage() {}
    SpecificLineage(const std::string& operatorName, const std::vector<std::string>& parentNames) {
        set(operatorName, parentNames);
    }
    
    void set(const std::string& operatorName, const std::vector<std::string>& parentNames) {
        m_operatorName = operatorName;
        m_parentNames = parentNames;
        m_parents.clear();
    }
    
    void set(const std::string& operatorName, const std::vector< T* >& parents) {
        m_operatorName = operatorName;
        for (auto parent : parents) {
            m_parentNames.push_back(parent->getId());
        }
        m_parents = parents;
    }
    
    virtual void step() {
        Lineage::step();
        m_parents.clear();
    }
    
    const std::vector<T*>& getParents() const { return m_parents; }
};

}

}

#endif
