/**
 * @file LineInformation.h
 * Definition of the LineInformation class, used to assimilate text files.
 * @see LineInformation.cc
 */

#ifndef HEADER_UGP3_LINEINFORMATION
#define HEADER_UGP3_LINEINFORMATION

#include <string>
#include <vector>

namespace ugp3
{

	class LineInformation
	{
	// everything is public
	public:
		// attributes
		std::string text;
		std::string label;
		std::string macro;
		std::string occurrence;
		int referenceTo;
		bool matched;

		std::vector<std::string> macroParameters;

		// constructor
		/**
		 * Creates instance starting from text.
		 * @param text String used as text.
		*/
		LineInformation(std::string text);
		/**
		 * Creates empty instance of the class.
		*/	
		LineInformation();
		/** 	Resets everything, except for the text
		*/
		void clearMacro();
	};
}

#endif
