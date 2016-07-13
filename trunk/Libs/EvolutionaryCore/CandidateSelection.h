/***********************************************************************\
|                                                                       |
| IndividualSelection.h                                                 |
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

/**
 * @file CandidateSelection.h
 * Definition of the CandidateSelection class.
 * @see CandidateSelection.xml.cc
 */

#ifndef HEADER_UGP3_CORE_INDIVIDUALSELECTION
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_INDIVIDUALSELECTION

#include "CandidateSolution.h"
#include "Random.h"

#include <numeric>

// forward declaration
namespace Ui
{
	class MicroGPMainWindow;
}

/**
 * ugp3 namespace
 */
namespace ugp3
{

/**
 * ugp3::core namespace
 */
namespace core
{

class Population;

class CandidateSelection : public xml::XMLIFace
{
private:
    CandidateSelection(CandidateSelection&);
    CandidateSelection& operator=(CandidateSelection& selection);
    
    
protected:
    // Reverse the selection?
    mutable bool m_reverse = false;
    // List of candidates among which the selection is performed.
    mutable std::vector<CandidateSolution*> m_selectable;
    
    typedef std::vector<CandidateSolution*>::iterator CandVecIt;
    
    void sortByFitness(const Population& population, CandVecIt begin, CandVecIt end) const;
    void sortByHole(const Population& population, CandVecIt begin, CandVecIt end) const;
    
    /**
     * Utility function to avoid code duplication.
     * Does `count' times the following operations:
     *  1. select an element from the given range
     *  2. loop on the range minus the selected element
     * except that it leaves the range in its inital state on returning.
     * WARNING: during the selections, however, the ordering is NOT
     * preserved: use only for random selections.
     * WARNING: the selection function must NOT modify the range.
     */
    template <typename FSel>
    void uniqueSelectionsAndDontMessVector(unsigned int count, CandVecIt begin, CandVecIt end, FSel sel) const {
        Assert(std::distance(begin, end) >= count);
        std::function<void(unsigned int)> rec = [&] (unsigned int i) {
            if (i == count)
                return;
            CandVecIt it = sel(begin + i, end);
            Assert(*it);
            std::iter_swap(begin + i, it);
            rec(i + 1);
            std::iter_swap(begin + i, it);
        };
        rec(0);
    }
    
    /**
     * Subclasses must define selection in the case of infinite pressure.
     */
    virtual std::vector<CandidateSolution*> infinitePressureSelection(const Population& population, unsigned int count) const = 0;
    
    /**
     * Subclasses must override this function with their custom
     * selection behavior. When this function is called, pressureMultiplier
     * is guaranted to be in ]0, +inf[.
     */
    virtual std::vector<CandidateSolution*> customSelection(
        const Population& population, unsigned int count, double pressionMultiplier) const = 0;
    
public:
    /** Name of this xml element */
    static const std::string XML_NAME;
    /** Tag used to specify the type of a xml attribute */
    static const std::string XML_ATTRIBUTE_TYPE;
    
    /** 
     * Instantiates a CandidateSelection with the information specified by the xml element
     * @param element Xml element with the CandidateSelection information to instantiate
     * @throws Any exception Exception if the CandidateSelection type to instantiate is unknown
     */
    static std::unique_ptr<CandidateSelection> instantiate(const xml::Element& element);
    
public:
    /** 
     * Constructor of the class. NOT IMPLEMENTED.
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    CandidateSelection();
    
    /** 
     * Selects `count' individuals from the given range of selectable candidates.
     * Only live candidates are selected.
     * If the given range of selectable candidates is too small,
     * the function will return less candidates than the requested number.
     * To perform a reverse selection (select the worst instead of the best),
     * give a negative @p pressionMultiplier value.
     */
    template <typename RandomAccessIterator>
    std::vector<CandidateSolution*> selectCandidates(
        RandomAccessIterator begin, RandomAccessIterator end,
        const Population& population, unsigned int count, double pressionMultiplier) const {
        if (count == 0) {
            return {};
        }
        m_selectable.clear();
        for (auto it = begin; it != end; ++it) {
            Assert(*it);
            if ((*it)->isAlive()) {
                m_selectable.push_back(*it);
            }
        }
        // NOTE/DET Sort selectable by id so that the selection depends only on random state and comparison functions.
        std::sort(m_selectable.begin(), m_selectable.end(), CandidateSolution::OrderById());
        
        if (m_selectable.size() <= count) {
            LOG_VERBOSE << "Candidate selection: not enough candidates to select from, returning all of them." << std::ends;
            return m_selectable;
        }
        
        // Maybe reverse the selection
        m_reverse = (pressionMultiplier < 0.0);
        pressionMultiplier = std::abs(pressionMultiplier);
        
        std::vector<CandidateSolution*> selectedCandidates;
        // Some special cases that might be used by special operators
        // (e.g. all Differential Evolution ones)
        if (pressionMultiplier == 0) {
            LOG_DEBUG << "Candidate selection : a pressure multiplier of " << pressionMultiplier << " is detected" << std::ends;
            // random choice
            uniqueSelectionsAndDontMessVector(
                count, m_selectable.begin(), m_selectable.end(),
                [&] (CandVecIt begin, CandVecIt end) {
                    // Select a random individual
                    CandVecIt selected = begin + ugp3::Random::nextUInteger(0, std::distance(begin, end) - 1);
                    selectedCandidates.push_back(*selected);
                    return selected;
                });
        } else if (pressionMultiplier == std::numeric_limits<double>::infinity()) {
            selectedCandidates = infinitePressureSelection(population, count);
        } else {
            selectedCandidates = customSelection(population, count, pressionMultiplier);
        }
        
        return selectedCandidates;
    }
    
    /** 
     * Updates the endogen parameters (like the number of individual to take in a selection) in base to the results of the previous steps in the evolution of the population specified.
     * @param population Population to use to update the parameters
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    virtual void updateEndogenParameters(Population& population) = 0;
    
public: // Xml interface
    virtual const std::string& getXmlName() const;
        
public: // CSV interface
    virtual double getCsvVal(void) const = 0;
    virtual std::string getCsvText(void) const = 0;
    
public: // friend class, used for the gui
    friend class Ui::MicroGPMainWindow;
};


inline const std::string& CandidateSelection::getXmlName() const
{
    return XML_NAME;
}

inline CandidateSelection::CandidateSelection()
{ }

}

}

#endif
