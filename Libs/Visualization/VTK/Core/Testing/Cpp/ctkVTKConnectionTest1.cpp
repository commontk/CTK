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
#include <QStringList>


// CTK includes
#include <ctkCallback.h>

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
int total_event_count;

//-----------------------------------------------------------------------------
void spy(void* data)
{
  Q_UNUSED(data);
  ++total_event_count;
}

//-----------------------------------------------------------------------------
void doit(vtkObject* vtkNotUsed(obj), unsigned long vtkNotUsed(event),
          void* client_data, void* vtkNotUsed(param))
{
  ctkCallback* t = reinterpret_cast<ctkCallback*>(client_data);
  t->invoke();
}

//-----------------------------------------------------------------------------
void displayDartMeasurement(const char* name, double value)
{
  std::cout << "<DartMeasurement name=\""<< name <<"\" "
            << "type=\"numeric/double\">"
            << value << "</DartMeasurement>" << std::endl;
}

//-----------------------------------------------------------------------------
// This function will trigger a ModifiedEvent <eventCount> times
// where <objectCount> objects listen that same event.
bool computeConnectionTiming(const char* name,
                               int eventCount,
                               int objectCount, vtkObject* obj,
                               double & elapsedTime)
{
  elapsedTime = 0;
  total_event_count = 0;

  vtkNew<vtkTimerLog> timerLog;
  timerLog->StartTimer();
  for (int i = 0; i < eventCount; ++i)
    {
    obj->Modified();
    }
  timerLog->StopTimer();

  int expected_total_event_count = eventCount * objectCount;
  if (total_event_count != expected_total_event_count)
    {
    std::cerr << "Problem with " << name << "\n"
              << "\tcurrent total_event_count:" << total_event_count << "\n"
              << "\texpected total_event_count:" << expected_total_event_count
              << std::endl;
    return false;
    }

  elapsedTime = timerLog->GetElapsedTime();

  QString measurementName = QString("time-%1-%2-%3").arg(name).arg(eventCount).arg(objectCount);
  displayDartMeasurement(qPrintable(measurementName), elapsedTime);

  return true;
}

//-----------------------------------------------------------------------------
bool computeConnectionTiming(const char* name,
                               int eventCount,
                               int objectCount, vtkObject* obj)
{
  double elapsedTime = 0;
  return computeConnectionTiming(name, eventCount, objectCount, obj, elapsedTime);
}

} // end of anonymous namespace

//-----------------------------------------------------------------------------
int ctkVTKConnectionTest1( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  int testCase = -1;
  if (argc > 1)
    {
    testCase = app.arguments().at(1).toInt();
    }

  total_event_count = 0;
  int objects = 1000;
  int events = 100;

  vtkObject* obj1 = vtkObject::New();
  vtkObject* obj2 = vtkObject::New();
  vtkObject* obj3 = vtkObject::New();
  vtkObject* obj4 = vtkObject::New();
  vtkObject* obj5 = vtkObject::New();

  // NA for connection1
  int connection2_observeDeletion = 0;
  int connection3_observeDeletion = 1;
  int connection4_observeDeletion = 0;
  int connection5_observeDeletion = 1;

  QObject*   topObject = new QObject(0);
  ctkCallback*    slotObject = new ctkCallback(spy, topObject);

  for (int i = 0; i < objects; ++i)
    {
    // connection1: regular callback
    vtkCallbackCommand* callback = vtkCallbackCommand::New();
    callback->SetClientData(slotObject);
    callback->SetCallback(doit);
    obj1->AddObserver(vtkCommand::ModifiedEvent, callback);
    callback->Delete();

    // connection2
    ctkVTKConnection* connection2 = new ctkVTKConnection(topObject);
    connection2->observeDeletion(connection2_observeDeletion);
    connection2->setup(obj2, vtkCommand::ModifiedEvent,
                      slotObject, SLOT(invoke()));

    // connection3
    ctkVTKConnection* connection3 = new ctkVTKConnection(topObject);
    connection3->observeDeletion(connection3_observeDeletion);
    connection3->setup(obj3, vtkCommand::ModifiedEvent,
                      slotObject, SLOT(invoke()));

    // connection4
    ctkVTKConnection* connection4 = new ctkVTKConnection(topObject);
    connection4->observeDeletion(connection4_observeDeletion);
    connection4->setup(obj4, vtkCommand::ModifiedEvent,
                      new ctkCallback(spy, topObject), SLOT(invoke()));

    // connection5
    ctkVTKConnection* connection5 = new ctkVTKConnection(topObject);
    connection5->observeDeletion(connection5_observeDeletion);
    connection5->setup(obj5, vtkCommand::ModifiedEvent,
                      new ctkCallback(spy, topObject), SLOT(invoke()));
    }

  // Compute timing for connection1: Callback only
  double time_connection1 = 0;
  if (testCase == -1 || testCase == 1)
    {
    if (!computeConnectionTiming("connection1", events , objects, obj1, time_connection1))
      {
      return EXIT_FAILURE;
      }
    }

  // Compute timing for connection2
  // observeDeletion = 0
  double time_connection2 = 0;
  if (testCase == -1 || testCase == 2)
    {
    if (!computeConnectionTiming("connection2", events , objects, obj2, time_connection2))
      {
      return EXIT_FAILURE;
      }
    }

  // Compute timing for connection3
  // observeDeletion = 1
  if (testCase == -1 || testCase == 3)
    {
    if (!computeConnectionTiming("connection3", events , objects, obj3))
      {
      return EXIT_FAILURE;
      }
    }

  // Compute timing for connection4 - 1-1
  // observeDeletion = 0
  if (testCase == -1 || testCase == 4)
    {
    if (!computeConnectionTiming("connection4", events , objects, obj4))
      {
      return EXIT_FAILURE;
      }
    }

  // Compute timing for connection5 - 1-1
  // observeDeletion = 1
  if (testCase == -1 || testCase == 5)
    {
    if (!computeConnectionTiming("connection5", events , objects, obj4))
      {
      return EXIT_FAILURE;
      }
    }

  obj1->Delete();
  obj2->Delete();
  obj3->Delete();

  delete topObject;

  obj4->Delete();
  obj5->Delete();

  return EXIT_SUCCESS;
}
