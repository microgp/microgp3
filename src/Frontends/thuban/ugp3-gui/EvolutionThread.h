#ifndef EVOLUTIONTHREAD
#define EVOLUTIONTHREAD

#include <QMutex>
#include <QString>
#include <QThread>
#include <QVector>

#include "MicroGPMainWindow.h"

namespace Ui
{
class EvolutionThread : public QThread
{
	Q_OBJECT

private:
	MicroGPMainWindow* window;
	bool isRunning;
	bool stopConditionReached;

protected:
	virtual void run();

public:
	void setParentWindow(MicroGPMainWindow* window);

public slots:
	void pauseEvolution();
	void resetEvolution();

signals:
	void signalTextEdit(QString);
  	void signalQwtPlot(QVector<float>);

	void signalUpdateGraph();
	void signalUpdateStatistics(QString);

	void signalEvolutionPaused();
	void signalEvolutionTerminated();
	
	void signalEvolutionCannotStart();
};

} // namespace Ui

#endif
