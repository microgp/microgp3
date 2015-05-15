#ifndef REGEXMATCH
#define REGEXMATCH

#include <string>
#include <vector>

namespace ugp3
{

// forward declaration
class LineInformation;

	class RegexMatch
	{
	private:
		// constructor is private, because this class is static
		RegexMatch();
	public:
		// some constants
		static const int topDown;
		static const int bottomUp;

		// return a string for an error code
		static std::string errorCodeToText( int code );

		// this is basically a C++ wrapper of regex.h
		// that will later become wrapper for the <regex> C++11 library
		static unsigned int regexMatch(std::string textToMatch, std::string regex, std::vector< std::string >& returnMatches);
		
		// incremental rollback matching function
		static int incrementalRollbackMatch( std::string regularExpression, std::vector<LineInformation>& textToMatch, int topLine, int bottomLine, int direction );
	};

}

#endif
