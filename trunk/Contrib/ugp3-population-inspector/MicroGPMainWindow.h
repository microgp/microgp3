// Inherited class; the Ui_ugpMainWindow might be changed by
// Qt Designer, so the relevant bindings must operate here

#ifndef MICROGPMAINWINDOW
#define MICROGPMAINWINDOW

#include "ui_ugp3-population-inspector.h"

// forward declaration
namespace ugp3
{
	namespace core
	{
		class EvolutionaryAlgorithm;
		class EnhancedIndividual;
	}
}

namespace Ui 
{

// everything in the Ui_ugpMainWindow is public
// multiple inheritance requires QObject to be first
class MicroGPMainWindow : public QObject, public Ui_populationInspectorMainWindow
{

Q_OBJECT

public:
	// pointer to main window
	QMainWindow* mainWindow;
	// Evolutionary Algorithm, storing all data related to evolution
	ugp3::core::EvolutionaryAlgorithm* algorithm;
	
	// starting individual for Entropic Distance computation
	const ugp3::core::EnhancedIndividual* entropicDistanceStartingPoint;
	// all other individuals to compute the Entropic Distance to
	std::vector<const ugp3::core::EnhancedIndividual*> entropicDistanceEndPoints;

public:
	// initialize window
	void setupUi(QMainWindow* mainWindow);

// slot that will be connected to signals generated by objects
public slots:
	// load status file and populate table widget accordingly
	void loadStatusFile();
	// preview selected individual
	void previewIndividual(int row, int column, int previousRow, int previousColumn);
	// set individual as starting point for entropic distance computation
	void setAsStartingPoint();
	// add individual to end points for entropic distance computation
	void addToEndPoints();
	// save current (highlighted) individual to file
	void saveIndividualToFile();
	// show entropic distance window
	void showEntropicDistance();
	// populate table
	void populateStatisticsTable(); 
	// show program information
	void about();
};

} // namespace Ui

#endif
