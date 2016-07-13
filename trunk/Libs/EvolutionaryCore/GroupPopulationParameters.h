/***********************************************************************\
|                                                                       |
| GroupPopulationParameters.h                                           |
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
 * @file GroupPopulationParameters.h
 * Definition of the GroupPopulationParameters class.
 * @see GroupPopulationParameters.cc
 * @see GroupPopulationParameters.xml.cc
 */

#ifndef HEADER_UGP3_CORE_GROUPPOPULATIONPARAMETERS
/** Defines that this file has been included */
#define HEADER_UGP3_CORE_GROUPPOPULATIONPARAMETERS

#include "PopulationParameters.h"
#include "GroupFitnessEvaluator.h"
#include "GroupOperator.h"
#include "Statistics.h"
#include "IndividualPopulationParameters.h"
#include "GEIndividual.h"
#include "TournamentSelection.h"

namespace ugp3 {
namespace core {
    
/**
 * Keeps the parameters of a group population
 */
class GroupPopulationParameters : public SpecificIndividualPopulationParameters<GEIndividual>
{
private:
    /** TODO check the meaning and write good explanations for these operators */
    /** Maximum number of individuals per group */
    unsigned int maxIndPerGroup = 0;
    /** Minimum number of individuals per group */
    unsigned int minIndPerGroup = 0;
    /** Number of groups created to setup the population */
    unsigned int groupNu = 0;
    /** Number of groups that survive after a generation */
    unsigned int groupMu = 0;
    bool m_groupFitnessSharingEnabled = 0;
    double m_groupFitnessSharingRadius = 0;
    double m_individualContributionScalingFactor = 0;
    
    GroupFitnessEvaluator m_evaluator;
    
private:
    /** Xml tag to specify the maximum number of individuals per group */
    static const std::string XML_CHILDELEMENT_MAXINDPERGROUP;
    /** Xml tag to specify the minimum number of individuals per group */
    static const std::string XML_CHILDELEMENT_MININDPERGROUP;
    /** DEPRECATED Xml tag to specify the maximum number of groups in the population */
    static const std::string XML_CHILDELEMENT_MAXGROUPS;
    /** DEPRECATED Xml tag to specify the maximum number of groups in the population */
    static const std::string XML_CHILDELEMENT_MINGROUPS;
    static const std::string XML_CHILDELEMENT_GROUP_NU;
    static const std::string XML_CHILDELEMENT_GROUP_MU;
    static const std::string XML_CHILDELEMENT_GROUPFITNESSSHARING;
    static const std::string XML_CHILDELEMENT_INDIVIDUALCONTRIBUTIONSCALINGFACTOR;
    
    virtual void writeInternalXml(std::ostream& output) const;
    
public:
    /** Type of this xml element */
    static const std::string XML_SCHEMA_TYPE;
    
public:
    GroupPopulationParameters(GroupPopulation* population);
    ~GroupPopulationParameters();
    
public:
    /**
     * Selects the given number of groups with the given pressure.
     * If there are not enough groups to select from, this function will return
     * a vector with less than the required number of groups.
     */
    std::vector<Group*> groupSelection(unsigned int cardinality, double pressureMultiplier) const;
    
    // We wanted to enforce different fitness sizes for groups and individuals
    // FIXME delete this when things work
    const GroupFitnessEvaluator& getEvaluator() const noexcept { return m_evaluator; }
    GroupFitnessEvaluator& getEvaluator() noexcept { return m_evaluator; }
    
    unsigned int getIndividualFitnessParametersCount() const {
        return getFitnessParametersCount();
    }
    unsigned int getGroupFitnessParametersCount() const {
        return getEvaluator().getGroupFitnessComponentMultiplier() * getFitnessParametersCount();
    }
    
    /** 
     * Returns the maximum number of individuals that a group can have
     * @returns unsigned int The maximum number of individuals that a group can have
     */
    unsigned int getMaxIndividualsPerGroup() const { return maxIndPerGroup; }
    
    /** 
     * Returns the minimum number of individuals that a group can have
     * @returns unsigned int The minimum number of individuals that a group can have
     */
    unsigned int getMinIndividualsPerGroup() const { return minIndPerGroup; }
    
    /** 
     * Returns the maximum number of groups that a population can have
     * @returns unsigned int The maximum number of groups that a population can have
     */
    unsigned int getGroupNu() const { return groupNu; }
    
    /** 
     * Returns the minimum number of groups that a population can have
     * @returns unsigned int The minimum number of groups that a population can have
     */
    unsigned int getGroupMu() const { return groupMu; }
    
    bool getGroupFitnessSharingEnabled() const { return m_groupFitnessSharingEnabled; }
    double getGroupFitnessSharingRadius() const { return m_groupFitnessSharingRadius; }
    
    /**
     * TODO doc
     * 0 to deactivate
     */
    double getIndividualContributionScalingFactor() const { return m_individualContributionScalingFactor; }
    
public: //Setters 
    /** 
     * Sets the maximum number of individuals that a group can have
     * @param value The maximum number of individuals that a group can have
     */
    void setMaxIndividualsPerGroup(unsigned int value) {
        if (value < minIndPerGroup)
            throw ArgumentException("The maximum number of individuals in a group can't be lower than the minimum", LOCATION);
        maxIndPerGroup=value;
    }
    
    /** 
     * Sets the minimum number of individuals that a group can have
     * @param value The minimum number of individuals that a group can have
     */
    void setMinIndividualsPerGroup(unsigned int value) {
        if (value > maxIndPerGroup)
            throw ArgumentException("The minimum number of individuals in a group can't be greater than the maximum", LOCATION);
        minIndPerGroup=value;
    }
    
    /** 
     */
    void setGroupNu(unsigned int value) {
        groupNu = value;
    }
    
    /** 
     */
    void setGroupMu(unsigned int value) {
        groupMu = value;
    }
    
    void setGroupFitnessSharingRadius(double value) noexcept {
        // TODO check bounds
        m_groupFitnessSharingEnabled = true;
        m_groupFitnessSharingRadius = value;
    }
    
    void setIndividualContributionScalingFactor(double value) {
        if (value < 0) {
            throw ArgumentException("The individual contribution scaling factor must be positive or null.", LOCATION);
        }
        m_individualContributionScalingFactor = value;
    }
    
    /**
     * XML interface.
     */
    virtual const std::string& getType() const noexcept { return XML_SCHEMA_TYPE; }
    virtual void readXml(const xml::Element& element);
};
}
}

#endif
