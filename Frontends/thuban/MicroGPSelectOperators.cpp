#include "MicroGPSelectOperators.h"

// standard classes
#include <iostream>
#include <fstream>

// MicroGP classes
#include "Operators/AllopatricDifferentialOperator.h"
#include "Operators/AlterationMutation.h"
#include "Operators/InsertionMutation.h"
#include "Operators/InverOverCrossover.h"
#include "Operators/OnePointCrossover.h"
#include "Operators/OnePointImpreciseCrossover.h"
#include "Operators/RandomizerOperator.h"
#include "Operators/RemovalMutation.h"
#include "Operators/ReplacementMutation.h"
#include "Operators/ScanMutationBitArray.h"
#include "Operators/ScanMutationConstant.h"
#include "Operators/ScanMutationFloat.h"
#include "Operators/ScanMutationInnerLabel.h"
#include "Operators/ScanMutationInteger.h"
#include "Operators/SimpleDifferentialOperator.h"
#include "Operators/SingleParameterAlterationMutation.h"
#include "Operators/SubGraphInsertionMutation.h"
#include "Operators/SubGraphRemovalMutation.h"
#include "Operators/SubGraphReplacementMutation.h"
#include "Operators/SwapMutation.h"
#include "Operators/TwoPointCrossover.h"
#include "Operators/TwoPointImpreciseCrossover.h"

// other classes
#include "MicroGPMainWindow.h"

using namespace Ui;
using namespace std;

void MicroGPSelectOperators::setupUi(MicroGPMainWindow* parentWindow, QDialog* parentDialog)
{
	// setup the user interface like parent class
	Ui_SelectOperators::setupUi(parentDialog);
	
	// read operators from the table and populate list on the right
	//cout << "Reading values from tableWidget_Operators..." << endl;
	
	for(unsigned int r = 0; r < parentWindow->tableWidget_Operators->rowCount(); r++)
	{
		// names of the operators are in column 0
		this->listWidget_SelectedOperators->addItem( parentWindow->tableWidget_Operators->item(r,0)->text() );
		this->listWidget_SelectedOperators->sortItems(Qt::AscendingOrder);
	}
	
	// TODO: map <string,string> with operator name linked to its description?
	
	// read ALL operator names (from where?) and populate list on the left
	ugp3::core::AllopatricDifferentialOperator ado;
	this->listWidget_AllOperators->addItem( QString::fromStdString( ado.getName() ) );

	ugp3::core::AlterationMutationOperator amo;
	this->listWidget_AllOperators->addItem( QString::fromStdString( amo.getName() ) );

	ugp3::core::InsertionMutationOperator imo;
	this->listWidget_AllOperators->addItem( QString::fromStdString( imo.getName() ) );

	ugp3::core::InverOverCrossoverOperator ioco;
	this->listWidget_AllOperators->addItem( QString::fromStdString( ioco.getName() ) );

	ugp3::core::OnePointCrossoverOperator opco;
	this->listWidget_AllOperators->addItem( QString::fromStdString( opco.getName() ) );

	ugp3::core::OnePointImpreciseCrossoverOperator opico;
	this->listWidget_AllOperators->addItem( QString::fromStdString( opico.getName() ) );

	ugp3::core::RandomizerOperator ro;
	this->listWidget_AllOperators->addItem( QString::fromStdString( ro.getName() ) );

	ugp3::core::RemovalMutationOperator rmo;
	this->listWidget_AllOperators->addItem( QString::fromStdString( rmo.getName() ) );

	ugp3::core::ReplacementMutationOperator rpmo;
	this->listWidget_AllOperators->addItem( QString::fromStdString( rpmo.getName() ) );

	ugp3::core::ScanMutationBitArrayOperator smbao;
	this->listWidget_AllOperators->addItem( QString::fromStdString( smbao.getName() ) );

	ugp3::core::ScanMutationConstantOperator smco;
	this->listWidget_AllOperators->addItem( QString::fromStdString( smco.getName() ) );

	ugp3::core::ScanMutationFloatOperator smfo;
	this->listWidget_AllOperators->addItem( QString::fromStdString( smfo.getName() ) );

	ugp3::core::ScanMutationInnerLabelOperator smilo;
	this->listWidget_AllOperators->addItem( QString::fromStdString( smilo.getName() ) );

	ugp3::core::ScanMutationIntegerOperator smio;
	this->listWidget_AllOperators->addItem( QString::fromStdString( smio.getName() ) );

	ugp3::core::SimpleDifferentialOperator sdo;
	this->listWidget_AllOperators->addItem( QString::fromStdString( sdo.getName() ) );

	ugp3::core::SingleParameterAlterationMutationOperator spamo;
	this->listWidget_AllOperators->addItem( QString::fromStdString( spamo.getName() ) );

	ugp3::core::SubGraphInsertionMutationOperator sgimo;
	this->listWidget_AllOperators->addItem( QString::fromStdString( sgimo.getName() ) );

	ugp3::core::SubGraphRemovalMutationOperator sgrmo;
	this->listWidget_AllOperators->addItem( QString::fromStdString( sgrmo.getName() ) );

	ugp3::core::SubGraphReplacementMutationOperator sgrpmo;
	this->listWidget_AllOperators->addItem( QString::fromStdString( sgrpmo.getName() ) );

	ugp3::core::SwapMutationOperator smo;
	this->listWidget_AllOperators->addItem( QString::fromStdString( smo.getName() ) );

	ugp3::core::TwoPointCrossoverOperator tpco;
	this->listWidget_AllOperators->addItem( QString::fromStdString( tpco.getName() ) );

	ugp3::core::TwoPointImpreciseCrossoverOperator tpico;
	this->listWidget_AllOperators->addItem( QString::fromStdString( tpico.getName() ) );

	// sort items
	this->listWidget_AllOperators->sortItems(Qt::AscendingOrder);

	// associate signals with public slots
	// - Add button with function that adds operators from the list on the left to the list on the right
	connect(this->pushButton_Add, SIGNAL(clicked()), this, SLOT(addOperator()));
	// - Remove button with function that removes operators from the list on the right
	connect(this->pushButton_Remove, SIGNAL(clicked()), this, SLOT(removeOperator()));
	
	// - Ok and Cancel are already connected at the level of the ugp3-selectOperators class/ui
}

// slot to add operators (from list on the left to list on the right)
void MicroGPSelectOperators::addOperator()
{
	// if no text is highlighted on the left, do nothing
	if( this->listWidget_AllOperators->currentItem() == nullptr ) return;
	
	// otherwise, add text of current item on the left to list on the right,
	// but only if it is not already there!
	bool found = false;
	for(unsigned int r = 0; r < this->listWidget_SelectedOperators->count() && !found; r++)
	{
		if( this->listWidget_SelectedOperators->item(r)->text().compare( this->listWidget_AllOperators->currentItem()->text() ) == 0 )
			found = true;
	}

	if( found == false )
		this->listWidget_SelectedOperators->addItem( this->listWidget_AllOperators->currentItem()->text() );
	
	// sort items
	this->listWidget_SelectedOperators->sortItems(Qt::AscendingOrder);
}

// slot to remove operators
void MicroGPSelectOperators::removeOperator()
{
	// if no text is highlighted on the right, do nothing
	if( this->listWidget_SelectedOperators->currentItem() == nullptr ) return;

	// otherwise, remove current item from the list and re-sort it
	QListWidgetItem* toRemove = this->listWidget_SelectedOperators->takeItem( this->listWidget_SelectedOperators->currentRow() );
	this->listWidget_SelectedOperators->sortItems(Qt::AscendingOrder);
	
	// free memory associated to removed item
	delete(toRemove);
}
