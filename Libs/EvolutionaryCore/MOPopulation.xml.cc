#include "EvolutionaryCore.h"
using namespace std;
using namespace ugp3::core;

const string MOPopulation::XML_SCHEMA_TYPE = "multiObjective";
const string MOPopulation::XML_CHILD_ELEMENT_MAXLEVEL = "maxLevel";
const string MOPopulation::XML_ATTRIBUTE_VALUE = "value";

struct IndividualComparatorPlaceholder
{
    bool operator()(const CandidateSolution* i1, const CandidateSolution* i2)
    {
        int result = ((MOIndividual *)i1)->getPlaceholder() - ((MOIndividual *)i2)->getPlaceholder();
        return (result > 0);
    }
};

struct IndividualComparatorPlaceholderDec
{
    bool operator()(const CandidateSolution* i1, const CandidateSolution* i2)
    {
        int result = ((MOIndividual *)i1)->getPlaceholder() - ((MOIndividual *)i2)->getPlaceholder();
        return (result < 0);
    }
};

void MOPopulation::readXml(const xml::Element& element)
{
    _STACK;

    SpecificIndividualPopulation::readXml(element);
   
    // sort the individuals
    std::stable_sort(m_individuals.begin(), m_individuals.end(), IndividualComparatorPlaceholderDec());
}

void MOPopulation::writeInnerXml(ostream& output) const
{
    _STACK;
    
    SpecificIndividualPopulation::writeInnerXml(output);

    LOG_DEBUG << "Serializing object ugp3::core::MOPopulation" << ends;

	output << '<' << XML_CHILD_ELEMENT_MAXLEVEL << ' ' << XML_ATTRIBUTE_VALUE << "='" << m_maxLevel << "'/>" << endl;

}



