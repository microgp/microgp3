// class definition
#include "MicroGPMainWindow.h"

// standard library

// Qt classes
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTextStream>

// Other classes
#include "MicroGPSelectOperators.h"

// MicroGP classes
#include "Constraints.h"
#include "EvolutionaryCore.h"
#include "EnhancedPopulation.h"
#include "EnhancedPopulationParameters.h"
#include "MOPopulation.h"
#include "MOPopulationParameters.h"
#include "GroupPopulation.h"
#include "GroupPopulationParameters.h"
#include "PopulationParameters.h"

// a couple of defines, used for the operators tableWidget (maybe const string was better?)
#define COLUMN_NAME 0
#define COLUMN_VALUE 1
#define COLUMN_MINIMUM 2
#define COLUMN_MAXIMUM 3

#define GROUPS_AVERAGE_SIZE_LABEL "Groups Average Size"
#define GROUPS_AVERAGE_SIZE_ROW 1
#define GROUPS_AVERAGE_SIZE_TOOLTIP "Average size of the groups\n(used during kickstart)"
#define GROUPS_MAXIMUM_SIZE_LABEL "Groups Maximum Size"
#define GROUPS_MAXIMUM_SIZE_ROW 2
#define GROUPS_MAXIMUM_SIZE_TOOLTIP "Maximum size of groups"
#define GROUPS_MINIMUM_SIZE_LABEL "Groups Minimum Size"
#define GROUPS_MINIMUM_SIZE_ROW 0
#define GROUPS_MINIMUM_SIZE_TOOLTIP "Minimum size of groups"

#define INDEX_ENHANCED_POPULATION 1
#define INDEX_GROUP_POPULATION 3
#define INDEX_MULTIOBJECTIVE_POPULATION 2

#define INDEX_SELECTION_TOURNAMENT 1
#define INDEX_SELECTION_RANKING 2

#define ELITE_LABEL "Elite/Immortals"
#define ELITE_ROW 0
#define ELITE_TOOLTIP "If a Maximum Age is specified, the individuals will eventually disappear from the population. The specified number of individuals,\nstarting from the best one, will not age, and thus will not disappear unless better individuals are found." 
#define MAXIMUM_STEADY_STATE_GENERATIONS_LABEL "Stagnation"
#define MAXIMUM_STEADY_STATE_GENERATIONS_ROW 1
#define MAXIMUM_STEADY_STATE_GENERATIONS_TOOLTIP "Stop condition. The algorithm will stop if the best fitness value\nremains the same for the given number of generations."

// namespace
using namespace std;
using namespace Ui;

void MicroGPMainWindow::setupTabPopulation()
{
	// TAB: Population Settings
		// - connect "Load" button to slot that loads the population file
		connect(this->pushButton_LoadPopulation, SIGNAL(clicked()), this, SLOT(loadPopulationFile()));
		
		// - connect "Save" button to slot that saves the population file
		connect(this->pushButton_SavePopulation, SIGNAL(clicked()), this, SLOT(savePopulationFile()));
		
		// - connet signal of modified cell from tableWidget to management of values
		connect(this->tableWidget_Operators, SIGNAL(cellChanged(int, int)), this, SLOT(checkOperatorValues(int,int)));

		// - make combo boxes' first item not-selectable
		// -- population type combo box
		QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->comboBox_PopulationType->model());
		QModelIndex firstIndex = model->index(0, comboBox_PopulationType->modelColumn(), 
							 comboBox_PopulationType->rootModelIndex());
		QStandardItem* firstItem = model->itemFromIndex(firstIndex);
		firstItem->setSelectable(false);
		firstItem->setData(Qt::lightGray , Qt::BackgroundRole); 
		// -- selection type combo box
		model = qobject_cast<QStandardItemModel*>(this->comboBox_IndividualSelection->model());
		firstIndex = model->index(0, comboBox_IndividualSelection->modelColumn(), 
					     comboBox_IndividualSelection->rootModelIndex());
		firstItem = model->itemFromIndex(firstIndex);
		firstItem->setSelectable(false);
		firstItem->setData(Qt::lightGray , Qt::BackgroundRole); 

		// - connect population type combo box to function that changes the frames according to the type of population
		connect(this->comboBox_PopulationType, SIGNAL(currentIndexChanged(int)), this, SLOT(changePopulationSpecificFrame(int)));

		// - connect individual selection combo box to function that changes the frames for individual selection
		connect(this->comboBox_IndividualSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndividualSelectionFrame(int)));
		
		// - connect "Add" button to function that adds an operator in tableWidget_Operators
		connect(this->pushButton_OperatorsAdd, SIGNAL(clicked()), this, SLOT(addOperators()));		

		// - connect "Remove" button to function that removes a line from the tableWidget_Operators
		connect(this->pushButton_OperatorsRemove, SIGNAL(clicked()), this, SLOT(removeOperators()));
		
		// - create event filter for click on lineTextEdit
		// 	(see http://www.qtforum.org/article/18841/implement-clicked-signal-on-qlineedit.html?s=fe96d74cdc4651e172bfc398a162f13ef3446856#post72113 )
}

// load population file, modifying the frames accordingly
void MicroGPMainWindow::loadPopulationFile()
{
	this->populationFile = QFileDialog::getOpenFileName(this->textEdit, 
							tr("Open XML Population File"), "", tr("XML (*.xml);;All Files (*)"));
	
	// if a choice has been made
	if( !this->populationFile.isEmpty() )
	{
		// read xml and change all widgets' content 
		this->relaxedReadPopulationXml();
	}	
}

void MicroGPMainWindow::changePopulationSpecificFrame(int newIndex)
{
	// first remove all widgets from the current layout
	QLayoutItem* child = nullptr;
        while( (child = this->gridLayout_SpecificSettings->takeAt(0)) != nullptr )
        {
                delete( child->widget() );
                delete( child );
        }

	// check the type of population selected
	if( this->comboBox_PopulationType->itemText(newIndex).compare("Enhanced Population") == 0 )
	{
		// -- enhanced population

		// - maximumSteadyStateGenerations
		QLabel* stagnationLabel = new QLabel( QString(MAXIMUM_STEADY_STATE_GENERATIONS_LABEL) );
		stagnationLabel->setToolTip(MAXIMUM_STEADY_STATE_GENERATIONS_TOOLTIP); 
                this->gridLayout_SpecificSettings->addWidget( stagnationLabel, MAXIMUM_STEADY_STATE_GENERATIONS_ROW, 0 ) ;
                // set value and other characteristics of doubleSpinBox
                QSpinBox* spinBox = new QSpinBox();
                double spinBoxValue = QString( "0" ).toInt();
                spinBox->setValue(spinBoxValue);
                spinBox->setSingleStep(1);
		// set special value for the spinBox
		spinBox->setSpecialValueText("Off");
                // add one doubleSpinBox
                this->gridLayout_SpecificSettings->addWidget( spinBox, MAXIMUM_STEADY_STATE_GENERATIONS_ROW, 1 );
		
		// - eliteSize
		QLabel* eliteLabel = new QLabel( QString(ELITE_LABEL) );
		eliteLabel->setToolTip(ELITE_TOOLTIP);
                this->gridLayout_SpecificSettings->addWidget( eliteLabel, ELITE_ROW, 0 ) ;
                // set value and other characteristics of doubleSpinBox
                spinBox = new QSpinBox();
                spinBoxValue = QString( "5" ).toInt();
                spinBox->setValue(spinBoxValue);
                spinBox->setSingleStep(1);
                // add one doubleSpinBox
                this->gridLayout_SpecificSettings->addWidget( spinBox, ELITE_ROW, 1 );
	
	}
	else if( this->comboBox_PopulationType->itemText(newIndex).compare("Multi-Objective Population") == 0 )
	{
		// -- multi-objective population
		QLabel* nothing = new QLabel( QString("<no parameters unique to this population type>") );
		nothing->setWordWrap(true);
                this->gridLayout_SpecificSettings->addWidget( nothing, 0, 0 ) ;
	}
	else if( this->comboBox_PopulationType->itemText(newIndex).compare("Group Population <experimental>") == 0 )
	{
		// -- group population
		// size of groups (min, max, avg)
		QLabel* groupMinimumLabel = new QLabel( QString(GROUPS_MINIMUM_SIZE_LABEL) );
               	groupMinimumLabel->setToolTip(GROUPS_MINIMUM_SIZE_TOOLTIP);
                this->gridLayout_SpecificSettings->addWidget( groupMinimumLabel, GROUPS_MINIMUM_SIZE_ROW, 0 ) ;

                QSpinBox* spinBox = new QSpinBox();
                double spinBoxValue = QString( "2" ).toInt();
                spinBox->setValue(spinBoxValue);
                spinBox->setSingleStep(1);
                this->gridLayout_SpecificSettings->addWidget( spinBox, GROUPS_MINIMUM_SIZE_ROW, 1 );
                
                QLabel* groupAverageLabel = new QLabel( QString(GROUPS_AVERAGE_SIZE_LABEL) );
                groupAverageLabel->setToolTip(GROUPS_AVERAGE_SIZE_TOOLTIP);
                this->gridLayout_SpecificSettings->addWidget( groupAverageLabel, GROUPS_AVERAGE_SIZE_ROW, 0 ) ;

                spinBox = new QSpinBox();
               	spinBoxValue = QString( "3" ).toInt();
                spinBox->setValue(spinBoxValue);
                spinBox->setSingleStep(1);
                this->gridLayout_SpecificSettings->addWidget( spinBox, GROUPS_AVERAGE_SIZE_ROW, 1 );

                QLabel* groupMaximumLabel = new QLabel( QString(GROUPS_MAXIMUM_SIZE_LABEL) );
                groupMaximumLabel->setToolTip(GROUPS_MAXIMUM_SIZE_TOOLTIP);
                this->gridLayout_SpecificSettings->addWidget( groupMaximumLabel, GROUPS_MAXIMUM_SIZE_ROW, 0 ) ;

                spinBox = new QSpinBox();
               	spinBoxValue = QString( "10" ).toInt();
                spinBox->setValue(spinBoxValue);
                spinBox->setSingleStep(1);
                this->gridLayout_SpecificSettings->addWidget( spinBox, GROUPS_MAXIMUM_SIZE_ROW, 1 );
	}
}

// save current panel to file
void MicroGPMainWindow::savePopulationFile()
{
	// preliminary checks
	// is populationType set?
	if( this->comboBox_PopulationType->currentIndex() == 0 )
	{
		QMessageBox::warning(this->comboBox_IndividualSelection, "Warning!", "The type of population is not initialized. Please, specify the type of population before saving.");
		return;
	}

	// is selectionType set?
	if( this->comboBox_IndividualSelection->currentIndex() == 0 )
	{
		int choice = QMessageBox::question(this->comboBox_IndividualSelection, "Warning!", "The type of individual selection is not initialized. Thus, the individual selection information will not reported in the saved file. Continue?", QMessageBox::Yes, QMessageBox::No);
		if( choice == QMessageBox::No ) return;
	}
	
	// what if there are no genetic operators?
	if( this->tableWidget_Operators->rowCount() == 0 )
	{
		int choice = QMessageBox::question(this->tableWidget_Operators, "Warning!", "No genetic operators specified. Without genetic operators, individuals cannot reproduce, so the evolution will not work at all. Are you sure you want to continue?", QMessageBox::Yes, QMessageBox::No);
		if( choice == QMessageBox::No ) return;

	}
	
	// and what about the constraints?
	if( this->lineEdit_ConstraintsFile->text().length() == 0 )
	{
		int choice = QMessageBox::question(this->comboBox_IndividualSelection, "Warning!", "No constraints file specified. Without constraints file, the evolution will not start at all. Are you sure you want to continue?", QMessageBox::Yes, QMessageBox::No);
		if( choice == QMessageBox::No ) return;

	}

	// and what about the evaluator
	if( this->lineEdit_ConstraintsFile->text().length() == 0 )
	{
		int choice = QMessageBox::question(this->comboBox_IndividualSelection, "Warning!", "No evaluator specified. Without evaluator, the evolution will crash when the first individual should be evaluated. Are you sure you want to continue?", QMessageBox::Yes, QMessageBox::No);
		if( choice == QMessageBox::No ) return;
	}

	// read the widgets and write in file
	QString fileName = 	QFileDialog::getSaveFileName(this->textEdit, 
				tr("Save Population File in XML"), "", tr("XML (*.xml);;All Files (*)"));

        // if a choice has not been made, return 
        if( fileName.isEmpty() ) return;
	
	// associate current population filename with filename
	this->populationFile = fileName;

	// create QFile and start writing in it, by examining the widgets
	QFile outputPopulation(fileName);
	if( !outputPopulation.open(QFile::WriteOnly | QFile::Text) )
	{
		// display warning and return
		QMessageBox::warning(this->textEdit, "Warning!", "Cannot write on file \"" + fileName + "\"");
		return;
	}

	// create text stream
	QTextStream outStream(&outputPopulation);

	// XML header
	outStream << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << endl; 	
	
	// read population type combo box
	string populationType;
	if( this->comboBox_PopulationType->currentIndex() == INDEX_ENHANCED_POPULATION )
		populationType = ugp3::core::EnhancedPopulationParameters::XML_SCHEMA_TYPE;
	else if( this->comboBox_PopulationType->currentIndex() == INDEX_GROUP_POPULATION )
		populationType = ugp3::core::GroupPopulationParameters::XML_SCHEMA_TYPE;
	else if( this->comboBox_PopulationType->currentIndex() == INDEX_MULTIOBJECTIVE_POPULATION )
		populationType = ugp3::core::MOPopulationParameters::XML_SCHEMA_TYPE;
	else	populationType = "unknown";

	outStream 	
	<< "<" << QString::fromStdString( ugp3::core::PopulationParameters::XML_NAME ) 
	<< " type=\"" << QString( populationType.c_str() ) << "\">" << endl;
	
	// parameters common to every population type
	// constraints
	outStream 	
	<< "\t<!-- " << this->lineEdit_ConstraintsFile->toolTip() << " -->" << endl
	<< "\t<" << QString::fromStdString( ugp3::constraints::Constraints::XML_NAME ) 
	<< " value=\"" << this->lineEdit_ConstraintsFile->displayText() << "\"/>" << endl;

	// mu
	outStream 	
	<< "\t<!-- " << this->spinBox_Mu->toolTip() << " -->" << endl
	<< "\t<" << QString::fromStdString( ugp3::core::PopulationParameters::XML_CHILDELEMENT_MU )
	<< " value=\"" << this->spinBox_Mu->cleanText().toInt() << "\"/>" << endl;

	// nu
	outStream 	
	<< "\t<!-- " << this->spinBox_Nu->toolTip() << " -->" << endl
	<< "\t<" << QString::fromStdString( ugp3::core::PopulationParameters::XML_CHILDELEMENT_NU )
	<< " value=\"" << this->spinBox_Nu->cleanText().toInt() << "\"/>" << endl;

	// lambda
	outStream
	<< "\t<!-- " << this->spinBox_Lambda->toolTip() << " -->" << endl
	<< "\t<" << QString::fromStdString( ugp3::core::PopulationParameters::XML_CHILDELEMENT_LAMBDA )
	<< " value=\"" << this->spinBox_Lambda->cleanText().toInt() << "\"/>" << endl;

	// sigma 
	outStream	
	<< "\t<!-- " << this->doubleSpinBox_Sigma->toolTip() << " -->" << endl
	<< "\t<" << QString::fromStdString( ugp3::core::PopulationParameters::XML_CHILDELEMENT_SIGMA )
	<< " value=\"" << this->doubleSpinBox_Sigma->cleanText().toDouble() << "\"/>" << endl;

	// inertia 
	outStream	
	<< "\t<!-- " << this->doubleSpinBox_Inertia->toolTip() << " -->" << endl
	<< "\t<" << QString::fromStdString( ugp3::core::PopulationParameters::XML_CHILDELEMENT_INERTIA )
	<< " value=\"" << this->doubleSpinBox_Inertia->cleanText().toDouble() << "\"/>" << endl;

	// clone-scaling factor
	outStream
	<< "\t<!-- " << this->doubleSpinBox_CloneScalingFactor->toolTip() << " -->" << endl
	<< "\t<" << QString::fromStdString( ugp3::core::EnhancedPopulationParameters::XML_CHILDELEMENT_CLONESCALINGFACTOR )
	<< " value=\"" << this->doubleSpinBox_CloneScalingFactor->cleanText().toDouble() << "\"/>" << endl;

	// maximum age
	outStream
	<< "\t<!-- " << this->spinBox_MaximumAge->toolTip() << " -->" << endl
	<< "\t<" << QString::fromStdString( ugp3::core::EnhancedPopulationParameters::XML_CHILDELEMENT_MAXIMUMAGE )
	<< " value=\"" << this->spinBox_MaximumAge->cleanText().toInt() << "\"/>" << endl;

	// discard fitness after generation
	outStream
	<< "\t<!-- " << this->checkBox_DiscardFitness->toolTip() << " -->" << endl
	<< "\t<" << QString::fromStdString( ugp3::core::PopulationParameters::XML_CHILDELEMENT_INVALIDATEFITNESS )
	<< " value=\"" << this->checkBox_DiscardFitness->isChecked() << "\"/>" << endl;
	
	// stop conditions! they are all optional, so they are included only if they are set to a value
	// maximum generations
	if( this->spinBox_MaximumGenerations->value() > 0 )
	{
		outStream 
		<< "\t<!-- " << this->spinBox_MaximumGenerations->toolTip() << " -->" << endl
		<< "\t<" << QString::fromStdString( ugp3::core::PopulationParameters::XML_CHILDELEMENT_MAXIMUMGENERATIONS )
		<< " value=\"" << this->spinBox_MaximumGenerations->value() << "\"/>" << endl;
	}
	
	// maximum evaluations
	if( this->spinBox_MaximumEvaluations->value() > 0 )
	{
		outStream 
		<< "\t<!-- " << this->spinBox_MaximumEvaluations->toolTip() << " -->" << endl
		<< "\t<" << QString::fromStdString( ugp3::core::PopulationParameters::XML_CHILDELEMENT_MAXIMUMEVALUATIONS )
		<< " value=\"" << this->spinBox_MaximumEvaluations->value() << "\"/>" << endl;
	}

	// maximum time
	if( this->timeEdit_MaximumTime->time() != this->timeEdit_MaximumTime->minimumTime() )
	{
		outStream 
		<< "\t<!-- " << this->timeEdit_MaximumTime->toolTip() << " -->" << endl
		<< "\t<" << QString::fromStdString( ugp3::core::EnhancedPopulationParameters::XML_CHILDELEMENT_MAXTIME )
		<< " " << QString::fromStdString( ugp3::core::EnhancedPopulationParameters::XML_ATTRIBUTE_HOURS) 
		<< "=\"" << this->timeEdit_MaximumTime->time().hour()
		<< "\" " << QString::fromStdString( ugp3::core::EnhancedPopulationParameters::XML_ATTRIBUTE_MINUTES) 
		<< "=\"" << this->timeEdit_MaximumTime->time().minute()
		<< "\" " << QString::fromStdString( ugp3::core::EnhancedPopulationParameters::XML_ATTRIBUTE_SECONDS) 
		<< "=\"" << this->timeEdit_MaximumTime->time().second() << "\" />" << endl;
	}

	// population-specific parts!
	if( populationType == ugp3::core::EnhancedPopulationParameters::XML_SCHEMA_TYPE )
	{
		QDoubleSpinBox* doubleSpinBox;

		// elite size
		doubleSpinBox = (QDoubleSpinBox*) this->gridLayout_SpecificSettings->itemAtPosition(ELITE_ROW, 1)->widget();
		outStream 
		<< "\t<!-- " << QString( ELITE_TOOLTIP ) << " -->" << endl
		<< "\t<" << QString::fromStdString( ugp3::core::EnhancedPopulationParameters::XML_CHILDELEMENT_ELITE )
		<< " value=\"" << doubleSpinBox->value() << "\" />" << endl;
		
		// maximum steady state generations
		doubleSpinBox = (QDoubleSpinBox*) this->gridLayout_SpecificSettings->itemAtPosition(MAXIMUM_STEADY_STATE_GENERATIONS_ROW, 1)->widget();
		// report the stop condition ONLY if the value is > 0
		if( doubleSpinBox->value() > 0 )
		outStream
                << "\t<!-- " << QString( MAXIMUM_STEADY_STATE_GENERATIONS_TOOLTIP ) << " -->" << endl
                << "\t<" << QString::fromStdString( ugp3::core::EnhancedPopulationParameters::XML_CHILDELEMENT_STEADYSTATEGENERATIONS )
                << " value=\"" << doubleSpinBox->value() << "\" />" << endl;
	}
	else if( populationType == ugp3::core::GroupPopulationParameters::XML_SCHEMA_TYPE )
	{
		QSpinBox* spinBox;

		// TODO: modify here, the scope for the groups is different, something like
		// <groupSize average="" minimum="" maximum=""/>
		outStream << "\t<groupSize>";
	}
	
	// evaluator settings!
	// number of fitness values is strictly connected, but not part of the scope
	outStream
	<< "\t<!-- " << this->spinBox_FitnessParameters->toolTip() << " -->" << endl
	<< "\t<" << QString::fromStdString( ugp3::core::PopulationParameters::XML_CHILDELEMENT_FITNESSPARAMETERS )
	<< " value=\"" << this->spinBox_FitnessParameters->value() << "\"/>" << endl;
	
	// open evaluation scope
	outStream << "\t<!-- Options specific to individual evaluation -->" << endl;
	outStream << "\t<evaluation>" << endl;
	// evaluator path
	outStream
	<< "\t\t<" << QString::fromStdString( ugp3::core::Evaluator::XML_CHILDELEMENT_EVALUATORPATHNAME )
	<< " value=\"" << this->lineEdit_EvaluatorPath->text() << "\"/>" << endl;
	// name of the individuals
	outStream
	<< "\t\t<" << QString::fromStdString( ugp3::core::Evaluator::XML_CHILDELEMENT_EVALUATORINPUTPATHNAME )
	<< " value=\"" << this->lineEdit_EvaluatorInputPathName->text() << "\"/>" << endl;
	// name of the fitness file
	outStream
	<< "\t\t<" << QString::fromStdString( ugp3::core::Evaluator::XML_CHILDELEMENT_EVALUATOROUTPUTPATHNAME )
	<< " value=\"" << this->lineEdit_EvaluatorOutputPathName->text() << "\"/>" << endl;
	// concurrent evaluations
	outStream
	<< "\t\t<" << QString::fromStdString( ugp3::core::Evaluator::XML_CHILDELEMENT_CONCURRENTEVALUATIONS )
	<< " value=\"" << this->spinBox_ConcurrentEvaluations->value() << "\"/>" << endl;
	// remove temporary individual files
	outStream
	<< "\t\t<" << QString::fromStdString( ugp3::core::Evaluator::XML_CHILDELEMENT_REMOVETEMPFILES )
	<< " value=\"" << this->checkBox_RemoveTemporaryFiles->isChecked() << "\"/>" << endl;
	// close evaluator tag
	outStream << "\t</evaluation>" << endl;
	
	// evaluator
	outStream << "\t<!-- " << this->label_IndividualSelector->toolTip() << " -->" << endl;
	// first, the type
	outStream << "\t<" << QString::fromStdString( ugp3::core::IndividualSelection::XML_NAME ) << " type=\"";
	if( this->comboBox_IndividualSelection->currentIndex() == INDEX_SELECTION_TOURNAMENT )
		outStream << QString::fromStdString( ugp3::core::TournamentSelectionWithFitnessHole::XML_SCHEMA_TYPE );
	else if( this->comboBox_IndividualSelection->currentIndex() == INDEX_SELECTION_RANKING )
		outStream << QString::fromStdString( ugp3::core::RankingSelection::XML_SCHEMA_TYPE );
	else	
		outStream << "unknownSelection -> modify this!";
	
	outStream << "\"";
	
	// then, this is tricky: for each attribute, there's a label with the name on the same row as the spinBox with the value
	// so, the idea is to iterate over each line of the gridLayout and add the attributes one by one
	for(unsigned int r = 0; r < this->gridLayout_IndividualSelection->rowCount(); r++)
	{
		// label
		QLabel* label = (QLabel*) this->gridLayout_IndividualSelection->itemAtPosition(r, 0)->widget();
		outStream << " " << label->text();
		
		// spinBox
		QDoubleSpinBox* doubleSpinBox = (QDoubleSpinBox*) this->gridLayout_IndividualSelection->itemAtPosition(r, 1)->widget();
		outStream << "=\"" << doubleSpinBox->value() << "\""; 
	}
	outStream << " />" << endl;
	
	// finally, the evolutionary operators!
	// open operator statistics tag
	outStream << "\t<!-- " << this->tableWidget_Operators->toolTip() << " -->" << endl;
	outStream << "\t<" << QString::fromStdString( ugp3::core::Statistics<ugp3::core::GeneticOperator>::XML_NAME ) << ">" << endl;

	// adjust activation probabilities before starting
	this->tableWidget_Operators->setCurrentItem(nullptr);
	this->adjustOperatorTableValues();
	
	// create a tag for each operator in the table and fill it with corresponding data
	//	<operator ref="onePointImpreciseCrossover">
	//		<weight current="1" minimum="0" maximum="1"/>
	//	</operator>
	for(unsigned int r = 0; r < this->tableWidget_Operators->rowCount(); r++)
	{
		// open operator tag
		outStream
		<< "\t\t<" << QString::fromStdString( ugp3::core::Data<ugp3::core::GeneticOperator>::XML_NAME )
		<< " " << QString::fromStdString( ugp3::core::Data<ugp3::core::GeneticOperator>::XML_ATTRIBUTE_REF )
		<< "=\"" << this->tableWidget_Operators->item(r, COLUMN_NAME)->text()
		<< "\">" << endl;

		// open weight tag
		outStream
		<< "\t\t\t<" << QString::fromStdString( ugp3::core::Data<ugp3::core::GeneticOperator>::XML_CHILD_ELEMENT_WEIGHT );
		
		// read weights from columns
		outStream 
		<< " " << QString::fromStdString( ugp3::core::Data<ugp3::core::GeneticOperator>::XML_ATTRIBUTE_CURRENT )
		<< "=\"" << this->tableWidget_Operators->item(r, COLUMN_VALUE)->text().toDouble()
		<< "\" " << QString::fromStdString( ugp3::core::Data<ugp3::core::GeneticOperator>::XML_ATTRIBUTE_MAXIMUM )
		<< "=\"" << this->tableWidget_Operators->item(r, COLUMN_MAXIMUM)->text().toDouble()
		<< "\" " << QString::fromStdString( ugp3::core::Data<ugp3::core::GeneticOperator>::XML_ATTRIBUTE_MINIMUM )
		<< "=\"" << this->tableWidget_Operators->item(r, COLUMN_MINIMUM)->text().toDouble()
		<< "\"";

		// close weight tag
		outStream
		<< " />" << endl;		

		// close operator tag
		outStream
		<< "\t\t</" << QString::fromStdString( ugp3::core::Data<ugp3::core::GeneticOperator>::XML_NAME ) << ">" << endl;
	}

	// close operator statistics tag
	outStream << "\t</" << QString::fromStdString( ugp3::core::Statistics<ugp3::core::GeneticOperator>::XML_NAME ) << ">" << endl;

	// close parameters tag
	outStream << "</parameters>" << endl;
	
	// end of output
	outputPopulation.close();

}

void MicroGPMainWindow::relaxedReadPopulationXml()
{
	// re-implement here all the ugp3::core::PopulationParameters::readXml method, but do not throw exceptions for missing elements;
	// instead, just collect and print out some warnings while widget content is changed
	QString warnings;
	xml::Document xmlPopulation; 
	xmlPopulation.LoadFile( this->populationFile.toStdString() );
	
	// first find out the type of population
	xml::Element& element = *xmlPopulation.RootElement();
	if(element.ValueStr() != ugp3::core::PopulationParameters::XML_NAME)
	{
		throw xml::SchemaException("expected element 'parameters' (found '" + element.ValueStr() + "').", LOCATION);
	}
	
	// adjust the combo box accordingly
	const string& type = xml::Utility::attributeValueToString(element, ugp3::core::PopulationParameters::XML_ATTRIBUTE_TYPE);
	if (type == ugp3::core::EnhancedPopulationParameters::XML_SCHEMA_TYPE)
	{
		this->comboBox_PopulationType->setCurrentIndex(1);
	}
	else if (type == ugp3::core::MOPopulationParameters::XML_SCHEMA_TYPE)
	{
		this->comboBox_PopulationType->setCurrentIndex(2);
	}
	else if (type == ugp3::core::GroupPopulationParameters::XML_SCHEMA_TYPE)
	{
		this->comboBox_PopulationType->setCurrentIndex(3);
	}
	else
	{
		throw xml::SchemaException("Population type \"" + type + "\" not recognized.", LOCATION);
		this->comboBox_PopulationType->setCurrentIndex(0);
	}
	
	// fill in all the other parts
	const xml::Element* childElement = element.FirstChildElement();
	
	while( childElement != nullptr )
	{
		const string& elementName = childElement->ValueStr();
		if(elementName == ugp3::core::PopulationParameters::XML_CHILDELEMENT_MU)
		{
		    try
		    {
			this->spinBox_Mu->setValue(xml::Utility::attributeValueToUInt(*childElement, ugp3::core::PopulationParameters::XML_ATTRIBUTE_VALUE));
		    }
		    catch(const exception& e)
		    {
			throw e;
		    }
		}
		else if(elementName == ugp3::core::PopulationParameters::XML_CHILDELEMENT_LAMBDA)
		{
		    try
		    {
			this->spinBox_Lambda->setValue(xml::Utility::attributeValueToUInt(*childElement, ugp3::core::PopulationParameters::XML_ATTRIBUTE_VALUE));
		    }
		    catch(const exception& e)
		    {
			throw e;
		    }
		}
		else if(elementName == ugp3::core::PopulationParameters::XML_CHILDELEMENT_NU)
		{
		    try
		    {
			this->spinBox_Nu->setValue(xml::Utility::attributeValueToUInt(*childElement, ugp3::core::PopulationParameters::XML_ATTRIBUTE_VALUE));
		    }
		    catch(const exception& e)
		    {
			throw e;
		    }
		}
		/*else if(elementName == ugp3::core::PopulationParameters::XML_CHILDELEMENT_ENNE)
		{
		    try
		    {
			this->setEnne(xml::Utility::attributeValueToUInt(*childElement, XML_ATTRIBUTE_VALUE));
		    }
		    catch(const exception& e)
		    {
			LOG_ERROR << "While parsing population data: " << e.what() << ends;
			throw e;
		    }
		}
		*/
		else if(elementName == ugp3::core::PopulationParameters::XML_CHILDELEMENT_INERTIA)
		{
		    try
		    {
			    this->doubleSpinBox_Inertia->setValue(xml::Utility::attributeValueToDouble(*childElement, ugp3::core::PopulationParameters::XML_ATTRIBUTE_VALUE));
		    }
		    catch(const exception& e)
		    {
			throw e;
		    }
		}
		/*
		else if(elementName == ugp3::core::PopulationParameters::XML_CHILDELEMENT_DIFFEVOCP)
		{
		    try
		    {
		    this->setDiffEvoCP(xml::Utility::attributeValueToDouble(*childElement, ugp3::core::PopulationParameters::XML_ATTRIBUTE_VALUE));
		    }
		    catch(const exception& e)
		    {
			LOG_ERROR << "While parsing population data: " << e.what() << ends;
			throw e;
		    }
		}
		else if(elementName == ugp3::core::PopulationParameters::XML_CHILDELEMENT_DIFFEVODW)
		{
		    try
		    {
		    this->setDiffEvoDW(xml::Utility::attributeValueToDouble(*childElement, ugp3::core::PopulationParameters::XML_ATTRIBUTE_VALUE));
		    }
		    catch(const exception& e)
		    {
			throw e;
		    }
		}
		*/
		else if(elementName == ugp3::core::PopulationParameters::XML_CHILDELEMENT_MAXIMUMAGE)
		{
		    try
		    {
			this->spinBox_MaximumAge->setValue(xml::Utility::attributeValueToUInt(*childElement, ugp3::core::PopulationParameters::XML_ATTRIBUTE_VALUE));
		    }
		    catch(const exception& e)
		    {
			throw e;
		    }

		}
		else if(elementName == ugp3::core::PopulationParameters::XML_CHILDELEMENT_FITNESSPARAMETERS)
		{
		    try
		    {
			this->spinBox_FitnessParameters->setValue(xml::Utility::attributeValueToUInt(*childElement, ugp3::core::PopulationParameters::XML_ATTRIBUTE_VALUE));
		    }
		    catch(const exception& e)
		    {
			throw e;
		    }
		}
		else if(elementName == ugp3::core::PopulationParameters::XML_CHILDELEMENT_SIGMA)
		{
		    try
		    {
			this->doubleSpinBox_Sigma->setValue(xml::Utility::attributeValueToDouble(*childElement, ugp3::core::PopulationParameters::XML_ATTRIBUTE_VALUE));
		    }
		    catch(const exception& e)
		    {
			throw e;
		    }
		}
		else if(elementName == ugp3::core::PopulationParameters::XML_CHILDELEMENT_MAXIMUMGENERATIONS)
		{
		    try
		    {
			this->spinBox_MaximumGenerations->setValue(xml::Utility::attributeValueToUInt(*childElement, ugp3::core::PopulationParameters::XML_ATTRIBUTE_VALUE));
		    }
		    catch(const exception& e)
		    {
			throw e;
		    }
		}
		else if(elementName == ugp3::core::PopulationParameters::XML_CHILDELEMENT_MAXIMUMEVALUATIONS)
		{
		    try
		    {
			this->spinBox_MaximumEvaluations->setValue(xml::Utility::attributeValueToULong(*childElement, ugp3::core::PopulationParameters::XML_ATTRIBUTE_VALUE));
		    }
		    catch(const exception& e)
		    {
			throw e;
		    }
		}
		else if(elementName == ugp3::core::FitnessEvaluator::XML_NAME)
		{
		    // read all details concerning the evaluator
		    try
		    {
			const xml::Element* evaluatorElement = childElement->FirstChildElement();
			
			while( evaluatorElement != nullptr )
			{
				string evaluatorElementName = evaluatorElement->ValueStr();			

				if(evaluatorElementName == ugp3::core::Evaluator::XML_CHILDELEMENT_CONCURRENTEVALUATIONS/*"concurrentEvaluations"*/)
				{
				    	this->spinBox_ConcurrentEvaluations->setValue(xml::Utility::attributeValueToUInt(*evaluatorElement, "value"));
				}
				else if(evaluatorElementName == ugp3::core::Evaluator::XML_CHILDELEMENT_REMOVETEMPFILES/*"removeTempFiles"*/)
				{
					if( xml::Utility::attributeValueToBool(*evaluatorElement, "value") )
						this->checkBox_RemoveTemporaryFiles->setCheckState( Qt::Checked );
					else
						this->checkBox_RemoveTemporaryFiles->setCheckState( Qt::Unchecked );
				}
				else if(evaluatorElementName == ugp3::core::Evaluator::XML_CHILDELEMENT_EVALUATORPATHNAME/*"evaluatorPathName"*/)
				{
					this->lineEdit_EvaluatorPath->setText(xml::Utility::attributeValueToString(*evaluatorElement, "value").c_str());
				}
				else if(evaluatorElementName == ugp3::core::Evaluator::XML_CHILDELEMENT_EVALUATORINPUTPATHNAME/*"evaluatorInputPathName"*/)
				{
					this->lineEdit_EvaluatorInputPathName->setText(xml::Utility::attributeValueToString(*evaluatorElement, "value").c_str() );
				}
				else if(evaluatorElementName == ugp3::core::Evaluator::XML_CHILDELEMENT_EVALUATOROUTPUTPATHNAME/*"evaluatorOutputPathName"*/)
				{
					this->lineEdit_EvaluatorOutputPathName->setText(xml::Utility::attributeValueToString(*evaluatorElement, "value").c_str() );
				}

				evaluatorElement = evaluatorElement->NextSiblingElement();
			}
			
		    }
		    catch(const exception& e)
		    {
			throw e;
		    }
		}
		else if(elementName == ugp3::core::PopulationParameters::XML_CHILDELEMENT_INVALIDATEFITNESS)
		{
		    try
		    {
			bool checked = xml::Utility::attributeValueToBool(*childElement, ugp3::core::PopulationParameters::XML_ATTRIBUTE_VALUE);
			if( checked )
		    		this->checkBox_DiscardFitness->setCheckState( Qt::Checked );
			else
				this->checkBox_DiscardFitness->setCheckState( Qt::Unchecked );
			
		    }
		    catch(const exception& e)
		    {
			throw e;
		    }
		}
		else if(elementName == ugp3::constraints::Constraints::XML_NAME)
		{
		    const string& constraintsFile =
			xml::Utility::attributeValueToString(*childElement, ugp3::core::PopulationParameters::XML_ATTRIBUTE_VALUE);

			// modify here to add complete path
			// int dirPosition = this->populationFile.lastIndexOf("/");
			// copy all the rest...
			this->lineEdit_ConstraintsFile->setText( constraintsFile.c_str() );
		}
		else if(elementName == ugp3::core::Statistics<ugp3::core::GeneticOperator>::XML_NAME)
		{
			// clear current operators
			for(int i = this->tableWidget_Operators->rowCount() - 1; i >= 0; --i)
				this->tableWidget_Operators->removeRow(i);

			// collect all data related to evolutionary operators
			const xml::Element* operatorElement = childElement->FirstChildElement();
			int operatorIndex = 0;
			while( operatorElement != nullptr )
			{
				// check if needed to extend the table
				this->tableWidget_Operators->insertRow( operatorIndex );

				// collect name
				QString operatorName = ( xml::Utility::attributeValueToString(*operatorElement, 
							ugp3::core::Data<ugp3::core::GeneticOperator>::XML_ATTRIBUTE_REF).c_str() );
				
				// put into tableWidget (not editable)
				QTableWidgetItem* tableWidgetItem = new QTableWidgetItem( operatorName );
				tableWidgetItem->setFlags(tableWidgetItem->flags() ^ Qt::ItemIsEditable);
				this->tableWidget_Operators->setItem(operatorIndex, COLUMN_NAME, tableWidgetItem);
				
				// child
				const xml::Element* weightElement = operatorElement->FirstChildElement();
				
				// collect minimum and put into tableWidget
				QString operatorMin = ( xml::Utility::attributeValueToString(*weightElement, 
							ugp3::core::Data<ugp3::core::GeneticOperator>::XML_ATTRIBUTE_MINIMUM).c_str() );
				tableWidgetItem = new QTableWidgetItem( operatorMin );
				this->tableWidget_Operators->setItem(operatorIndex, COLUMN_MINIMUM, tableWidgetItem);
			
				// collect current value and put into tableWidget
				QString operatorCurrent = ( xml::Utility::attributeValueToString(*weightElement, 
							ugp3::core::Data<ugp3::core::GeneticOperator>::XML_ATTRIBUTE_CURRENT).c_str() );
				tableWidgetItem = new QTableWidgetItem( operatorCurrent );
				this->tableWidget_Operators->setItem(operatorIndex, COLUMN_VALUE, tableWidgetItem);

				// collect maximum and put into tableWidget
				QString operatorMax = ( xml::Utility::attributeValueToString(*weightElement, 
							ugp3::core::Data<ugp3::core::GeneticOperator>::XML_ATTRIBUTE_MAXIMUM).c_str() );
				tableWidgetItem = new QTableWidgetItem( operatorMax );
				this->tableWidget_Operators->setItem(operatorIndex, COLUMN_MAXIMUM, tableWidgetItem);

				// next element
				operatorElement = operatorElement->NextSiblingElement();
				operatorIndex++;
			}
			
			//cout << "About to normalize stuff..." << endl;
			// normalize current value of the operators
			this->adjustOperatorTableValues();

		}
		else if(elementName == ugp3::core::IndividualSelection::XML_NAME)
		{
			// remove all the labels and spinBoxes associated with the layout
			QLayoutItem* child = nullptr;
			while( (child = this->gridLayout_IndividualSelection->takeAt(0)) != nullptr )
			{
				delete( child->widget() );
				delete( child );
			}
			
			// check type and set comboBox accordingly
			string type = xml::Utility::attributeValueToString(*childElement,
                                                        ugp3::core::IndividualSelection::XML_ATTRIBUTE_TYPE);
			
			if( type == ugp3::core::TournamentSelectionWithFitnessHole::XML_SCHEMA_TYPE )
				this->comboBox_IndividualSelection->setCurrentIndex(INDEX_SELECTION_TOURNAMENT);
			else if( type == ugp3::core::RankingSelection::XML_SCHEMA_TYPE )
				this->comboBox_IndividualSelection->setCurrentIndex(INDEX_SELECTION_RANKING);
			else
				throw ugp3::Exception("Individual Selection \"" + type + "\" not recognized." , LOCATION);

			// iterate over the attributes and create one spinBox for each one
			const xml::Attribute* individualSelectionAttribute = childElement->FirstAttribute();
			int gridRow = 0;

			while( individualSelectionAttribute != nullptr )
			{
				if( ugp3::core::IndividualSelection::XML_ATTRIBUTE_TYPE.compare( individualSelectionAttribute->Name() ) != 0 )
				{
					// add one label
					this->gridLayout_IndividualSelection->addWidget( 
						new QLabel( QString(individualSelectionAttribute->Name())), gridRow, 0 
					);
					
					// set value and other characteristics of doubleSpinBox
					QDoubleSpinBox* spinBox = new QDoubleSpinBox();
					double spinBoxValue = QString( individualSelectionAttribute->Value() ).toDouble();
					spinBox->setValue(spinBoxValue);
					spinBox->setSingleStep(0.01);

					// add one doubleSpinBox
					this->gridLayout_IndividualSelection->addWidget(
						spinBox, gridRow, 1
					);
					
					// increase row
					gridRow++;
				}

				individualSelectionAttribute = individualSelectionAttribute->Next();
			}
			
		}
		// enhanced population specific parameters
		else if( type == ugp3::core::EnhancedPopulationParameters::XML_SCHEMA_TYPE && 
			elementName == ugp3::core::EnhancedPopulationParameters::XML_CHILDELEMENT_ELITE )
		{
			// read element and set value to appropriate spinBox
			unsigned int value = xml::Utility::attributeValueToUInt(*childElement, "value");
			QWidget* spinBox = this->gridLayout_SpecificSettings->itemAtPosition(ELITE_ROW, 1)->widget();
			((QSpinBox *) spinBox)->setValue(value);
		}
		else if( type == ugp3::core::EnhancedPopulationParameters::XML_SCHEMA_TYPE && 
			elementName == ugp3::core::EnhancedPopulationParameters::XML_CHILDELEMENT_STEADYSTATEGENERATIONS )
		{
			// read element and set value to appropriate spinBox
			unsigned int value = xml::Utility::attributeValueToUInt(*childElement, "value");
			QWidget* spinBox = this->gridLayout_SpecificSettings->itemAtPosition(MAXIMUM_STEADY_STATE_GENERATIONS_ROW, 1)->widget();
			((QSpinBox *) spinBox)->setValue(value);
		}

		childElement = childElement->NextSiblingElement();
	    }
	// also, parse the parts specific to population type
}

// do some checks on the values whenever the tableWidget related to operators is changed
void MicroGPMainWindow::checkOperatorValues(int row, int column)
{
	// columns
	// 0 = name (unmodifiable)
	// 1 = min
	// 2 = current
	// 3 = max

	if( column == COLUMN_MINIMUM )
	{
		// minimum: convert string to int, then 0 < check < 1; if not, set to the closest
		float minimumValue = -1;
		QTableWidgetItem* minimum = this->tableWidget_Operators->item(row, column);
		minimumValue = minimum->text().toFloat();

		// check if the maximum exists
		float maximumValue = 0.99;
		QTableWidgetItem* maximum = this->tableWidget_Operators->item(row, COLUMN_MAXIMUM);   
		if( maximum != nullptr ) maximumValue = maximum->text().toFloat();
			
		if( minimumValue < 0 )
			minimum->setText("0");
		else if( minimumValue >= maximumValue )
			minimum->setText( QString("%1").arg(maximumValue) );

		// eventually modify the current value
		minimumValue = minimum->text().toFloat();
		QTableWidgetItem* current = this->tableWidget_Operators->item(row, COLUMN_VALUE);
		if( current != nullptr )
		{
			float currentValue = current->text().toFloat();
			if( currentValue < minimumValue) current->setText( QString("%1").arg(minimumValue) );
		} 
		
	}
	else if( column == COLUMN_MAXIMUM )
	{
		// maximum: convert string to int, then check <= 1; if not, set to 1
		float maximumValue = 2;
		QTableWidgetItem* maximum = this->tableWidget_Operators->item(row, column);
		maximumValue = maximum->text().toFloat();
		
		// check if the minimum exists
		float minimumValue = 0.01;
		QTableWidgetItem* minimum = this->tableWidget_Operators->item(row, COLUMN_MINIMUM);   
		if( minimum != nullptr ) minimumValue = minimum->text().toFloat();
	
		// adjust maximum value
		if( maximumValue < minimumValue )
			maximum->setText( QString("%1").arg(minimumValue) );
		else if( maximumValue > 1 )
			maximum->setText("1");
		
		// eventually modify the current value
		maximumValue = maximum->text().toFloat();
		QTableWidgetItem* current = this->tableWidget_Operators->item(row, COLUMN_VALUE);
		if( current != nullptr )
		{
			float currentValue = current->text().toFloat();
			if( currentValue > maximumValue) current->setText( QString("%1").arg(maximumValue) );
		} 
		
	}
	else if( column == COLUMN_VALUE )
	{
		// for this to work, all cells on the row must be set! also, I am interested only in
		// modifications to the current value of the cell
		if( 	this->tableWidget_Operators->item(row, COLUMN_MINIMUM) != nullptr && 
			this->tableWidget_Operators->item(row, COLUMN_MAXIMUM) != nullptr
		)
		{
			// current: this is tough! convert to int, then check whether between min and max of the same row
			// if not, set to the closest
			float currentValue = this->tableWidget_Operators->item(row, column)->text().toFloat();
			float minimumValue = this->tableWidget_Operators->item(row, COLUMN_MINIMUM)->text().toFloat();
			float maximumValue = this->tableWidget_Operators->item(row, COLUMN_MAXIMUM)->text().toFloat();
			
			if( currentValue < minimumValue )
				QMessageBox::warning( this->tableWidget_Operators, tr("Warning"), tr("The starting activation probability of an operator cannot be lower than the maximum value.") );
			else if( currentValue > maximumValue )
				QMessageBox::warning( this->tableWidget_Operators, tr("Warning"), tr("The starting activation probability of an operator cannot be higher than the maximum value.") );
			
			// set value
			this->tableWidget_Operators->item(row, column)->setText( QString("%1").arg(currentValue) );
			
			// then, look on the same column, and adjust all values so that the sum is 1
			// without modifying the current value (hopefully)
			if( this->tableWidget_Operators->item(row, column) == this->tableWidget_Operators->currentItem() )
				this->adjustOperatorTableValues();
		}
	}
}

// adjust operator values
void MicroGPMainWindow::adjustOperatorTableValues()
{
	QTableWidgetItem* current = this->tableWidget_Operators->currentItem();
	// if the current item is not set, then a population file has just been loaded
	if( current == nullptr )
	{
		// the correct thing to do is to iterate over the current values, sum them up and divide them by the number of rows
		float totalValue = 0;
		for(int r = 0; r < this->tableWidget_Operators->rowCount(); r++)
		{
			totalValue += this->tableWidget_Operators->item(r, COLUMN_VALUE)->text().toFloat();
		}
		
		if( totalValue != 1)
		for(int r = 0; r < this->tableWidget_Operators->rowCount(); r++)
		{
			QTableWidgetItem* item = this->tableWidget_Operators->item(r, COLUMN_VALUE);
			float currentValue = item->text().toFloat();
			float currentValueAdjusted = 1.0 * currentValue / totalValue;
			item->setText( QString("%1").arg(currentValueAdjusted) );
		}
	}
	else
	{
		//cout << "Current tableWidget_Operators item is " << current->text().toStdString() << endl;
		// normalization of the other operators
		float totalValue = 0;
		for(int r = 0; r < this->tableWidget_Operators->rowCount(); r++)
		{
			QTableWidgetItem* rowItem = this->tableWidget_Operators->item(r, COLUMN_VALUE); 
			totalValue += rowItem->text().toFloat();
		}
		
		if( totalValue != 1 )
		{
			QMessageBox::warning( this->tableWidget_Operators, tr("Warning"), tr("The sum of the activation probabilities of the genetic operators should be 1. If the sum is not 1, the activation probabilities will be normalized when the evolution starts.") );
		}
	}
}

// remove selected operator(s) from table
void MicroGPMainWindow::removeOperators()
{
	// find out which operator row(s) are selected
	QList<QTableWidgetSelectionRange> selected = this->tableWidget_Operators->selectedRanges();
	
	// if the list is empty, just print a warning and return
	if( selected.size() == 0 )
	{
		QMessageBox::warning( this->tableWidget_Operators, tr("Warning"), tr("No operators selected.") );
		return;
	}

	// for each range, collect the rows
	QList<int> rows;
	for(int s = 0; s < selected.size(); s++)
	{
		QTableWidgetSelectionRange range = selected.at(s);
		for(int r = range.topRow(); r <= range.bottomRow(); r++)
			rows.push_back(r);
	}
	
	// sort the rows in ascending order (default); there are no duplicates
	qSort(rows);
	
	// iterate in descending order and remove rows
	for(int r = rows.size() - 1; r >= 0; r--)
		this->tableWidget_Operators->removeRow( rows.at(r) );
}

// change individual selection frame 
void MicroGPMainWindow::changeIndividualSelectionFrame(int newIndex)
{
	// remove widgets from the current layout
	QLayoutItem* child = nullptr;
	while( (child = this->gridLayout_IndividualSelection->takeAt(0)) != nullptr )
	{
		delete( child->widget() );
		delete( child );
	}
	
	// depending on the index selected, change the layout
	if( this->comboBox_IndividualSelection->itemText(newIndex).compare("TournamentSelection") == 0)
	{
		// tau
		// add label
		this->gridLayout_IndividualSelection->addWidget( new QLabel( QString(ugp3::core::TournamentSelection::XML_ATTRIBUTE_TAU.c_str())), 0, 0 ) ;
		// set value and other characteristics of doubleSpinBox
		QDoubleSpinBox* spinBox = new QDoubleSpinBox();
		double spinBoxValue = QString( "2" ).toDouble();
		spinBox->setValue(spinBoxValue);
		spinBox->setSingleStep(0.01);
		// add one doubleSpinBox
		this->gridLayout_IndividualSelection->addWidget( spinBox, 0, 1 );
	
		// tauMin
		// add label
		this->gridLayout_IndividualSelection->addWidget( new QLabel( QString( ugp3::core::TournamentSelection::XML_ATTRIBUTE_TAUMIN.c_str())), 1, 0 );
		// set value and other characteristics of doubleSpinBox
		spinBox = new QDoubleSpinBox();
		spinBoxValue = QString( "1" ).toDouble();
		spinBox->setValue(spinBoxValue);
		spinBox->setSingleStep(0.01);
		// add one doubleSpinBox
		this->gridLayout_IndividualSelection->addWidget( spinBox, 1, 1 );
		
		// tauMax
		// add label
		this->gridLayout_IndividualSelection->addWidget( new QLabel( QString(ugp3::core::TournamentSelection::XML_ATTRIBUTE_TAUMAX.c_str())), 2, 0 );
		// set value and other characteristics of doubleSpinBox
		spinBox = new QDoubleSpinBox();
		spinBoxValue = QString( "4" ).toDouble();
		spinBox->setValue(spinBoxValue);
		spinBox->setSingleStep(0.01);
		// add one doubleSpinBox
		this->gridLayout_IndividualSelection->addWidget( spinBox, 2, 1 );
		
		// fitnessHole
		// add label
		this->gridLayout_IndividualSelection->addWidget( new QLabel( QString(ugp3::core::TournamentSelectionWithFitnessHole::XML_ATTRIBUTE_FITNESSHOLE.c_str())), 3, 0 );
		// set value and other characteristics of doubleSpinBox
		spinBox = new QDoubleSpinBox();
		spinBoxValue = QString( "0" ).toDouble();
		spinBox->setValue(spinBoxValue);
		spinBox->setSingleStep(0.01);
		// add one doubleSpinBox
		this->gridLayout_IndividualSelection->addWidget( spinBox, 3, 1 );
	}
	else if( this->comboBox_IndividualSelection->itemText(newIndex).compare("RankingSelection") == 0 )
	{
		// pressure
		// add label
		this->gridLayout_IndividualSelection->addWidget( new QLabel( QString(ugp3::core::RankingSelection::XML_ATTRIBUTE_PRESSURE.c_str())), 0, 0 ) ;
		// set value and other characteristics of doubleSpinBox
		QDoubleSpinBox* spinBox = new QDoubleSpinBox();
		double spinBoxValue = QString( "1.35" ).toDouble();
		spinBox->setValue(spinBoxValue);
		spinBox->setSingleStep(0.01);
		// add one doubleSpinBox
		this->gridLayout_IndividualSelection->addWidget( spinBox, 0, 1 );
		
		// pressureMin
		// add label
		this->gridLayout_IndividualSelection->addWidget( new QLabel( QString(ugp3::core::RankingSelection::XML_ATTRIBUTE_PRESSUREMIN.c_str())), 1, 0 ) ;
		// set value and other characteristics of doubleSpinBox
		spinBox = new QDoubleSpinBox();
		spinBoxValue = QString( "1" ).toDouble();
		spinBox->setValue(spinBoxValue);
		spinBox->setSingleStep(0.01);
		// add one doubleSpinBox
		this->gridLayout_IndividualSelection->addWidget( spinBox, 1, 1 );
		
		// pressureMax
		// add label
		this->gridLayout_IndividualSelection->addWidget( new QLabel( QString(ugp3::core::RankingSelection::XML_ATTRIBUTE_PRESSUREMAX.c_str())), 2, 0 ) ;
		// set value and other characteristics of doubleSpinBox
		spinBox = new QDoubleSpinBox();
		spinBoxValue = QString( "2" ).toDouble();
		spinBox->setValue(spinBoxValue);
		spinBox->setSingleStep(0.01);
		// add one doubleSpinBox
		this->gridLayout_IndividualSelection->addWidget( spinBox, 2, 1 );
	}
	
	return;	
}

// function that opens operator selection window and adds operators to the population
void MicroGPMainWindow::addOperators()
{
	// create new dialog
	QDialog* dialogSelectOperators = new QDialog();

	// initialize the operator selection window
	MicroGPSelectOperators selectOperatorsWindow;
	selectOperatorsWindow.setupUi(this, dialogSelectOperators);
	
	// show the window and wait for return value
	if( dialogSelectOperators->exec() == QDialog::Accepted)
	{
		// clear current operators
		for(int i = this->tableWidget_Operators->rowCount() - 1; i >= 0; --i)
			this->tableWidget_Operators->removeRow(i);

		// copy values in tableWidget_Operators
		for(unsigned int i = 0; i < selectOperatorsWindow.listWidget_SelectedOperators->count(); i++)
		{
			// add row and set corresponding fields
			this->tableWidget_Operators->insertRow(i);
			this->tableWidget_Operators->setItem( i, COLUMN_NAME, 
				new QTableWidgetItem( selectOperatorsWindow.listWidget_SelectedOperators->item(i)->text() ) );
			this->tableWidget_Operators->setItem( i, COLUMN_VALUE, new QTableWidgetItem( "1" ) );
			this->tableWidget_Operators->setItem( i, COLUMN_MINIMUM, new QTableWidgetItem( "0" ) );
			this->tableWidget_Operators->setItem( i, COLUMN_MAXIMUM, new QTableWidgetItem( "1" ) );
		}
		
		// normalize values
		this->adjustOperatorTableValues();
	}

	// free memory
	delete(dialogSelectOperators);
	
	return;
}

