// Inherited class; the  might be changed by
// Qt Designer, so the relevant bindings must operate here

#ifndef MICROGPVISUALIZEINDIVIDUAL
#define MICROGPVISUALIZEINDIVIDUAL

#include "ui_ugp3-visualize-individual.h"

// forward declaration
namespace ugp3
{
namespace core
{
class CandidateSolution;
}
}

namespace Ui 
{

// another forward declaration
class MicroGPMainWindow;

// everything in the Ui_SelectOperators is public
// multiple inheritance requires QObject to be first
class MicroGPVisualizeIndividual : public QObject, public Ui_VisualizeIndividual
{

Q_OBJECT

private:
	QDialog* parentDialog;
	MicroGPMainWindow* parentWindow;

public:
	// general setup
	void setupUi(MicroGPMainWindow* parentWindow, QDialog* parentDialog, const ugp3::core::CandidateSolution* bestSolution);
};

} // namespace Ui

#endif
