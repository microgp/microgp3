#include "MicroGPVisualizeIndividual.h"

// standard classes
#include <fstream>
#include <sstream>
#include <vector>

// ugp3 classes
#include "CandidateSolution.h"
#include "File.h"
#include "Log.h"

using namespace std;
using namespace Ui;
using namespace ugp3;
using namespace ugp3::core;

void MicroGPVisualizeIndividual::setupUi(MicroGPMainWindow* parentWindow, QDialog* parentDialog, const CandidateSolution* bestSolution)
{

	// TODO block window resizing using setWindowsFlags() or something like that
	Ui_VisualizeIndividual::setupUi(parentDialog);
	this->parentWindow = parentWindow;
	
	// set the textEdit in read-only mode
	this->plainTextEdit->setReadOnly(true);
	
	// dump the candidate solution to text, and show it inside the textEdit
	// unfortunately, CandidateSolution only has a method toCode that writes to a FILE (or a set of files)
	// so, we have to behave accordingly
	string filename = ".tmp.best.individual.txt";
	vector<string> outfiles;
	bestSolution->toCode(filename, &outfiles);
	
	// now, read the file and put the text to the textEdit
	LOG_DEBUG << "Candidate solution has been written to file(s):" << ends;
	for(unsigned int f = 0; f < outfiles.size(); f++)
	{
		LOG_DEBUG << "- \"" << outfiles[f] << "\"" << ends;
		ifstream inputFile( outfiles[f].c_str() );
		
		if( !inputFile.is_open() ) return; // TODO: throw an exception?
		
		while( inputFile.good() )
		{
			string buffer;
			getline(inputFile, buffer);
			this->plainTextEdit->appendPlainText( QString::fromStdString( buffer ) );
		}
	}
	
	// finally, remove the file(s)
	for(unsigned int f = 0; f < outfiles.size(); f++) File::remove(outfiles[f]);
	
	// the push button is already connected to the slot that closes the window, so we are done 
	return;
}
