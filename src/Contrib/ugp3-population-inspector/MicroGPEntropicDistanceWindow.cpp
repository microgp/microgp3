// class definition
#include "MicroGPEntropicDistanceWindow.h"

// Qt classes
#include <QFileDialog>
#include <QMessageBox>

// MicroGP classes
#include "EvolutionaryCore.h"
#include "EvolutionaryAlgorithm.h"
#include "Entropy.h"

// other classes
#include "MicroGPMainWindow.h"

// define
#define TABLE_INDIVIDUAL_COLUMN_ID 0
#define TABLE_INDIVIDUAL_COLUMN_NUMBEROFSYMBOLS 1
#define TABLE_INDIVIDUAL_COLUMN_ENTROPICDISTANCE 2

#define TABLE_SYMBOLS_COLUMN_SYMBOL 0
#define TABLE_SYMBOLS_COLUMN_ISINSTARTINGPOINT 1
#define TABLE_SYMBOLS_COLUMN_ISINENDPOINT 2
#define TABLE_SYMBOLS_COLUMN_ELEMENT 3

using namespace std;
using namespace Ui;

/*
	TODO
	- Output diff in nuova finestra per vedere le differenze fra individui
*/

// re-define setupUi to include new bindings
void MicroGPEntropicDistanceWindow::setupUi(MicroGPMainWindow* parentWindow, QDialog* parentDialog)
{
	// run the inherited setupUi
	Ui_entropicDistanceWindow::setupUi(parentDialog);

	// keep track of main window
	this->mainWindow = parentWindow;
	
	/*
	// copy the starting and end points
	this->startingPoint = const_cast<ugp3::core::EnhancedIndividual*>( this->mainWindow->entropicDistanceStartingPoint );
	for(unsigned int i = 0; i < this->mainWindow->entropicDistanceEndPoints.size(); i++)
	{
		ugp3::core::EnhancedIndividual* individual = const_cast<ugp3::core::EnhancedIndividual*>( this->mainWindow->entropicDistanceEndPoints[i] );
		this->endPoints.push_back(individual);
	}

	// first, table widget of the starting point
	// set table widget to fit column size to the content
	//this->tableWidget_StartingPoint->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	// populate table widget with information from the main window
	this->tableWidget_StartingPoint->insertRow(0);
	this->tableWidget_StartingPoint->setItem(0,TABLE_INDIVIDUAL_COLUMN_ID, new QTableWidgetItem( QString::fromStdString( this->mainWindow->entropicDistanceStartingPoint->getId() ) ) );
	
	// number of symbols
	unsigned int numberOfSymbols = this->startingPoint->getMessage().size();

	this->tableWidget_StartingPoint->setItem(0,TABLE_INDIVIDUAL_COLUMN_NUMBEROFSYMBOLS, new QTableWidgetItem( QString("%1").arg( numberOfSymbols ) ) );
	this->tableWidget_StartingPoint->setItem(0,TABLE_INDIVIDUAL_COLUMN_ENTROPICDISTANCE, new QTableWidgetItem( QString("-") ) );
	
	// then, table widget of all end points
	//this->tableWidget_EndPoints->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	for(unsigned int i = 0; i < this->endPoints.size(); i++)
	{
		this->tableWidget_EndPoints->insertRow(i);
		this->tableWidget_EndPoints->setItem(	i,TABLE_INDIVIDUAL_COLUMN_ID, 
							new QTableWidgetItem( QString::fromStdString( this->endPoints[i]->getId() ) ) );
		numberOfSymbols = 0; //= this->endPoints[i]->getMessage().size();
		this->tableWidget_EndPoints->setItem(	i,TABLE_INDIVIDUAL_COLUMN_NUMBEROFSYMBOLS, 
							new QTableWidgetItem( QString("%1").arg( numberOfSymbols ) ) );
        	this->tableWidget_EndPoints->setItem(i,TABLE_INDIVIDUAL_COLUMN_ENTROPICDISTANCE, new QTableWidgetItem( QString("-") ) );
		
		// compute and set entropic distance
		double entropicDistance = this->computeEntropicDistance( this->startingPoint, this->endPoints[i] );
		this->tableWidget_EndPoints->setItem(   i,TABLE_INDIVIDUAL_COLUMN_ENTROPICDISTANCE, 
                                                        new QTableWidgetItem( QString("%1").arg( entropicDistance ) ) );
	}
	
	// set the first end point as selected
	this->tableWidget_EndPoints->setRangeSelected( QTableWidgetSelectionRange(0,0,TABLE_INDIVIDUAL_COLUMN_ID,TABLE_INDIVIDUAL_COLUMN_ENTROPICDISTANCE), true);
	
	// then, table widget of symbols
	//this->tableWidget_Symbols->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	// populate the two previews accordingly
	ostringstream ss1, ss2;
	ugp3::ctgraph::IdentityRelabeller relabeller;
	this->startingPoint->getGraphContainer().writeExternalRepresentation(ss1, relabeller);
	this->textBrowser_StartingPoint->setText( QString::fromStdString( ss1.str() ));
	this->endPoints[0]->getGraphContainer().writeExternalRepresentation(ss2, relabeller);
	this->textBrowser_EndPoint->setText( QString::fromStdString( ss2.str() ));
	
	// fill the symbols table
	this->fillSymbolsTable(0,0,-1,-1);
	
	// connect selection changed to fill of the symbols table
	connect(this->tableWidget_EndPoints, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(fillSymbolsTable(int,int,int,int)));
	*/
}


double MicroGPEntropicDistanceWindow::computeEntropicDistance(	ugp3::core::EnhancedIndividual* startingPoint, 
								ugp3::core::EnhancedIndividual* endPoint)
{
	// lots of parts are commented out, because the structure of Message is no longer the same
	// and most functions (namely getMessageInformation and getMessageMap) no longer exist
	
	// NEW COMPUTATION
	// first, compute the entropy of both individuals
	/*
	ugp3::Message totalMessage;
	totalMessage += startingPoint->getMessage();
	totalMessage += endPoint->getMessage();
	
	// then, entropy of starting point alone
	ugp3::Message startingPointMessage;
	startingPointMessage += startingPoint->getMessage();
	
	// finally, entropy of the end point alone
	ugp3::Message endPointMessage;
	endPointMessage += endPoint->getMessage();
	
	// entropic distance is the the difference between the two
	return 2*totalMessage.getSize() - startingPointMessage.getSize() - endPointMessage.getSize();
	*/
	return 0;
}


// fill the symbols table with information extracted from starting point and current end point
void MicroGPEntropicDistanceWindow::fillSymbolsTable(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	/*
	LOG_DEBUG << "Now filling symbols table..." << ends;

	// find current endPoint
	unsigned int i = 0;
	while( this->endPoints[i]->getId().compare( this->tableWidget_EndPoints->item(currentRow, TABLE_INDIVIDUAL_COLUMN_ID)->text().toStdString() ) != 0 && i < this->endPoints.size() ) i++;
	ugp3::core::EnhancedIndividual* endPoint = this->endPoints[i];

	// clear and fill the tableWidget_Symbols
	while(this->tableWidget_Symbols->rowCount() > 0) this->tableWidget_Symbols->removeRow( this->tableWidget_Symbols->rowCount() - 1);

	// map with correspondence between symbols and corresponding strings
	LOG_DEBUG << "Adding symbols for the starting individual." << ends;
	map< hash_t,string > symbolsStrings = this->startingPoint->getMessage().getMessageMap();
	LOG_DEBUG << "Size of the map symbolStrings is " << symbolsStrings.size() << ends;

	LOG_DEBUG << "Adding symbols for the end points." << ends;
	map< hash_t,string > symbolsStrings2 = endPoint->getMessage().getMessageMap();
	symbolsStrings.insert( symbolsStrings2.begin(), symbolsStrings2.end() );

	// populate a map with correspondance between hash_t and individual containing them
	map< hash_t,vector<ugp3::core::EnhancedIndividual*> > symbols;
	// first with symbols of the startingPoint
	for(unsigned int s = 0; s < this->startingPoint->getMessage().size(); s++)
	{
		symbols[ this->startingPoint->getMessage()[s] ].push_back( this->startingPoint );
	}
	// then with symbols of the endPoint
	for(unsigned int s = 0; s < endPoint->getMessage().size(); s++)
	{
		symbols[ endPoint->getMessage()[s] ].push_back( endPoint );
	}
	
	// debug
	LOG_DEBUG << "Symbols in symbolsString:" << ends;
	for(map<hash_t,string>::iterator it = symbolsStrings.begin(); it != symbolsStrings.end(); it++)
		LOG_DEBUG << "- Key: \"" << it->first << "\" -> Value: \"" << it->second << "\"" << ends;
	
	LOG_DEBUG << "Symbols in symbols:" << ends;
	for(map< hash_t,vector<ugp3::core::EnhancedIndividual*> >::iterator it = symbols.begin(); it != symbols.end(); it++)
		LOG_DEBUG << "- Key: \"" << it->first << "\" -> Size of array: \"" << it->second.size() << ends;
	
	// fill the table
	i = 0;
	for(map< hash_t,vector<ugp3::core::EnhancedIndividual*> >::iterator it = symbols.begin(); it != symbols.end(); it++)
	{
		this->tableWidget_Symbols->insertRow(i);
		
		// hash_t is kinda tricky to print...
		this->tableWidget_Symbols->setItem(i, TABLE_SYMBOLS_COLUMN_SYMBOL, new QTableWidgetItem( QString("%1").arg(it->first) ) );
		
		// look if a symbol appears in the starting point
		if( it->second[0] == this->startingPoint )
			this->tableWidget_Symbols->setItem(i, TABLE_SYMBOLS_COLUMN_ISINSTARTINGPOINT, new QTableWidgetItem( QString( "Yes" ) ) );
		else
			this->tableWidget_Symbols->setItem(i, TABLE_SYMBOLS_COLUMN_ISINSTARTINGPOINT, new QTableWidgetItem( QString( "No" ) ) );
		// look if a symbol appears in the end point
		if( it->second[0] == endPoint || ( it->second.size() > 1 && it->second[1] == endPoint) )
			this->tableWidget_Symbols->setItem(i, TABLE_SYMBOLS_COLUMN_ISINENDPOINT, new QTableWidgetItem( QString( "Yes"  ) ) );
		else
			this->tableWidget_Symbols->setItem(i, TABLE_SYMBOLS_COLUMN_ISINENDPOINT, new QTableWidgetItem( QString( "No"  ) ) );
		
		// finally, the representation of the symbol
		LOG_DEBUG << "Symbol #" << i << " is: \"" << symbolsStrings[it->first] << "\"" << ends;
		this->tableWidget_Symbols->setItem(	i, TABLE_SYMBOLS_COLUMN_ELEMENT, 
							new QTableWidgetItem( QString::fromStdString( symbolsStrings[it->first] ) ) ); 
		
		// resize row to contents (experimental)
		this->tableWidget_Symbols->resizeRowToContents(i);
		
		// increase index
		i++;
	}
	
	// resize table
	this->tableWidget_Symbols->resizeColumnsToContents();
	
	// also changed the individual displayed in the preview
	ostringstream ss;
	ugp3::ctgraph::IdentityRelabeller relabeller;
	endPoint->getGraphContainer().writeExternalRepresentation(ss, relabeller);
	this->textBrowser_EndPoint->setText( QString::fromStdString( ss.str() ));
*/
	return;
}

