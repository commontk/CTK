
// Qt includes
#include <QApplication>
#include <QDebug>
#include <QWidget>

// CTKVTK includes
#include "ctkVTKConnection.h"

// STD includes
#include <cstdlib>
#include <iostream>

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vtkTimerLog.h>

void doit(vtkObject* obj, unsigned long event, void* client_data, void* param)
{
  QWidget* w = reinterpret_cast<QWidget*>(client_data);
  w->setFocus();
}

int ctkVTKConnectionTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);
  vtkObject* obj = vtkObject::New();
  QWidget topLevelWidget;

  int objects = 1000;
  int events = 100;
  
  for (int i = 0; i < objects; ++i)
    {
    ctkVTKConnection* objectTest = new ctkVTKConnection(&topLevelWidget);
    objectTest->SetParameters(obj, vtkCommand::ModifiedEvent,
                              &topLevelWidget, SLOT(setFocus()));
    objectTest->setEnabled(true);
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

  obj->Delete();

  vtkObject* obj2 = vtkObject::New();
  for (int i = 0; i < objects; ++i)
    {
    vtkCallbackCommand* callback = vtkCallbackCommand::New();
    callback->SetClientData(&topLevelWidget);
    callback->SetCallback(doit);
    
    obj2->AddObserver(vtkCommand::ModifiedEvent, callback);
    callback->Delete();
    }
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
  // Ideally a ratio of 2 (a callback and a signal/slot connection is used 
  // is used in ctkVTKConnection
  if (ratio > 2.5)
    {
    return EXIT_FAILURE;
    }
  obj2->Delete();
  return EXIT_SUCCESS;
}
