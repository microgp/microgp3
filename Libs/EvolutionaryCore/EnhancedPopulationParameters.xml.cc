#include "EvolutionaryCore.h"
using namespace ugp3::core;
using namespace std;

const string EnhancedPopulationParameters::XML_SCHEMA_TYPE = "enhanced";

void EnhancedPopulationParameters::writeInternalXml(ostream& output) const
{
    _STACK;

    output
      << "<!-- basic population parameters -->" << endl;

    PopulationParameters::writeInternalXml(output);
}

void EnhancedPopulationParameters::readXml(const xml::Element& element)
{
    _STACK;

    PopulationParameters::readXml(element);

    const xml::Element* childElement = element.FirstChildElement();
    while (childElement != nullptr)
    {
        const string& elementName = childElement->ValueStr();

        childElement = childElement->NextSiblingElement();
    }
}

