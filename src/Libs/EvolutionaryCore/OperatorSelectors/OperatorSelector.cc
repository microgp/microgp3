/***********************************************************************\
|                                                                       |
| OperatorSelector.cc                                                   |
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

#include "OperatorSelector.h"
#include "RandomOperatorSelector.h"
#include "SelfAdaptiveOperatorSelector.h"
#include "DMABOperatorSelector.h"
#include "PDMABOperatorSelector.h"
#include "KappaRouletteOperatorSelector.h"
#include "Statistics.h"

using namespace std;

namespace ugp3 {
namespace core {
    
unique_ptr< OperatorSelector > OperatorSelector::instanciate(
    const string& name, Statistics& stats)
{
    if (name == RandomOperatorSelector::XML_NAME) {
        return unique_ptr<OperatorSelector>(new RandomOperatorSelector(stats));
    } else if (name == SelfAdaptiveOperatorSelector::XML_NAME) {
        return unique_ptr<OperatorSelector>(new SelfAdaptiveOperatorSelector(stats));
    } else if (name == DMABOperatorSelector::XML_NAME) {
        return unique_ptr<OperatorSelector>(new DMABOperatorSelector(stats));
    } else if (name == PDMABOperatorSelector::XML_NAME) {
        return unique_ptr<OperatorSelector>(new PDMABOperatorSelector(stats));
    } else if (name == KappaRouletteOperatorSelector::XML_NAME) {
        return unique_ptr<OperatorSelector>(new KappaRouletteOperatorSelector(stats));
    }
    return unique_ptr<OperatorSelector>();
}

void OperatorSelector::prepareForSelections()
{
    m_newOperatorActivated = false;
    
    for (unsigned int i = 0; i < getDataCount(); ++i) {
        Data& data = getData(i);
        if (data.enabled) {
            data.pending = 0;
            if (data.isPseudoDeactivated) {
                ++data.generationsSinceLastToken;
                if (data.generationsSinceLastToken >= 10) {
                    data.tokens = 1;
                    data.generationsSinceLastToken = 0;
                    LOG_VERBOSE << "Operator pseudo-deactivation; "
                    << data.getOperatorName() << " got a new token.." << ends;
                }
            } else {
                data.tokens = m_stats.getPopulation()->getParameters().getLambda();
            }
        }
    }
    
    prepareForSelectionsImpl();
}

OperatorSelector::Result OperatorSelector::select()
{
    /*
     * If all operators are still pseudo-deactivated but have already used
     * their tokens, we redistribute tokens (should happen extremely rarely)
     */
    bool allDeactivatedWithNoToken = true;
    for (unsigned int i = 0; i < getDataCount(); i++) {
        auto& data = getData(i);
        if (data.enabled && !(data.isPseudoDeactivated && data.tokens == 0)) {
            allDeactivatedWithNoToken = false;
        }
    }
    if (allDeactivatedWithNoToken) {
        LOG_ERROR << "All enabled operators are failing repeatedly, check your configuration." << std::ends;
        for (unsigned int i = 0; i < getDataCount(); i++) {
            auto& data = getData(i);
            if (data.enabled && data.isPseudoDeactivated) {
                data.tokens = 3;
                data.generationsSinceLastToken = 0;
            }
        }
    }
    
    // If a pseudo-deactivated operator has a token to use, we choose it.
    for (unsigned int i = 0; i < getDataCount(); i++) {
        auto& data = getData(i);
        if (data.enabled && data.isPseudoDeactivated && data.tokens > data.getCallData().size()) {
            LOG_VERBOSE << "Operator selection: selected " << data.getOperatorName()
            << " because it has a pseudo-deactivation token to use." << std::ends;
            return Result(&data, data.getOperator(), 0);
        }
    }
    
    /*
     * If all enabled operators are out of tokens (none is selectable), redistribute.
     */
    bool tokenShortage = true;
    for (unsigned int i = 0; i < getDataCount(); i++) {
        auto& data = getData(i);
        if (data.getSelectable()) {
            tokenShortage = false;
        }
    }
    if (tokenShortage) {
        LOG_ERROR << "All enabled operators are failing repeatedly, check your configuration." << std::ends;
        for (unsigned int i = 0; i < getDataCount(); i++) {
            auto& data = getData(i);
            if (data.enabled && !data.isPseudoDeactivated) {
                data.tokens = m_stats.getPopulation()->getParameters().getLambda();
                data.generationsSinceLastToken = 0;
            }
        }
    }
    
    return selectImpl();
}

void OperatorSelector::success(const OperatorSelector::Result& selected)
{
    Data& data = *selected.data;
    ++data.pending;
    if (data.isPseudoDeactivated) {
        m_newOperatorActivated = true;
        data.isPseudoDeactivated = false;
        data.tokens = m_stats.getPopulation()->getParameters().getLambda();
        LOG_VERBOSE << "Operator pseudo-deactivation: " << data.getOperatorName()
        << " produced valid children. Activated for good." << ends;
    }
}

void OperatorSelector::failure(const OperatorSelector::Result& selected)
{
    Assert(selected.data->tokens > 0);
    --selected.data->tokens;
}

void OperatorSelector::readXml(const xml::Element& element)
{

}

void OperatorSelector::writeInnerXml(ostream& output) const
{

}


}
}