#include "ui_ugp3-gui.h"
#include "MicroGPMainWindow.h"
#include "MicroGPApplication.h"

using namespace std;
using namespace Ui;

// main
int main(int argc, char* argv[])
{
	// create application
	MicroGPApplication app(argc, argv); 
	// create widget that will be used as anchor for the ugp3.ui interface
	QMainWindow* mainWindow = new QMainWindow;
	// create window class defined by the ugp3.ui file
	MicroGPMainWindow ugp3Window;

	// TODO: everything below should probably be moved to a "window" class inheriting from Ui_ugpMainWindow
	// bind window to widget
	ugp3Window.setupUi(mainWindow);
	
	// show window
	mainWindow->show();

	// return exit value of the application
	return app.exec();
}
