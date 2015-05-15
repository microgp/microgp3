// Redefinition of QApplication to implement notify
// Alberto Tonda, 2012 <alberto.tonda@gmail.com>

#ifndef MICROGPAPPLICATION
#define MICROGPAPPLICATION

#include <QtGui>
#include <QApplication>
#include <QMessageBox>

class MicroGPApplication : public QApplication
{
public:
	MicroGPApplication(int& argc, char ** argv) : QApplication(argc, argv) { }
	virtual ~MicroGPApplication() { }

	// reimplemented from QApplication so we can throw exceptions in slots
	virtual bool notify(QObject * receiver, QEvent * event)
	{
    		try 
		{
      			return QApplication::notify(receiver, event);
    		}
		catch(std::exception& e) 
		{
			QMessageBox::warning( 0, tr("An error occurred"), e.what() );
    		}	
    		
		return false;
  	}
};

#endif
