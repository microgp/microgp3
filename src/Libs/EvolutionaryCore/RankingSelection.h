/***********************************************************************\
|                                                                       |
| RankingSelection.h 	                                                |
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

#ifndef HEADER_UGP3_CORE_RANKINGSELECTION
#define HEADER_UGP3_CORE_RANKINGSELECTION

// headers from this module
#include "CandidateSelection.h"

// forward declaration
namespace Ui
{
	class MicroGPMainWindow;
}

namespace ugp3
{

namespace core
{
    
class Individual;

class RankingSelection : public CandidateSelection
{
private:
	double pressure, pressureMin, pressureMax;

private:
    /**
     * Return an index into m_selectable.
     */
    unsigned int ranking() const;

protected:
    static const std::string XML_ATTRIBUTE_PRESSURE;
    static const std::string XML_ATTRIBUTE_PRESSUREMIN;
    static const std::string XML_ATTRIBUTE_PRESSUREMAX;
    
    virtual std::vector< CandidateSolution* > infinitePressureSelection(
        const Population& population, unsigned int count) const;
    
    virtual std::vector< CandidateSolution* > customSelection(
        const Population& population, unsigned int count, double pressionMultiplier) const;
        
public:
    static const std::string XML_SCHEMA_TYPE;

public:
	RankingSelection();

public:
        
    virtual void updateEndogenParameters(Population& population);

public:
	double getPressure() const;
	double getPressureMin() const;
	double getPressureMax() const;

	void setPressure(double value);
	void setPressureMin(double value);
	void setPressureMax(double value);

public: // Xml interface
	virtual void writeXml(std::ostream& output) const;
	virtual void readXml(const xml::Element& element);

public: // CSV interface
    virtual double getCsvVal(void) const;
    virtual std::string getCsvText(void) const;

public: // friend class used for the graphical user interface
    friend class Ui::MicroGPMainWindow;
};

inline double RankingSelection::getPressure() const
{
    	return this->pressure;
}

inline double RankingSelection::getPressureMin() const
{
		return this->pressureMin;
}

inline double RankingSelection::getPressureMax() const
{
	return this->pressureMax;
}

inline void RankingSelection::setPressure(double value)
{
	_STACK;

    if(value < pressureMin || value > pressureMax) throw ArgumentException("Pressure should be in [pressureMin, pressureMax].", LOCATION);

    this->pressure = value;
}

inline double RankingSelection::getCsvVal(void) const
{
	return getPressure();
}

inline std::string RankingSelection::getCsvText(void) const
{
	return "Pressure";
}

} // end namespace
} // end namespace
#endif
