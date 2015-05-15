#include "LineInformation.h"

using namespace ugp3;

// constructor
LineInformation::LineInformation(std::string text) :
text(text),
label(""),
macro(""),
occurrence(""),
referenceTo(-1)
{}

// empty constructor
LineInformation::LineInformation() :
text(""),
label(""),
macro(""),
occurrence(""),
referenceTo(-1)
{}
