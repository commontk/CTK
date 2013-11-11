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

#include "ctkXnatSessionTest.h"

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

#include <ctkXnatDataModel.h>
#include <ctkXnatSession.h>
#include <ctkXnatSessionFactory.h>
#include <ctkXnatProject.h>
#include <ctkXnatSubject.h>

class ctkXnatSessionTestCasePrivate
{
public:
  ctkXnatSessionFactory* SessionFactory;
  ctkXnatSession* Session;

  QString ServerUri;
  QString UserName;
  QString Password;

  QString Project;
  QString Subject;
  QString Experiment;
};

// --------------------------------------------------------------------------
ctkXnatSessionTestCase::ctkXnatSessionTestCase()
: d_ptr(new ctkXnatSessionTestCasePrivate())
{
}

// --------------------------------------------------------------------------
ctkXnatSessionTestCase::~ctkXnatSessionTestCase()
{
}

// --------------------------------------------------------------------------
void ctkXnatSessionTestCase::initTestCase()
{
  Q_D(ctkXnatSessionTestCase);

  d->ServerUri = "https://central.xnat.org";
  d->UserName = "ctk";
  d->Password = "ctk";

  d->SessionFactory = new ctkXnatSessionFactory();
  d->Session = d->SessionFactory->makeConnection(d->ServerUri, d->UserName, d->Password);
  d->Session->setProfileName("ctk");
}

void ctkXnatSessionTestCase::cleanupTestCase()
{
  Q_D(ctkXnatSessionTestCase);

  delete d->Session;
  delete d->SessionFactory;
}

void ctkXnatSessionTestCase::testProjectList()
{
  Q_D(ctkXnatSessionTestCase);

  ctkXnatObject* dataModel = d->Session->dataModel();
  dataModel->fetch();

  QList<ctkXnatObject*> projects = dataModel->children();

  QVERIFY(projects.size() > 0);
}

void ctkXnatSessionTestCase::testResourceUri()
{
  Q_D(ctkXnatSessionTestCase);

  ctkXnatObject* dataModel = d->Session->dataModel();
  QVERIFY(!dataModel->resourceUri().isNull());
  QVERIFY(dataModel->resourceUri().isEmpty());
}

void ctkXnatSessionTestCase::testParentChild()
{
  Q_D(ctkXnatSessionTestCase);

  ctkXnatDataModel* dataModel = d->Session->dataModel();

  ctkXnatProject* project = new ctkXnatProject(dataModel);

  QVERIFY(project->parent() == dataModel);

  QVERIFY(dataModel->children().contains(project));

  dataModel->add(project);

  int numberOfOccurrences = 0;
  foreach (ctkXnatObject* serverProject, dataModel->children())
  {
    if (serverProject == project || serverProject->id() == project->id())
    {
      ++numberOfOccurrences;
    }
  }
  QVERIFY(numberOfOccurrences == 1);

  dataModel->remove(project);
  numberOfOccurrences = 0;
  foreach (ctkXnatObject* serverProject, dataModel->children())
  {
    if (serverProject == project || serverProject->id() == project->id())
    {
      ++numberOfOccurrences;
    }
  }
  QVERIFY(numberOfOccurrences == 0);
  delete project;
}

void ctkXnatSessionTestCase::testCreateProject()
{
  Q_D(ctkXnatSessionTestCase);

  ctkXnatDataModel* dataModel = d->Session->dataModel();

  QString projectId = QString("CTK_") + QUuid::createUuid().toString().mid(1, 8);
  d->Project = projectId;

  ctkXnatProject* project = new ctkXnatProject(dataModel);
  project->setId(projectId);
  project->setName(projectId);
  project->setDescription("CTK_test_project");

  bool exists = d->Session->exists(project);
  QVERIFY(!exists);

  d->Session->save(project);

  exists = d->Session->exists(project);
  QVERIFY(exists);

  d->Session->remove(project);

  exists = d->Session->exists(project);
  QVERIFY(!exists);
}

void ctkXnatSessionTestCase::testCreateSubject()
{
  Q_D(ctkXnatSessionTestCase);

  ctkXnatDataModel* dataModel = d->Session->dataModel();

  QString projectId = QString("CTK_") + QUuid::createUuid().toString().mid(1, 8);
  d->Project = projectId;

  ctkXnatProject* project = new ctkXnatProject(dataModel);
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
int ctkXnatSessionTest(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  ctkXnatSessionTestCase test;
  return QTest::qExec(&test, argc, argv);
}
