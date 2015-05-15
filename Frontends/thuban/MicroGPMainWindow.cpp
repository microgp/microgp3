// Inherited class; the Ui_ugpMainWindow might be changed by
// Qt Designer, so the relevant bindings must operate here

// class definition
#include "MicroGPMainWindow.h"

// MicroGP classes

using namespace std;
using namespace Ui;

/* 	TODO

	- in tabConstraints, add "New" or (better) "Edit" button, connected to a step-by-step XML visual editor (new window)
		- read current constraints; if empty or illegal, propose to start from scratch
		- add section
		- add subsection
		- add macro
		- add typeDefinition
		- add ?
		- save as
		- save
	
	- in tabPopulation, find a smart way to include the maximumFitness stop condition, knowing that the number of fitness parameters
		might vary (and it is actually specified in another field)
	
	- in tabEvolution, in the status bars, write something about what is happening
		- label: which population, which step
		- generationBar: what I am doing
		- evolutionBar: evaluate % of stop conditions, plot the one with the highest value
			- maxGenerations = currentGeneration / maxGenerations
			- maxEvaluations = currentEvaluations / maxEvaluations
			- maximumTime = currentTimeElapsed / maximumTime
			- maximumSteadyStateGenerations = currentSteadyStateGenerations / maximumSteadyStateGenerations
			- maximumFitness = currentBestFitness / maximumFitness
	TODO */


// re-define setupUi to include new bindings
void MicroGPMainWindow::setupUi(QMainWindow* ugpMainWindow)
{
	// run the inherited setupUi
	Ui_ugpMainWindow::setupUi(ugpMainWindow);

	// associate internal pointer to main window
	this->mainWindow = ugpMainWindow;
	
	// TAB: Individual Constraints
	this->setupTabConstraints();
	
	// TAB: Population Settings
	this->setupTabPopulation();

	// TAB: Evolution Start
	this->setupTabEvolution();
	
	// TAB: Results (?)

}
