#include "LineInformation.h"

using namespace ugp3;

// constructor
LineInformation::LineInformation(std::string text) :
text(text),
label(""),
macro(""),
occurrence(""),
referenceTo(-1),
matched(false)
{}

// empty constructor
LineInformation::LineInformation() :
text(""),
label(""),
macro(""),
occurrence(""),
referenceTo(-1),
matched(false)
{}

// clear everything, except text
void LineInformation::clearMacro()
{
	this->label = "";
	this->macro = "";
	this->occurrence = "";
	this->referenceTo = -1;
	this->matched = false;
	this->macroParameters.clear();
}
