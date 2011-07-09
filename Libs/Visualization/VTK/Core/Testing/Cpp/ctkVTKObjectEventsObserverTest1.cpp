/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QDebug>
#include <QList>
#include <QTimer>

// CTKVTK includes
#include "ctkVTKObjectEventsObserver.h"

// STD includes
#include <cstdlib>
#include <iostream>

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vtkTimerLog.h>

int ctkVTKObjectEventsObserverTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);
  
  int objects = 1000;
  int events = 100;
  
  vtkObject* obj = vtkObject::New();
  QObject*   topObject = new QObject(0);
  
  ctkVTKObjectEventsObserver* observer = new ctkVTKObjectEventsObserver(topObject);
  for (int i = 0; i < objects; ++i)
    {
    QTimer*    slotObject = new QTimer(topObject);
    observer->addConnection(obj, vtkCommand::ModifiedEvent,
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

  obj->Delete();

  delete topObject;
  
  return EXIT_SUCCESS;
}
