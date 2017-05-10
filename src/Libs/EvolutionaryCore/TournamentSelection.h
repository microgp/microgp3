/***********************************************************************\
|                                                                       |
| TournamentSelection.h                                                 |
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

#ifndef HEADER_UGP3_CORE_TOURNAMENTSELECTION
#define HEADER_UGP3_CORE_TOURNAMENTSELECTION

// headers from this module
#include "CandidateSelection.h"
#include<limits>

// forward declaration
namespace Ui
{
	class MicroGPMainWindow;
}

namespace ugp3
{

namespace core
{

class TournamentSelection : public CandidateSelection
{
protected:
	double tau, tauMin, tauMax, metaTau, metaTauMin, metaTauMax;
    
    double fitnessHole = 0.0;

protected:
    static const std::string XML_ATTRIBUTE_TAU;
    static const std::string XML_ATTRIBUTE_TAUMAX;
    static const std::string XML_ATTRIBUTE_TAUMIN;

    static const std::string XML_ATTRIBUTE_METATAU;
    static const std::string XML_ATTRIBUTE_METATAUMAX;
    static const std::string XML_ATTRIBUTE_METATAUMIN;
    
    virtual std::vector< CandidateSolution* > infinitePressureSelection(
        const Population& population, unsigned int count) const;
    
    virtual std::vector< CandidateSolution* > customSelection(
        const Population& population, unsigned int count, double pressureMultiplier) const;
        
    /**
     * Select one candidate from the given range and return an iterator to it.
     * If the boolean @p useHole is set, the candidate will be selected
     * according to the fitness-hole-related comparison, else the function
     * will use selection-related comparison.
     */
    CandVecIt tournamentSelect(const ugp3::core::Population& population, double pressureMultiplier, CandVecIt begin, CandVecIt end, bool useHole) const;
    
    /**
     * Computes a suitable value of tau using the internal parameters.
     */
    unsigned int getActualTau(const Population& population, double pressureMultiplier) const;

public:
    static const std::string XML_SCHEMA_TYPE;

public:
	TournamentSelection();

public:
	virtual void updateEndogenParameters(Population& population);

public:
	double getTau() const { return tau; }
	double getTauMin() const { return tauMin; }
	double getTauMax() const { return tauMax; }

	void setTau(double value);
	void setTauMin(double value);
	void setTauMax(double value);

	double getMetaTau() const { return metaTau; }
	double getMetaTauMin() const { return metaTauMin; }
	double getMetaTauMax() const { return metaTauMax; }

	void setMetaTau(double value);
	void setMetaTauMin(double value);
	void setMetaTauMax(double value);

public: // Xml interface
	virtual void writeXml(std::ostream& output) const;
	virtual void readXml(const xml::Element& element);

public: // CSV interface
    virtual double getCsvVal(void) const;
    virtual std::string getCsvText(void) const;

public: // friend class, used for the graphical user interface
    friend class Ui::MicroGPMainWindow;

};

class TournamentSelectionWithFitnessHole : public TournamentSelection
{
private:
	static const std::string XML_ATTRIBUTE_FITNESSHOLE;
    
public:
    static const std::string XML_SCHEMA_TYPE;

public:
	double getFitnessHole() const { return fitnessHole; }
	void setFitnessHole(double value);

public: // Xml interface
	virtual void writeXml(std::ostream& output) const;
	virtual void readXml(const xml::Element& element);

public: // friend class, used for the graphical user interface
    friend class Ui::MicroGPMainWindow;
};

/**
 * When selecting only one candidate, works just like TournamentSelectionWithFitnessHole.
 * When selecting a group of candidates, first selects good candidates, then selects the most diverse.
 */
class TwoStepTournamentSelectionWithFitnessHole : public TournamentSelectionWithFitnessHole
{
public:
    static const std::string XML_SCHEMA_TYPE;
    
protected:
    virtual std::vector< CandidateSolution* > customSelection(const Population& population, unsigned int count, double pressionMultiplier) const;
};

inline void TournamentSelectionWithFitnessHole::setFitnessHole(double value)
{
    if (value < 0 || value > 1)
        throw ArgumentException("Fitness hole should be in [0, 1].", LOCATION);

    this->fitnessHole = value;
}

inline void TournamentSelection::setTau(double value)
{
	_STACK;

    if(value < tauMin || value > tauMax) throw ArgumentException("Tau should be in [tauMin, tauMax].", LOCATION);

    this->tau = value;
}

inline void TournamentSelection::setMetaTau(double value)
{
	_STACK;

    if(value < metaTauMin || value > metaTauMax || value <= 0 || value > 1.0) throw ArgumentException(XML_ATTRIBUTE_METATAU + " should be in [" + XML_ATTRIBUTE_METATAUMIN + "," + XML_ATTRIBUTE_METATAUMAX + "] and ]0,1[.", LOCATION);

    this->metaTau = value;
}

inline double TournamentSelection::getCsvVal(void) const
{
	if( this->metaTau > 0 )
		return getMetaTau();
	else
		return getTau();
}

inline std::string TournamentSelection::getCsvText(void) const
{
	if( this->metaTau > 0 )
		return "MetaTau";
	else
		return "Tau";
}

}

}

#endif
