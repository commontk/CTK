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
#include <QFileInfo>
#include <QStandardPaths>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMTester.h"

// STD includes
#include <iostream>
#include <cstdlib>


int ctkDICOMTesterTest1(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();
  Q_UNUSED(testName);

  ctkDICOMTester tester;

  // Usage: ctkDICOMTesterTest1 [<dcmqrscp>] [<configfile>]
  if (arguments.count() > 0)
    {
    QString dcmqrscpExecutable(arguments.at(0));
    tester.setDCMQRSCPExecutable(dcmqrscpExecutable);
    CHECK_QSTRING(tester.dcmqrscpExecutable(), dcmqrscpExecutable);
    }
  if (arguments.count() > 1)
    {
    QString dcmqrscpConfigFile(arguments.at(1));
    tester.setDCMQRSCPConfigFile(argv[2]);
    CHECK_QSTRING(tester.dcmqrscpConfigFile(), dcmqrscpConfigFile);
    }

  QString dcmqrscp(tester.dcmqrscpExecutable());
  QString dcmqrscpConf(tester.dcmqrscpConfigFile());

  if (dcmqrscp == "dcmqrscp")
    {
    // If not found, assume the executable is in the PATH
    dcmqrscp = QStandardPaths::findExecutable(dcmqrscp);
    }
  CHECK_BOOL(QFileInfo::exists(dcmqrscp), true);
  CHECK_BOOL(QFileInfo(dcmqrscp).isExecutable(), true);

  CHECK_BOOL(QFileInfo::exists(dcmqrscpConf), true);
  CHECK_BOOL(QFileInfo(dcmqrscpConf).isReadable(), true);

  QProcess* process = tester.startDCMQRSCP();

  // Check if dcmqrscp started
  CHECK_NOT_NULL(process);

  // Check if dcmqrscp can be stopped
  CHECK_BOOL(tester.stopDCMQRSCP(), true);

  // Check if dcmqrscp can be re-started
  process = tester.startDCMQRSCP();
  CHECK_NOT_NULL(process);

  // Check if dcmqrscp is already running (in that
  // case, it returns null)
  process = tester.startDCMQRSCP();
  CHECK_NULL(process);

  // Check if dcmqrscp can be stopped
  CHECK_BOOL(tester.stopDCMQRSCP(), true);

  // there should be no process to stop
  CHECK_BOOL(tester.stopDCMQRSCP(), false);

  return EXIT_SUCCESS;
}
