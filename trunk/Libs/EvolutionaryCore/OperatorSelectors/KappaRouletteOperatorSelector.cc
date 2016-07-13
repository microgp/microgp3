/***********************************************************************\
|                                                                       |
| KappaRouletteOperatorSelector.cc |
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
 * @file KappaRouletteOperatorSelector.cc
 *
 */

#include "KappaRouletteOperatorSelector.h"
#include "Random.h"

namespace ugp3 {
namespace core {

const std::string KappaRouletteOperatorSelector::XML_NAME = "kappaRouletteDMAB";
const std::string KappaRouletteOperatorSelector::XML_CHILD_ELEMENT_KAPPA = "kappa";
const std::string KappaRouletteOperatorSelector::XML_ATTRIBUTE_VALUE = "value";

KappaRouletteOperatorSelector::KappaRouletteOperatorSelector(Statistics& stats)
: DMABOperatorSelector(stats)
{
}

OperatorSelector::Result KappaRouletteOperatorSelector::selectWithMAB() const
{
    // Russian roulette exponentiated DMAB scores
    std::vector<OperatorSelector::Result> results;
    
    double maxScore = 0;
    for (unsigned int i = 0; i < getDataCount(); i++) {
        auto& data = getData(i);
        if (data.getSelectable()) {
            auto score = std::max(0., getMABScore(data));
            results.emplace_back(&data, data.getOperator(), score);
            maxScore = std::max(maxScore, score);
        }
    }
    Assert(maxScore > 0);
    
    if (results.empty()) {
        throw "DMAB: no selectable operator.";
    }
    
    // Normalize and exponentiate
    double totalScore = 0;
    for (auto& result: results) {
        result.score = std::pow(result.score/maxScore, kappa);
        totalScore += result.score;
    }
    
    double chosen = ugp3::Random::nextDouble(0, totalScore);
    double cumSum = 0;
    for (auto& result: results) {
        cumSum += result.score;
        if (cumSum > chosen) {
            return result;
        }
    }
    Assert(false);
    return results.front();
}

void KappaRouletteOperatorSelector::readXml(const xml::Element& element)
{
    DMABOperatorSelector::readXml(element);
    
    const xml::Element* childElement = element.FirstChildElement();
    while (childElement) {
        const auto& elementName = childElement->ValueStr();
        if (elementName == XML_CHILD_ELEMENT_KAPPA) {
            kappa = xml::Utility::attributeValueToDouble(*childElement, XML_ATTRIBUTE_VALUE);
            if (kappa < 0) {
                throw ArgumentException("The kappa parameter for roulette-wheel "
                "selection must be >= 0.", LOCATION);
            }
            LOG_VERBOSE << "KappaRoulette: using kappa=" << kappa << std::ends;
        }
        childElement = childElement->NextSiblingElement();
    }
}

void KappaRouletteOperatorSelector::writeInnerXml(std::ostream& output) const
{
    output << "<kappa value=\"" << kappa << "\" />\n";
    
    DMABOperatorSelector::writeInnerXml(output);
}


}
}
