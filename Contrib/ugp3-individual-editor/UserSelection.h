/***********User****************************************************\
|                                                                       |
| UserSelection.h 	                                                |
|                                                                       |
\***********************************************************************/

#ifndef HEADER_UGP3_CORE_USERSELECTION
#define HEADER_UGP3_CORE_USERSELECTION

// headers from this module
#include <QtCore/QVariant>
//#include <QtGui/QAction>
//#include <QtGui/QApplication>
//#include <QtGui/QButtonGroup>
//#include <QtGui/QComboBox>
//#include <QtGui/QGridLayout>
//#include <QtGui/QHeaderView>
//#include <QtGui/QLabel>
//#include <QtGui/QListWidget>
//#include <QtGui/QMainWindow>
//#include <QtGui/QMenu>
//#include <QtGui/QMenuBar>
//#include <QtGui/QStatusBar>
//#include <QtGui/QTextBrowser>
//#include <QtGui/QTreeWidget>
//#include <QtGui/QWidget>

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextBrowser>
#include <QTreeWidget>
#include <QWidget>

#include "CandidateSelection.h"
#include "EnhancedIndividual.h"

// forward declaration
namespace Ui
{
	class MicroGPMainWindow;
}

namespace ugp3
{

namespace core
{

class UserSelection : public QObject, public CandidateSelection
{

Q_OBJECT

private:
	Ui::MicroGPMainWindow* microGpMainWindow;
	
//	unsigned int numberOfIndividuals;
//	std::vector<Individual*> currentIndividuals;
//	std::vector<EnhancedIndividual*> individualsInWindow;
	
	// Qt elements
//	std::vector<QComboBox*> comboBoxArray;
//	std::vector<QTextEdit*> textEditArray;

public:
	UserSelection();

public:
	virtual void updateEndogenParameters(Population& population);
	virtual std::vector<Individual*> selectIndividuals(
	    	const Population& population,
        	unsigned int count,
		double pressionMultiplier) const;
	
	// needed virtual functions
	virtual std::vector<CandidateSolution*> infinitePressureSelection(const Population& population, unsigned int count) const;
	
	virtual std::vector<ugp3::core::CandidateSolution*> customSelection(const ugp3::core::Population&, unsigned int, double) const;

// methods exclusive of this class
	void setParentWindow(Ui::MicroGPMainWindow* microGpMainWindow);

// slots
public slots:
	void updatePreview(int index);

public: // Xml interface
	virtual void writeXml(std::ostream& output) const;
	virtual void readXml(const xml::Element& element);

public: // CSV interface
	virtual double getCsvVal(void) const;
	virtual std::string getCsvText(void) const;

public: // friend class used for the graphical user interface
	friend class Ui::MicroGPMainWindow;
};


inline double UserSelection::getCsvVal(void) const
{
	return 0;
}

inline std::string UserSelection::getCsvText(void) const
{
	return "User";
}

inline void UserSelection::setParentWindow(Ui::MicroGPMainWindow* microGpMainWindow)
{
	this->microGpMainWindow = microGpMainWindow;
}


} // end namespace
} // end namespace
#endif
