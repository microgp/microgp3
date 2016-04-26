#include "RegexMatch.h"

// this DEFINE must be commented until regex becomes a part of standard C++11
// or we find a way to organize the regex wrapper that does not need regex.h
//#define USE_EXPERIMENTAL_REGEX

// C++ std libraries
#include <exception>
#include <iostream>

// C POSIX libraries
#ifdef USE_EXPERIMENTAL_REGEX
	#include <regex.h>
#else
	#include <regex>
#endif

#include <string.h>
#include <sys/types.h>

// ugp3 libraries
#include "Exception.h"
#include "LineInformation.h"
#include "Log.h"

using namespace std;
using namespace ugp3;
using namespace std;
using namespace std::regex_constants;

const int RegexMatch::topDown = 0;
const int RegexMatch::bottomUp = 1;

string RegexMatch::stringToRegex( const string& originalString )
{
	// the tricky part is that some special characters
	// must be escaped both for the regex syntax and 
	// for the C++ string syntax
	// regex: ^ $ \ . * + ? ( ) [ ] { } |
	// string: \ %
	string resultingRegex = "";
	
	for(unsigned int i = 0; i < originalString.length(); i++)
	{
		// lots of characters should be replaced by the '\s+' matching
		if( originalString[i] == ' ' || originalString[i] == '\t')
		{
			resultingRegex += "[\\s]+";
			while( originalString[i+1] == ' ' || originalString[i+1] == '\t' ) i++;

		}

		else if( originalString[i] == '%' ) resultingRegex += "\%";

		else if( originalString[i] == '^' ) resultingRegex += "\\^";
		else if( originalString[i] == '$' ) resultingRegex += "\\$";
		else if( originalString[i] == '\\' ) resultingRegex += "\\\\";
		else if( originalString[i] == '.' ) resultingRegex += "\\.";
		else if( originalString[i] == '*' ) resultingRegex += "\\*";
		else if( originalString[i] == '+' ) resultingRegex += "\\+";
		else if( originalString[i] == '?' ) resultingRegex += "\\?";
		else if( originalString[i] == '(' ) resultingRegex += "\\(";
		else if( originalString[i] == ')' ) resultingRegex += "\\)";
		else if( originalString[i] == '[' ) resultingRegex += "\\[";
		else if( originalString[i] == ']' ) resultingRegex += "\\]";
		else if( originalString[i] == '{' ) resultingRegex += "\\{";
		else if( originalString[i] == '}' ) resultingRegex += "\\}";
		else if( originalString[i] == '|' ) resultingRegex += "\\|";

		// also add some spaces before and after the new lines
		else if( originalString[i] == '\n' ) resultingRegex += "[\\s]*\\n[\\s]*";
		else resultingRegex += originalString[i];
	}
	
	return resultingRegex;
}

string RegexMatch::errorCodeToText( int code )
{
	string error;
	
	if( code == error_collate )
		error = "error_collate:	the expression contains an invalid collating element name";
	else if(code == error_ctype)
		error = "error_ctype: the expression contains an invalid character class name";
	else if(code == error_escape)	
		error = "error_escape: the expression contains an invalid escaped character or a trailing escape";
	else if(code == error_backref)
		error = "error_backref: the expression contains an invalid back reference";
	else if(code == error_brack)
		error = "error_brack     the expression contains mismatched square brackets ('[' and ']')";
/*
error_backref	the expression contains an invalid back reference
error_brack	the expression contains mismatched square brackets ('[' and ']')
error_paren	the expression contains mismatched parentheses ('(' and ')')
error_brace	the expression contains mismatched curly braces ('{' and '}')
error_badbrace	the expression contains an invalid range in a {} expression
error_range	the expression contains an invalid character range (e.g. [b-a])
error_space	there was not enough memory to convert the expression into a finite state machine
error_badrepeat	one of *?+{ was not preceded by a valid regular expression
error_complexity	the complexity of an attempted match exceeded a predefined level
error_stack	there was not enough memory to perform a match
*/
	
	return error;
}

unsigned int RegexMatch::regexMatch(string textToMatch, string stringRegex, vector<string>& returnMatches)
{
#ifdef USE_EXPERIMENTAL_REGEX
	// obtain the C-style strings
	char* pattern = (char*)malloc(sizeof(char) * (stringRegex.length()+1));
	strncpy( pattern, stringRegex.c_str(), stringRegex.length() );
	pattern[regex.length()] = '\0';

	char* text = (char*)malloc(sizeof(char) * (textToMatch.length()+1));
	strncpy( text, textToMatch.c_str(), textToMatch.length() );
	text[textToMatch.length()] = '\0';

	LOG_DEBUG << "Matching regex \"" << pattern << "\" with string \"" << text << "\"..." << ends;

	char* pstr = text;
	int errcode;
	regex_t *preg = (regex_t*)malloc(sizeof(regex_t));
	regmatch_t *pmatch;

	if ( (errcode=regcomp(preg, pattern, REG_EXTENDED|REG_NEWLINE)) )
	{
		char* errbuf;
		size_t errbuf_size;

		errbuf_size = regerror(errcode, preg, nullptr, 0);
		if (!(errbuf=(char*)malloc(errbuf_size)))
		{
			perror("malloc error!");
			exit(255);
		}
		regerror(errcode, preg, errbuf, errbuf_size);
		fprintf(stderr, "Regex compilation error: %s\n", errbuf);
		return 0;
	}
	
	// the first match is just a big expression with everything that matched
	unsigned int numberOfMatches = 1 + preg->re_nsub;

	/* Allocate substrings buffer, N+1 because in the presence of multiple parenthesis,  */
	pmatch = (regmatch_t*)malloc( sizeof(regmatch_t) * numberOfMatches );

	/* Match regex to string */
	if(regexec(preg, pstr, numberOfMatches, pmatch, 0))
	{
		LOG_ERROR << "Nothing matched with \"" << pstr << "\"..." << ends;
		free(pmatch);
		free(preg);
		return 0;
	}
	
	/* Loop to collect all the matches */
	LOG_DEBUG << "Collecting ALL the matches for the " << preg->re_nsub << " different expressions!" << ends;
	unsigned int characterStart = 0;
	do {
		if( pmatch[0].rm_so != -1 )
		{
			LOG_DEBUG 	<< "The matching of EVERYTHING goes from character #" << (long int)(characterStart + pmatch[0].rm_so)
					<< " to #" << (long int)(characterStart + pmatch[0].rm_eo) << ends;

			for(unsigned int i = 0; i < numberOfMatches; i++)
			{
				char *submatch;
				size_t matchlen = pmatch[i].rm_eo - pmatch[i].rm_so;
				submatch = (char*)malloc(matchlen+1);
				strncpy(submatch, pstr+pmatch[i].rm_so, matchlen+1);
				submatch[matchlen]='\0';
			
				LOG_DEBUG 	<< "Match[" << i << "] from character #" << (long int)(characterStart + pmatch[i].rm_so) 
						<< " to #" << (long int)(characterStart + pmatch[i].rm_eo) 
						<< ": \"" << submatch << "\"" << ends;
				
				// matches contained in pmatch[0] are everything
				if( i != 0 || numberOfMatches == 1) returnMatches.push_back( submatch );
			
				free(submatch);
			}
		}

		pstr += pmatch[0].rm_eo;	/* Restart from last match */
		characterStart += pmatch[0].rm_eo;
		
		// This debug output is not super-useful
		/*
		LOG_DEBUG 	<< "The pointer moved forward to " << (int) (pstr - text) 	
				<< " or at character #" << characterStart << ends;
		*/
	} while(!regexec(preg, pstr, numberOfMatches, pmatch, 0));
	
	// free (reset/delete) regular expression
	regfree(preg);

	// free allocated memory
	free(pmatch);
	free(text);
	free(pattern);

	return returnMatches.size();
#else
	// when the C++11 regex library starts working properly
	// (that should be gcc-4.9)
	// this part will also work
	
	// important thing to keep in mind for C++11 <regex>
	// - regex_search should be called _several times_ to get all matches inside a string
	// - smatch type has a few interesting parts, so for smatch m:
	//   -- m[0] is the match
	//   -- m.prefix() is what comes before the match
	//   -- m.suffix() is what comes after the match
	try
	{
		// first of all, compile the regex and
		// strongly hope it does not return an error
		regex compiledRegex( stringRegex );
		
		// match!
		smatch matches;
		
		//if( regex_search( textToMatch, matches, compiledRegex) )
		if( regex_match( textToMatch, matches, compiledRegex) )
		{
			LOG_DEBUG << "Match found!" << ends;
			
			// Now, there are two possible cases: 
			if( matches.size() > 1 )
			{
				// if there was a group "()" inside the regex:
				// matches[0] contains the whole text
				// matches[1] contains the matched group, the part between "()"
				// matches[2] contains the rest of the text
				for(unsigned m = 1; m < matches.size(); m++)
				{
					LOG_DEBUG << "matches[" << m << "] = " << matches[m].str() << ends;
					returnMatches.push_back( matches[m].str() );
				}
			}
			else
			{
				// If there was no group, matches[0] simply contains the matched text
				returnMatches.push_back( matches[0].str() );
			}
		}
		else
		{
			// commented, otherwise the debug log becomes unreadable
			//LOG_DEBUG << "No match found..." << ends;
		}
	}
	catch( const regex_error& r )
	{
		LOG_ERROR 	<< r.what() << ": regex_error, error code " << r.code() 
				<< ": \"" << RegexMatch::errorCodeToText( r.code() ) << "\""
				<< ends;
	}

	return returnMatches.size();
#endif
}

// the incredible incremental rollbacking function of ...
// behavior:
// - try to match the given regex inside the limits
// - return false if you cannot match, return true if you can
// - use topLine and bottomLine to delimit the part that was actually matched
// TODO: add reference to the macro, so that it can be inserted in the corresponding lines
bool RegexMatch::incrementalRollbackMatch( string regularExpression, string macroPath, vector<LineInformation>& textToMatch, unsigned int& topLine, unsigned int& bottomLine, int direction )
{
	int currentLine, increment, limit, start;

	// first of all, check the direction and set parameters accordingly
	if( direction == RegexMatch::topDown )
	{
		start = currentLine = topLine;
		increment = 1;
		limit = bottomLine;
	}
	else if( direction == RegexMatch::bottomUp )
	{
		start = currentLine = bottomLine;
		increment = -1;
		limit = topLine;
	}
	else
	{
		LOG_ERROR << "The only two valid values for parameter \"direction\" in function RegexMatch::incrementalRollbackMatch are \"RegexMatch::topDown\" == 0 and \"RegexMatch::bottomUp\" == 1" << ends;
		throw Exception("Invalid value for parameter \"direction\" in function RegexMatch::incrementalRollbackMatch", LOCATION);
	}
	
	LOG_DEBUG << "Current line is " << currentLine << " and I am going " << (( direction == RegexMatch::topDown ) ? "top-down" : "bottom-up") << ", but no more than " << limit << ends;
	
	// if there is no regular expression, return
	if( regularExpression.length() == 0 )
	{
		LOG_WARNING << "Empty regular expression." << ends;
		return false;
	}
	
	// will contain all the current text to be matched
	string currentText = textToMatch[currentLine].text + "\n";
	// will contain all the parameters
	vector<string> matches;
	
	LOG_DEBUG << "Trying to match regular expression \"" << regularExpression << "\" on as many lines of text as possible." << ends;
	
	// while there is no match, let's add lines to the text to match (some macros can match multiple lines)
	while( RegexMatch::regexMatch( currentText, regularExpression, matches) == 0 && currentLine != (limit + increment) )
	{
		currentLine += increment;
		
		if( direction == RegexMatch::topDown )
		{
			currentText += textToMatch[currentLine].text + "\n";
		}
		else // bottom-up
		{
			currentText = textToMatch[currentLine].text + "\n" + currentText;
		}
		
		//LOG_DEBUG << "Current text I'm trying to match:\"" << currentText << "\"" << ends;
	}
	
	// ok, now what happened?
	if( currentLine != (limit + increment) )
	{
		LOG_DEBUG << "Matching! From line #" << start << " to #" << currentLine << ends;

		// unfortunately, I have to recompile the regex
		// here, to get some additional information;
		// since there was a match, try{} shouldn't be necessary
		regex compiledRegex( regularExpression );
		
		if( direction == RegexMatch::topDown )
		{
			for(unsigned int i = start; i <= currentLine; i++)
			{
				textToMatch[i].matched = true;
				textToMatch[i].occurrence = start;
				textToMatch[i].macro = macroPath;
			}
			
			// change the two limits!
			topLine = start;
			bottomLine = currentLine;
			
			// now, if there were groups, write the
			// matches inside the first line of the macro
			if( compiledRegex.mark_count() > 0 )
			{
				textToMatch[start].macroParameters = matches;
			}
		}
		else // bottom-up
		{
			for(unsigned int i = start; i >= currentLine ; i--)
			{
				textToMatch[i].matched = true;
				textToMatch[i].occurrence = currentLine;
				textToMatch[i].macro = macroPath;
			}

			// change the two limits!
			topLine = currentLine;
			bottomLine = start;
			
			// now, if there were groups, write the
			// matches inside the first line of the macro
			if( compiledRegex.mark_count() > 0 )
			{
				textToMatch[currentLine].macroParameters = matches;
			}
		}

		// TODO: see line #198 BORG.pl 
		
		// return
		return true;
	}
	else
	{
		LOG_DEBUG << "No match found." << ends;
		return false;
	}
}

/* old function
int RegexMatch::incrementalRollbackMatch( string regularExpression, vector<LineInformation>& textToMatch, int topLine, int bottomLine, int direction )
{
	int currentLine, increment, limit, start;

	// first of all, check the direction and set parameters accordingly
	if( direction == RegexMatch::topDown )
	{
		start = currentLine = topLine;
		increment = 1;
		limit = bottomLine;
	}
	else if( direction == RegexMatch::bottomUp )
	{
		start = currentLine = bottomLine;
		increment = -1;
		limit = topLine;
	}
	else
	{
		LOG_ERROR << "The only two valid values for parameter \"direction\" in function RegexMatch::incrementalRollbackMatch are \"RegexMatch::topDown\" == 0 and \"RegexMatch::bottomUp\" == 1" << ends;
		throw Exception("Invalid value for parameter \"direction\" in function RegexMatch::incrementalRollbackMatch", LOCATION);
	}
	
	LOG_DEBUG << "Current line is " << currentLine << " and I am going " << (( direction == RegexMatch::topDown ) ? "top-down" : "bottom-up") << ", but no more than " << limit << ends;
	
	// if there is no regular expression, return
	if( regularExpression.length() == 0 )
	{
		LOG_WARNING << "Empty regular expression." << ends;
		return currentLine;
	}
	
	// will contain all the current text to be matched
	string currentText = textToMatch[currentLine].text + "\n";
	// will contain all the parameters
	vector<string> matches;
	
	LOG_DEBUG << "Trying to match regular expression \"" << regularExpression << "\" on as many lines of text as possible." << ends;
	
	// while there is no match, let's add lines to the text to match (some macros can match multiple lines)
	while( RegexMatch::regexMatch( currentText, regularExpression, matches) == 0 && currentLine != limit )
	{
		currentLine += increment;
		
		if( direction == RegexMatch::topDown )
		{
			currentText += textToMatch[currentLine].text + "\n";
		}
		else // bottom-up
		{
			currentText = textToMatch[currentLine].text + "\n" + currentText;
		}
		
		LOG_DEBUG << "Current text I'm trying to match:\"" << currentText << "\"" << ends;
	}
	
	// ok, now what happened?
	if( currentLine != limit )
	{
		LOG_DEBUG << "Matching! From line #" << start << " to #" << currentLine << ends;
		// TODO: add reference to macro
		
		if( direction == RegexMatch::topDown )
		{
			textToMatch[currentLine].occurrence = start;
		}
		else // bottom-up
		{
			textToMatch[currentLine].occurrence = currentLine;
		}

		// TODO: see line #198 BORG.pl 
		// this is just a test for the svn commit mechanism
		
		// return the next unmatched line
		return currentLine+1;
	}
	else
	{
		LOG_DEBUG << "No match found." << ends;
		return limit;
	}
}
*/
