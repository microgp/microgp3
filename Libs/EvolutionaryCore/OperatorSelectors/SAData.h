/***********************************************************************\
|                                                                       |
| SAData.h |
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
 * @file SAData.h
 * Blah
 */

#ifndef HEADER_UGP3_CORE_SADATA
#define HEADER_UGP3_CORE_SADATA

#include "Data.h"

namespace ugp3 {
namespace core {

class SAData : public Data
{
private: // Data used by the self-adaptive operator selector

    // The current activation weight of the operator: it is updated at the end of each generation.
    double weight = 1.0;

    // The minimum acceptable value for the operator's activation.
    double minWeight = 0.0;

    // The minimum acceptable value for the operator's activation.
    double maxWeight = 1.0;
    
protected:
    virtual void clear();
    
    /** Tag to set the maximum attribute in a xml element */
    static const std::string XML_ATTRIBUTE_MAXIMUM;
    /** Tag to set the current attribute in a xml element */
    static const std::string XML_ATTRIBUTE_CURRENT;
    /** Tag to set the minimum attribute in a xml element */
    static const std::string XML_ATTRIBUTE_MINIMUM;
    /** Tag to set the weight attribute in a xml element */
    static const std::string XML_CHILD_ELEMENT_WEIGHT;
    
public:
    SAData();
    SAData(Operator& op);
    SAData(const std::string& operatorName);
    
    /**
     * Returns the minimum operator's activation weight.
     * @returns double The minimum weight
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    double getMinWeight() const;
    /**
     * Returns the maximum operator's activation weight.
     * @returns double The maximum weight
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    double getMaxWeight() const; 
    /**
     * Returns the current operator's activation weight.
     * @returns double The current weight
     * @throws nothing. If an exception is thrown, the execution is aborted.
     */
    double getWeight() const;
    /**
     * Sets the current operator's activation weight.
     * @param value The current weight to set
     * @throws std::exception ArgumentOutOfRangeException if the weight isn't whithin the limits and the macro MY_PATCH_NOBOUNDS isn't defined.
     */
    void setWeight(double value);
    /**
     * Sets the minimum operator's activation weight.
     * @param value The minimum weight to set
     * @throws std::exception ArgumentOutOfRangeException if the weight isn't whithin 0 and the maximum
     */
    void setMinWeight(double value);
    /**
     * Sets the maximum operator's activation weight.
     * @param value The maximum weight to set
     * @throws std::exception ArgumentOutOfRangeException if the weight isn't whithin the minimum and 1
     */
    void setMaxWeight(double value);
    
    virtual void dumpStatistics(std::ostream& output) const override;
    virtual void dumpStatisticsHeader(const std::string& name, std::ostream& output) const override;
    
    virtual void writeInnerXml(std::ostream& output) const override;
    virtual void readXml(const xml::Element& element) override;
};

}
}

#endif // HEADER_UGP3_CORE_SADATA
