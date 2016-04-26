// the gui is separated into several classes, one per tab
// Alberto Tonda, 2012 <alberto.tonda@gmail.com>

// class definition
#include "MicroGPMainWindow.h"

// standard library
#include <iostream>
#include <sstream>

// Qt classes
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTextStream>

// Other classes
#include "XmlHighlighter.h"

// MicroGP classes
#include "CNode.h"
#include "CGraphContainer.h"
#include "Constraints.h"

// namespace
using namespace std;
using namespace Ui;

// setup the constraints tab
void MicroGPMainWindow::setupTabConstraints()
{
	// TAB: Individual Constraints
		// - connect "Load" button to slot that loads the constraints file
		connect(this->pushButton_LoadConstraints, SIGNAL(clicked()), this, SLOT(loadConstraintsFile()));
		// - connect "Save and Preview" button to slot that saves the constraints file and shows the preview
		connect(this->pushButton_PreviewConstraints, SIGNAL(clicked()), this, SLOT(previewConstraintsFile()));
		// - connect "Save" button to slot that saves the constraints file
		connect(this->pushButton_SaveConstraints, SIGNAL(clicked()), this, SLOT(saveConstraintsFile()));
		// set text editor text and text background color
		//this->textEdit->setTextColor(Qt::white);
		//this->textEdit->setTextBackgroundColor(Qt::black);
		// bind text editor widget with syntax highlighter
		XmlHighlighter* highlighter = new XmlHighlighter( this->textEdit->document());

}

// save and preview constraints file (slot to be connected to a button)
void MicroGPMainWindow::previewConstraintsFile()
{
	// check if the file has been already saved
	if( this->constraintsFile.isEmpty() )
	{
		// if not, dialog and save it 
		this->saveConstraintsFile();
	}
	else
	{
		// save it anyway
		QFile outputConstraints( this->constraintsFile );
		
		if( outputConstraints.open(QFile::WriteOnly | QFile::Text) )
		{
			// text stream towards output file
			QTextStream outStream(&outputConstraints);
			outStream << this->textEdit->toPlainText();
			outputConstraints.close();
		}
		else
		{
			// display warning
			QMessageBox::warning(this->textEdit, "Warning!", 
			"Cannot write on file \"" + this->constraintsFile + "\"");
		}
	}

	// generating a random individual is quite hard; probably the best way to go is to start by creating an instance
	// of the ugp3::constraints::Constraints class, feed it to a ugp3::ctgraph::CGraphContainer class
	// feed the latter to a ugp3::core::Individual class and print to screen...
	unique_ptr<ugp3::constraints::Constraints> constraints = ugp3::constraints::Constraints::fromFile( this->constraintsFile.toStdString() );
	
	// create CGraphContainer instance 
	ugp3::ctgraph::CGraphContainer* cgraphContainer = new ugp3::ctgraph::CGraphContainer();
	// bind it to the constraints file
	cgraphContainer->setConstrain( *constraints );
	
	// create a random graph: not all constraints might always generate valid individuals;
	// warnings are managed in the MicroGPApplication::notify() class 
	cgraphContainer->buildRandom();
	
	if( cgraphContainer->validate() == false )
	{
		throw ugp3::Exception("Failed to create random individual. If you get this message too often, check your contraints: they might contain indications that make it hard or impossible to create valid individuals. Usually, the problem arises with innerForwardLabel and innerBackwardLabel parameters.", LOCATION);
	}
		
	// the relabeller is necessary to relabel stuff
	ugp3::ctgraph::IdentityRelabeller relabeller;
	// stringstream to collect the individual
	stringstream ss ( stringstream::in | stringstream::out );
	cgraphContainer->writeExternalRepresentation( ss, relabeller );
	ss << ends;
	
	// visualize output on textBrowse widget
	this->textBrowser_Preview->setText( QString( ss.str().c_str() ) );

	// free memory
	delete(cgraphContainer);
}

// save constraints file (slot to be connected to a button) 
void MicroGPMainWindow::saveConstraintsFile()
{
	// get the file name
	QString fileName = QFileDialog::getSaveFileName(this->textEdit, tr("Save Constraints File in XML"), "", tr("XML (*.xml);;All Files (*)"));
	
	if( !fileName.isEmpty() )
	{
		// create QFile
		QFile outputConstraints(fileName);
		
		if( outputConstraints.open(QFile::WriteOnly | QFile::Text) )
		{
			// text stream towards output file
			QTextStream outStream(&outputConstraints);
			outStream << this->textEdit->toPlainText();
			outputConstraints.close();

			// save internally the name of the constraints file
			this->constraintsFile = fileName;
		}
		else
		{
			// display warning
			QMessageBox::warning(this->textEdit, "Warning!", "Cannot write on file \"" + fileName + "\"");
		}
	}
}

// load constraints file (slot to be connected to a button) 
void MicroGPMainWindow::loadConstraintsFile()
{
	// get the file name
	this->constraintsFile = QFileDialog::getOpenFileName(this->textEdit, tr("Open XML Constraints File"), "", tr("XML (*.xml);;All Files (*)"));
	
	if( !this->constraintsFile.isEmpty() )
	{
		// create QFile
		QFile inputConstraints(this->constraintsFile);
		
		// try to open it
		if (inputConstraints.open(QFile::ReadOnly | QFile::Text))
		{
			// load text in the textEdit
            		this->textEdit->setPlainText(inputConstraints.readAll());
			// also reset the text in the preview box
			this->textBrowser_Preview->setText("");
		}
		else
		{
			// display warning
			QMessageBox::warning(this->textEdit, "Warning!", "Cannot read file \"" + this->constraintsFile + "\"");
		}
	}	
}
