#ifndef SIMULATION_VIEWER_HH_INCLUDED
# define SIMULATION_VIEWER_HH_INCLUDED

#include <qmessagebox.h>
#include <qapplication.h>
#include <qobject.h>
#include <QGLViewer/qglviewer.h>

class SimulationViewer : public QGLViewer
{
  Q_OBJECT

public:
  SimulationViewer(void (*onStep)(SimulationViewer*))
    : onStep_(onStep) {}

  ~SimulationViewer();

  int execute();

  static SimulationViewer* makeSimulationViewer
  (int, char**, void (*)(SimulationViewer*));

public slots:
  virtual void onTimer() { onStep_(this); }

protected:
  virtual void draw();
  virtual void drawWithNames();
  virtual void postSelection(const QPoint& point);
  virtual void init();
  virtual QString helpString() const;

private:
  qglviewer::Vec orig;
  qglviewer::Vec dir;
  qglviewer::Vec selectedPoint;
  void (*onStep_)(SimulationViewer*);
  QTimer* timer_;
  QApplication* app_;
};

#endif // SIMULATION_VIEWER_HH_INCLUDED
