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

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVariant>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMCore includes
#include "ctkDICOMScheduler.h"
#include "ctkDICOMServer.h"
#include "ctkDICOMServerNodeWidget2.h"

int ctkDICOMServerNodeWidget2Test1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();
  Q_UNUSED(testName);

  bool interactive = arguments.removeOne("-I");

  ctkDICOMServerNodeWidget2 widget;

  // Test the default values
  // Check the default values of storage AE title and port
  CHECK_QSTRING(widget.storageAETitle(), "CTKSTORE");
  CHECK_INT(widget.storagePort(), 11112);

  // Test setting and getting storage AE title
  widget.setStorageAETitle("MyStorage");
  CHECK_QSTRING(widget.storageAETitle(), "MyStorage");

  // Test setting and getting storage port
  widget.setStoragePort(12345);
  CHECK_INT(widget.storagePort(), 12345);

  // Test default servers
  ctkDICOMScheduler scheduler;
  widget.setScheduler(scheduler);
  CHECK_QSTRING(widget.getServerNameFromIndex(0), "ExampleHost");
  CHECK_BOOL(widget.getServer("ExampleHost")->queryRetrieveEnabled(), false);
  CHECK_BOOL(widget.getServer("ExampleHost")->storageEnabled(), false);
  CHECK_QSTRING(widget.getServer("ExampleHost")->callingAETitle(), "CTK");
  CHECK_QSTRING(widget.getServer("ExampleHost")->calledAETitle(), "AETITLE");
  CHECK_QSTRING(widget.getServer("ExampleHost")->host(), "dicom.example.com");
  CHECK_INT(widget.getServer("ExampleHost")->port(), 11112);
  CHECK_QSTRING(widget.getServer("ExampleHost")->retrieveProtocolAsString(), "CGET");
  CHECK_INT(widget.getServer("ExampleHost")->connectionTimeout(), 30);

  CHECK_QSTRING(widget.getServerNameFromIndex(1), "MedicalConnections");
  CHECK_BOOL(widget.getServer("MedicalConnections")->queryRetrieveEnabled(), false);
  CHECK_BOOL(widget.getServer("MedicalConnections")->storageEnabled(), false);
  CHECK_QSTRING(widget.getServer("MedicalConnections")->callingAETitle(), "CTK");
  CHECK_QSTRING(widget.getServer("MedicalConnections")->calledAETitle(), "ANYAE");
  CHECK_QSTRING(widget.getServer("MedicalConnections")->host(), "dicomserver.co.uk");
  CHECK_INT(widget.getServer("MedicalConnections")->port(), 104);
  CHECK_QSTRING(widget.getServer("MedicalConnections")->retrieveProtocolAsString(), "CGET");
  CHECK_INT(widget.getServer("MedicalConnections")->connectionTimeout(), 30);

  // Test adding and removing servers
  ctkDICOMServer* server = new ctkDICOMServer();
  server->setConnectionName("server");
  widget.addServer(server);
  CHECK_INT(widget.serversCount(), 3);
  CHECK_INT(widget.getServerIndexFromName("server"), 2);
  CHECK_QSTRING(widget.getServerNameFromIndex(2), "server");
  CHECK_QSTRING(widget.getServer("server")->connectionName(), server->connectionName());
  widget.removeServer("server");
  CHECK_INT(widget.serversCount(), 2);
  delete server;

  if (!interactive)
  {
    QTimer::singleShot(200, &app, SLOT(quit()));
  }

  return app.exec();
}
