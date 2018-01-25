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
#include <QStringList>
#include <QTimer>

// CTK includes
#include <ctkCallback.h>

// CTKVTK includes
#include <ctkVTKConnection.h>

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
template<typename T>
bool check(int line, const char* valueName, T current, T expected)
{
  if (current != expected)
    {
    std::cerr << "Line " << line << "\n"
              << "\tcurrent " << valueName << ":" << current << "\n"
              << "\texpected " << valueName << ":" << expected
              << std::endl;
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
void displayDartMeasurement(const char* name, double value)
{
  std::cout << "<DartMeasurement name=\""<< name <<"\" "
            << "type=\"numeric/double\">"
            << value << "</DartMeasurement>" << std::endl;
}

//-----------------------------------------------------------------------------
bool computeTimingAfterObjectDelete(bool observeDeletion,
                                    int connectionCount,
                                    int objectCount,
                                    bool deleteVTK,
                                    bool deleteQt)
{
  vtkNew<vtkTimerLog> timerLog;

  QObject* topObject = new QObject(0);
  ctkCallback* slotObject = new ctkCallback(spy, topObject);

  QList< ctkVTKConnection* > connections;
  QList< vtkSmartPointer<vtkObject> > objects;

  total_event_count = 0;

  for (int connectionIdx = 0; connectionIdx < connectionCount; ++connectionIdx)
    {

    for (int objectIdx = 0; objectIdx < objectCount; ++objectIdx)
      {
      vtkNew<vtkObject> object;

      ctkVTKConnection* connection = new ctkVTKConnection(topObject);
      connection->observeDeletion(observeDeletion);
      connection->setup(object.GetPointer(), vtkCommand::ModifiedEvent,
                        slotObject, SLOT(invoke()));

      connections.append(connection);
      objects.append(object.GetPointer());
      }
    }

  QString attribute(observeDeletion ? "observeDeletion" : "noObserveDeletion");

  {
    timerLog->StartTimer();
    foreach(vtkSmartPointer<vtkObject> object, objects)
      {
      object->Modified();
      }
    timerLog->StopTimer();

    QString measurementName = QString("time_%1-%2-%3-%4").arg(
          "connection-object-modified").arg(attribute).arg(connectionCount).arg(objectCount);
    displayDartMeasurement(qPrintable(measurementName), timerLog->GetElapsedTime());

    if (!check<int>(__LINE__, "total_event_count",
                    /* current= */ total_event_count,
                    /* expected = */ connectionCount * objectCount))
      {
      return false;
      }
  }

  if (deleteVTK)
    {
    total_event_count = 0;

    timerLog->StartTimer();
    objects.clear();
    timerLog->StopTimer();

    QString measurementName = QString("time_%1-%2-%3-%4").arg(
          "connection-vtkobject-deleted").arg(attribute).arg(connectionCount).arg(objectCount);
    displayDartMeasurement(qPrintable(measurementName), timerLog->GetElapsedTime());

    if (!check<int>(__LINE__, "total_event_count",
                    /* current= */ total_event_count,
                    /* expected = */ 0))
      {
      return false;
      }

    if (!check<void*>(__LINE__, "connection_0_vtkobject",
                    /* current= */ connections.at(0)->vtkobject(),
                    /* expected = */ 0))
      {
      return false;
      }

    if (!check<void*>(__LINE__, "connection_last_vtkobject",
                    /* current= */ connections.last()->vtkobject(),
                    /* expected = */ 0))
      {
      return false;
      }
    }

  if (deleteQt)
    {
    total_event_count = 0;

    timerLog->StartTimer();
    QTimer::singleShot(0, slotObject, SLOT(deleteLater()));
    QCoreApplication::sendPostedEvents();
    QCoreApplication::processEvents();
    timerLog->StopTimer();

    QString measurementName = QString("time_%1-%2-%3-%4").arg(
          "connection-qtobject-deleted").arg(attribute).arg(connectionCount).arg(objectCount);
    displayDartMeasurement(qPrintable(measurementName), timerLog->GetElapsedTime());

    if (!check<int>(__LINE__, "total_event_count",
                    /* current= */ total_event_count,
                    /* expected = */ 0))
      {
      return false;
      }

    if (!check<void*>(__LINE__, "connection_0_qtobject",
                    /* current= */ connections.at(0)->object(),
                    /* expected = */ 0))
      {
      return false;
      }

    if (!check<void*>(__LINE__, "connection_last_qtobject",
                    /* current= */ connections.last()->object(),
                    /* expected = */ 0))
      {
      return false;
      }
    }

  delete topObject;

  return true;
}

} // end of anonymous namespace

//-----------------------------------------------------------------------------
int ctkVTKConnectionTestObjectDelete( int argc, char * argv [] )
{
  QCoreApplication app(argc, argv);

  int testCase = -1;
  if (argc > 1)
    {
    testCase = app.arguments().at(1).toInt();
    }

  int connectionCount = 1000;
  int objectCount = 100;

  if (testCase == -1 || testCase == 1)
    {
    if (!computeTimingAfterObjectDelete(/* observeDeletion = */ true,
                                        connectionCount, objectCount,
                                        /* deleteVTK = */ true,
                                        /* deleteQt = */ false))
      {
      return EXIT_FAILURE;
      }
    }

  if (testCase == -1 || testCase == 2)
    {
    if (!computeTimingAfterObjectDelete(/* observeDeletion = */ false,
                                        connectionCount, objectCount,
                                        /* deleteVTK = */ true,
                                        /* deleteQt = */ false))
      {
      return EXIT_FAILURE;
      }
    }

  if (testCase == -1 || testCase == 3)
    {
    if (!computeTimingAfterObjectDelete(/* observeDeletion = */ true,
                                        connectionCount, objectCount,
                                        /* deleteVTK = */ false,
                                        /* deleteQt = */ true))
      {
      return EXIT_FAILURE;
      }
    }

  if (testCase == -1 || testCase == 4)
    {
    if (!computeTimingAfterObjectDelete(/* observeDeletion = */ false,
                                        connectionCount, objectCount,
                                        /* deleteVTK = */ false,
                                        /* deleteQt = */ true))
      {
      return EXIT_FAILURE;
      }
    }

  return EXIT_SUCCESS;
}
