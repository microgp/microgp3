#include "EnhancedPopulationParameters.h"
#include "EnhancedPopulation.h"

using namespace ugp3::core;
using namespace std;

EnhancedPopulationParameters::EnhancedPopulationParameters(EnhancedPopulation* population)
: SpecificIndividualPopulationParameters(population)
{
    this->selector = std::unique_ptr<CandidateSelection>(new TournamentSelectionWithFitnessHole());
}

