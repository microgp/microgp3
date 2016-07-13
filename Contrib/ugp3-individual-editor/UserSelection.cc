/***********************************************************************\
|                                                                       |
| UserSelection.cc                                                   |
|                                                                       |
| This file is part of MicroGP v3 (ugp3)                                |
| http://ugp3.sourceforge.net/                                          |
|                                                                       |
| Copyright (c) 2002-2016 Giovanni Squillero                            |
|                                                                       |
|-----------------------------------------------------------------------|
|                                                                       |
| This program is free software; you can redistribute it and/or modify  |
| it under the terms of the GNU General Public License as published by  |
| the Free Software Foundation, either version 3 of the License, or (at |
| your option) any later version.                                       |
|                                                                       |
| This program is distributed in the hope that it will be useful, but   |
| WITHOUT ANY WARRANTY; without even the implied warranty of            |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      |
| General Public License for more details                               |
|                                                                       |
|***********************************************************************'
| $Revision: 647 $
| $Date: 2015-02-24 22:32:04 +0100 (Tue, 24 Feb 2015) $
\***********************************************************************/

#include "UserSelection.h"
#include "MicroGPMainWindow.h"
#include "ugp3_config.h"
#include "EvolutionaryCore.h"

// Qt inclusions
#include <QDialog>
#include <QPushButton>

using namespace std;
using namespace ugp3::core;

// KILL ME BECAUSE I AM USING GLOBAL VARIABLES INSIDE A C++ CLASS! SHAME ON ME!
// unfortunately, the function selectIndividuals cannot change any member of this class (it's const)
// so I cannot define these things as members; and I cannot pass them by reference in a signal,
// I would need to extend the comboBox class just for that, and it would not be straightforward

vector<EnhancedIndividual*>* individualsInWindow;

// arrays to store
// N comboBox, to populate with individuals inside microGpMainWindow->individuals
// N textEdit, to populate with the preview of the current individual(s)
vector<QComboBox*> comboBoxArray;
vector<QTextEdit*> textEditArray;

UserSelection::UserSelection(/*const Population& population*/)
: CandidateSelection(/*population*/)
{}

// destructor
/*
UserSelection::~UserSelection()
{
	for(unsigned int i = 0; i < comboBoxArray.size(); i++)
	{
		delete( comboBoxArray[i] );
	}
}
*/

// the only relevant function
vector<Individual*> UserSelection::selectIndividuals(const Population& population, unsigned int count, double pressionMultiplier) 
const
{
	_STACK;

	vector<Individual*> selectedIndividuals;
	vector<Individual*> populationIndividuals;

	LOG_VERBOSE	<< "userSelection : performing user selection for " << count 
		<< " individuals" << ends;

	// set up the dialog to select the individuals
	QDialog dialog;
	dialog.setWindowTitle("Select individuals");

	// set the N current individuals

	// create all the appropriate widgets
	// create the layout
	QGridLayout* layout = new QGridLayout();
	unsigned int layoutRow = 0;

	// label to explain the action required
	QLabel* label = new QLabel(QString("Select %1 individual(s) as parents for the operator.").arg(count));
	layout->addWidget( label, layoutRow, 0, 1, -1);
	layoutRow++;

	// obtain a reference to the individuals inside the window
	individualsInWindow = &this->microGpMainWindow->individuals;

	// fill the comboBoxes
	for(unsigned int i = 0; i < count; i++)
	{
		QComboBox* comboBox = new QComboBox();
		// add individuals from know MicroGPMainWindow class
		for(unsigned int j = 0; j < individualsInWindow->size(); j++)
		{
			comboBox->addItem( QString::fromStdString( individualsInWindow->at(j)->getId() ) );
		}
		layout->addWidget( comboBox, layoutRow, 0 );

		// TODO modify the layout so that the combobox is on left, but in the top row (?)
		QTextEdit* textEdit = new QTextEdit();
		layout->addWidget( textEdit, layoutRow, 1 );
		
		// set the text of the textEdit to the phenotype of the first individual
		ostringstream ss;
		ugp3::ctgraph::IdentityRelabeller relabeller;
		individualsInWindow->at(0)->getGraphContainer().writeExternalRepresentation( ss, relabeller );
		textEdit->setText( QString::fromStdString( ss.str() ) );
		
		// connect comboBox change of selection with changement of current individual and preview update
		connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreview(int)));

		// store the references to the two widgets
		comboBoxArray.push_back(comboBox);
		textEditArray.push_back(textEdit);

		layoutRow++;
	}

	// push buttons to close the dialog or abort
	QPushButton* pushButton = new QPushButton("Ok");
	layout->addWidget( pushButton, layoutRow, 2 );

	// connections of widget signals to this class' slots
	// connect pushButton to thing to close the dialog
	connect(pushButton, SIGNAL(clicked()), &dialog, SLOT(close()));

	// associate dialog to layout
	dialog.setLayout(layout);
	// execute dialog
	dialog.exec();
	
	// collect the individuals in each comboBox
	for(unsigned int i = 0; i < comboBoxArray.size(); i++)
	{
		// the index in the comboBox is the same as the index in the population
		selectedIndividuals.push_back( individualsInWindow->at( comboBoxArray[i]->currentIndex() ) );
	}

	// once the dialog is executed, recover the current individuals
	LOG_DEBUG << "Selection complete. " << selectedIndividuals.size() << " individuals selected" << ends;
	for(unsigned int i = 0; i < selectedIndividuals.size(); i++) LOG_DEBUG << "- " << *selectedIndividuals[i] << ends;

	// free everything
	delete(layout);
	delete(pushButton);
	for(unsigned int i = 0; i < comboBoxArray.size(); i++)
	{
		delete( comboBoxArray[i] );
		delete( textEditArray[i] );
	}
	comboBoxArray.clear();
	textEditArray.clear();
	individualsInWindow = nullptr;

	// return the selected individuals
	return selectedIndividuals;
}

// all functions below MUST be defined for the class to compile correctly (since it's inherited from IndividualSelection)
// but here are basically empty because they make no sense
void UserSelection::updateEndogenParameters(Population& population)
{
	unsigned int goodIndividuals = 0;
	unsigned int veryGoodIndividuals = 0;
	unsigned int badIndividuals = 0;
	
	for (unsigned int i = 0; i < population.getParameters().getActivations().getDataCount(); i++)
	{
		const Data& data = population.getParameters().getActivations().getData(i);
		veryGoodIndividuals += data.getPerformance(Performance::VeryGood);
		goodIndividuals += data.getPerformance(Performance::Good);
		badIndividuals += data.getPerformance(Performance::Bad) + data.getPerformance(Performance::VeryBad);
	}
}

void UserSelection::writeXml(std::ostream& output) const
{

}

void UserSelection::readXml(const xml::Element& element)
{

}

// slot to update the preview of the currently selected individual in a comboBox
void UserSelection::updatePreview(int index)
{
	// in fact, I am updating all textEdit, there should be a maximum of 3, so it's not a problem
	for(unsigned int i = 0; i < comboBoxArray.size(); i++)
	{
		ostringstream ss;
		ugp3::ctgraph::IdentityRelabeller relabeller;
		individualsInWindow->at( comboBoxArray[i]->currentIndex() )->getGraphContainer().writeExternalRepresentation(ss, relabeller);
		textEditArray[i]->setText( QString::fromStdString(ss.str()) );
	}
}

std::vector<CandidateSolution*> UserSelection::infinitePressureSelection(const Population& population, unsigned int count) const
{
	std::vector<CandidateSolution*> empty;
	return empty;
}


std::vector<ugp3::core::CandidateSolution*> UserSelection::customSelection(const ugp3::core::Population&, unsigned int, double) const
{
	std::vector<CandidateSolution*> empty;
	return empty;
}


