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
#include <ctkXnatSubject.h>

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

void ctkXnatConnectionTestCase::testParentChild()
{
  Q_D(ctkXnatConnectionTestCase);

  ctkXnatServer* server = d->Connection->server();

  ctkXnatProject* project = new ctkXnatProject(server);

  QVERIFY(project->parent() == server);

  QVERIFY(server->children().contains(project));

  server->add(project);

  int numberOfOccurrences = 0;
  foreach (ctkXnatObject* serverProject, server->children())
  {
    if (serverProject == project || serverProject->id() == project->id())
    {
      ++numberOfOccurrences;
    }
  }
  QVERIFY(numberOfOccurrences == 1);

  server->remove(project);
  numberOfOccurrences = 0;
  foreach (ctkXnatObject* serverProject, server->children())
  {
    if (serverProject == project || serverProject->id() == project->id())
    {
      ++numberOfOccurrences;
    }
  }
  QVERIFY(numberOfOccurrences == 0);
  delete project;
}

void ctkXnatConnectionTestCase::testCreateProject()
{
  Q_D(ctkXnatConnectionTestCase);

  ctkXnatServer* server = d->Connection->server();

  QString projectId = QString("CTK_") + QUuid::createUuid().toString().mid(1, 8);
  d->Project = projectId;

  ctkXnatProject* project = new ctkXnatProject(server);
  project->setId(projectId);
  project->setName(projectId);
  project->setDescription("CTK_test_project");

  bool exists = d->Connection->exists(project);
  QVERIFY(!exists);

  d->Connection->save(project);

  exists = d->Connection->exists(project);
  QVERIFY(exists);

  d->Connection->remove(project);

  exists = d->Connection->exists(project);
  QVERIFY(!exists);
}

void ctkXnatConnectionTestCase::testCreateSubject()
{
  Q_D(ctkXnatConnectionTestCase);

  ctkXnatServer* server = d->Connection->server();

  QString projectId = QString("CTK_") + QUuid::createUuid().toString().mid(1, 8);
  d->Project = projectId;

  ctkXnatProject* project = new ctkXnatProject(server);
  project->setId(projectId);
  project->setName(projectId);
  project->setDescription("CTK_test_project");

  QVERIFY(!project->exists());

  project->save();

  QVERIFY(project->exists());

  ctkXnatSubject* subject = new ctkXnatSubject(project);

  QString subjectName = QString("CTK_S") + QUuid::createUuid().toString().mid(1, 8);
  subject->setId(subjectName);
  subject->setName(subjectName);

  subject->save();

  QVERIFY(!subject->id().isNull());

  subject->erase();

  QVERIFY(!subject->exists());

  project->erase();

  QVERIFY(!project->exists());
}

// --------------------------------------------------------------------------
int ctkXnatConnectionTest(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  ctkXnatConnectionTestCase test;
  return QTest::qExec(&test, argc, argv);
}
