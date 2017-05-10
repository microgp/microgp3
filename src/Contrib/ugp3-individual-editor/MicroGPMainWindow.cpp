// Inherited class; the Ui_ugpMainWindow might be changed by
// Qt Designer, so the relevant bindings must operate here

// class definition
#include "MicroGPMainWindow.h"

// Qt classes
#include <QCheckBox>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>

// MicroGP classes
#include "File.h"
#include "ConsoleHandler.h"
#include "ConstantParameter.h"
#include "EnhancedIndividual.h"
#include "EnhancedPopulation.h"
#include "EvolutionaryAlgorithm.h"
#include "EvolutionaryCore.h"
#include "Log.h"
#include "Statistics.h"

#include "Program.h"

// other classes
#include "UserSelection.h"

// define
#define MENU_ADD_GRAPH 0
#define MENU_ADD_SUBGRAPH 1
#define MENU_ADD_NODE 2
#define MENU_REMOVE_GRAPH 0
#define MENU_REMOVE_SUBGRAPH 1
#define MENU_REMOVE_NODE 2
#define MENU_ADD 0
#define MENU_REMOVE 1
#define MENU_CHANGE_PARAMETERS 3
#define MENU_CHANGE_MACRO 4

#define TABLE_COLUMN_NAME 0
#define TABLE_COLUMN_EXPRESSION 1

#define TEXT_INDIVIDUAL "Individual: "
#define TEXT_SECTION "Graph of Section: "
#define TEXT_SUBSECTION "Subgraph of SubSection: "
#define TEXT_MACRO "Node of Macro: "
#define TEXT_UNSAVED "*"

#define INNERLABEL_TYPE_ALL "InnerLabel"
#define INNERLABEL_TYPE_BACKWARD "InnerBackwardLabel"
#define INNERLABEL_TYPE_FORWARD "InnerForwardLabel"

#define DEFAULT_POPULATION_NAME "createdByIndividualEditor"
#define DEFAULT_MU 40
#define DEFAULT_NU 40
#define DEFAULT_LAMBDA 40
#define DEFAULT_SIGMA 0.9
#define DEFAULT_INERTIA 0.9


#define ABOUT_PROGRAMNAME "Individual Editor v0.5 (alpha)"
#define ABOUT_INFO "Individual Editor is part of the MicroGP3 (ugp3) project, coded by Alberto Tonda <alberto.tonda@gmail.com>. For more information, visit http://ugp3.sourceforge.net"

using namespace std;
using namespace Ui;

/* 	TODO

	TODO */


// re-define setupUi to include new bindings
void MicroGPMainWindow::setupUi(QMainWindow* ugpMainWindow)
{
	// run the inherited setupUi
	Ui_individualEditorMainWindow::setupUi(ugpMainWindow);

	// store the reference to the main window
	this->mainWindow = ugpMainWindow;
	
	// connect the signals to the slots
	// load constraints from the menu
	connect(this->actionLoad_Constraints, SIGNAL(triggered()), this, SLOT(loadConstraints()));
	// load population
	connect(this->actionLoad_Population, SIGNAL(triggered()), this, SLOT(loadPopulation()));
	// new individual
	connect(this->actionNew_Individual, SIGNAL(triggered()), this, SLOT(newIndividual()));
	// save population
	connect(this->actionSave_Population, SIGNAL(triggered()), this, SLOT(savePopulation()));
	// exit, but before that, release everything
	connect(this->actionExit, SIGNAL(triggered()), this, SLOT(exitAndRelease()));
	
	// the selection changes in the list widget
	connect(this->listWidget_Population, SIGNAL(currentRowChanged(int)), this, SLOT(changeCurrentIndividual(int)));
	
	// right click in the tree widget
	// set the proper policy for the widget
	connect(this->treeWidget_individualStructure, SIGNAL(customContextMenuRequested(QPoint)), 
		this, SLOT(rightClickMenuTreeWidget(QPoint)));
	
	// open console handler
	ugp3::log::ConsoleHandler* debugHandler = new ugp3::log::ConsoleHandler();
	::log_.addHandler(*debugHandler); //::log_ is a global instance of a Log class
	debugHandler->setLevel(ugp3::log::Level::Debug);
	
	// create right-click menu for the tree widget
	this->treeWidgetContextMenu = new QMenu( this->treeWidget_individualStructure );

	QMenu* treeWidgetContextMenuAdd = new QMenu( this->treeWidgetContextMenu );
	treeWidgetContextMenuAdd->setTitle( "Add" );
	treeWidgetContextMenuAdd->addAction( "Add graph from section" );
	treeWidgetContextMenuAdd->addAction( "Add subgraph from subsection" );
	treeWidgetContextMenuAdd->addAction( "Add node from macro" );
	
	QMenu* treeWidgetContextMenuRemove = new QMenu( this->treeWidgetContextMenu );
	treeWidgetContextMenuRemove->setTitle( "Remove" );
	treeWidgetContextMenuRemove->addAction( "Remove graph" );
	treeWidgetContextMenuRemove->addAction( "Remove subgraph" );
	treeWidgetContextMenuRemove->addAction( "Remove node" );

	this->treeWidgetContextMenu->addMenu( treeWidgetContextMenuAdd );
	this->treeWidgetContextMenu->addMenu( treeWidgetContextMenuRemove );
	this->treeWidgetContextMenu->addSeparator();
	this->treeWidgetContextMenu->addAction( "Change macro parameters" );
	this->treeWidgetContextMenu->addAction( "Change macro to..." );
	
	// tie actions from menu with slots
	connect(this->treeWidgetContextMenu->actions().at( MENU_CHANGE_PARAMETERS ), SIGNAL(triggered()), 
		this, SLOT(changeMacroParameters())); 
	connect(treeWidgetContextMenuAdd->actions().at( MENU_ADD_NODE ), SIGNAL(triggered()), 
		this, SLOT(addNode())); 
	connect(treeWidgetContextMenuRemove->actions().at( MENU_REMOVE_NODE ), SIGNAL(triggered()), 
		this, SLOT(removeNode())); 

	// set some internal pointers to null
	this->constraints = nullptr;
	this->evolutionaryAlgorithm = nullptr;
	this->population = nullptr;
	this->currentIndividual = nullptr;
	
	// register the operators (needed later, both to load the status file and to execute the single operators on the individuals)
	ugp3::frontend::Program::registerOperators();
	
	// create menu to select an operator
	QMenu* operatorsMenu = new QMenu( this->menuIndividual );
	operatorsMenu->setTitle( "Apply operator..." );
	for(unsigned int op = 0; op < ugp3::core::GeneticOperator::getOperatorCount(); op++)
	{
		operatorsMenu->addAction( QString::fromStdString( ugp3::core::GeneticOperator::getOperator(op)->getName() ) );
	}
	this->menuIndividual->addMenu( operatorsMenu );
	
	// bind the selection of an action in the menu to slot that infers the operator and creates the new individuals
	connect(operatorsMenu, SIGNAL(triggered(QAction*)), this, SLOT(applyOperator(QAction*)));
	
	// bind the selection of an action
	connect(this->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	
	// warning here
	/*
	QMessageBox::warning( this->textBrowser_individualPreview, tr("Warning: alpha version"),
				"This version of Individual Editor is a work in progress. It has been tested with "
				"individuals composed of integer, float and constant parameters, but it still might have some "
				"issues with individuals containing references. Expect Individual Editor to crash often. "
				"Expect it to crash at the worst possible time. It still hates humans.");
	*/
}

// function to load constraints file and populate the selection windows
void MicroGPMainWindow::loadConstraints()
{
	// display warning
	if( this->constraints != nullptr )
	{
		if( QMessageBox::question(this->textBrowser_individualPreview, tr("Warning!"), 
		tr("When a new constraints file is loaded, all current individuals are discarded. If they are not saved, you will lose your precious work. Are you sure you want to continue?"), QMessageBox::Ok, QMessageBox::Cancel ) != QMessageBox::Ok) return;
	}

	// get the file name
	QString constraintsFile = 
		QFileDialog::getOpenFileName(	this->textBrowser_individualPreview, 
						tr("Open XML Constraints File"), "", 
						tr("XML (*.xml);;All Files (*)"));

	if( !constraintsFile.isEmpty() )
	{
		// try to create constraints from file
		unique_ptr<ugp3::constraints::Constraints> tempConstraints = 
			ugp3::constraints::Constraints::fromFile( constraintsFile.toStdString() );

		if( tempConstraints.get() != nullptr )
		{
			// reset all things
			// reset individual preview
			this->textBrowser_individualPreview->clear();
			// reset tree widget
			this->treeWidget_individualStructure->clear();
			// reset list widget
			this->listWidget_Population->clear();
			// population has not been saved
			this->unsavedMap.clear();
			
			// delete current constraints, replace with new one
			if( this->constraints != nullptr ) delete( this->constraints );
			this->constraints = tempConstraints.release();
	
			// create new evolutionary algorithm
			LOG_DEBUG << "Building new evolutionary algorithm..." << ends;
			if( this->evolutionaryAlgorithm != nullptr ) delete( this->evolutionaryAlgorithm );
			this->evolutionaryAlgorithm = new ugp3::core::EvolutionaryAlgorithm();
			
			// create new population parameters
			/*
			LOG_DEBUG << "Building parameters..." << ends;
			unique_ptr<ugp3::core::EnhancedPopulationParameters> parameters( new ugp3::core::EnhancedPopulationParameters() );
			// TODO: set some default value for the parameters?
			parameters->setMu( DEFAULT_MU );
			parameters->setNu( DEFAULT_NU );
			parameters->setLambda( DEFAULT_LAMBDA );
			parameters->setInertia( DEFAULT_INERTIA );
			parameters->setSigma( DEFAULT_SIGMA );
			*/

			// create new population
			LOG_DEBUG << "Building population..." << ends;
			if( this->population != nullptr) delete(this->population);
			unique_ptr<ugp3::core::Population> population( 
				new ugp3::core::EnhancedPopulation( /*parameters,*/ *this->evolutionaryAlgorithm ) 
			);
			// add population to the algorithm
			this->evolutionaryAlgorithm->addPopulation( std::move(population) );
			// this below is just some default setting to avoid crashes
			this->evolutionaryAlgorithm->getPopulation(0).getParameters().setFitnessParametersCardinality(1);
			this->evolutionaryAlgorithm->getPopulation(0).getParameters().setConstraints( *this->constraints );
			this->population = (ugp3::core::EnhancedPopulation*) &this->evolutionaryAlgorithm->getPopulation(0);
			this->population->setName( DEFAULT_POPULATION_NAME );

			// create new individual instance
			LOG_DEBUG << "Building new individual..." << ends;
			vector<string> emptyParentList;
			unique_ptr<ugp3::core::Individual> individual( 
				new ugp3::core::EnhancedIndividual( 	0, ugp3::core::GeneticOperator::NO_OPERATOR, 
									emptyParentList, 
									*this->population ) );
			// bind individual's cgraphcontainer to constraints
			LOG_DEBUG << "Setting individual constraints..." << ends;
			unique_ptr<ugp3::ctgraph::CGraphContainer> graphContainer( new ugp3::ctgraph::CGraphContainer() );
			graphContainer->setConstrain(*this->constraints);
			individual->setGraphContainer( std::move(graphContainer) );

			// reset current individual
			this->currentIndividual = (ugp3::core::EnhancedIndividual*) individual.get();
			// and create the individual
			this->createIndividual();
			
			// individual has not been saved
			this->unsavedMap[ individual->getId() ] = false;
			// insert individual in the individual array
			//this->population->addIndividual( individual );
			this->individuals.push_back( (ugp3::core::EnhancedIndividual*) individual.release() );
			// the individual should already be inside the population, so update population list
			this->updateListWidget();
		}
		else
		{
			// display warning
			QMessageBox::warning(this->textBrowser_individualPreview, "Warning!", "Cannot read file \"" + constraintsFile + "\"");
		}
	}
}

// update list widget of individuals
void MicroGPMainWindow::updateListWidget()
{
	// debug
	LOG_DEBUG << "Updating list widget..." << ends;

	// erase all individuals in the list widget
	if( this->listWidget_Population->count() > 0 ) this->listWidget_Population->clear();
	
	LOG_DEBUG << "Rewriting list..." << ends;
	// update list
	for(unsigned int i = 0; i < this->individuals.size(); i++)
	{
		QString id = QString::fromStdString( this->individuals[i]->getId() );
		if( this->unsavedMap.find( id.toStdString() ) != this->unsavedMap.end() ) id += QString( TEXT_UNSAVED );
		QString name = TEXT_INDIVIDUAL + id;
		
		this->listWidget_Population->addItem( new QListWidgetItem( name ) );
	}
	
	LOG_DEBUG << "List widget updated!" << ends;
}

// save the algorithm/population to file
void MicroGPMainWindow::savePopulation()
{
	QString statusFile = QFileDialog::getSaveFileName(	this->textBrowser_individualPreview,
								tr("Save XML Status File"), "",
                                                                tr("XML (*.xml);;All Files (*)"));
	
	if( !statusFile.isEmpty() )
	{
		/*
		// remove all individuals from population: TODO try to understand why I did that
		while( this->population->getIndividualCount() > 0 )
			this->population->removeIndividual( this->population->getIndividualCount() - 1 );
		*/

		// add all individuals in the vector to the population
		for(unsigned int i = 0; i < this->individuals.size(); i++)
		{
			unique_ptr<ugp3::core::Individual> pointerToIndividual( this->individuals[i] );
			this->population->addIndividual( std::move(pointerToIndividual) );
		}
		
		// set status file name and other details
		// conditional compiling because Windows... -> it turns out, it is useless because Qt kinda "normalizes" the file paths
		// into the Linux standard form with "/"
/*#if  defined(_WIN32) || defined (_WIN64)
		LOG_DEBUG << "No doubt, I am under a Windows system." << ends;
		int size = statusFile.length() - statusFile.lastIndexOf("\\") - 1;
#else
		LOG_DEBUG << "I am not under a Windows system." << ends;*/
		int size = statusFile.length() - statusFile.lastIndexOf("/") - 1;

		QString relativeStatusFile = statusFile.right(size); // take the last 'size' characters
		LOG_DEBUG << "The relative file name is \"" << relativeStatusFile.toStdString() << "\"." << ends;
		
		//this->evolutionaryAlgorithm->setOutputPathName( statusFile.toStdString() );
		this->evolutionaryAlgorithm->setOutputPathName( relativeStatusFile.toStdString() );
		this->evolutionaryAlgorithm->setStatisticsPathName( "statistics.csv" );

		// save to specified file
		this->evolutionaryAlgorithm->save( statusFile.toStdString() );

		// all individuals are saved
		this->unsavedMap.clear();
		this->updateListWidget();
	}
}

// request for the creation of a new individual
void MicroGPMainWindow::newIndividual()
{
	// if the population is empty, warning and return
	if( this->population == nullptr )
	{
		QMessageBox::warning( 	this->menuQuestionMark, "No constraints file specified!",
					"In order to build a new individual, you have to either load a constraints file "
					"or load a population, from menu \"File\".");
		return;
	}

	// block signals from list widget
	bool oldStateList = this->listWidget_Population->blockSignals(true);

	// create new individual instance
	LOG_DEBUG << "Building new individual..." << ends;
	vector<string> emptyParentList;
	unique_ptr<ugp3::core::Individual> individual( 
		new ugp3::core::EnhancedIndividual( 	this->population->getGeneration(), 
							ugp3::core::GeneticOperator::NO_OPERATOR, 
							emptyParentList, 
							*this->population ) );

	// bind individual's cgraphcontainer to constraints
	LOG_DEBUG << "Setting individual constraints..." << ends;
	unique_ptr<ugp3::ctgraph::CGraphContainer> graphContainer( new ugp3::ctgraph::CGraphContainer() );
	graphContainer->setConstrain(*this->constraints);
	individual->setGraphContainer( std::move(graphContainer) );
	
	// set it as the current individual
	this->currentIndividual = (ugp3::core::EnhancedIndividual*) individual.get();
	// and ask for every detail
	this->createIndividual();
	
	// individual has not been saved
	this->unsavedMap[ individual->getId() ] = false;
	// insert individual in the individual array
	this->individuals.push_back( (ugp3::core::EnhancedIndividual*) individual.release() );
	
	// update the list widget, and set the last individual as selected
	this->updateListWidget();
	this->listWidget_Population->setCurrentRow( this->listWidget_Population->count() - 1 );
	
	// unblock signals
	this->listWidget_Population->blockSignals(oldStateList);
}

// initialize all tables (now they do not exist, but the same code can be re-used to populate the menus
/*
void MicroGPMainWindow::populateAllTables()
{
	// if the unique_ptr is not valid, exit
	if( this->constraints.get() == nullptr ) return;
	
	// let's start with the section table!
	for(unsigned int s = 0; s < this->constraints->getSectionCount(); s++)
	{
		QString name = QString::fromStdString( this->constraints->getSection(s).getId() );
		this->tableWidget_Sections->insertRow(s);
		this->tableWidget_Sections->setItem( s, TABLE_COLUMN_NAME, new QTableWidgetItem( name ) );
	}
	
	// set the first line as selected
	this->tableWidget_Sections->setCurrentCell(0, TABLE_COLUMN_NAME);
	
	// populate the subsection table, following the currently selected section
	this->populateSubSectionTable(0, TABLE_COLUMN_NAME);
	
	return;
}

// initialize subsection table 
void MicroGPMainWindow::populateSubSectionTable(int row, int column)
{
	// get the corrisponding section
	ugp3::constraints::Section& section = this->constraints->getSection(row);

	// change label of subsection table
	QString name = QString::fromStdString( section.getId() );
	this->label_SubSections->setText("Subsections of section \"" + name + "\"");
	
	// now, for each subsection in the section, fill the table!
	for(unsigned int s = 0; s < section.getSubSectionCount(); s++)
	{
		QString name = QString::fromStdString( section.getSubSection(s).getId() );
		this->tableWidget_SubSections->insertRow(s);
		this->tableWidget_SubSections->setItem( s, TABLE_COLUMN_NAME, new QTableWidgetItem( name ) );
	}
	
	// set the first line as selected
	this->tableWidget_SubSections->setCurrentCell(0, TABLE_COLUMN_NAME);

	// populate the macro table, following the currently selected subsection
	this->populateMacroTable(0, TABLE_COLUMN_NAME);

	return;
}

// initialize macro table
void MicroGPMainWindow::populateMacroTable(int row, int column)
{
	// get the corresponding subsection
	ugp3::constraints::SubSection& subsection = 
		this->constraints->getSection( this->tableWidget_Sections->currentRow() )
		.getSubSection( this->tableWidget_SubSections->currentRow() );
	
	// change label of macros table
	QString name = QString::fromStdString( subsection.getId() );
	this->label_Macros->setText("Macros of section \"" + name + "\"");
	
	// for each macro in the section, fill the table!
	for(unsigned int m = 0; m < subsection.getMacroCount(); m++)
	{
		QString name = QString::fromStdString( subsection.getMacro(m).getId() );
		this->tableWidget_Macros->insertRow(m);
		this->tableWidget_Macros->setItem( m, TABLE_COLUMN_NAME, new QTableWidgetItem( name ) );
	}
	
	// set the first item as selected
	this->tableWidget_Macros->setCurrentCell(0, TABLE_COLUMN_NAME);

	return;
}
*/

// exit, after destroying everything
void MicroGPMainWindow::exitAndRelease()
{
	//this->constraints.reset();
	//this->currentIndividual.reset();
	//this->cgraphContainer.reset();
	//this->individual.reset();
	//this->mainWindow->close();

	return;
}

// change current individual
void MicroGPMainWindow::changeCurrentIndividual(int index)
{
	if( index < 0 || index >= (int)this->individuals.size() ) return;

	// this is triggered by a signal that the current individual is changed inside the list widget
	this->currentIndividual = this->individuals[ index ];
	
	// update everything
	this->updatePreview();
	this->updateTreeWidget();
}

// update individual preview
void MicroGPMainWindow::updatePreview()
{
	ostringstream ssIndividual;
	ugp3::ctgraph::IdentityRelabeller relabeller;
	this->currentIndividual->getGraphContainer().writeExternalRepresentation( ssIndividual, relabeller );
	this->textBrowser_individualPreview->setText( QString::fromStdString( ssIndividual.str() ) );
	LOG_DEBUG << "External individual representation is: \"" << ssIndividual.str() << "\"" << ends;
}

// update tree widget
void MicroGPMainWindow::updateTreeWidget()
{
	// TODO In parallel, create new tree widget; keep same expanded/folded stuff; finally, free the old one and replace.
	//	However, Widgets cannot be copied (!) So, an alternative idea could be re-writing the tree, analyzing each
	//	children node and rewriting it...

	// debug
	LOG_DEBUG << "Updating tree widget and individual map..." << ends;

	// first of all, remove everything from the tree widget, but keep track if the element was expanded or collapsed
	vector<bool> elementExpanded;
	while( this->treeWidget_individualStructure->topLevelItemCount() != 0 ) 
	{
		elementExpanded.push_back( this->treeWidget_individualStructure->topLevelItem(0)->isExpanded() );
		delete( this->treeWidget_individualStructure->topLevelItem(0) );
	}
	LOG_DEBUG << "Vector of expanded structures is " << elementExpanded.size() << ends;
	
	// and clear all maps that tie parts of the individual to the QTreeWidgetItems
	this->treeToNode.clear();
	this->treeToSubGraph.clear();
	this->treeToGraph.clear();

	// initializing the treeWidget; column count is NOT the maximum depth
	this->treeWidget_individualStructure->setColumnCount(1);
	
	// add globalPrologue to treeWidget
	QTreeWidgetItem* qtwiGlobalPrologue = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString( "globalPrologue" )));
	this->treeWidget_individualStructure->addTopLevelItem( qtwiGlobalPrologue );
	// add macro as child of globalPrologue in the treeWidget
	ostringstream ssGlobalPrologue;
	ugp3::ctgraph::IdentityRelabeller relabeller;
	this->currentIndividual->getGraphContainer().getPrologue().writeExternalRepresentation(ssGlobalPrologue, relabeller);
	qtwiGlobalPrologue->addChild(
		new QTreeWidgetItem( qtwiGlobalPrologue, QStringList(QString::fromStdString(ssGlobalPrologue.str()))));
	// associate node to to item inside the tree
	this->treeToNode[ qtwiGlobalPrologue->child( qtwiGlobalPrologue->childCount() - 1 ) ] = 
		this->currentIndividual->getGraphContainer().getNode( 
			this->currentIndividual->getGraphContainer().getPrologue().getId() );
	
	// for each graph inside the individual
	for(unsigned int g = 0; g < this->currentIndividual->getGraphContainer().getCGraphCount(); g++)
	{
		// get current graph
		ugp3::ctgraph::CGraph& graph = this->currentIndividual->getGraphContainer().getCGraph(g);

		// get the constraints that express the current section 
		const ugp3::constraints::Section& section = graph.getSection();
		
		// add section to treeWidget
		QTreeWidgetItem* qtwiSection = new QTreeWidgetItem(
			(QTreeWidget*)0, QStringList(QString::fromStdString( TEXT_SECTION + section.getId() ) ) );
		this->treeWidget_individualStructure->addTopLevelItem( qtwiSection );
		// add QTreeWidgetItem to corresponding graph
		this->treeToGraph[ qtwiSection ] = &graph;
		
		// add section prologue to treeWidget
		QTreeWidgetItem* qtwiSectionPrologue = new QTreeWidgetItem((QTreeWidget*)0, QStringList(
			QString::fromStdString( graph.getPrologue().getGenericMacro().getId() )));
		qtwiSection->addChild(qtwiSectionPrologue);
		// add actual macro as a child node
		ostringstream ssSectionPrologue;
		graph.getPrologue().writeExternalRepresentation( ssSectionPrologue, relabeller );
		QTreeWidgetItem* qtwiSectionPrologueMacro = 
			new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString::fromStdString( ssSectionPrologue.str()  )));
		qtwiSectionPrologue->addChild(qtwiSectionPrologueMacro);
		// associate node to to item inside the tree
		this->treeToNode[ qtwiSectionPrologue->child( qtwiSectionPrologue->childCount() - 1 ) ] = 
			this->currentIndividual->getGraphContainer().getNode( graph.getPrologue().getId() );

		// now, for each subgraph in the graph 
		for(unsigned int sg = 0; sg < graph.getSubGraphCount(); sg++)
		{
			// get current subgraph
			ugp3::ctgraph::CSubGraph& subgraph = graph.getSubGraph(sg);
			const ugp3::constraints::SubSection& subsection = subgraph.getSubSection();
			
			// add subsection to treeWidget
			QTreeWidgetItem* qtwiSubSection = new QTreeWidgetItem(
				(QTreeWidget*)0, QStringList(QString::fromStdString( TEXT_SUBSECTION + subsection.getId() ) ) );
			qtwiSection->addChild(qtwiSubSection);
			this->treeToSubGraph[ qtwiSubSection ] = &subgraph;

			// add subsection prologue to treeWidget
			QTreeWidgetItem* qtwiSubSectionPrologue = new QTreeWidgetItem((QTreeWidget*)0, QStringList(
				QString::fromStdString( subgraph.getPrologue().getGenericMacro().getId() )));
			qtwiSubSection->addChild(qtwiSubSectionPrologue);
			ostringstream ssSubSectionPrologue;
			subgraph.getPrologue().writeExternalRepresentation( ssSubSectionPrologue, relabeller );
			QTreeWidgetItem* qtwiSubSectionPrologueMacro =
				new QTreeWidgetItem((QTreeWidget*)0, QStringList(
					QString::fromStdString( ssSubSectionPrologue.str() )));
			qtwiSubSectionPrologue->addChild(qtwiSubSectionPrologueMacro);
			// associate node to to item inside the tree
			this->treeToNode[ qtwiSubSectionPrologue->child( qtwiSubSectionPrologue->childCount() - 1 ) ] = 
				this->currentIndividual->getGraphContainer().getNode( subgraph.getPrologue().getId() );
			
			// for each node inside the slice of the subgraph
			// count starts from 1 and goes up to dimension-1 to skip the prologue and the epilogue...maybe
			for(unsigned int n = 0; n < subgraph.getSlice().getSize(); n++)
			{
				ugp3::ctgraph::CNode& node = subgraph.getSlice().getNode(n);
				
				if( node != subgraph.getPrologue() && node != subgraph.getEpilogue() )
				{
					// add macro to tree widget
					ostringstream ssMacro;
					node.writeExternalRepresentation( ssMacro, relabeller );
					QTreeWidgetItem* qtwiMacro = 
						new QTreeWidgetItem((QTreeWidgetItem*)0, QStringList(
							QString::fromStdString(ssMacro.str())));
					//qtwiSubSection->insertChild(n+1, qtwiMacro);
					qtwiSubSection->addChild(qtwiMacro);
					// associate node to to item inside the tree
					this->treeToNode[ qtwiSubSection->child( qtwiSubSection->childCount() - 1 ) ] = 
						this->currentIndividual->getGraphContainer().getNode( node.getId() );
				}

			} // end for each node

			// add subsection prologue to treeWidget
			QTreeWidgetItem* qtwiSubSectionEpilogue = new QTreeWidgetItem((QTreeWidget*)0, QStringList(
				QString::fromStdString( subgraph.getEpilogue().getGenericMacro().getId() )));
			qtwiSubSection->addChild(qtwiSubSectionEpilogue);
			// ask for macro parameters, set them and put macro as child
			ostringstream ssSubSectionEpilogue;
			subgraph.getEpilogue().writeExternalRepresentation( ssSubSectionEpilogue, relabeller );
			QTreeWidgetItem* qtwiSubSectionEpilogueMacro =
				new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString::fromStdString( ssSubSectionEpilogue.str() )));
			//qtwiSubSectionEpilogue->insertChild(0, qtwiSubSectionEpilogueMacro);
			qtwiSubSectionEpilogue->addChild(qtwiSubSectionEpilogueMacro);
			// associate node to to item inside the tree
			this->treeToNode[ qtwiSubSectionEpilogue->child( qtwiSubSectionEpilogue->childCount() - 1 ) ] = 
				this->currentIndividual->getGraphContainer().getNode( subgraph.getEpilogue().getId() );
		
		} // end for each subgraph

		// add section epilogue to treeWidget
		QTreeWidgetItem* qtwiSectionEpilogue = new QTreeWidgetItem((QTreeWidget*)0, QStringList(
			QString::fromStdString( graph.getEpilogue().getGenericMacro().getId() )));
		qtwiSection->addChild(qtwiSectionEpilogue);
		// add actual macro as child node
		ostringstream ssSectionEpilogue;
		graph.getEpilogue().writeExternalRepresentation( ssSectionEpilogue, relabeller );
		QTreeWidgetItem* qtwiSectionEpilogueMacro =
			new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString::fromStdString( ssSectionEpilogue.str()  )));
		qtwiSectionEpilogue->addChild(qtwiSectionEpilogueMacro);	
		// associate node to to item inside the tree
		this->treeToNode[ qtwiSectionEpilogue->child( qtwiSectionEpilogue->childCount() - 1 ) ] = 
			this->currentIndividual->getGraphContainer().getNode( graph.getEpilogue().getId() );
	
	} // end for each graph
	
	// add global epilogue to the tree
	QTreeWidgetItem* qtwiGlobalEpilogue = new QTreeWidgetItem( (QTreeWidget*)0, QStringList(QString( "globalEpilogue" ) ) ); 
	this->treeWidget_individualStructure->addTopLevelItem( qtwiGlobalEpilogue );
	// add macro as child of globalPrologue in the treeWidget
	ostringstream ssGlobalEpilogue;
	this->currentIndividual->getGraphContainer().getEpilogue().writeExternalRepresentation(ssGlobalEpilogue, relabeller);
	qtwiGlobalEpilogue->addChild( 
		new QTreeWidgetItem( qtwiGlobalEpilogue, QStringList(QString::fromStdString( ssGlobalEpilogue.str() ))));
	// associate node to to item inside the tree
	this->treeToNode[ qtwiGlobalEpilogue->child( qtwiGlobalEpilogue->childCount() - 1 ) ] = 
		this->currentIndividual->getGraphContainer().getNode( 
			this->currentIndividual->getGraphContainer().getEpilogue().getId() );
	
	// TODO debug here the treeToNode
	LOG_DEBUG << "Current tree widget for individual " << this->currentIndividual->getId() << ends;
	for(map<QTreeWidgetItem*,ugp3::ctgraph::CNode*>::iterator it = this->treeToNode.begin(); it != this->treeToNode.end(); ++it)
	{
		if( it->first == nullptr || it->second == nullptr ) LOG_DEBUG << "AAAAARGH" << ends;
		LOG_DEBUG	<< "Widget: \"" << it->first->text(0).toStdString() << "\"" << ends;
		LOG_DEBUG 	<< "\t-> Node: \"" << it->second->getId() << "\"" << ends;
	}
	
	return;	
}

// initial dialog for individual creation
void MicroGPMainWindow::createIndividual()
{
	// erase all individuals in the list widget
	while( this->treeWidget_individualStructure->topLevelItemCount() != 0 ) 
	{
		LOG_DEBUG 	<< "Number of top level items in tree widget is " 
				<< this->treeWidget_individualStructure->topLevelItemCount() << ends;

		delete( this->treeWidget_individualStructure->topLevelItem(0) );
	}

	// initializing the treeWidget; column count is NOT the maximum depth
	this->treeWidget_individualStructure->setColumnCount(1);
	
	// add global prologue
	this->currentIndividual->getGraphContainer().setPrologue( unique_ptr<ugp3::ctgraph::CNode>(new ugp3::ctgraph::CNode(this->currentIndividual->getGraphContainer())) );
	this->currentIndividual->getGraphContainer().getPrologue().setConstrain(this->constraints->getPrologue());
	// add globalPrologue to treeWidget
	QTreeWidgetItem* qtwiGlobalPrologue = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString( "globalPrologue" )));
	this->treeWidget_individualStructure->addTopLevelItem( qtwiGlobalPrologue );
	// ask for macro parameters and set them
	this->askMacroParameters( &this->currentIndividual->getGraphContainer().getPrologue() );
	// add macro as child of globalPrologue in the treeWidget
	ostringstream ssGlobalPrologue;
	ugp3::ctgraph::IdentityRelabeller relabeller;
	this->currentIndividual->getGraphContainer().getPrologue().writeExternalRepresentation(ssGlobalPrologue, relabeller);
	qtwiGlobalPrologue->insertChild(0, 
		new QTreeWidgetItem( qtwiGlobalPrologue, QStringList(QString::fromStdString(ssGlobalPrologue.str()))));

	// for each section
	for(unsigned int s = 0; s < this->constraints->getSectionCount(); s++)
	{
		// first of all, get the section
		ugp3::constraints::Section& section = this->constraints->getSection(s);

		unique_ptr<ugp3::ctgraph::CGraph> newGraph ( new ugp3::ctgraph::CGraph(this->currentIndividual->getGraphContainer()) );
		newGraph->setConstrain(section);
		
		LOG_DEBUG << "Setting prologue for section \"" << section.getId() << "\"..." << ends;
		newGraph->setPrologue( unique_ptr<ugp3::ctgraph::CNode>( new ugp3::ctgraph::CNode(*newGraph) ) );
		newGraph->getPrologue().setConstrain(section.getPrologue());
		
		const ugp3::constraints::GenericMacro& macro = newGraph->getPrologue().getGenericMacro();
		LOG_DEBUG << "Id for prologue's macro is \"" << macro.getId() << "\"" << ends;
		LOG_DEBUG << "Number of parameters for prologue's macro is " << macro.getParameterCount() << ends;

		LOG_DEBUG << "Setting epilogue for section \"" << section.getId() << "\"..." << ends;
		newGraph->setEpilogue( unique_ptr<ugp3::ctgraph::CNode>( new ugp3::ctgraph::CNode(*newGraph) ) );
		newGraph->getEpilogue().setConstrain(section.getEpilogue());
		
		// add section to treeWidget
		QTreeWidgetItem* qtwiSection = new QTreeWidgetItem(
			(QTreeWidget*)0, QStringList(QString::fromStdString( TEXT_SECTION + section.getId() ) ) );

		this->treeWidget_individualStructure->addTopLevelItem( qtwiSection );
		
		// add section prologue to treeWidget
		QTreeWidgetItem* qtwiSectionPrologue = new QTreeWidgetItem((QTreeWidget*)0, QStringList(
			QString::fromStdString( newGraph->getPrologue().getGenericMacro().getId() )));
		qtwiSection->insertChild(0, qtwiSectionPrologue);
		this->askMacroParameters( &newGraph->getPrologue() );
		// add actual macro as a child node
		ostringstream ssSectionPrologue;
		newGraph->getPrologue().writeExternalRepresentation( ssSectionPrologue, relabeller );
		QTreeWidgetItem* qtwiSectionPrologueMacro = 
			new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString::fromStdString( ssSectionPrologue.str()  )));
		qtwiSectionPrologue->insertChild(0, qtwiSectionPrologueMacro);

		// now, for each subsection in the section
		for(unsigned int ss = 0; ss < section.getSubSectionCount(); ss++)
		{
			// create subgraph
			ugp3::constraints::SubSection& subsection = section.getSubSection(ss);
			LOG_DEBUG << "Creating subgraph for subsection \"" << subsection.getId() << "\"..." << ends;
			unique_ptr<ugp3::ctgraph::CSubGraph> newSubGraph ( new ugp3::ctgraph::CSubGraph(*newGraph) );
			newSubGraph->setConstrain(subsection);
			
			// add subsection to treeWidget
			QTreeWidgetItem* qtwiSubSection = new QTreeWidgetItem(
				(QTreeWidget*)0, QStringList(QString::fromStdString( TEXT_SUBSECTION + subsection.getId() ) ) );
			qtwiSection->insertChild(ss+1, qtwiSubSection);

			// create subsection prologue
			LOG_DEBUG << "Creating subsection prologue..." << ends;
			unique_ptr<ugp3::ctgraph::CNode> nodePrologue = 
				unique_ptr<ugp3::ctgraph::CNode>( new ugp3::ctgraph::CNode(*newSubGraph) );
			nodePrologue->setConstrain(subsection.getPrologue());
			newSubGraph->setPrologue( std::move(nodePrologue) );
			// add subsection prologue to treeWidget
			QTreeWidgetItem* qtwiSubSectionPrologue = new QTreeWidgetItem((QTreeWidget*)0, QStringList(
				QString::fromStdString( newSubGraph->getPrologue().getGenericMacro().getId() )));
			qtwiSubSection->insertChild(0, qtwiSubSectionPrologue);
			// ask for macro parameters, set them and put macro as child
			this->askMacroParameters( &newSubGraph->getPrologue() );
			ostringstream ssSubSectionPrologue;
			newSubGraph->getPrologue().writeExternalRepresentation( ssSubSectionPrologue, relabeller );
			QTreeWidgetItem* qtwiSubSectionPrologueMacro =
				new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString::fromStdString( ssSubSectionPrologue.str() )));
			qtwiSubSectionPrologue->insertChild(0, qtwiSubSectionPrologueMacro);
			
			// check the minimum number of macros in the constraints, and open the dialog
			// to pick each macro from the ones available and add it to the subsection
			int maximumMacros = subsection.getMaximumMacroInstances();
			int minimumMacros = subsection.getMinimumMacroInstances();
			// dialogue to select the number of macros
			QDialog dialogNumberOfMacros;
			dialogNumberOfMacros.setWindowTitle("Select number of macros");
			QGridLayout* layoutNumberOfMacros = new QGridLayout();
			//label
			QLabel* labelNumberOfMacros = new QLabel( QString("The minimum number of macros in subsection \"%1\" is %2 (the maximum is %3).\n Enter the desired number of macros, then choose them one by one.").arg(subsection.getId().c_str()).arg(minimumMacros).arg(maximumMacros) );
			layoutNumberOfMacros->addWidget( labelNumberOfMacros, 0, 0, 1, -1 );
			// spin box
			QSpinBox* spinboxNumberOfMacros = new QSpinBox();
			spinboxNumberOfMacros->setMinimum( minimumMacros );
			spinboxNumberOfMacros->setMaximum( maximumMacros );
			layoutNumberOfMacros->addWidget( spinboxNumberOfMacros, 1, 0 );
			// "next" push button
			QPushButton* pushbuttonNumberOfMacros = new QPushButton("Next");
			layoutNumberOfMacros->addWidget( pushbuttonNumberOfMacros, 2, 1 );
			// connect signals
			connect(pushbuttonNumberOfMacros, SIGNAL(clicked()), &dialogNumberOfMacros, SLOT(close()));
			// bind layout to dialog
			dialogNumberOfMacros.setLayout( layoutNumberOfMacros );
			// exec dialog
			dialogNumberOfMacros.exec();
			
			// after the dialog is executed, get the number of macros
			int numberOfMacros = spinboxNumberOfMacros->cleanText().toInt();
			
			for(int m = 0; m < numberOfMacros; m++)
			{
				// create and open dialog to choose the macro
				QDialog dialog;
				// add title to window
				dialog.setWindowTitle("Select a macro");
				// create grid layout for window
				QGridLayout* layout = new QGridLayout();
				// add elements to grid layout
				// label
				QLabel* label = new QLabel(QString("Choose macro #%1 for subsection \"%2\"")
					.arg(m).arg( subsection.getId().c_str() ));
				layout->addWidget( label, 0, 0, 1, -1 );
				// combo box to select macro
				QComboBox* comboBox = new QComboBox();
				for(unsigned int mt = 0; mt < subsection.getMacroCount(); mt++)
				{
					comboBox->addItem( QString::fromStdString( subsection.getMacro(mt).getId() ));
				}
				layout->addWidget(comboBox, 1, 0);
				// "next" button
				QPushButton* pushButton = new QPushButton("Next");
				layout->addWidget( pushButton, 2, 1 );
				// connect button to slot that closes the window
				connect(pushButton, SIGNAL(clicked()), &dialog, SLOT(close()));
				// bind layout to window
				dialog.setLayout(layout);
				// exec dialog
				dialog.exec();
				
				// create a node with the chosen macro
				unique_ptr<ugp3::ctgraph::CNode> node = 
					unique_ptr<ugp3::ctgraph::CNode>( new ugp3::ctgraph::CNode(*newSubGraph) );
				node->setConstrain( subsection.getMacro( comboBox->currentIndex() ) );
				
				// ask for macro parameters
				this->askMacroParameters(node.get());
				
				// add node to individual
				newSubGraph->addNode( *node );
				newSubGraph->getSlice().append( *node );
				
				// add macro to tree widget 
				//TODO 	this is wrong, because macros with labels cannot be written immediately;
				//	you need the whole individual, or the program will crash
				/*
				ostringstream ssMacro;
				node->writeExternalRepresentation( ssMacro );
				QTreeWidgetItem* qtwiMacro = 
					new QTreeWidgetItem((QTreeWidgetItem*)0, QStringList(QString::fromStdString(ssMacro.str())));
				qtwiSubSection->insertChild(m+1, qtwiMacro);
				*/

				// maybe I also need to release the node to avoid problems...yes, I do
				node.release();
			
			} // end for each macro

			// create subsection epilogue
			LOG_DEBUG << "Creating subsection epilogue..." << ends;
			unique_ptr<ugp3::ctgraph::CNode> nodeEpilogue = 
				unique_ptr<ugp3::ctgraph::CNode>( new ugp3::ctgraph::CNode(*newSubGraph) );
			nodeEpilogue->setConstrain(subsection.getEpilogue());
			newSubGraph->setEpilogue( std::move(nodeEpilogue) );
			// add subsection prologue to treeWidget
			QTreeWidgetItem* qtwiSubSectionEpilogue = new QTreeWidgetItem((QTreeWidget*)0, QStringList(
				QString::fromStdString( newSubGraph->getEpilogue().getGenericMacro().getId() )));
			qtwiSubSection->insertChild(minimumMacros+1, qtwiSubSectionEpilogue);
			// ask for macro parameters, set them and put macro as child
			this->askMacroParameters( &newSubGraph->getEpilogue() );
			ostringstream ssSubSectionEpilogue;
			newSubGraph->getEpilogue().writeExternalRepresentation( ssSubSectionEpilogue, relabeller );
			QTreeWidgetItem* qtwiSubSectionEpilogueMacro =
				new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString::fromStdString( ssSubSectionEpilogue.str() )));
			qtwiSubSectionEpilogue->insertChild(0, qtwiSubSectionEpilogueMacro);

			// add subsection to graph
			LOG_DEBUG << "Adding subgraph to graph..." << ends;
			if( newGraph->attachSubGraph( *newSubGraph ) == true )
				LOG_DEBUG << "Apparently, the subgraph was successfully added!" << ends;
			else
				LOG_DEBUG << "Ouch! Something went wrong when attaching subgraph to graph!" << ends;
			
			// I don't understand why, but this is necessary. Maybe the unique_ptr tries to destroy the object
			// contained by it, making everything crash...but it should have been copied...?
			newSubGraph.release();
		
		} // end for each subsection
		
		// add section epilogue to treeWidget
		LOG_DEBUG << "Adding section epilogue to tree widget..." << ends;
		QTreeWidgetItem* qtwiSectionEpilogue = new QTreeWidgetItem((QTreeWidget*)0, QStringList(
			QString::fromStdString( newGraph->getEpilogue().getGenericMacro().getId() )));
		qtwiSection->insertChild(section.getSubSectionCount() + 1, qtwiSectionEpilogue);
		this->askMacroParameters( &newGraph->getEpilogue() );
		// add actual macro as child node
		ostringstream ssSectionEpilogue;
		newGraph->getEpilogue().writeExternalRepresentation( ssSectionEpilogue, relabeller );
		QTreeWidgetItem* qtwiSectionEpilogueMacro =
			new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString::fromStdString( ssSectionEpilogue.str()  )));
		qtwiSectionEpilogue->insertChild(0, qtwiSectionEpilogueMacro);	
		
		LOG_DEBUG << "Adding graph to cgraphContainer..." << ends;
		this->currentIndividual->getGraphContainer().addCGraph(newGraph);
		newGraph.release();
	}

	// add global epilogue
	this->currentIndividual->getGraphContainer().setEpilogue( 
		unique_ptr<ugp3::ctgraph::CNode>(new ugp3::ctgraph::CNode(this->currentIndividual->getGraphContainer())) );
	this->currentIndividual->getGraphContainer().getEpilogue().setConstrain(this->constraints->getEpilogue());
	// add global epilogue to the tree
	QTreeWidgetItem* qtwiGlobalEpilogue = new QTreeWidgetItem( (QTreeWidget*)0, QStringList(QString( "globalEpilogue" ) ) ); 
	this->treeWidget_individualStructure->addTopLevelItem( qtwiGlobalEpilogue );
	// ask for macro parameters and set them
	this->askMacroParameters( &this->currentIndividual->getGraphContainer().getEpilogue() );
	// add macro as child of globalPrologue in the treeWidget
	ostringstream ssGlobalEpilogue;
	this->currentIndividual->getGraphContainer().getEpilogue().writeExternalRepresentation(ssGlobalEpilogue, relabeller);
	qtwiGlobalEpilogue->insertChild(0, 
		new QTreeWidgetItem( qtwiGlobalEpilogue, QStringList(QString::fromStdString( ssGlobalEpilogue.str() ))));
	
	// attach floating edges
	if( this->currentIndividual->getGraphContainer().attachFloatingEdges() == false )
	{
		QMessageBox::critical( 	this->treeWidget_individualStructure, "Error: non-valid individual",
					"The individual is not valid and will not be saved. There is probably something wrong with innerLabel parameters. When you retry, double-check that all innerLabels' offset are correct.");
		
		// delete the individual
		delete(this->currentIndividual);
		this->currentIndividual = nullptr;

		return;
	}
	
	// write individual to preview
	this->updatePreview();

	return;	
}

// add a section
void MicroGPMainWindow::addSection()
{
        //unique_ptr<ugp3::ctgraph::CGraph> newGraph ( new ugp3::ctgraph::CGraph( *this->cgraphContainer.get() ) );
	//newGraph->setConstrain(constraints.getSection(i));
	//newGraph->buildRandom();
	
	return;
}

// ask parameters of macro
void MicroGPMainWindow::askMacroParameters( ugp3::ctgraph::CNode* node )
{
	// the first thing is to verify whether the macro HAS some parameters;
	// if not, there is no point in opening a dialog window
	LOG_DEBUG << "Asking parameters for macro \"" << node->getGenericMacro().getId() << "\"..." << ends;
	if( node->getGenericMacro().getParameterCount() <= 0 ) 
	{
		LOG_DEBUG << "No parameters in the macro, dialogue will not be opened." << ends;
		return;
	}

	// create dialog
	QDialog dialog;
	// add title to window
	dialog.setWindowTitle("Select macro parameters");

	// create grid layout for window
	QGridLayout* layout = new QGridLayout();

	// add label	
	QLabel* label = new QLabel( 	QString("Select parameters for macro " + 
					QString::fromStdString( node->getGenericMacro().getId() )) );
	layout->addWidget( label, 0, 0, 1, -1 );

	// add ALL THE WIDGETS to the layout
	for(unsigned int p = 0; p < node->getGenericMacro().getParameterCount(); p++)
	{
		// first, label with the parameter
		const ugp3::constraints::Parameter& parameter = node->getGenericMacro().getParameter(p);
		QLabel* parameterName = new QLabel( QString::fromStdString( parameter.getName() ) );
		layout->addWidget( parameterName, p+1, 0 );
		
		// now, depending on the type of parameter, we are going to add a different widget
		// TODO
		if( dynamic_cast<const ugp3::constraints::ConstantParameter*>( &parameter ) != nullptr )
		{
			const ugp3::constraints::ConstantParameter* constantParameter = 
					dynamic_cast<const ugp3::constraints::ConstantParameter*>( &parameter );
			
			// if it's a constant parameter with a finite number of alternative choices, create a comboBox and fill it
			QComboBox* comboBox = new QComboBox();
			const vector<string> values = constantParameter->getValues();
			for(unsigned int i = 0; i < values.size(); i++)
			{
				comboBox->addItem( QString::fromStdString( values[i] ) );
			}
			layout->addWidget( comboBox, p+1, 1 );
		}
		else if(dynamic_cast<const ugp3::constraints::FloatParameter*>( &parameter ) != nullptr )
		{
			const ugp3::constraints::FloatParameter* floatParameter =
				dynamic_cast<const ugp3::constraints::FloatParameter*>( &parameter );
			
			// if it's a floating point parameter, create a doubleSpinBox, set minimum and maximum
			QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox();
			doubleSpinBox->setMinimum( floatParameter->getMinimum() );
			doubleSpinBox->setMaximum( floatParameter->getMaximum() );
			doubleSpinBox->setSingleStep( 0.01 );
			layout->addWidget( doubleSpinBox, p+1, 1 );
		}
		else if(dynamic_cast<const ugp3::constraints::IntegerParameter*>( &parameter ) != nullptr )
		{
			const ugp3::constraints::IntegerParameter* integerParameter =
				dynamic_cast<const ugp3::constraints::IntegerParameter*>( &parameter );
			
			// if it's a floating point parameter, create a doubleSpinBox, set minimum and maximum
			QSpinBox* spinBox = new QSpinBox();
			spinBox->setMinimum( integerParameter->getMinimum() );
			spinBox->setMaximum( integerParameter->getMaximum() );
			layout->addWidget( spinBox, p+1, 1 );
		}
		else if(dynamic_cast<const ugp3::constraints::InnerLabelParameter*>( &parameter ) != nullptr )
		{
			const ugp3::constraints::InnerLabelParameter* innerLabelParameter =
				dynamic_cast<const ugp3::constraints::InnerLabelParameter*>( &parameter );
			
			// - go forward/backwards by how many spaces? -> need ref to individual -> can get that from CNode?
			// ok, to determine the minimum and maximum, it's necessary to find the current position of the node
			// in this graph, then the maximum number of nodes in the subsection
			// to obtain the maximum number of nodes, first of all get the container of the node
			ugp3::ctgraph::NodeContainer* nodeContainer = node->getParentContainer();
			
			// then, try to cast it as a CSubGraph
			if( dynamic_cast<ugp3::ctgraph::CSubGraph*>( nodeContainer ) == nullptr )
			{
				// warning message
				QMessageBox::warning(	this->treeWidget_individualStructure, "Constraints error!",
							"You cannot place an innerLabel/innerForwardLabel/innerBackwardLabel"
							" parameter outside a SubSection. If you want to create a reference"
							" to an external Section/SubSection, try outerLabel instead.");
				return;
			}
			ugp3::ctgraph::CSubGraph* subgraph = dynamic_cast<ugp3::ctgraph::CSubGraph*>( nodeContainer );
			const ugp3::constraints::SubSection& subsection = subgraph->getSubSection();
			
			unsigned int maximumNodes = subsection.getMaximumMacroInstances();

			QSpinBox* spinBoxReference = new QSpinBox();
			//ugp3::ctgraph::CNode* pNode = node;

			// find minimum
			unsigned int numberOfCurrentNodes = subgraph->getSize();
			int minimum = 0 - numberOfCurrentNodes;
			if( innerLabelParameter->getPrologueIsValid() == false) minimum++;
			spinBoxReference->setMinimum( minimum );

			// find maximum
			spinBoxReference->setMaximum( maximumNodes - numberOfCurrentNodes );
			// add widget
			layout->addWidget( spinBoxReference, p+1, 1 );
		}
		else if(dynamic_cast<const ugp3::constraints::BitArrayParameter*>( &parameter ) != nullptr )
		{
			const ugp3::constraints::BitArrayParameter* bitArrayParameter =
				dynamic_cast<const ugp3::constraints::BitArrayParameter*>( &parameter );
			
			// for each bit in the bit array
			string pattern = bitArrayParameter->getPattern();
			for(unsigned int c = 0; c < pattern.length(); c++)
			{
				if( pattern[c] == '-' )
				{
					// create comboBox with 0/1
					QComboBox* comboBox01 = new QComboBox();
					comboBox01->addItem("0");
					comboBox01->addItem("1");
					layout->addWidget( comboBox01, p+1, c+1 );
				}
				else
				{
					// fixed label
					QLabel* fixedTerm = new QLabel( QString( pattern[c] ) );
					layout->addWidget( fixedTerm, p+1, c+1 );
				}
			}
		}	
	}

	// then, add button "Next"
	QPushButton* pushButton = new QPushButton("Next");
	layout->addWidget( pushButton, node->getGenericMacro().getParameterCount()+1, layout->columnCount() );
	// connect button to slot that closes the window
	connect(pushButton, SIGNAL(clicked()), &dialog, SLOT(close()));
	
	// it would be cool to add a textBrowser to display the current macro as the process goes on...
	// but it would require a lot of "connect" statements I am not sure I can put here...
	// TODO
	
	// bind layout to window
	dialog.setLayout(layout);
	// exec dialog
	dialog.exec();
	
	// collect all values and set them in the node (it should be possible by getting the widgets from the layout and using getText)
	// but I have to iterate again over the parameters and read the values from corresponding widgets in the layout
	// TODO
	// count starts from p = 1 because the positions are all shifted due to the label in position 0
	for(unsigned int p = 1; p < node->getGenericMacro().getParameterCount()+1; p++)
	{
		const ugp3::constraints::Parameter& parameter = node->getGenericMacro().getParameter(p-1);

		if( dynamic_cast<const ugp3::constraints::ConstantParameter*>( &parameter ) != nullptr )
		{
			// get comboBox's current value
			QString value = ( (QComboBox*) layout->itemAtPosition(p, 1)->widget() )->currentText();
			LOG_DEBUG << "Value for parameter \"" << parameter.getName() << "\" is \"" << value.toStdString() << "\"" << ends;
			// set parameter value in the node
			node->removeTag(ugp3::ctgraph::CNode::Escape + parameter.getName()); // eventually remove parameter
			node->addTag(ugp3::ctgraph::CNode::Escape + parameter.getName(), value.toStdString() );
		}
		else if( dynamic_cast<const ugp3::constraints::FloatParameter*>( &parameter ) != nullptr )
		{
			// get doubleSpinBox's current value
			QString value = ( (QDoubleSpinBox*) layout->itemAtPosition(p, 1)->widget() )->cleanText();
			node->removeTag(ugp3::ctgraph::CNode::Escape + parameter.getName()); // eventually remove parameter
			node->addTag(ugp3::ctgraph::CNode::Escape + parameter.getName(), value.toStdString() );
		}
		else if( dynamic_cast<const ugp3::constraints::IntegerParameter*>( &parameter ) != nullptr )
		{
			// get spinBox's current value
			QString value = ( (QSpinBox*) layout->itemAtPosition(p, 1)->widget() )->cleanText();
			node->removeTag(ugp3::ctgraph::CNode::Escape + parameter.getName()); // eventually remove parameter
			node->addTag(ugp3::ctgraph::CNode::Escape + parameter.getName(), value.toStdString() );
		}
		else if( dynamic_cast<const ugp3::constraints::BitArrayParameter*>( &parameter ) != nullptr )
		{
			// get value from all the comboBoxes and process it
			const ugp3::constraints::BitArrayParameter* bitArrayParameter =
				dynamic_cast<const ugp3::constraints::BitArrayParameter*>( &parameter );
			// pattern
			string pattern = bitArrayParameter->getPattern();
			
			// value
			QString value;
			for(unsigned int c = 0; c < pattern.length(); c++)
			{
				value += ( (QComboBox*) layout->itemAtPosition(p, c+1)->widget() )->currentText();
			}
			LOG_DEBUG 	<< "Value for parameter \"" << bitArrayParameter->getName() 
					<< "\" is " << value.toStdString() << ends;
			node->removeTag(ugp3::ctgraph::CNode::Escape + parameter.getName()); // eventually remove parameter
			node->addTag(ugp3::ctgraph::CNode::Escape + parameter.getName(), value.toStdString() );
		}	
		else if(dynamic_cast<const ugp3::constraints::InnerLabelParameter*>( &parameter ) != nullptr )
		{
			// an InnerLabel parameter is transformed into several tags (I think)
			// or actually, we have to add an edge?
			const ugp3::constraints::InnerLabelParameter* innerLabelParameter = 
					dynamic_cast<const ugp3::constraints::InnerLabelParameter*>( &parameter );

			// get spinBox's current value
			QString offset = ( (QSpinBox*) layout->itemAtPosition(p, 1)->widget() )->cleanText();
			
			// now, set the value
			// node->addTag( ugp3::ctgraph::CNode::Escape + tgraph::Edge::offsetTagName, offset.toStdString() );
    			// int offset = subGraph->getOffset(*this, *targetNode);
    			tgraph::Edge* newEdge = node->addFloatingEdge( *innerLabelParameter );
			newEdge->addTag( tgraph::Edge::offsetTagName, offset.toStdString() );
		}
	}
	return;
}

// context menu for the tree widget (it exploits information inside the MicroGPMainWindow)
void MicroGPMainWindow::rightClickMenuTreeWidget(QPoint pos)
{
	LOG_DEBUG << "Right click in the tree widget!" << ends;
	
	// move the menu to the point where the right-click was
	this->treeWidgetContextMenu->move( 	this->treeWidget_individualStructure->mapToGlobal(pos).x(), 
						this->treeWidget_individualStructure->mapToGlobal(pos).y() );
	
	// handlers for the menus
	QList<QMenu*> menus = this->treeWidgetContextMenu->findChildren<QMenu*>();
	QMenu* addMenu = menus[0];
	QMenu* removeMenu = menus[1];

	// disable all the options
	this->treeWidgetContextMenu->actions().at( MENU_CHANGE_PARAMETERS )->setDisabled(true);
	addMenu->actions().at( MENU_ADD_NODE )->setDisabled(true);
	addMenu->actions().at( MENU_ADD_SUBGRAPH )->setDisabled(true);
	addMenu->actions().at( MENU_ADD_GRAPH )->setDisabled(true);
	removeMenu->actions().at( MENU_REMOVE_NODE )->setDisabled(true);
	removeMenu->actions().at( MENU_REMOVE_SUBGRAPH )->setDisabled(true);
	removeMenu->actions().at( MENU_REMOVE_GRAPH )->setDisabled(true);

	// depending on the currently selected item, activate and de-activate some of the options
	QTreeWidgetItem* currentItem = this->treeWidget_individualStructure->currentItem();

	if( this->treeToNode.find(currentItem) != this->treeToNode.end() )
	{
		// if it's a node, abilitate the possibility of changing parameters
		this->treeWidgetContextMenu->actions().at( MENU_CHANGE_PARAMETERS )->setEnabled(true);
		addMenu->actions().at( MENU_ADD_NODE )->setEnabled(true);
		removeMenu->actions().at( MENU_REMOVE_NODE )->setEnabled(true);
	}
	else if( this->treeToSubGraph.find(currentItem) != this->treeToSubGraph.end() )
	{
		// if it's a subgraph, enable relative options
		addMenu->actions().at( MENU_ADD_NODE )->setEnabled(true);
		removeMenu->actions().at( MENU_REMOVE_NODE )->setEnabled(true);
	}
	else if( this->treeToGraph.find(currentItem) != this->treeToGraph.end() )
	{
		// if it's a graph, enable all options relative to graphs
		addMenu->actions().at( MENU_ADD_SUBGRAPH )->setEnabled(true);
		removeMenu->actions().at( MENU_REMOVE_SUBGRAPH )->setEnabled(true);
	}
	else
	{
		// if it's not anything of the above, check if it's the root node
		if( this->treeWidget_individualStructure->itemAbove( currentItem ) == nullptr )
		{
			addMenu->actions().at( MENU_ADD_GRAPH )->setEnabled(true);
			removeMenu->actions().at( MENU_REMOVE_GRAPH )->setEnabled(true);
		}
	}

	// show it
	this->treeWidgetContextMenu->exec();
	
	return;
}

// slot that calls the function to change the tags of the currently selected node
void MicroGPMainWindow::changeMacroParameters()
{
	// TODO: 	actually, a COPY of the individual should be created before calling the modifications,
	// 		to be able to UNDO in case of mistakes

	// first, some controls
	QTreeWidgetItem* currentItem = this->treeWidget_individualStructure->currentItem();
	if( this->treeToNode.find(currentItem) == this->treeToNode.end() ) return; // TODO error message?
	
	// find the node currently selected
	ugp3::ctgraph::CNode* node = this->treeToNode[ currentItem ];
	
	// detach all edges (?)

	// call the method to ask the parameters
	this->askMacroParameters(node); 
	
	// attach floating edges
        if( this->currentIndividual->getGraphContainer().attachFloatingEdges() == false )
        {
                QMessageBox::critical(  this->treeWidget_individualStructure, "Error: non-valid individual",
                                        "The individual is not valid and will not be saved. There is probably something wrong with innerLabel parameters. When you retry, double-check that all innerLabels' offset are correct.");

                // TODO restore the previous individual

                return;
        }

	// update everything
	this->unsavedMap[ this->currentIndividual->getId() ] = false;
	this->updateTreeWidget();
	this->updatePreview();
	this->updateListWidget();
}

// add a node to the individual
void MicroGPMainWindow::addNode()
{
	// first, some controls
	QTreeWidgetItem* currentItem = this->treeWidget_individualStructure->currentItem();
	if( this->treeToNode.find(currentItem) == this->treeToNode.end() ) return; // TODO error message?
	
	// find the node currently selected
	ugp3::ctgraph::CNode* node = this->treeToNode[ currentItem ];
	
	// choose macro among the ones availables in the same subsection
	ugp3::ctgraph::NodeContainer* parentContainer = node->getParentContainer();
	if( dynamic_cast<ugp3::ctgraph::CSubGraph*>( parentContainer ) != nullptr )
	{
		ugp3::ctgraph::CSubGraph* subgraph = dynamic_cast<ugp3::ctgraph::CSubGraph*>( parentContainer );
		const ugp3::constraints::SubSection& subsection = subgraph->getSubSection();
		
		// check whether the number of nodes is already over the limit
		if( (int)subgraph->getSize() >= subsection.getMaximumMacroInstances() )
		{
			QMessageBox::warning(	this->treeWidget_individualStructure, tr("Warning!"),
						tr("Maximum number of nodes in the subgraph reached! Cannot add another one."));
			return;
		}
		
		// create and open dialog to choose the macro
		QDialog dialog;
		// add title to window
		dialog.setWindowTitle("Select a macro");
		// create grid layout for window
		QGridLayout* layout = new QGridLayout();
		// add elements to grid layout
		// label
		QLabel* label = new QLabel(QString("Choose macro from those available in subsection \"%1\"")
			.arg( subsection.getId().c_str() ));
		layout->addWidget( label, 0, 0, 1, -1 );
		// combo box to select macro
		QComboBox* comboBox = new QComboBox();
		for(unsigned int mt = 0; mt < subsection.getMacroCount(); mt++)
		{
			comboBox->addItem( QString::fromStdString( subsection.getMacro(mt).getId() ));
		}
		layout->addWidget(comboBox, 1, 0);
		// "next" button
		QPushButton* pushButton = new QPushButton("Next");
		layout->addWidget( pushButton, 2, 1 );
		// connect button to slot that closes the window
		connect(pushButton, SIGNAL(clicked()), &dialog, SLOT(close()));
		// bind layout to window
		dialog.setLayout(layout);
		// exec dialog
		dialog.exec();
		
		// create a node with the chosen macro
		unique_ptr<ugp3::ctgraph::CNode> newNode = 
			unique_ptr<ugp3::ctgraph::CNode>( new ugp3::ctgraph::CNode(*subgraph) );
		newNode->setConstrain( subsection.getMacro( comboBox->currentIndex() ) );
		
		// ask for macro parameters
		this->askMacroParameters(newNode.get());
		
		// insert the node inside the individual
		unsigned int insertPoint = ugp3::Convert::toUInt( node->getTag(ugp3::ctgraph::CNode::TAG_PLACE).getValue() ) + 1;
		unique_ptr<ugp3::ctgraph::Slice> slice (new ugp3::ctgraph::Slice(std::move(newNode)));
		subgraph->getSlice().spliceSlice(std::move(slice), insertPoint);
		
		this->currentIndividual->getGraphContainer().attachFloatingEdges();
	
		// update everything
		this->unsavedMap[ this->currentIndividual->getId() ] = false;
		this->updateTreeWidget();
		this->updatePreview();
		this->updateListWidget();
	}
	
	// end
}

// remove a node from the individual
void MicroGPMainWindow::removeNode()
{
	// first, some controls
	QTreeWidgetItem* currentItem = this->treeWidget_individualStructure->currentItem();
	if( this->treeToNode.find(currentItem) == this->treeToNode.end() ) return; // TODO error message?
	
	
	// find the node currently selected
	ugp3::ctgraph::CNode* node = this->treeToNode[ currentItem ];

	// check whether the container is a subgraph
	ugp3::ctgraph::NodeContainer* parentContainer = node->getParentContainer();
	if( dynamic_cast<ugp3::ctgraph::CSubGraph*>( parentContainer ) != nullptr )
	{
		ugp3::ctgraph::CSubGraph* subgraph = dynamic_cast<ugp3::ctgraph::CSubGraph*>( parentContainer );
		const ugp3::constraints::SubSection& subsection = subgraph->getSubSection();

		// the actual size of the subgraph also takes into account prologue and epilogue
		unsigned int actualSize = subgraph->getSize() - 2;

		// check whether the number of nodes is already over the limit
		if( (int)actualSize <= subsection.getMinimumMacroInstances() )
		{
			QMessageBox::warning(	this->treeWidget_individualStructure, tr("Warning!"),
						tr("Minimum number of nodes in the subgraph reached! Cannot remove one."));
			return;
		}
	
		// TODO error control: what if it's prologue/epilogue?
		unsigned int position = ugp3::Convert::toUInt( node->getTag(ugp3::ctgraph::CNode::TAG_PLACE).getValue() );
		subgraph->getSlice().cutSlice(position, position);

		this->currentIndividual->getGraphContainer().attachFloatingEdges();
	
		// update everything
		this->unsavedMap[ this->currentIndividual->getId() ] = false;
		this->updateTreeWidget();
		this->updatePreview();
		this->updateListWidget();
	}
}

// function that loads a population from a status file
void MicroGPMainWindow::loadPopulation()
{
	// display warning
	if( 	this->constraints != nullptr || 
		this->individuals.size() != 0 ||
		this->evolutionaryAlgorithm != nullptr ||
		this->population != nullptr)
	{
		if( QMessageBox::question(this->textBrowser_individualPreview, tr("Warning!"), 
		tr("When a new status file is loaded, all current individuals are discarded. If they are not saved, you will lose your precious work. Are you sure you want to continue?"), QMessageBox::Ok, QMessageBox::Cancel ) != QMessageBox::Ok) return;
	}

	// get the file name
	QString statusFile = 
		QFileDialog::getOpenFileName(	this->textBrowser_individualPreview, 
						tr("Open XML Status File"), "", 
						tr("XML (*.xml);;All Files (*)"));
	// if a file name has been specified
	if( !statusFile.isEmpty() )
	{
		// first, delete every pointer
		if( this->evolutionaryAlgorithm != nullptr ) delete( this->evolutionaryAlgorithm );
		if( this->constraints != nullptr ) delete( this->constraints );
		if( this->population != nullptr ) delete( this->population );
		if( this->individuals.size() > 0 ) this->individuals.clear();
		
		// set current individual to nullptr and clear unsaved map
		this->currentIndividual = nullptr;
		this->unsavedMap.clear();

		// create an evolutionary algorithm from file
		unique_ptr<ugp3::core::EvolutionaryAlgorithm> tempAlgorithm( new ugp3::core::EvolutionaryAlgorithm() );
 
		tempAlgorithm->fromFile(statusFile.toStdString(), false); 
		this->evolutionaryAlgorithm = tempAlgorithm.release();
		
		// create dialog to select the population
		QDialog dialog;
		// add title to window
		dialog.setWindowTitle("Select a population");
		// create grid layout for window
		QGridLayout* layout = new QGridLayout();
		// create combobox widget
		QComboBox* comboBox = new QComboBox();
		for(unsigned int p = 0; p < this->evolutionaryAlgorithm->getPopulationCount(); p++)
		{
			QString itemName = QString::fromStdString( this->evolutionaryAlgorithm->getPopulation(p).getName() );
			comboBox->addItem( QString("%1: ").arg(p) + itemName );
		}
		// create label
		QLabel* label = new QLabel( "Population" );
		// create button
		QPushButton* pushButton = new QPushButton( "Next" );
		// connect button to slot that closes the window
		connect(pushButton, SIGNAL(clicked()), &dialog, SLOT(close()));
		// setup layout
		layout->addWidget(label, 0, 0);
		layout->addWidget(comboBox, 0, 1);
		layout->addWidget(pushButton, 1, 1);
		// associate layout to dialog
		dialog.setLayout(layout);
		// execute dialog	
		dialog.exec();
		
		// from the combobox, get the index of the chosen population
		this->population = dynamic_cast<ugp3::core::EnhancedPopulation*>
			( &this->evolutionaryAlgorithm->getPopulation( comboBox->currentIndex() ) );
		// from the population, get the associated constraints
		this->constraints = const_cast<ugp3::constraints::Constraints*> (&this->population->getParameters().getConstraints() );
		
		// now, some checks, and then for each individual in the population, push it in the individual vector
		if( this->population == nullptr ) return; // TODO error message
		for(unsigned int i = 0; i < this->population->getIndividualCount(); i++)
		{
			// TODO maybe a more elegant solution would be to clone each individual? then, the IDs would be lost...
			ugp3::core::EnhancedIndividual* individual = const_cast<ugp3::core::EnhancedIndividual*>( 
				dynamic_cast<const ugp3::core::EnhancedIndividual*>( &this->population->getIndividual(i) ) );
			
			if( individual != nullptr ) this->individuals.push_back( individual );
		}
		
		// set current individual as the first one
		this->currentIndividual = this->individuals[0];
		
		// finally, update ALL the widgets!
		this->updateListWidget();
		this->updateTreeWidget();
		this->updatePreview();
		
	}
	
}

void MicroGPMainWindow::applyOperator(QAction* action)
{
	// if the population is empty, warning and return
	if( this->population == nullptr )
	{
		QMessageBox::warning( 	this->menuQuestionMark, "No constraints file specified!",
					"In order to apply an operator, you have to either load a constraints file "
					"and create a population, or load a population, from menu \"File\".");
		return;
	}
	

	LOG_DEBUG << "Apply selected operator: \"" << action->text().toStdString() << "\"" << ends;
	
	// get operator reference
	LOG_DEBUG << "Getting operator reference..." << ends;
	ugp3::core::Operator* standardOperator = ugp3::core::Operator::getOperator( action->text().toStdString() );
	
	ugp3::core::GeneticOperator* geneticOperator = dynamic_cast<ugp3::core::GeneticOperator*>( standardOperator );
	if( geneticOperator == nullptr )
	{
		LOG_WARNING << "Operator is not a genetic operator!" << ends;
		return;
	}

	// array of children and parents (to be used later)
	vector<ugp3::core::Individual*> children;
	vector<ugp3::core::Individual*> parents;

	// Ok, this is tricky; I need to create a Selection inherited class that allows the user to
	// select the individuals through an interface; and create a Parameters object around it

	// first create the parameters' instance
	//LOG_DEBUG << "Creating parameters' instance..." << ends;
	//ugp3::core::EnhancedPopulationParameters* parameters = new ugp3::core::EnhancedPopulationParameters();

	// then the selector
	LOG_DEBUG << "Creating the selector's instance..." << ends;
	ugp3::core::UserSelection* selection = new ugp3::core::UserSelection();
	
	// associate selector to this class
	selection->setParentWindow(this);
	
	LOG_DEBUG << "Choosing parents..." << ends;
	selection->selectIndividuals( *population, geneticOperator->getParentsCardinality(), 0);

	// associate selector to parameters
	//LOG_DEBUG << "Associating parameters to selector..." << ends;
	//parameters->setSelector(selection);
	// TODO currently all operators return an error if sigma is exactly zero; I don't think this makes much sense...
	//parameters->setSigma(0.1);

	// finally, generate the individuals
	// TODO change all operators' references to callSelector to individualSelector
	// TODO also, we need to selected the parents...
	LOG_DEBUG << "Generating individuals!" << ends;
	geneticOperator->generate(parents, children, *population);
	
	// and now, add back the generated individual(s) to the array of individuals here
	for(unsigned int i = 0; i < children.size(); i++)
	{
		ugp3::core::EnhancedIndividual* childIndividual = dynamic_cast<ugp3::core::EnhancedIndividual*>( children[i] );
		if( childIndividual != nullptr)
		{
			this->individuals.push_back( childIndividual );
			// if an individual has been produced, set it as the current individual
			if( i == 0 ) this->currentIndividual = this->individuals[ this->individuals.size() - 1 ];
		}
		// TODO error control?
	}
	
	// refresh the window with individuals list
	this->updateListWidget();
	//this->updateTreeWidget();
	//this->updatePreview();
	
	// free memory
	//delete(parameters);
	//LOG_DEBUG << "Parameters freed..." << ends;
}

void MicroGPMainWindow::about()
{
	// visualize the "about" message
	QMessageBox::about( this->menuQuestionMark, ABOUT_PROGRAMNAME, ABOUT_INFO);
	return;
}
