// Inherited class; the Ui_ugpMainWindow might be changed by
// Qt Designer, so the relevant bindings must operate here

// class definition
#include "MicroGPMainWindow.h"

// project classes
#include "EvolutionThread.h"
#include "MicroGPVisualizeIndividual.h"

// MicroGP classes
#include "ConsoleHandler.h"
#include "Exception.h"
#include "Level.h"
#include "Population.h"
#include "Program.h"

// Qt classes
#include <QFileDialog>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QtConcurrentRun> 

// Qwt classes
#include <qwt_legend.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>

// standard classes
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// some defines
#define FITNESS_LABEL "fitness_"
#define FITNESS_MAX "_Best_f"
#define FITNESS_MIN "_Worst_f"
#define FITNESS_AVG "_Avg_f"

using namespace std;
using namespace Ui;

using namespace ugp3;
using namespace ugp3::core;
using namespace ugp3::frontend;
using namespace ugp3::log;

/* 	TODO
	
	- in tabEvolution, in the status bars, write something about what is happening
		- label: which population, which step
		- generationBar: what I am doing
		- evolutionBar: evaluate % of stop conditions, plot the one with the highest value
			- maxGenerations = currentGeneration / maxGenerations
			- maxEvaluations = currentEvaluations / maxEvaluations
			- maximumTime = currentTimeElapsed / maximumTime
			- maximumSteadyStateGenerations = currentSteadyStateGenerations / maximumSteadyStateGenerations
			- maximumFitness = currentBestFitness / maximumFitness
	TODO */

#define UGP3_DEBUG

// re-define setupUi to include new bindings
void MicroGPMainWindow::setupUi(QMainWindow* ugpMainWindow)
{
	// run the inherited setupUi
	Ui_MainWindow::setupUi(ugpMainWindow);

	// associate internal pointer to main window
	this->mainWindow = ugpMainWindow;
	
	// initialize the two zoomers associated to the QwtPlots
	this->zoomerLeft = new QwtPlotZoomer( this->qwtPlotLeft->canvas() );
	this->zoomerLeft->setZoomBase();
	this->zoomerRight = new QwtPlotZoomer( this->qwtPlotRight->canvas() );
	this->zoomerRight->setZoomBase();

	// binding slots to signals
	// load population (button and menu item)
	connect( this->pushButton_LoadPopulation, SIGNAL(clicked()), this, SLOT(loadPopulation()) );
	//connect( this->action_LoadPopulation, SIGNAL(triggered()), this, SLOT(loadPopulation()) );
	
	// load status
	connect( this->action_LoadStatus, SIGNAL(triggered()), this, SLOT(loadStatus()) );
	
	// visualize best individual
	connect( this->action_ViewBestIndividual, SIGNAL(triggered()), this, SLOT(visualizeBestIndividual()));
	
	// start evolution
	connect( this->pushButton_StartEvolution, SIGNAL(clicked()), this, SLOT(startEvolution()) );
	
	// stop evolution
	connect( this->pushButton_StopEvolution, SIGNAL(clicked()), this, SLOT(stopEvolution()) );
	
	// an index in a combobox changed: re-draw the graphs
	connect(this->comboBox_1X, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGraph1()));
	connect(this->comboBox_1Y, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGraph1()));
	connect(this->comboBox_2X, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGraph2()));
	connect(this->comboBox_2Y, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGraph2()));
	
	// set the textEdit to "read only"
	this->textEdit->setReadOnly(true);
	
	// gray out some of the buttons
	this->pushButton_StartEvolution->setEnabled(false);
	this->pushButton_StopEvolution->setEnabled(false);
	
	// after connecting everything, initialize stuff 
	// ok, so it's probably better to write a separate function, since
	// I will have to register all the operators and stuff...or not?
	
	// there is no evolution running at the moment
	this->runningEvolution = false;

	// open the stringstream that will be used to store all the logging information
	ConsoleHandler* consoleHandler = new ConsoleHandler();
	consoleHandler->setConsole(&this->ssLog);
	consoleHandler->setLevel(Level::Info); // TODO change here for a different level of logging activity
	::log_.addHandler(*consoleHandler);
	
	// this part is actually used for debugging purposes, only;
	// it creates a new console on standard output
#if defined(UGP3_DEBUG)
	ConsoleHandler* consoleDebug = new ConsoleHandler();
	consoleDebug->setLevel(Level::Debug);
	::log_.addHandler(*consoleDebug);
#endif
	
	// register operators
	Program::registerOperators();

	// publish a nice warning
	LOG_WARNING << "This version only supports ABSOLUTE PATHS for scripts and constraints in the configuration files. Either that, or it needs to be run in the working directory." << ends;

	// create the thread, and connect all the appropriate signals
	this->evolutionThread = new EvolutionThread();
	this->evolutionThread->setParentWindow(this);

	// necessary connections
	// update graphs
	connect(this->evolutionThread, SIGNAL(signalUpdateGraph()), this, SLOT(updateGraph1()));
	connect(this->evolutionThread, SIGNAL(signalUpdateGraph()), this, SLOT(updateGraph2()));
	// update the statistics when the thread produces something new
	connect(this->evolutionThread, SIGNAL(signalUpdateStatistics(QString)), this, SLOT(updateStatistics(QString)));
	// evolution has been successfully paused
	connect(this->evolutionThread, SIGNAL(signalEvolutionPaused()), this, SLOT(pauseEvolution()));
	// evolution has reached a stop condition
	connect(this->evolutionThread, SIGNAL(signalEvolutionTerminated()), this, SLOT(evolutionTerminated()));
	// evolution cannot start, because a stop condition has been reached
	connect(this->evolutionThread, SIGNAL(signalEvolutionCannotStart()), this, SLOT(warningEvolutionStopped()));
	
	// force the evolution to a pause
	connect(this, SIGNAL(signalPauseEvolution()), this->evolutionThread, SLOT(pauseEvolution()));
	// reset (and restart) the current evolutionary process
	connect(this, SIGNAL(signalResetEvolution()), this->evolutionThread, SLOT(resetEvolution()));

	// finally, update the textEdit
	this->updateTextEdit();
}

// load evolution from population file
void MicroGPMainWindow::loadPopulation()
{
	QString populationFileName = QFileDialog::getOpenFileName(this->pushButton_LoadPopulation, tr("Open XML Constraints File"), "", tr("XML (*.xml);;All Files (*)"));
	
	QString populationWorkingDirectory = QFileInfo(populationFileName).absoluteDir().absolutePath();

	//cout << "The current directory is " << populationWorkingDirectory.toStdString() << endl;
	
	if( populationFileName.isEmpty() )
	{
		// the user did not select anything, just return
		return;
	}
	
	// here, the user selected something; so, if the evolutionary algorithm is already initialized, 
	// send a message box to check whether everything is all right
	if( this->evolutionaryAlgorithm.get() != nullptr )
	{
		int choice = QMessageBox::warning(this->pushButton_LoadPopulation, "Warning!", "Loading a new population will reset ALL current configuration. Are you sure you want to proceed?", QMessageBox::Cancel, QMessageBox::Ok);
		
		if( choice == QMessageBox::Ok )
		{
			this->resetEverything();
		}
		else
		{
			return;
		}
	}
	
	// otherwise, try to create an EvolutionaryAlgorithm instance starting from the population
	this->evolutionaryAlgorithm = unique_ptr<EvolutionaryAlgorithm>( new EvolutionaryAlgorithm() );
	
	// create the population, then add it to the evolutionary algorithm
	unique_ptr<Population> population;
	string populationName = "Population";
	try
	{
		LOG_INFO 	<< "Adding population " << this->evolutionaryAlgorithm->getPopulationCount() 
				<< " \"" << populationName << "\" from file \"" 
				<< populationFileName.toStdString() << "\"" 
				<< ends;

		// ok, here we have the usual problem: fromParametersFile assumes that the constraints file is
		// inside the WORKING DIRECTORY, while the correct behavior should be to look for it in the
		// SAME DIRECTORY as the the population file
		// TODO: I should really, really modify Population::fromParametersFile...
		//cout << "Population file: \"" << populationFileName.toStdString() << "\"" << endl;
		population = Population::fromParametersFile( *evolutionaryAlgorithm, populationFileName.toStdString() );
		population->setName(populationName);
	}
	catch (const Exception& e)
	{
		// just throwing the exception actually visualizes it in a message box
		throw;
	}
	
	// finally, add the population inside the evolutionary algorithm
	// maybe I should remove all the current populations?
	// NOPE, because the evolutionaryAlgorithm is reset at every loading
	this->evolutionaryAlgorithm->addPopulation( std::move(population) );	
	
	LOG_DEBUG << "Added population to the evolutionary algorithm" << ends;
	
	// also, the button now becomes available
	this->pushButton_StartEvolution->setEnabled(true);
	
	// not only, but we fill the comboboxes with the headers of the statistics; however, we need to get that
	// sweet Population header, for each population (even if there's only one, at the moment)
	ostringstream ssPopHeader;
	for(unsigned int p = 0; p < this->evolutionaryAlgorithm->getPopulationCount(); p++)
	{
		this->evolutionaryAlgorithm->getPopulation(p).dumpStatisticsHeader(ssPopHeader);
	}
	// parse the header and get a vector of strings
	vector<string> tokens = this->split(ssPopHeader.str(), ",");

	// add "CPU" at the beginning (?) TODO if you do that, remember to change the indexes below
	// tokens.insert(tokens.begin(), "CPU");

	// this is a special part, where we try to add extra labels
	// for example, let's try to make an extra label for every fitness value
	// fitness values all share the "_fX" ending, where X is a number
	// it is added here, because now we can freely manipulate all the tokens vector
	vector<string> fitnessLabels;
	bool fitnessFound = true;

	while(fitnessFound)
	{
		fitnessFound = false;
		unsigned int fitnessStatistics = 0;

		string currentFitnessLabel = "_f";
		currentFitnessLabel += to_string( fitnessLabels.size() );

		LOG_DEBUG << "Current fitness label: \"" << currentFitnessLabel << "\"" << ends;

		for(unsigned int t = 0; t < tokens.size(); t++)
		{
			if( tokens[t].find( currentFitnessLabel ) != string::npos )
			{
				fitnessStatistics++;
			}
		}
		
		// now, if we found three (avg, max, min), we are ok!
		if( fitnessStatistics == 3 ) 
		{	
			// add the label to the vector
			fitnessFound = true;
			fitnessLabels.push_back( FITNESS_LABEL + to_string( fitnessLabels.size() ) );
		}
	}
	
	// finally, fill the comboboxes
	for(unsigned int t = 0; t < tokens.size(); t++)
	{
		this->comboBox_1X->addItem( QString::fromStdString(tokens[t]) );
		this->comboBox_1Y->addItem( QString::fromStdString(tokens[t]) );

		this->comboBox_2X->addItem( QString::fromStdString(tokens[t]) );
		this->comboBox_2Y->addItem( QString::fromStdString(tokens[t]) );
	}
	
	// also add the new labels
	for(unsigned int fl = 0; fl < fitnessLabels.size(); fl++)
	{
		// the special labels are only on the Y graphs
		this->comboBox_1Y->addItem( QString::fromStdString(fitnessLabels[fl]) );
		this->comboBox_2Y->addItem( QString::fromStdString(fitnessLabels[fl]) );
	}

	
	// set the initial values
	this->comboBox_1X->setCurrentIndex(0);
	this->comboBox_1Y->setCurrentIndex(this->comboBox_1Y->findText( QString::fromStdString( fitnessLabels[0] ) ));

	// if there is more than one fitness, the second graph is set to that; otherwise, to EvalCount
	this->comboBox_2X->setCurrentIndex(0);
	if( fitnessLabels.size() > 1 )
		this->comboBox_2Y->setCurrentIndex( this->comboBox_2Y->findText( QString::fromStdString( fitnessLabels[1] ) ) );
	else
		this->comboBox_2Y->setCurrentIndex( 6 );
	
	// now, before updating the text and exiting, we can initialize the structure
	// that will later be used to store the statistics (generation by generation)
	for(unsigned int t = 0; t < tokens.size(); t++)
	{
		vector<double> tempStats;
		this->statistics[ tokens[t] ] = tempStats;
		this->indexToName[ t ] = tokens[t];
	} 

	this->updateTextEdit();

	
	return;
}

// load evolution from status file
void MicroGPMainWindow::loadStatus()
{
	QString statusFileName = QFileDialog::getOpenFileName(this->pushButton_LoadPopulation, tr("Open XML Constraints File"), "", tr("XML (*.xml);;All Files (*)"));
	
	if( statusFileName.isEmpty() )
	{
		// the user did not select anything, just return
		return;
	}

	this->updateTextEdit();
}

void MicroGPMainWindow::resetEverything()
{
	// the unique_ptr with the evolutionary algorithm is deleted!
	this->evolutionaryAlgorithm.reset(nullptr);
	
	// TODO more stuff!
	// - close the logging outputs?
	// - reset/delete all content from the textEdit?
	this->textEdit->clear();
	
}

void MicroGPMainWindow::updateTextEdit()
{
	// simple function that adds the text in the ostringstream to the textEdit, then clears the ostringstream
	// it should be added at the end of every method in the class, in order to keep the log updated
	this->textEdit->append( QString::fromStdString( this->ssLog.str() ) );
	QTextCursor cursor = this->textEdit->textCursor();
	cursor.movePosition(QTextCursor::End);
	this->textEdit->setTextCursor(cursor);

	this->ssLog.clear();
}

void MicroGPMainWindow::startEvolution()
{
	// first, check if the evolutionary algorithm is ok
	if( this->evolutionaryAlgorithm.get() == nullptr )
	{
		LOG_WARNING << "No population or status file loaded: cannot start the evolution." << ends;
		this->updateTextEdit();
		return;
	}
	
	// if the evolution is already running, just invoke the stop condition and change the label
	// on the pushButton
	if( this->runningEvolution == true )
	{
		this->runningEvolution = false;
		this->pushButton_StartEvolution->setText("Pausing...");
		emit signalPauseEvolution();
		return;
	}
	
	// otherwise, let's start the evolutionary process
	LOG_INFO << "Starting evolution..." << ends;
	this->runningEvolution = true;
	this->updateTextEdit();

	// change label on the pushButton
	this->pushButton_StartEvolution->setText("Pause Evolution");
	
	/*
	// and now, let's create another thread
	EvolutionThread* evolutionThread = new EvolutionThread();

	// necessary connections
	connect(evolutionThread, SIGNAL(signalUpdateGraph()), this, SLOT(updateGraph1()));
	connect(evolutionThread, SIGNAL(signalUpdateGraph()), this, SLOT(updateGraph2()));
	connect(evolutionThread, SIGNAL(signalUpdateStatistics(QString)), this, SLOT(updateStatistics(QString)));
	connect(evolutionThread, SIGNAL(signalEvolutionPaused()), this, SLOT(pauseEvolution()));
	connect(evolutionThread, SIGNAL(signalEvolutionTerminated()), this, SLOT(evolutionTerminated()));
	connect(this, SIGNAL(signalPauseEvolution()), evolutionThread, SLOT(pauseEvolution()));

	// start the thread
	evolutionThread->setParentWindow(this);
	*/
	this->evolutionThread->start();
	
	return;
}

void MicroGPMainWindow::stopEvolution()
{
	// setting this value should be enough
	this->runningEvolution = false;
	
	return;
}

const string MicroGPMainWindow::getAbsoluteScript(const string dir, const string script)
{
	// first, let's prepare the directory
	string currentDir = dir;
	if( dir[ dir.length() -1 ] != '/' )
	{
		currentDir += '/';
	}
	
	// parse the script on spaces
	vector<string> tokens = this->split(script, " ");
	
	// check if some of the parts start with "./" or end with ".py" or ".pl" or ".exe"
	for(unsigned int i = 0; i < tokens.size(); i++)
	{
		bool isScript = false;

		if( tokens[i][ tokens[i].length() -3 ] == '.' && tokens[i][ tokens[i].length() -2 ] == 'p' && ( tokens[i][ tokens[i].length() -2 ] == 'y' || tokens[i][ tokens[i].length() -2 ] == 'l' ) )
		{
			LOG_DEBUG << "Perl/Python script detected!" << ends;
			isScript = true;
		}
		else if( tokens[i][ tokens[i].length() -3 ] == '.' && tokens[i][ tokens[i].length() -2 ] == 's' && tokens[i][ tokens[i].length() -1 ] == 'h' )
		{
			LOG_DEBUG << "Shell script detected!" << ends;
			isScript = true;
		}
		
	}
	
	return currentDir;
}

// utility function to split stuff
vector<string> MicroGPMainWindow::split(const string& str, const string& delimiters)
{
	vector<string> tokens;
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
	
	return tokens;
}

void MicroGPMainWindow::runEvolution()
{
/*
	// this will be used to temporary store the stream with the statistics
	ostringstream ssStatistics;

	// change lable on the pushButton
	this->pushButton_StartEvolution->setText("Pause Evolution");
	
	// start the evolutionary algorithm, kickstarting all the populations
	for(unsigned int i = 0; i < this->evolutionaryAlgorithm->getPopulationCount(); i++)
	{
		Population& population = this->evolutionaryAlgorithm->getPopulation(i);
		
		LOG_INFO 	<< "* Population \"" 
				<< this->evolutionaryAlgorithm->getPopulation(i).getName() 
				<< "\" (kickstart)" 
				<< ends;

		if( population.getGeneration() == 0 ) 
		{
		    population.setupPopulation();
		}

		population.evaluateAndHandleClones();
		population.removeDeadCandidates();
		population.showStatistics();
		
		// update the statistics
		population.dumpStatistics( ssStatistics );
		this->updateStatistics( &ssStatistics );
	}
	
	// save "step 0" of the algorithm
	this->evolutionaryAlgorithm->saveDumpState();
	// show the statistics
	this->evolutionaryAlgorithm->writeStatisticsStream();
	// update the window
	this->updateTextEdit();
	
	emit updateGraph();
	
	// main loop
	bool checkStopCondition = false;
	while( this->runningEvolution == true && checkStopCondition == false)
	{
		checkStopCondition = true;

		// perform step for each population, then check
		// if the global stop condition was reached
		for(unsigned int p = 0; p < this->evolutionaryAlgorithm->getPopulationCount(); p++)
		{
			Population& population = this->evolutionaryAlgorithm->getPopulation(p);
			
			if( !population.checkStopCondition() )
			{
				LOG_INFO << "* Population \"" << population.getName() << "\" generation " << (population.getGeneration() + 1)  << ends;
				population.step();
				population.showStatistics();

				population.dumpStatistics( ssStatistics );
				this->updateStatistics( &ssStatistics );
				
				this->updateTextEdit();
				emit updateGraph();
			}
			
			checkStopCondition = checkStopCondition && population.checkStopCondition();
		}
		// TODO implement migration and stuff

		this->evolutionaryAlgorithm->writeStatisticsStream();
		this->evolutionaryAlgorithm->saveDumpState();

		this->updateTextEdit();
	}
	
	if( this->runningEvolution == false )
	{
		LOG_INFO << "Evolution stopped by the user." << ends;
		this->pushButton_StartEvolution->setText("Resume Evolution");
	}
	else
	{
		LOG_INFO << "Evolution stopped because a stop condition was reached." << ends;
		this->pushButton_StartEvolution->setText("Start Evolution");
	}

	this->updateTextEdit();

	return;
*/

}

void MicroGPMainWindow::updateGraph1()
{
	// detach everything (curves) from the graph
	this->qwtPlotLeft->detachItems();

	// change axis labels
	this->qwtPlotLeft->setAxisTitle(QwtPlot::xBottom, this->comboBox_1X->currentText() );
	this->qwtPlotLeft->setAxisTitle(QwtPlot::yLeft, this->comboBox_1Y->currentText() );
	
	// now, let's check if this is one of the special labels
	if( this->comboBox_1Y->currentText().startsWith( QString( FITNESS_LABEL ) ) == true )
	{
		// x data is just taken from the text in the combobox
		QVector<double> x = QVector<double>::fromStdVector( this->statistics[ this->comboBox_1X->currentText().toStdString() ] ); 
		
		// y data is fetched from the statistics on the basis of the fitness we're looking at; so we first have to
		// catch the number TODO this is on the assumption that the fitness label is 8 characters long
		QString justTheNumber = this->comboBox_1Y->currentText().remove( QString::fromStdString( FITNESS_LABEL ) );
		int fitnessNumber = std::stoi( justTheNumber.toStdString() );

		QwtPlotCurve* curveMax = new QwtPlotCurve("fitness #" + justTheNumber + " max");
		QwtPlotCurve* curveMin = new QwtPlotCurve("fitness #" + justTheNumber + " min");
		QwtPlotCurve* curveAvg = new QwtPlotCurve("fitness #" + justTheNumber + " avg");

		string labelMax = FITNESS_MAX + to_string( fitnessNumber );
		string labelMin = FITNESS_MIN + to_string( fitnessNumber );
		string labelAvg = FITNESS_AVG + to_string( fitnessNumber );
		
		QVector<double> yMax;	
		QVector<double> yMin;
		QVector<double> yAvg;

		for(map<string, vector<double> >::iterator it = this->statistics.begin(); it != this->statistics.end(); it++)
		{
			if( it->first.find(labelMax) != string::npos )
			{ 
		 		yMax = QVector<double>::fromStdVector( it->second );
			}
			else if( it->first.find(labelMin) != string::npos )
			{
				yMin = QVector<double>::fromStdVector( it->second );
			}
			else if( it->first.find(labelAvg) != string::npos )
			{
				yAvg = QVector<double>::fromStdVector( it->second );
			}
		}
		
		
		// prepare the curves
		curveMax->setSamples(x, yMax);
		curveMin->setSamples(x, yMin);
		curveAvg->setSamples(x, yAvg);

		curveMax->setRenderHint(QwtPlotCurve::RenderAntialiased);
		curveMin->setRenderHint(QwtPlotCurve::RenderAntialiased);
		curveAvg->setRenderHint(QwtPlotCurve::RenderAntialiased);

		curveMax->setPen(QPen(Qt::red));
		curveMin->setPen(QPen(Qt::blue));
		curveAvg->setPen(QPen(Qt::green));

		curveMax->attach(this->qwtPlotLeft);
		curveMin->attach(this->qwtPlotLeft);
		curveAvg->attach(this->qwtPlotLeft);
		
		// check to understand whether we should render or not
		if( x.size() > 0 && yMax.size() > 0 && yMin.size() > 0 && yAvg.size() > 0 )
		{
			QwtLegend* legend = new QwtLegend();
			legend->setFrameStyle(QFrame::Box|QFrame::Sunken);
			this->qwtPlotLeft->insertLegend( legend, QwtPlot::BottomLegend /*, QwtPlot::ExternalLegend*/ );
			
			// now, rescale the axis
			this->qwtPlotLeft->setAxisAutoScale(QwtPlot::xBottom);
			this->qwtPlotLeft->setAxisAutoScale(QwtPlot::yLeft);
			this->qwtPlotLeft->updateAxes();

			this->qwtPlotLeft->replot();
			this->zoomerLeft->setZoomBase(true);
		}
		
		return;
	}
	
	// prepare the curve
	QwtPlotCurve* curve = new QwtPlotCurve(this->comboBox_1Y->currentText() + " in function of " + this->comboBox_1X->currentText());
	// x and y values for the graph are extracted from the statistics
	QVector<double> xcurve = QVector<double>::fromStdVector( this->statistics[ this->comboBox_1X->currentText().toStdString() ] ); 
	QVector<double> ycurve = QVector<double>::fromStdVector( this->statistics[ this->comboBox_1Y->currentText().toStdString() ] );
	
	// prepare the curve
	curve->setSamples(xcurve, ycurve);
	curve->setRenderHint(QwtPlotCurve::RenderAntialiased);
	curve->setPen(QPen(Qt::red));
	curve->attach(this->qwtPlotLeft);

	// rescale the axis (IF there are some data...)
	if( xcurve.size() > 0 && ycurve.size() > 0 )
	{
		// for some reason, curve->minXValue() and so on, no longer work...
		//this->qwtPlotLeft->setAxisScale(QwtPlot::xBottom, curve->minXValue(), curve->maxXValue());
		//this->qwtPlotLeft->setAxisScale(QwtPlot::yLeft, curve->minYValue(), curve->maxYValue());

		// show the final plot
		this->qwtPlotLeft->replot();
		this->qwtPlotLeft->show();
	}
	
	return;
}

void MicroGPMainWindow::updateGraph2()
{
	// detach everything (curves) from the graph
	this->qwtPlotRight->detachItems();

	// change axis labels
	this->qwtPlotRight->setAxisTitle(QwtPlot::xBottom, this->comboBox_2X->currentText() );
	this->qwtPlotRight->setAxisTitle(QwtPlot::yLeft, this->comboBox_2Y->currentText() );
	
	// now, let's check if this is one of the special labels
	if( this->comboBox_2Y->currentText().startsWith( QString( FITNESS_LABEL ) ) == true )
	{
		QwtPlotCurve* curveMax = new QwtPlotCurve("fitness max");
		QwtPlotCurve* curveMin = new QwtPlotCurve("fitness min");
		QwtPlotCurve* curveAvg = new QwtPlotCurve("fitness avg");
		
		// x data is just taken from the text in the combobox
		QVector<double> x = QVector<double>::fromStdVector( this->statistics[ this->comboBox_2X->currentText().toStdString() ] ); 
		
		// y data is fetched from the statistics on the basis of the fitness we're looking at; so we first have to
		// catch the number 
		QString justTheNumber = this->comboBox_2Y->currentText().remove( QString::fromStdString( FITNESS_LABEL ) );
		int fitnessNumber = std::stoi( justTheNumber.toStdString() );

		string labelMax = FITNESS_MAX + to_string( fitnessNumber );
		string labelMin = FITNESS_MIN + to_string( fitnessNumber );
		string labelAvg = FITNESS_AVG + to_string( fitnessNumber );
		
		QVector<double> yMax;	
		QVector<double> yMin;
		QVector<double> yAvg;

		for(map<string, vector<double> >::iterator it = this->statistics.begin(); it != this->statistics.end(); it++)
		{
			if( it->first.find(labelMax) != string::npos )
			{ 
		 		yMax = QVector<double>::fromStdVector( it->second );
			}
			else if( it->first.find(labelMin) != string::npos )
			{
				yMin = QVector<double>::fromStdVector( it->second );
			}
			else if( it->first.find(labelAvg) != string::npos )
			{
				yAvg = QVector<double>::fromStdVector( it->second );
			}
		}
		
		// prepare the curves
		curveMax->setSamples(x, yMax);
		curveMin->setSamples(x, yMin);
		curveAvg->setSamples(x, yAvg);

		curveMax->setRenderHint(QwtPlotCurve::RenderAntialiased);
		curveMin->setRenderHint(QwtPlotCurve::RenderAntialiased);
		curveAvg->setRenderHint(QwtPlotCurve::RenderAntialiased);

		curveMax->setPen(QPen(Qt::red));
		curveMin->setPen(QPen(Qt::blue));
		curveAvg->setPen(QPen(Qt::green));

		curveMax->attach(this->qwtPlotRight);
		curveMin->attach(this->qwtPlotRight);
		curveAvg->attach(this->qwtPlotRight);
		
		// check to understand whether we should render or not
		if( x.size() > 0 && yMax.size() > 0 && yMin.size() > 0 && yAvg.size() > 0 )
		{
			// create/attach a legend
			QwtLegend* legend = new QwtLegend();
			legend->setFrameStyle(QFrame::Box|QFrame::Sunken);
			this->qwtPlotRight->insertLegend( legend, QwtPlot::BottomLegend /*, QwtPlot::ExternalLegend*/ );
			
			// now, rescale the axis
			this->qwtPlotRight->setAxisAutoScale(QwtPlot::xBottom);
			this->qwtPlotRight->setAxisAutoScale(QwtPlot::yLeft);
			this->qwtPlotRight->updateAxes();

			this->qwtPlotRight->replot();
			this->zoomerRight->setZoomBase(true);
		}
		
		return;
	}
	
	// prepare the curve
	QwtPlotCurve* curve = new QwtPlotCurve(this->comboBox_2Y->currentText() + " in function of " + this->comboBox_2X->currentText());
	// x and y values for the graph are extracted from the statistics
	QVector<double> xcurve = QVector<double>::fromStdVector( this->statistics[ this->comboBox_2X->currentText().toStdString() ] ); 
	QVector<double> ycurve = QVector<double>::fromStdVector( this->statistics[ this->comboBox_2Y->currentText().toStdString() ] );
	
	// prepare the curve
	curve->setSamples(xcurve, ycurve);
	curve->setRenderHint(QwtPlotCurve::RenderAntialiased);
	curve->setPen(QPen(Qt::red));
	curve->attach(this->qwtPlotRight);

	// rescale the axis (IF there are some data...)
	if( xcurve.size() > 0 && ycurve.size() > 0 )
	{
		// for some reason, curve->minXValue() and so on, no longer work...
		//this->qwtPlotLeft->setAxisScale(QwtPlot::xBottom, curve->minXValue(), curve->maxXValue());
		//this->qwtPlotLeft->setAxisScale(QwtPlot::yLeft, curve->minYValue(), curve->maxYValue());

		// show the final plot
		this->qwtPlotRight->replot();
		this->qwtPlotRight->show();
	}
	
	return;
}

void MicroGPMainWindow::updateStatistics(ostringstream* stream)
{
	// split the string into tokens
	vector<string> tokens = this->split( stream->str(), "," );

	// store the statistics
	for(unsigned int t = 0; t < tokens.size(); t++)
	{
		// find the name of the stat
		string statName = this->indexToName[ t ];
		
		// obtain the value
		float statValue = atof( tokens[t].c_str() );
		
		// append the value
		this->statistics[ statName ].push_back( statValue );
	}
	
	return;
}

void MicroGPMainWindow::updateStatistics(QString stream)
{
	// split the string into tokens
	vector<string> tokens = this->split( stream.toStdString(), "," );

	// store the statistics
	for(unsigned int t = 0; t < tokens.size(); t++)
	{
		// find the name of the stat
		string statName = this->indexToName[ t ];
		
		// obtain the value
		float statValue = atof( tokens[t].c_str() );
		
		// append the value
		this->statistics[ statName ].push_back( statValue );
	}
	
	return;
}

void MicroGPMainWindow::pauseEvolution()
{
	this->runningEvolution = false;
	this->pushButton_StartEvolution->setText("Resume Evolution");
}

void MicroGPMainWindow::evolutionTerminated()
{
	this->runningEvolution = false;
	this->pushButton_StartEvolution->setText("Start Evolution");
}

void MicroGPMainWindow::warningEvolutionStopped()
{
	// send a warning!
	int choice = QMessageBox::warning(this->pushButton_StartEvolution, "Warning!", "The current population reached a stop condition. Restarting the evolution will cancel all the results obtained so far. Are you sure you want to proceed?", QMessageBox::Cancel, QMessageBox::Ok);
	
	if( choice == QMessageBox::Ok )
	{
		emit signalResetEvolution();
	}
	
}

void MicroGPMainWindow::visualizeBestIndividual()
{
	// check if the best individual exists and the evolution thread is not running
	if( this->evolutionaryAlgorithm == nullptr )
	{
		QMessageBox::warning(this->pushButton_StartEvolution, "Warning!", "Empty population. Load a population and start an evolutionary run before trying to visualize the best individual.");
		return;
	}
	
	// if the algorithm exists, check if the evolution has been run
	if( this->evolutionaryAlgorithm->getPopulation(0).getBestCandidate() == nullptr )
	{
		QMessageBox::warning(this->pushButton_StartEvolution, "Warning!", "No best individual found (yet). Start an evolutionary run before trying to visualize the best individual.");
		return;

	}

	const CandidateSolution* bestSolution = this->evolutionaryAlgorithm->getPopulation(0).getBestCandidate();	
	QDialog* dialogVisualizeIndividual = new QDialog();
	
	// initialize the visualization window
	MicroGPVisualizeIndividual visualizeIndividualWindow;
	visualizeIndividualWindow.setupUi( this, dialogVisualizeIndividual, bestSolution );
	
	// visualize dialog
	dialogVisualizeIndividual->exec();
	
	// free memory
	delete(dialogVisualizeIndividual);
	
	return;
}
