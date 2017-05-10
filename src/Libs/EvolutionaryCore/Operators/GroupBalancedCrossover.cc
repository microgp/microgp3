#include "Operators/GroupBalancedCrossover.h"
#include "ArgumentException.h"
#include "GroupPopulationParameters.h"
#include <GEIndividual.h>
#include <Random.h>

#include <vector>
#include <algorithm>

using namespace std;
using namespace ugp3::core;

GroupBalancedCrossoverOperator::GroupBalancedCrossoverOperator()
{ }

bool GroupBalancedCrossoverOperator::doSwaps(unsigned int min, unsigned int max, double sigma,
                                             Group* child1, Group* child2,
                                             std::vector< GEIndividual* >& swappable1, std::vector< GEIndividual* >& swappable2) const
{
    while (!swappable1.empty() && !swappable2.empty()) {
        bool done = swapIndividuals(child1, swappable1.back(), child2, swappable2.back());
        Assert(done);
        swappable1.pop_back();
        swappable2.pop_back();
        
        if (ugp3::Random::nextDouble(0, 1) >= sigma)
            break;
    }
    return true;
}

const string GroupBalancedCrossoverOperator::getName() const
{
    return "groupBalancedCrossover";
}

const string GroupBalancedCrossoverOperator::getAcronym() const
{
    return "G.B.X";
}

const string GroupBalancedCrossoverOperator::getDescription() const
{
    return this->getName() + " selects two groups and performs a random number of individual exchanges between them, whitout changing the size of the resulting groups. Operates only when the group evolution is enabled.";
}