// Inherited class; the Ui_ugpMainWindow might be changed by
// Qt Designer, so the relevant bindings must operate here

// class definition
#include "MicroGPMainWindow.h"

// Qt classes
#include <QFileDialog>
#include <QMessageBox>

// std classes
#include <typeinfo>

// MicroGP classes
#include "ConsoleHandler.h"
#include "EvolutionaryCore.h"
#include "EvolutionaryAlgorithm.h"
#include "File.h"
#include "Log.h"
#include "Statistics.h"

#include "Program.h"

// other classes
#include "MicroGPEntropicDistanceWindow.h"

// define
#define TABLE_COLUMN_ID 0
#define TABLE_COLUMN_PARENTS 1
#define TABLE_COLUMN_OPERATOR 2
#define TABLE_COLUMN_DELTAENTROPY 3
#define TABLE_COLUMN_BIRTH 4
#define TABLE_COLUMN_FITNESS 5

#define TABLE_COLUMN_PARAMETER 0
#define TABLE_COLUMN_VALUE 1

#define POPULATION_ENHANCED "enhanced"

#define PUSHBUTTON_SETASSTARTINGPOINT_SET "Set as Starting Point"
#define PUSHBUTTON_SETASSTARTINGPOINT_REMOVE "Remove Starting Point"

#define PUSHBUTTON_ADDTOENDPOINTS_ADD "Add To End Points"
#define PUSHBUTTON_ADDTOENDPOINTS_REMOVE "Remove from End Points"

#define ABOUT_PROGRAMNAME "Population Inspector v0.9"
#define ABOUT_INFO "Population Inspector is part of the MicroGP3 (ugp3) project, coded by Alberto Tonda <alberto.tonda@gmail.com>. For more information, visit http://ugp3.sourceforge.net" 

using namespace std;
using namespace Ui;

/* 	TODO

	Some push buttons were hidden, they are prepared for future developments.

	TODO */


// re-define setupUi to include new bindings
void MicroGPMainWindow::setupUi(QMainWindow* ugpMainWindow)
{
	// run the inherited setupUi
	Ui_populationInspectorMainWindow::setupUi(ugpMainWindow);

	// store the reference to the main window
	this->mainWindow = ugpMainWindow;
	// set the reference to the algorithm to nullptr
	this->algorithm = nullptr;
	this->entropicDistanceStartingPoint = nullptr;
	
	// set table widgets to fit column size to the content
    //this->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    //this->statisticsTableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	// bind the signals to the functions
	// load status file when corresponding button is selected
	connect(this->pushButton_LoadStatusFile, SIGNAL(clicked()), this, SLOT(loadStatusFile()));
	// preview individual
	connect(this->tableWidget, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(previewIndividual(int,int,int,int)));
	// set individual as starting point
	connect(this->pushButton_SetAsStartingPoint, SIGNAL(clicked()), this, SLOT(setAsStartingPoint())); 
	// add individual to end points
	connect(this->pushButton_AddToEndPoints, SIGNAL(clicked()), this, SLOT(addToEndPoints()));
	// compute and visualize Entropic Distance
	connect(this->pushButton_EntropicDistance, SIGNAL(clicked()), this, SLOT(showEntropicDistance()));
	// save current (highlighted) individual to file
	connect(this->pushButton_IndividualToFile, SIGNAL(clicked()), this, SLOT(saveIndividualToFile()));

	// same thing as above, but for menus!
	connect(this->actionLoad_Population, SIGNAL(triggered()), this, SLOT(loadStatusFile()));
	connect(this->actionSave_Individual_as, SIGNAL(triggered()), this, SLOT(saveIndividualToFile()));
	connect(this->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

	// open logs
	/*
	ugp3::log::ConsoleHandler* consoleHandler = new ugp3::log::ConsoleHandler();
	::log_.addHandler(*consoleHandler); //::log_ is a global instance of a Log class
	consoleHandler->setLevel(ugp3::log::Level::Info);
	*/

	ugp3::log::ConsoleHandler* debugHandler = new ugp3::log::ConsoleHandler();
	::log_.addHandler(*debugHandler); //::log_ is a global instance of a Log class
	debugHandler->setLevel(ugp3::log::Level::Debug);
	

	// register genetic operators
	ugp3::frontend::Program::registerOperators();

	// TODO this part hides all push buttons, to be restored when development resumes
	this->pushButton_AddToEndPoints->hide();
	this->pushButton_EntropicDistance->hide();
	this->pushButton_IndividualToFile->hide();
	this->pushButton_LoadStatusFile->hide();
	this->pushButton_SetAsStartingPoint->hide();

}

// load status file and populate table widget
void MicroGPMainWindow::loadStatusFile()
{
	QString statusFile = 	QFileDialog::getOpenFileName(this->tableWidget, 
				tr("Open XML Status File"), "", tr("XML (*.xml);;All Files (*)"));
	
	// if a choice has not been made, return
	if( statusFile.isEmpty() ) return;

	// check whether the algorithm associated to this class needs to be reset
	if( this->algorithm != nullptr ) 
	{
		// reset other variables
		this->entropicDistanceStartingPoint = nullptr;
		this->entropicDistanceEndPoints.clear();

		LOG_DEBUG << "Deleting algorithm..." << endl;
		delete(this->algorithm);
		this->algorithm = nullptr;
		
		// set current selection to nullptr, so there are no issues with the preview
		this->tableWidget->setCurrentCell(-1,-1);
	
		// remove all elements from the table
		while( this->tableWidget->rowCount() > 0 ) this->tableWidget->removeRow( this->tableWidget->rowCount() - 1 );
		
		// clean the preview
		this->textBrowser->clear();
	}
	
	// if a choice has been made, recover the EvolutionaryAlgorithm from the status file
	LOG_VERBOSE << "Loading status file..." << ends;
	unique_ptr<ugp3::core::EvolutionaryAlgorithm> evolutionaryAlgorithm( new ugp3::core::EvolutionaryAlgorithm() ); 
	evolutionaryAlgorithm->fromFile(statusFile.toStdString(), false); 
	
	// TODO: here, a try{}catch construct could be used to catch the possible exception about the missing constraints file;
	//	 but then, how could we solve the issue? for example, prompt the user with another window to select the associated
	//	 constraint file...but inserting it in the loop might not be trivial. Random idea: create a copy of the original
	//	 status file, and write the complete path inside, then load it and delete it. Wow.
	
	// TODO: what happens if there is more than one population? open dialog to select one
	// populate the table widget
	LOG_VERBOSE << "Getting population 0..." << ends;
	ugp3::core::Population& firstPopulation = evolutionaryAlgorithm->getPopulation(0);
	
	if( dynamic_cast<ugp3::core::IndividualPopulation*>( &firstPopulation ) == nullptr )
	{
		LOG_ERROR << "The first population does not seem to be an IndividualPopulation..." << ends;
	}
	
	ugp3::core::IndividualPopulation* population = dynamic_cast<ugp3::core::IndividualPopulation*>( &firstPopulation );

	
	for(unsigned int i = 0; i < population->getIndividualCount(); i++)
	{

		LOG_DEBUG << "Updating table with individual #" << i << ends;
		const ugp3::core::Individual& individual = population->getIndividual(i);
		LOG_DEBUG << "Individual " << i << " is called \"" << individual.getId() <<  "\"" << ends;
		
		// add a new row in the table widget and fill the fields
		this->tableWidget->insertRow(i);
		
		// populate the first columns
		this->tableWidget->setItem(i, TABLE_COLUMN_ID, new QTableWidgetItem( QString::fromStdString( individual.getId() )));
		this->tableWidget->setItem(	i, TABLE_COLUMN_OPERATOR, 
						new QTableWidgetItem( QString::fromStdString( individual.getLineage().getOperatorName() )));

		// individuals from "generations 0" have no parents, so some checks are in order
		const vector<string>& parents = individual.getLineage().getParentNames();
		QString parentNames;

		if( parents.size() > 0 )
		{
			parentNames = parents[0].c_str();
			for(unsigned int p = 1; p < parents.size(); p++) 
			{
				parentNames += ",";
				parentNames += parents[p].c_str();
			}
		}
		else
		{
			parentNames = "None";
		}
		this->tableWidget->setItem( i, TABLE_COLUMN_PARENTS, new QTableWidgetItem( parentNames ));
		
		// check whether the individuals are EnhancedIndividuals: in this case, set the deltaEntropy appropriately
		double deltaEntropy = 0.0;
		if( dynamic_cast<const ugp3::core::EnhancedIndividual*>(&individual) != nullptr )
		{
			const ugp3::core::EnhancedIndividual* enhancedIndividual = 
				dynamic_cast<const ugp3::core::EnhancedIndividual*>(&individual);
			deltaEntropy = enhancedIndividual->getDeltaEntropy().getValue();
		}
		this->tableWidget->setItem(i, TABLE_COLUMN_DELTAENTROPY, new QTableWidgetItem( QString("%1").arg(deltaEntropy) ) );

		// if this is the first individual, also initialize the columns related to fitness
		vector<double> values = individual.getFitness().getValues();
		for(unsigned int f = 0; f < values.size(); f++)
		{
			if(i == 0)
			{
				this->tableWidget->insertColumn( this->tableWidget->columnCount() );
				this->tableWidget->setHorizontalHeaderItem( this->tableWidget->columnCount() - 1,
							new QTableWidgetItem( QString("fitness-%1").arg(f) ) );
			}

			this->tableWidget->setItem(	i, TABLE_COLUMN_FITNESS + f, 
							new QTableWidgetItem( QString("%1").arg(values[f]) ) );
		}
		
		// put birth
		this->tableWidget->setItem(i, TABLE_COLUMN_BIRTH, new QTableWidgetItem( QString("%1").arg(individual.getBirth()) ) );
		
	}

	// connect the algorithm to the internal reference
	this->algorithm = evolutionaryAlgorithm.release();

	// populate the statistics table
	this->populateStatisticsTable();

	return;
}

// preview individual when corresponding line is selected
void MicroGPMainWindow::previewIndividual(int row, int column, int previousRow, int previousColumn)
{
	// if the selection is wrong, return
	LOG_DEBUG << "Entering previewIndividual..." << ends;
	if( row < 0 ) return;

	// find out which individual was selected
	ugp3::core::IndividualPopulation* population = dynamic_cast<ugp3::core::IndividualPopulation*>( &this->algorithm->getPopulation(0) );
	const ugp3::core::Individual* individual = &population->getIndividual(row);
	
	// get individual text serialization
	ostringstream ss;
	ugp3::ctgraph::IdentityRelabeller relabeller;
	individual->getGraphContainer().writeExternalRepresentation(ss, relabeller);
	this->textBrowser->setText( QString::fromStdString( ss.str() ));

	// if it's selected as starting point, change the text of the button accordingly ("Add -> Remove", "Set -> Unset")
	if( this->entropicDistanceStartingPoint != nullptr && this->entropicDistanceStartingPoint == individual )
	{
		this->pushButton_SetAsStartingPoint->setText( QString( PUSHBUTTON_SETASSTARTINGPOINT_REMOVE ) );
	}
	else
	{
		this->pushButton_SetAsStartingPoint->setText( QString( PUSHBUTTON_SETASSTARTINGPOINT_SET ) );
	}
	
	// if it's among the end points, do the same for the other button
	bool found = false;
	for(unsigned int i = 0; i < this->entropicDistanceEndPoints.size() && !found; i++)
	{
		if( this->entropicDistanceEndPoints[i] == individual ) found = true;
	}
	
	if( found == true )
	{
		this->pushButton_AddToEndPoints->setText( QString( PUSHBUTTON_ADDTOENDPOINTS_REMOVE ) );
	}
	else
	{
		this->pushButton_AddToEndPoints->setText( QString( PUSHBUTTON_ADDTOENDPOINTS_ADD ) );
	}

	LOG_DEBUG << "Exiting previewIndividual..." << ends;
	return;
}

void MicroGPMainWindow::setAsStartingPoint()
{
	// if the current row is not valid, return
	if( this->tableWidget->currentRow() < 0 ) return;	

	// get individual currently selected
	ugp3::core::IndividualPopulation* population = dynamic_cast<ugp3::core::IndividualPopulation*>( &this->algorithm->getPopulation(0) );
	const ugp3::core::Individual& individual = population->getIndividual(this->tableWidget->currentRow() );
	
	// change button
	this->pushButton_SetAsStartingPoint->setText( QString( PUSHBUTTON_SETASSTARTINGPOINT_REMOVE ) );

	// TODO: remove selection from another individual
	if( this->entropicDistanceStartingPoint != nullptr )
	{
		// look for individual with same ID in population
		unsigned int r = 0;
		while( this->tableWidget->item(r, TABLE_COLUMN_ID)->text().toStdString().compare( this->entropicDistanceStartingPoint->getId() ) != 0)
		{
			r++;
		}
		
		if( r < this->tableWidget->rowCount() )
		{
			for(unsigned int c = 0; c < this->tableWidget->columnCount(); c++)
			{
				this->tableWidget->item(r, c)->setBackground( Qt::white );
			}
			
		}
		
		// if the current individual is the same, remove it and exit
		if( r == this->tableWidget->currentRow() )
		{
			this->entropicDistanceStartingPoint = nullptr;
			this->pushButton_SetAsStartingPoint->setText( QString( PUSHBUTTON_SETASSTARTINGPOINT_SET ) );
			return;
		}
	}

	// set it as starting point
	this->entropicDistanceStartingPoint = (const ugp3::core::EnhancedIndividual*) &individual;
	
	// mark it somehow on the table widget
	for(unsigned int c = 0; c < this->tableWidget->columnCount(); c++)
	{
		this->tableWidget->item(this->tableWidget->currentRow(), c)->setBackground( Qt::red );
	}
}

void MicroGPMainWindow::addToEndPoints()
{
	// if the current row is not valid, return
	if( this->tableWidget->currentRow() < 0 ) return;

	// get the selection
	QList<QTableWidgetSelectionRange> selectionRanges = this->tableWidget->selectedRanges();
	
	// for each selection (there should be only one, but who knows?)
	for(	QList<QTableWidgetSelectionRange>::iterator selectionRange = selectionRanges.begin(); 
		selectionRange != selectionRanges.end();
		selectionRange++
	)
	{
		// for the limits of the selection range
		LOG_DEBUG << "Analyzing rows from " << selectionRange->topRow() << " to " << selectionRange->bottomRow() << ends;
		for(unsigned int i = selectionRange->topRow(); i <= selectionRange->bottomRow(); i++)
		{
			// get individual currently selected
			ugp3::core::IndividualPopulation* population = dynamic_cast<ugp3::core::IndividualPopulation*>( &this->algorithm->getPopulation(0) );
			const ugp3::core::Individual* individual = &population->getIndividual( i );
			
			LOG_DEBUG << "Now analyzing individual \"" << individual->getId() << "\"" << ends;

			bool alreadyEndPoint = false;
			vector<const ugp3::core::EnhancedIndividual*>::iterator position;
			for(	position = this->entropicDistanceEndPoints.begin(); 
				position != this->entropicDistanceEndPoints.end() && alreadyEndPoint == false; 
				position++)
			{
				if( individual == *position ) alreadyEndPoint = true;
			}
			
			if( !alreadyEndPoint )
			{
				LOG_DEBUG << "Adding individual \"" << individual->getId() << "\" to end points..." << ends;

				// if it's not already there, add it to the end points
				this->entropicDistanceEndPoints.push_back( (const ugp3::core::EnhancedIndividual*) individual );
			
				// mark it somehow on the table widget
				for(unsigned int c = 0; c < this->tableWidget->columnCount(); c++)
				{
					this->tableWidget->item(i,c)->setBackground( Qt::green );
				}
				
				LOG_DEBUG << "Individual added!" << ends;
			}
			else
			{
				LOG_DEBUG << "Removing individual \"" << individual->getId() << "\" from end points..." << ends;

				// remove individual if it was already selected
				this->entropicDistanceEndPoints.erase( --position ); // it's actually the previous position
				
				LOG_DEBUG << "Individual removed!" << ends;
				
				// unmark it on the table widget
				for(unsigned int c = 0; c < this->tableWidget->columnCount(); c++)
                                {
                                        this->tableWidget->item(i,c)->setBackground( Qt::white );
                                }

			}
		}

	}
			
}

void MicroGPMainWindow::showEntropicDistance()
{
	// if the starting point is not set, show message and return
	if( this->entropicDistanceStartingPoint == nullptr )
	{
		// show message
		QMessageBox::warning(	this->pushButton_EntropicDistance, "Warning: starting point not selected.", 
					"You have to select a starting point for the Entropic Distance. Please, select a row and click the corresponding button.");
		return;
	}
	
	// if there are no end points, show message and return
	if( this->entropicDistanceEndPoints.size() == 0 )
	{
		// show message
		QMessageBox::warning(	this->pushButton_EntropicDistance, "Warning: end points not set.",
					"You have to select at least an end point for the Entropic Distance. Please, select a row and click the corresponding button.");
		return;
	}

	// create new dialog
	QDialog* dialogEntropicDistance = new QDialog();
	
	// initialize the entropic distance window
	MicroGPEntropicDistanceWindow entropicDistanceWindow;
	entropicDistanceWindow.setupUi(this, dialogEntropicDistance);
	
	if( dialogEntropicDistance->exec() == QDialog::Accepted)return;
	
}

void MicroGPMainWindow::saveIndividualToFile()
{
	// if the current row is not valid, return
	if( this->tableWidget->currentRow() < 0 ) return;	

	// get individual currently selected
	ugp3::core::IndividualPopulation* population = dynamic_cast<ugp3::core::IndividualPopulation*>( &this->algorithm->getPopulation(0) );
	const ugp3::core::Individual& individual = population->getIndividual( this->tableWidget->currentRow() );
	
	// open Save dialog
	QString individualFile = 	QFileDialog::getSaveFileName(this->tableWidget, 
				tr("Save Individual File"), "", tr("Individual (*.ind);;All Files (*)"));
	
	// if a choice has not been made, return
	if( individualFile.isEmpty() ) return;
	
	// otherwise, let's write to file!
	QFile outputFile( individualFile );
	if( !outputFile.open(QIODevice::WriteOnly | QIODevice::Text) ) return; // TODO: show error window

	// convert individual to QString and write to file
	ostringstream ss;
	ugp3::ctgraph::IdentityRelabeller relabeller;
	individual.getGraphContainer().writeExternalRepresentation(ss, relabeller);
	
	outputFile.write( ss.str().c_str() );

	outputFile.close();
	
	return;
}

void MicroGPMainWindow::populateStatisticsTable()
{
	// reset all values in the table
	// set current selection to nullptr, so there are no issues with the preview
	this->statisticsTableWidget->setCurrentCell(-1,-1);

	// remove all elements from the table
	while( this->statisticsTableWidget->rowCount() > 0 )
		this->statisticsTableWidget->removeRow( this->statisticsTableWidget->rowCount() - 1 );

	int currentRow = this->statisticsTableWidget->rowCount();

	// TODO: what happens where there is more than one population?
	ugp3::core::Population& firstPopulation = this->algorithm->getPopulation(0);
	ugp3::core::IndividualPopulation* population = dynamic_cast<ugp3::core::IndividualPopulation*>( &firstPopulation );

	// name of the population
	this->statisticsTableWidget->insertRow( currentRow );
	this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, new QTableWidgetItem( QString::fromStdString( "Name"  )));
	this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, new QTableWidgetItem( QString::fromStdString( population->getName() ) ));
	currentRow++;

	// type of the population
	this->statisticsTableWidget->insertRow( currentRow );
	this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, new QTableWidgetItem( QString::fromStdString( "Type" )));
	this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, new QTableWidgetItem( QString::fromStdString( population->getType() ) ));
	currentRow++;

	// generation 
	this->statisticsTableWidget->insertRow( currentRow );
	this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, new QTableWidgetItem( QString::fromStdString( "Generation"  )));
	this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, new QTableWidgetItem( QString("%1").arg( population->getGeneration() ) ));
	currentRow++;

	// population-specific parameters
	if( population->getType().compare(POPULATION_ENHANCED) == 0 )
	{
		ugp3::core::EnhancedPopulation* enhancedPopulation = dynamic_cast<ugp3::core::EnhancedPopulation*>( population );
		
		// mu 
		this->statisticsTableWidget->insertRow( currentRow );
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
			new QTableWidgetItem( QString::fromStdString( "Mu (maximum size of the population)"  )));
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
			new QTableWidgetItem( QString("%1").arg( enhancedPopulation->getParameters().getMu() ) ));
		currentRow++;
		
		// nu 
		this->statisticsTableWidget->insertRow( currentRow );
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
			new QTableWidgetItem( QString::fromStdString( "Nu (initial size of the population)"  )));
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
			new QTableWidgetItem( QString("%1").arg( enhancedPopulation->getParameters().getNu() ) ));
		currentRow++;
		
		// lambda 
		this->statisticsTableWidget->insertRow( currentRow );
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
			new QTableWidgetItem( QString::fromStdString( "Lambda (operators applied at each generation)"  )));
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
			new QTableWidgetItem( QString("%1").arg( enhancedPopulation->getParameters().getLambda() ) ));
		currentRow++;
		
		// maximum age
		this->statisticsTableWidget->insertRow( currentRow );
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
			new QTableWidgetItem( QString::fromStdString( "Maximum age (for individuals, in generations)"  )));
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
			new QTableWidgetItem( QString("%1").arg( enhancedPopulation->getParameters().getMaximumAge() ) ));
		currentRow++;
		
		// elite size 
		this->statisticsTableWidget->insertRow( currentRow );
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
			new QTableWidgetItem( QString::fromStdString( "Elite (best individuals that don't age)"  )));
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
			new QTableWidgetItem( QString("%1").arg( enhancedPopulation->getParameters().getEliteCardinality() ) ));
		currentRow++;
		
		// inertia
		this->statisticsTableWidget->insertRow( currentRow );
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
			new QTableWidgetItem( QString::fromStdString( "Inertia (regulates self-adapting)"  )));
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
			new QTableWidgetItem( QString("%1").arg( enhancedPopulation->getParameters().getInertia() ) ));
		currentRow++;
		
		// sigma
		this->statisticsTableWidget->insertRow( currentRow );
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
			new QTableWidgetItem( QString::fromStdString( "Sigma (strength of the genetic operators)"  )));
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
			new QTableWidgetItem( QString("%1").arg( enhancedPopulation->getParameters().getSigma() ) ));
		currentRow++;

		// elapsed time 
		this->statisticsTableWidget->insertRow( currentRow );
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
			new QTableWidgetItem( QString::fromStdString( "Total time elapsed"  )));
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
			new QTableWidgetItem( QString("%1 s").arg( chrono::duration_cast<chrono::seconds>( 
				enhancedPopulation->getAlgorithm().getElapsedTime() ).count()  ) ));
		currentRow++;
		
		// total evaluations
		this->statisticsTableWidget->insertRow( currentRow );
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
			new QTableWidgetItem( QString::fromStdString( "Total evaluations"  )));
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
			new QTableWidgetItem( QString("%1").arg( enhancedPopulation->getParameters().getEvaluator().getTotalEvaluations() ) ));
		currentRow++;
		
		// entropy
		this->statisticsTableWidget->insertRow( currentRow );
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
			new QTableWidgetItem( QString::fromStdString( "Population entropy"  )));
		this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
			new QTableWidgetItem( QString("%1").arg( enhancedPopulation->getEntropy() ) ));
		// TODO: current entropy is not correctly READ, probably in EnhancedPopulation::readXml
		currentRow++;
		
		// selector
		ugp3::core::CandidateSelection& selector = enhancedPopulation->getParameters().getSelector();
		
		try
		{
			// tournament selection with fitness hole
			ugp3::core::TournamentSelectionWithFitnessHole& tournamentSelection = dynamic_cast<ugp3::core::TournamentSelectionWithFitnessHole&>( selector );
		
			this->statisticsTableWidget->insertRow( currentRow );
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
				new QTableWidgetItem( QString::fromStdString( "Selection type"  )));
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
				new QTableWidgetItem( QString::fromStdString( tournamentSelection.XML_SCHEMA_TYPE ) ));

			// set cell background to light blue
			for(unsigned int c = 0; c < this->statisticsTableWidget->columnCount(); c++)
				this->statisticsTableWidget->item(currentRow, c)->setBackground( QColor(0, 0, 255, 127) );
			
			currentRow++;

			this->statisticsTableWidget->insertRow( currentRow );
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
				new QTableWidgetItem( QString::fromStdString( "Tau (individuals in the tournament)"  )));
			// tau and metatau actually need some preparation
			double tau = tournamentSelection.getTau();
			double metaTau = 1.0 * tau / enhancedPopulation->getParameters().getMu() * 100;			
			
			ostringstream ss;
			ss << setprecision(3) << tau;
			ss << " (" << setprecision(3) << metaTau << " \% of the population)";

			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
				new QTableWidgetItem( QString::fromStdString(ss.str() ) ));
 
			// set cell background to light blue
			for(unsigned int c = 0; c < this->statisticsTableWidget->columnCount(); c++)
				this->statisticsTableWidget->item(currentRow, c)->setBackground( QColor(0, 0, 255, 127) );
			
			currentRow++;

			this->statisticsTableWidget->insertRow( currentRow );
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
				new QTableWidgetItem( QString::fromStdString( "Tau min"  )));
			// tau and metatau actually need some preparation
			tau = tournamentSelection.getTauMin();
			metaTau = 1.0 * tau / enhancedPopulation->getParameters().getMu() * 100;			
			
			ss.str(string()); // empty stringstream
			ss << setprecision(3) << tau;
			ss << " (" << setprecision(3) << metaTau << " \% of the population)";

			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
				new QTableWidgetItem( QString::fromStdString(ss.str() ) ));
 
			// set cell background to light blue
			for(unsigned int c = 0; c < this->statisticsTableWidget->columnCount(); c++)
				this->statisticsTableWidget->item(currentRow, c)->setBackground( QColor(0, 0, 255, 127) );
			
			currentRow++;

			this->statisticsTableWidget->insertRow( currentRow );
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
				new QTableWidgetItem( QString::fromStdString( "Tau max"  )));
			// tau and metatau actually need some preparation
			tau = tournamentSelection.getTauMax();
			metaTau = 1.0 * tau / enhancedPopulation->getParameters().getMu() * 100;			
			
			ss.str(string()); // empty stringstream
			ss << setprecision(3) << tau;
			ss << " (" << setprecision(3) << metaTau << " \% of the population)";

			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
				new QTableWidgetItem( QString::fromStdString(ss.str() ) ));
 
			// set cell background to light blue
			for(unsigned int c = 0; c < this->statisticsTableWidget->columnCount(); c++)
				this->statisticsTableWidget->item(currentRow, c)->setBackground( QColor(0, 0, 255, 127) );
			
			currentRow++;

			this->statisticsTableWidget->insertRow( currentRow );
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
				new QTableWidgetItem( QString::fromStdString( "Fitness hole"  )));
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
				new QTableWidgetItem( QString("%1").arg(tournamentSelection.getFitnessHole() ) ));
			// set cell background to light blue
			for(unsigned int c = 0; c < this->statisticsTableWidget->columnCount(); c++)
				this->statisticsTableWidget->item(currentRow, c)->setBackground( QColor(0, 0, 255, 127) );
			
			currentRow++;
			
			
		}
		catch(std::bad_cast& e){ /* TODO: do something here? */ }
			
		try
		{
			// ranking selection
			ugp3::core::RankingSelection& rankingSelection = dynamic_cast<ugp3::core::RankingSelection&>( selector );	
			// TODO
		}
		catch(std::bad_cast& e){ /* TODO; do something here? */ }
		
		// genetic operators
		ugp3::core::Statistics& activations = 
			enhancedPopulation->getParameters().getActivations();
		
		for(unsigned int i = 0; i < activations.getDataCount(); i++)
		{
			ugp3::core::Data& data = activations.getData(i);
			ugp3::core::SAData* saData = dynamic_cast<ugp3::core::SAData*>( &data );
			this->statisticsTableWidget->insertRow( currentRow );
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
				new QTableWidgetItem( QString::fromStdString( "Operator " + data.getOperatorName() ) ));
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
				new QTableWidgetItem( QString("%1").arg( saData->getWeight() ) ));
			
			// set cell background to light yellow
			for(int c = 0; c < this->statisticsTableWidget->columnCount(); c++)
				this->statisticsTableWidget->item(currentRow, c)->setBackground( QColor(255, 255, 0, 127) );

			currentRow++;
		}
		
		// stop conditions
		// maximum fitness
		if( enhancedPopulation->getParameters().getMaximumFitnessStop() == true )
		{
			this->statisticsTableWidget->insertRow( currentRow );
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
				new QTableWidgetItem( QString::fromStdString( "Stop condition: maximum fitness"  )));
			
			// prepare maximum fitness
			vector<double> maxFitness = enhancedPopulation->getParameters().getMaximumFitness();
			ostringstream ss;
			for(unsigned int i = 0; i < maxFitness.size(); i++)
				ss << maxFitness[i] << " ";

			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
				new QTableWidgetItem( QString::fromStdString( ss.str() ) ));
			
			// set cell background to light red
			for(unsigned int c = 0; c < this->statisticsTableWidget->columnCount(); c++)
				this->statisticsTableWidget->item(currentRow, c)->setBackground( QColor(255, 0, 0, 127) );

			currentRow++;
		}
		
		// maximum generations
		if( enhancedPopulation->getParameters().getMaximumGenerationsStop() == true )
		{
			this->statisticsTableWidget->insertRow( currentRow );
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
				new QTableWidgetItem( QString::fromStdString( "Stop condition: maximum generations"  )));
			
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
				new QTableWidgetItem( QString("%1").arg( enhancedPopulation->getParameters().getMaximumGenerations() ) ));
			// set cell background to light red
			for(unsigned int c = 0; c < this->statisticsTableWidget->columnCount(); c++)
				this->statisticsTableWidget->item(currentRow, c)->setBackground( QColor(255, 0, 0, 127) );
			currentRow++;
		}

		// maximum evaluations 
		if( enhancedPopulation->getParameters().getMaximumEvaluationsStop() == true )
		{
			this->statisticsTableWidget->insertRow( currentRow );
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
				new QTableWidgetItem( QString::fromStdString( "Stop condition: maximum evaluations"  )));
			
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
				new QTableWidgetItem( QString("%1").arg( enhancedPopulation->getParameters().getMaximumEvaluations() ) ));
			// set cell background to light red
			for(unsigned int c = 0; c < this->statisticsTableWidget->columnCount(); c++)
				this->statisticsTableWidget->item(currentRow, c)->setBackground( QColor(255, 0, 0, 127) );
			currentRow++;
		}
	
		// maximum time 
		if( enhancedPopulation->getParameters().getMaximumTimeStop() == true )
		{
			this->statisticsTableWidget->insertRow( currentRow );
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
				new QTableWidgetItem( QString::fromStdString( "Stop condition: maximum time"  )));
			
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
				new QTableWidgetItem( QString("%1 s").arg( chrono::duration_cast<chrono::seconds>(enhancedPopulation->getParameters().getMaximumTime()).count() ) ));
			// set cell background to light red
			for(unsigned int c = 0; c < this->statisticsTableWidget->columnCount(); c++)
				this->statisticsTableWidget->item(currentRow, c)->setBackground( QColor(255, 0, 0, 127) );
			currentRow++;
		}
	
		// steady state
		if( enhancedPopulation->getParameters().getSteadyStateGenerationsStop() == true )
		{
			this->statisticsTableWidget->insertRow( currentRow );
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
				new QTableWidgetItem( QString::fromStdString( "Stop condition: steady state"  )));
			
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
				new QTableWidgetItem( QString("%1").arg( enhancedPopulation->getParameters().getMaximumSteadyStateGenerations() ) ));
			// set cell background to light red
			for(unsigned int c = 0; c < this->statisticsTableWidget->columnCount(); c++)
				this->statisticsTableWidget->item(currentRow, c)->setBackground( QColor(255, 0, 0, 127) );
			currentRow++;
			
			// also, add the current steady state generations
			this->statisticsTableWidget->insertRow( currentRow );
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_PARAMETER, 
				new QTableWidgetItem( QString::fromStdString( "Generations with no improvement"  )));
			
			this->statisticsTableWidget->setItem(currentRow, TABLE_COLUMN_VALUE, 
			new QTableWidgetItem( QString("%1").arg( enhancedPopulation->getParameters().getMaximumSteadyStateGenerations() ) ));
			// set cell background to light red
			for(unsigned int c = 0; c < this->statisticsTableWidget->columnCount(); c++)
				this->statisticsTableWidget->item(currentRow, c)->setBackground( QColor(255, 0, 0, 127) );
			currentRow++;
		}
		
	}
	
	return;
}

void MicroGPMainWindow::about()
{
	// visualize the "about" message
	QMessageBox::about( this->menuQuestionMark, ABOUT_PROGRAMNAME, ABOUT_INFO);
	return;
}
