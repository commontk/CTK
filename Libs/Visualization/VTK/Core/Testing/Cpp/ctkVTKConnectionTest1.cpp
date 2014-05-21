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
#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QTimer>

// CTKVTK includes
#include "ctkVTKConnection.h"

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkNew.h>
#include <vtkObject.h>
#include <vtkSmartPointer.h>
#include <vtkTimerLog.h>

// STD includes
#include <cstdlib>
#include <iostream>

namespace
{

//-----------------------------------------------------------------------------
void doit(vtkObject* vtkNotUsed(obj), unsigned long vtkNotUsed(event),
          void* client_data, void* vtkNotUsed(param))
{
  QTimer* t = reinterpret_cast<QTimer*>(client_data);
  t->stop();
}

//-----------------------------------------------------------------------------
double computeConnectionTiming(const QString& connectionDescription,
                              int eventCount,
                              int objectCount, vtkObject* obj)
{
  vtkNew<vtkTimerLog> timerLog;
  timerLog->StartTimer();
  for (int i = 0; i < eventCount; ++i)
    {
    obj->Modified();
    }
  timerLog->StopTimer();

  double elapsedTime = timerLog->GetElapsedTime();
  qDebug()
      << qPrintable(connectionDescription.leftJustified(30, ' ')) << ":"
      << "1 event invoked" << eventCount << "times and listened by" << objectCount
      << "objects: " << elapsedTime << "seconds";
  return elapsedTime;
}

} // end of anonymous namespace

//-----------------------------------------------------------------------------
int ctkVTKConnectionTest1( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  int objects = 1000;
  int events = 100;

  vtkObject* obj = vtkObject::New();
  vtkObject* obj2 = vtkObject::New();
  vtkObject* obj3 = vtkObject::New();
  vtkObject* obj4 = vtkObject::New();
  vtkObject* obj5 = vtkObject::New();

  int connection1_observeDeletion = 0;
  // NA for connection2
  int connection3_observeDeletion = 1;
  int connection4_observeDeletion = 0;
  int connection5_observeDeletion = 1;

  QObject*   topObject = new QObject(0);
  // It could be here any kind of Qt object, QTimer has a no op slot so use it
  QTimer*    slotObject = new QTimer(topObject);

  for (int i = 0; i < objects; ++i)
    {
    // connection1
    ctkVTKConnection* connection1 = new ctkVTKConnection(topObject);
    connection1->observeDeletion(connection1_observeDeletion);
    connection1->setup(obj, vtkCommand::ModifiedEvent,
                      slotObject, SLOT(stop()));

    // connection2: regular callback
    vtkCallbackCommand* callback = vtkCallbackCommand::New();
    callback->SetClientData(slotObject);
    callback->SetCallback(doit);

    obj2->AddObserver(vtkCommand::ModifiedEvent, callback);
    callback->Delete();

    // connection3
    ctkVTKConnection* connection3 = new ctkVTKConnection(topObject);
    connection3->observeDeletion(connection3_observeDeletion);
    connection3->setup(obj3, vtkCommand::ModifiedEvent,
                      slotObject, SLOT(stop()));

    // connection4
    ctkVTKConnection* connection4 = new ctkVTKConnection(topObject);
    connection4->observeDeletion(connection4_observeDeletion);
    connection4->setup(obj4, vtkCommand::ModifiedEvent,
                      new QTimer(topObject), SLOT(stop()));

    // connection5
    ctkVTKConnection* connection5 = new ctkVTKConnection(topObject);
    connection5->observeDeletion(connection5_observeDeletion);
    connection5->setup(obj5, vtkCommand::ModifiedEvent,
                      slotObject, SLOT(stop()));
    }

  // Compute timing for connection1
  QString connection1_description =
      QString("connection1 / %1").arg(connection1_observeDeletion);
  double time_connection1 = computeConnectionTiming(connection1_description, events , objects, obj);

  // Compute timing for connection2: Callback only
  QString connection2_description =
      QString("connection2 / vtkCallback");
  double time_connection2 = computeConnectionTiming(connection2_description, events , objects, obj2);

  // Compute timing for connection3
  QString connection3_description =
      QString("connection3 / %1").arg(connection3_observeDeletion);
  computeConnectionTiming(connection3_description, events , objects, obj3);

  // Compute timing for connection4
  QString connection4_description =
      QString("connection4 / %1 / 1-1").arg(connection4_observeDeletion);
  computeConnectionTiming(connection4_description, events , objects, obj4);

  double ratio = time_connection1 / time_connection2;
  qDebug().nospace() << "Ratio [ time_connection1 / time_connection2 ]: " << ratio;

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
