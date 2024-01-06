/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=============================================================================*/

// Qt includes
#include <QCoreApplication>

// ctk includes
#include "ctkCoreTestingMacros.h"

// ctkDICOMCore includes
#include "ctkDICOMServer.h"

int ctkDICOMServerTest1(int argc, char * argv []) {

  QCoreApplication app(argc, argv);

  ctkDICOMServer server;
  // Test the default values
  CHECK_QSTRING(server.connectionName(), "");
  CHECK_QSTRING(server.callingAETitle(), "");
  CHECK_QSTRING(server.calledAETitle(), "");
  CHECK_QSTRING(server.host(), "");
  CHECK_QSTRING(server.retrieveProtocolAsString(), "CGET");
  CHECK_QSTRING(server.moveDestinationAETitle(), "");
  CHECK_INT(server.port(), 80);
  CHECK_INT(server.connectionTimeout(), 10);
  CHECK_BOOL(server.queryRetrieveEnabled(), true);
  CHECK_BOOL(server.storageEnabled(), true);
  CHECK_BOOL(server.keepAssociationOpen(), false);

  // Test setting and getting
  server.setConnectionName("connectionName");
  CHECK_QSTRING(server.connectionName(), "connectionName");
  server.setCallingAETitle("callingAETitle");
  CHECK_QSTRING(server.callingAETitle(), "callingAETitle");
  server.setCalledAETitle("calledAETitle");
  CHECK_QSTRING(server.calledAETitle(), "calledAETitle");
  server.setHost("host");
  CHECK_QSTRING(server.host(), "host");
  server.setRetrieveProtocolAsString("CMOVE");
  CHECK_QSTRING(server.retrieveProtocolAsString(), "CMOVE");
  server.setMoveDestinationAETitle("moveDestinationAETitle");
  CHECK_QSTRING(server.moveDestinationAETitle(), "moveDestinationAETitle");
  server.setPort(11112);
  CHECK_INT(server.port(), 11112);
  server.setConnectionTimeout(30);
  CHECK_INT(server.connectionTimeout(), 30);
  server.setQueryRetrieveEnabled(false);
  CHECK_BOOL(server.queryRetrieveEnabled(), false);
  server.setStorageEnabled(false);
  CHECK_BOOL(server.storageEnabled(), false);
  server.setKeepAssociationOpen(true);
  CHECK_BOOL(server.keepAssociationOpen(), true);

  return EXIT_SUCCESS;
}

