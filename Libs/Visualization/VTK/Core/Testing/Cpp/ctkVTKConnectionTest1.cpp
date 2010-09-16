
// Qt includes
#include <QApplication>
#include <QDebug>
#include <QList>
#include <QTimer>

// CTKVTK includes
#include "ctkVTKConnection.h"

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vtkTimerLog.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
void doit(vtkObject* vtkNotUsed(obj), unsigned long vtkNotUsed(event),
          void* client_data, void* vtkNotUsed(param))
{
  QTimer* t = reinterpret_cast<QTimer*>(client_data);
  t->stop();
}

//-----------------------------------------------------------------------------
int ctkVTKConnectionTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  int objects = 1000;
  int events = 100;

  vtkObject* obj = vtkObject::New();
  vtkObject* obj2 = vtkObject::New();
  vtkObject* obj3 = vtkObject::New();
  vtkObject* obj4 = vtkObject::New();
  vtkObject* obj5 = vtkObject::New();

  QObject*   topObject = new QObject(0);
  // It could be here any kind of Qt object, QTimer has a no op slot so use it
  QTimer*    slotObject = new QTimer(topObject);

  for (int i = 0; i < objects; ++i)
    {
    ctkVTKConnection* connection = new ctkVTKConnection(topObject);
    connection->observeDeletion(false);
    connection->setup(obj, vtkCommand::ModifiedEvent,
                      slotObject, SLOT(stop()));

    vtkCallbackCommand* callback = vtkCallbackCommand::New();
    callback->SetClientData(slotObject);
    callback->SetCallback(doit);

    obj2->AddObserver(vtkCommand::ModifiedEvent, callback);
    callback->Delete();

    ctkVTKConnection* connection2 = new ctkVTKConnection(topObject);
    connection2->observeDeletion(true);
    connection2->setup(obj3, vtkCommand::ModifiedEvent,
                      slotObject, SLOT(stop()));

    ctkVTKConnection* connection3 = new ctkVTKConnection(topObject);
    connection3->observeDeletion(false);
    connection3->setup(obj4, vtkCommand::ModifiedEvent,
                      new QTimer(topObject), SLOT(stop()));

    ctkVTKConnection* connection4 = new ctkVTKConnection(topObject);
    connection4->observeDeletion(true);
    connection4->setup(obj5, vtkCommand::ModifiedEvent,
                      slotObject, SLOT(stop()));
    }

  vtkSmartPointer<vtkTimerLog> timerLog =
    vtkSmartPointer<vtkTimerLog>::New();

  timerLog->StartTimer();
  for (int i = 0; i < events; ++i)
    {
    obj->Modified();
    }
  timerLog->StopTimer();

  double t1 = timerLog->GetElapsedTime();
  qDebug() << events << "events listened by" << objects << "objects (ctkVTKConnection): " << t1 << "seconds";

  // Callback only

  vtkSmartPointer<vtkTimerLog> timerLog2 =
    vtkSmartPointer<vtkTimerLog>::New();
  timerLog2->StartTimer();
  for (int i = 0; i < events; ++i)
    {
    obj2->Modified();
    }
  timerLog2->StopTimer();

  double t2 = timerLog2->GetElapsedTime();
  qDebug() << events << "events listened by" << objects <<"objects (vtkCallbacks): " << t2 << "seconds";
  double ratio = t1 / t2;
  qDebug() << "ctkVTKConnection / vtkCallbacks: " << ratio;

  vtkSmartPointer<vtkTimerLog> timerLog3 =
    vtkSmartPointer<vtkTimerLog>::New();

  timerLog3->StartTimer();
  for (int i = 0; i < events; ++i)
    {
    obj3->Modified();
    }
  timerLog3->StopTimer();

  double t3 = timerLog3->GetElapsedTime();
  qDebug() << events << "events listened by" << objects << "objects (observed ctkVTKConnection): " << t3 << "seconds";

  vtkSmartPointer<vtkTimerLog> timerLog4 =
    vtkSmartPointer<vtkTimerLog>::New();

  timerLog4->StartTimer();
  for (int i = 0; i < events; ++i)
    {
    obj4->Modified();
    }
  timerLog4->StopTimer();

  double t4 = timerLog4->GetElapsedTime();
  qDebug() << events << "events listened by" << objects << "objects (ctkVTKConnection, 1-1): " << t4 << "seconds";


  obj->Delete();
  obj2->Delete();
  obj3->Delete();

  delete topObject;

  obj4->Delete();
  obj5->Delete();

#ifdef QT_NO_DEBUG // In Debug mode, the ratio can be over 2 !
  // Ideally a ratio ~= 1.
  if (ratio > 1.2)
    {
    return EXIT_FAILURE;
    }
#endif
  return EXIT_SUCCESS;
}
