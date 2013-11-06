/*=========================================================================

  Library:   CTK

  Copyright (c) 2013 University College London, Centre for Medical Image Computing

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

#include "ctkXnatConnectionTest.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QSignalSpy>
#include <QStringList>
#include <QTest>
#include <QTime>
#include <QTimer>
#include <QUrl>
#include <QUuid>

#include <ctkXnatConnection.h>
#include <ctkXnatConnectionFactory.h>
#include <ctkXnatProject.h>
#include <ctkXnatServer.h>

class ctkXnatConnectionTestCasePrivate
{
public:
  ctkXnatConnectionFactory* ConnectionFactory;
  ctkXnatConnection* Connection;

  QString ServerUri;
  QString UserName;
  QString Password;

  QString Project;
  QString Subject;
  QString Experiment;
};

// --------------------------------------------------------------------------
ctkXnatConnectionTestCase::ctkXnatConnectionTestCase()
: d_ptr(new ctkXnatConnectionTestCasePrivate())
{
}

// --------------------------------------------------------------------------
ctkXnatConnectionTestCase::~ctkXnatConnectionTestCase()
{
}

// --------------------------------------------------------------------------
void ctkXnatConnectionTestCase::initTestCase()
{
  Q_D(ctkXnatConnectionTestCase);

  d->ServerUri = "https://central.xnat.org";
  d->UserName = "ctk";
  d->Password = "ctk";

  d->ConnectionFactory = new ctkXnatConnectionFactory();
  d->Connection = d->ConnectionFactory->makeConnection(d->ServerUri, d->UserName, d->Password);
  d->Connection->setProfileName("ctk");
}

void ctkXnatConnectionTestCase::cleanupTestCase()
{
  Q_D(ctkXnatConnectionTestCase);

  delete d->ConnectionFactory;
}

void ctkXnatConnectionTestCase::testProjectList()
{
  Q_D(ctkXnatConnectionTestCase);

  ctkXnatServer* server = d->Connection->server();
  server->fetch();

  QList<ctkXnatObject*> projects = server->children();

  QVERIFY(projects.size() > 0);
}

void ctkXnatConnectionTestCase::testResourceUri()
{
  Q_D(ctkXnatConnectionTestCase);

  ctkXnatServer* server = d->Connection->server();
  QVERIFY(!server->resourceUri().isNull());
  QVERIFY(server->resourceUri().isEmpty());
}

void ctkXnatConnectionTestCase::testCreateProject()
{
  Q_D(ctkXnatConnectionTestCase);

  ctkXnatServer* server = d->Connection->server();

  QString projectId = QString("CTK_") + QUuid::createUuid().toString().mid(1, 8);
  d->Project = projectId;

  ctkXnatProject* project = new ctkXnatProject();
  project->setId(projectId);
  project->setName(projectId);
  project->setDescription("CTK test project");
  server->add(project);

  bool exists = d->Connection->exists(project);
  QVERIFY(!exists);

  d->Connection->create(project);

  exists = d->Connection->exists(project);
  QVERIFY(exists);
}

// --------------------------------------------------------------------------
int ctkXnatConnectionTest(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  ctkXnatConnectionTestCase test;
  return QTest::qExec(&test, argc, argv);
}
