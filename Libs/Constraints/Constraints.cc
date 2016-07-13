/***********************************************************************\
|                                                                       |
| Constraints.cc                                                        |
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

#include "ugp3_config.h"
#include "Constraints.h"
using namespace std;
using namespace ugp3::constraints;


Constraints::Constraints()
    : GenericSection(nullptr, "undefined"),
    typedefs(nullptr),
    filePath("constraints.xml")
   
{ 
	commentFormat = new CommentFormat();
    labelFormat = new LabelFormat();
    uniqueTagFormat = new UniqueTagFormat();
    identifierFormat = new IdentifierFormat();
}

Constraints::Constraints(const string& name)
    : GenericSection(nullptr, name),
    typedefs(nullptr),
    filePath("constraints.xml")
   
{ 
	commentFormat = new CommentFormat();
    labelFormat = new LabelFormat();
    uniqueTagFormat = new UniqueTagFormat();
    identifierFormat = new IdentifierFormat();
}

void Constraints::clear()
{
    _STACK;

    for(unsigned int i = 0; i < this->sections.size() ; i++)
    {
        delete this->sections[i];
        this->sections[i] = nullptr;
    }

    this->sections.clear();

    delete this->prologue;
    this->prologue = nullptr;

    delete this->epilogue;
    this->epilogue = nullptr;

    // typedefs must be destroyed AFTER sections, prologue and epilogue
    if(typedefs != nullptr)
    {
        for(unsigned int i = 0 ; i < this->typedefs->size() ; i++)
        {
            delete this->typedefs->at(i);
            this->typedefs->at(i) = nullptr;
        }

        delete this->typedefs;
        this->typedefs = nullptr;
    }
}

Constraints::~Constraints()
{
    this->clear();

    LOG_DEBUG << "Destructor: ugp3::constraints::Constraints" << ends;
}

const vector<Parameter*>* Constraints::getTypeDefs() const
{
    if(this->typedefs == nullptr)
    {
        throw NullReferenceException("Constraints::typedefs",LOCATION);
    }

    return this->typedefs;
}

void Constraints::removeSection(Section* section)
{
    _STACK;

    if(section == nullptr)
    {
        throw ArgumentNullException("section",LOCATION);
    }

    if(this->contains(*section) == false)
    {
        throw ArgumentException("the specified section is not contained in the constraints.",LOCATION);
    }


    for(vector<Section*>::iterator iter = this->sections.begin(); iter!=this->sections.end(); ++iter)
    {
        if(*iter==section)
        {
            sections.erase(iter);
            return;
        }
    }

    // this code should be unreachable
    Assert(false);
}

void Constraints::removeSection(unsigned int index)
{
    _STACK;

    if(index >= this->sections.size())
    {
        throw IndexOutOfBoundsException("sections",LOCATION);
    }

    this->sections.erase(sections.begin() + index);
}

void Constraints::addSection(Section* section)
{
    _STACK;

    if(section == nullptr)
    {
        throw ArgumentNullException("section",LOCATION);
    }

    if(this->getSection(section->getId()) != nullptr)
    {
        throw ArgumentException("A Section with the id \"" + section->getId() + "\" already exists.",LOCATION);
    }

    //if(section->parent != nullptr && section->parent != this)
   // {
    //    throw Exception("The section is already associated to another constraints.");
    //}
    
//    section->parent = this;
    
    this->sections.push_back(section);
}

const Section* Constraints::getSection(const string& identifier) const
{
    for(unsigned int i = 0; i < this->sections.size(); i++)
    {
        if(this->sections[i]->getId() == identifier)
        {
            return this->sections[i];
        }
    }

    return nullptr;
}


void Constraints::save(const string& xmlFile)
{
    _STACK;

    Assert(this->validate() == true);

    LOG_VERBOSE << "Saving xml file \"" << xmlFile << "\"..." << ends;

    ofstream output;
    output.open(xmlFile.c_str());

    if(output.is_open() == false)
    {
        throw Exception("Cannot access file \"" + xmlFile + "\"",LOCATION);
    }

    this->writeXml(output);
    output.close();

    LOG_VERBOSE << "Constraints successfully saved" << ends;
}

bool Constraints::validate() const
{
	LOG_DEBUG << "Validating constraints..." << ends;
	
    if(this->prologue == nullptr || this->epilogue == nullptr)
    {
        LOG_WARNING << "Constraints " << this << " is not valid because prologue or epilogue is nullptr" << ends; 
        return false;
    }

    for(unsigned int i = 0; i < this->sections.size(); i++)
    {
        if(this->sections[i]->validate() == false)
        {
        	LOG_WARNING << "Section " << this->sections[i] << " is not valid" << ends;
            return false;
        }
    }

    LOG_DEBUG << "Constraints " << this << " is valid" << ends;
    return true;
}

const GenericMacro* Constraints::getMacroByPath(const string& path)  const
{
    _STACK;

    if(path.empty() == true)
    {
        throw ArgumentException("The argument 'path' cannot be an empty string.",LOCATION);
    }

    string p = path.substr(1);

    string::size_type index = p.find_first_of('/');

    string constraintsName = p.substr(0, index);
    if(constraintsName != this->getId())
    {
        return nullptr;
    }
    p = p.substr(index + 1);
    index = p.find_first_of('/');

    if(index == string::npos)
    {
        if(this->getPrologue().getId() == p)
        {
            return &this->getPrologue();
        }
        else if(this->getEpilogue().getId() == p)
        {
            return &this->getEpilogue();
        }
    }
    else
    {
        string sectionName = p.substr(0, index);
        const Section* section = this->getSection(sectionName);
        if(section != nullptr)
        {
            string newPath = p.substr(index);
            return section->getMacroByPath(newPath);
        }
    }

    return nullptr;
}

bool Constraints::contains(const GenericMacro& macro) const
{
    if(this->prologue == &macro || this->epilogue == &macro)
    {
        return true;
    }

    for(unsigned int i = 0; i < this->sections.size(); i++)
    {
        if(this->sections[i]->contains(macro))
        {
            return true;
        }
    }

    return false;
}

unique_ptr<Constraints> Constraints::fromFile(const string& fileName)
{
    _STACK;

    try
    {
	// open the xml file
	xml::Document constraintsFile;
	constraintsFile.LoadFile(fileName);
    
	// create a new Constraints instance
	Constraints* constraints = new Constraints();
	constraints->filePath = fileName;

	// fill it with the xml data
	constraints->readXml(*constraintsFile.RootElement());

	// return the new instance
	return unique_ptr<Constraints>(constraints);
    }
    catch(const Exception& e)
    {
	//LOG_DEBUG << "I am Constraints::fromFile and I got this exception: \"" << e.what() << "\"." << ends;
	throw;
    }
}

bool Constraints::contains(const Section& section) const
{
    for(unsigned int i = 0; i < this->sections.size(); i++)
    {
        if(this->sections[i] == &section)
        {
            return true;
        }
    }

    return false;
}

Section& Constraints::getSection(unsigned int index) const
{
    _STACK;

    if(index >= sections.size())
    {
        throw IndexOutOfBoundsException("Constraints::sections",LOCATION);
    }

    return *this->sections[index];
}

Section* Constraints::operator[](unsigned int index) const
{
    _STACK;

    if(index >= sections.size())
    {
        throw IndexOutOfBoundsException("Constraints::sections",LOCATION);
    }

    return this->sections[index];
}

