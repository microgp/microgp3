#ifndef MAIN_H
#define MAIN_H

#include "ui_MainWindow.h"

class MainWindow: public Ui::MainWindow, public QMainWindow
{
	public:
		MainWindow();
		~MainWindow();
};
#endif
