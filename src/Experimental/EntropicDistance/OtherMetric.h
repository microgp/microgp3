#ifndef OTHERMETRIC
#define OTHERMETRIC

#include <iostream>
#include <vector>

// forward declarations
namespace ugp3
{
	namespace core
	{
		class EnhancedIndividual;
	}
}

// class implementing a problem-specific distance metric between individuals
class OtherMetric
{
public:
	std::string name;

public:
	OtherMetric();
	double compute(const ugp3::core::EnhancedIndividual* individual1, const ugp3::core::EnhancedIndividual* individual2);
	void getTokens(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);
};

#endif
