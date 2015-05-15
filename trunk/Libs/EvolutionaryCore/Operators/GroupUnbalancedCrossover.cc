#include "EvolutionaryCore.h"
#include "Operators/GroupUnbalancedCrossover.h"
#include <Random.h>

using namespace std;
using namespace ugp3::core;

GroupUnbalancedCrossoverOperator::GroupUnbalancedCrossoverOperator()
{ }

bool GroupUnbalancedCrossoverOperator::doSwaps(unsigned int min, unsigned int max, double sigma, Group* child1, Group* child2, std::vector< GEIndividual* >& swappable1, std::vector< GEIndividual* >& swappable2) const
{
    bool didSomething = false;
    do {
        unsigned int rand = ugp3::Random::nextUInteger(0, 1);
        if (rand) {
            // Move from 1 to 2
            if (!swappable1.empty() && child1->getIndividualCount() > min && child2->getIndividualCount() < max) {
                didSomething = moveIndividual(child1, swappable1.back(), child2);
                Assert(didSomething);
                swappable1.pop_back();
            }
        } else {
            // Move from 2 to 1
            if (!swappable2.empty() && child2->getIndividualCount() > min && child1->getIndividualCount() < max) {
                didSomething = moveIndividual(child2, swappable2.back(), child1);
                Assert(didSomething);
                swappable2.pop_back();
            }
        }
    } while (ugp3::Random::nextDouble(0, 1) < sigma);
    return didSomething;
}


const string GroupUnbalancedCrossoverOperator::getName() const
{
    return "groupUnbalancedCrossover";
}

const string GroupUnbalancedCrossoverOperator::getAcronym() const
{
    return "G.UB.X";
}

const string GroupUnbalancedCrossoverOperator::getDescription() const
{
	return 	this->getName() + " selects two groups and moves randomly some individuals between them, changing their compositions and sizes. Operates only when the group evolution is enabled. ";
}