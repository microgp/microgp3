// class definition
#include "MicroGPMainWindow.h"

// Qt classes
#include <QFileDialog>
#include <QInputDialog>

// other classes
#include <iostream>

// defines
#define TABLE_POPULATION_COLUMN_NUMBER 0
#define TABLE_POPULATION_COLUMN_NAME 1
#define TABLE_POPULATION_COLUMN_TYPE 2
#define TABLE_POPULATION_COLUMN_FILE 3
#define TABLE_POPULATION_COLUMN_RECOVERED 4
#define TABLE_POPULATION_COLUMN_MERGE 5

// namespace
using namespace std;
using namespace Ui;

// setup graphical interface
void MicroGPMainWindow::setupTabEvolution()
{
	// TAB: Evolution
	
	// - connect "Add" button to slot that loads population file
	connect(this->pushButton_PopulationsAdd, SIGNAL(clicked()), this, SLOT(addPopulation()));

}


// promemoria
void MicroGPMainWindow::startEvolution()
{
	// open an INFO log file for the console, and (eventually) a DEBUG one from the name in the corresponding field

	// create evolutionary algorithm
	
	// read all population files in the table and add them to the evolutionary algorithm
	
	// read statistics filename (eventually fill the comboBox with possible fitness values and outputs)

	// while a stop condition is not met
	
		// step of the evolutionary algorithm
		
		// adjust bars (how?)
		
		// display statistics and update graphs
}

// slot to open dialog 
void MicroGPMainWindow::addPopulation()
{
	QString populationToAddFile = QFileDialog::getOpenFileName(this->textEdit, 
				tr("Open XML Population File"), "", tr("XML (*.xml);;All Files (*)"));
	
	// if no population file has been selected, exit
	if( populationToAddFile.isEmpty() ) return;

	// open dialog window to input line and get population name
	//cout << this->tableWidget_Populations->rowCount() << endl;
	QString defaultPopulationName = QString("population-%1").arg( this->tableWidget_Populations->rowCount() );
	QString populationName = QInputDialog::getText(	this->tableWidget_Populations, "Set population name", "Population name", 
							QLineEdit::Normal, defaultPopulationName);
	
	// if no name is specified, set one by default
	if( populationName.isEmpty() ) populationName = defaultPopulationName; 
	
	// now insert population details inside the table widget
	unsigned int currentIndex = this->tableWidget_Populations->rowCount();
	this->tableWidget_Populations->insertRow( currentIndex );
	this->tableWidget_Populations->setItem(	currentIndex, TABLE_POPULATION_COLUMN_NUMBER,
						new QTableWidgetItem( QString("%1").arg( currentIndex ) ) );
	this->tableWidget_Populations->setItem(	currentIndex, TABLE_POPULATION_COLUMN_NAME,
						new QTableWidgetItem( populationName ) );
	this->tableWidget_Populations->setItem(	currentIndex, TABLE_POPULATION_COLUMN_FILE,
						new QTableWidgetItem( populationToAddFile ) );

	return;
}
