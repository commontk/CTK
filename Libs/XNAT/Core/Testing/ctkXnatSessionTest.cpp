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
#include <QCryptographicHash>
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
#include <ctkXnatException.h>
#include <ctkXnatFile.h>
#include <ctkXnatLoginProfile.h>
#include <ctkXnatProject.h>
#include <ctkXnatResource.h>
#include <ctkXnatResourceFolder.h>
#include <ctkXnatSession.h>
#include <ctkXnatSubject.h>

class ctkXnatSessionTestCasePrivate
{
public:
  ctkXnatSession* Session;

  ctkXnatLoginProfile LoginProfile;

  QString Project;
  QString Subject;
  QString Experiment;

  QDateTime DateTime;
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

  d->LoginProfile.setName("ctk");
  d->LoginProfile.setServerUrl(QString("https://central.xnat.org"));
  d->LoginProfile.setUserName("ctk");
  d->LoginProfile.setPassword("ctk-xnat2015");
}

// --------------------------------------------------------------------------
void ctkXnatSessionTestCase::init()
{
  Q_D(ctkXnatSessionTestCase);

  d->DateTime = QDateTime::currentDateTime();
  d->Session = new ctkXnatSession(d->LoginProfile);
  d->Session->open();
}

// --------------------------------------------------------------------------
void ctkXnatSessionTestCase::cleanupTestCase()
{
}

// --------------------------------------------------------------------------
void ctkXnatSessionTestCase::cleanup()
{
  Q_D(ctkXnatSessionTestCase);

  delete d->Session;
  d->Session = NULL;
}

// --------------------------------------------------------------------------
void ctkXnatSessionTestCase::testProjectList()
{
  Q_D(ctkXnatSessionTestCase);

  ctkXnatObject* dataModel = d->Session->dataModel();
  dataModel->fetch();

  QList<ctkXnatObject*> projects = dataModel->children();

  QVERIFY(projects.size() > 0);
}

// --------------------------------------------------------------------------
void ctkXnatSessionTestCase::testResourceUri()
{
  Q_D(ctkXnatSessionTestCase);

  ctkXnatObject* dataModel = d->Session->dataModel();
  QVERIFY(!dataModel->resourceUri().isNull());
  QVERIFY(dataModel->resourceUri().isEmpty());
}

// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
void ctkXnatSessionTestCase::testSession()
{
  Q_D(ctkXnatSessionTestCase);

  QVERIFY(d->Session->isOpen());
  QDateTime expirationDate = d->Session->expirationDate();

  QVERIFY(d->DateTime < expirationDate);

  QTest::qSleep(2000);

  QUuid uuid = d->Session->httpGet("/data/version");
  QVERIFY(!uuid.isNull());
  d->Session->httpSync(uuid);

  QVERIFY(expirationDate < d->Session->expirationDate());

  try
  {
    d->Session->httpSync(uuid);
    QFAIL("Exception for unknown uuid expected");
  }
  catch(const ctkInvalidArgumentException&)
  {}

  d->Session->close();
  try
  {
    d->Session->dataModel();
    QFAIL("Exception for closed session expected");
  }
  catch(const ctkXnatInvalidSessionException&)
  {}
}

// --------------------------------------------------------------------------
void ctkXnatSessionTestCase::testAuthenticationError()
{
  ctkXnatLoginProfile loginProfile;
  loginProfile.setName("error");
  loginProfile.setServerUrl(QString("https://central.xnat.org"));
  loginProfile.setUserName("x");
  loginProfile.setPassword("y");

  ctkXnatSession session(loginProfile);
  try
  {
    session.open();
    QFAIL("Authenication error exception expected");
  }
  catch (const ctkXnatAuthenticationException&)
  {}
}

// --------------------------------------------------------------------------
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

  project->save();

  exists = d->Session->exists(project);
  QVERIFY(exists);

  d->Session->remove(project);

  exists = d->Session->exists(project);
  QVERIFY(!exists);
}

// --------------------------------------------------------------------------
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
void ctkXnatSessionTestCase::testAddResourceFolder()
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

  ctkXnatResource* resource = project->addResourceFolder("TestResource", "testFormat", "testContent", "testTag1,testTag2");
  QVERIFY(resource->exists());
  QVERIFY(resource->name() == "TestResource");
  QVERIFY(resource->format() == "testFormat");
  QVERIFY(resource->content() == "testContent");
  QVERIFY(resource->tags() == "testTag1,testTag2");

  ctkXnatResourceFolder* folder = dynamic_cast<ctkXnatResourceFolder*>(resource->parent());
  QVERIFY(folder->exists());
  QVERIFY(folder != 0);
  QVERIFY(folder->name() == "Resources");

  project->erase();
  QVERIFY(!project->exists());
  QVERIFY(!folder->exists());
  QVERIFY(!resource->exists());
}

// --------------------------------------------------------------------------
void ctkXnatSessionTestCase::testUploadAndDownloadFile()
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

  ctkXnatResource* resource = project->addResourceFolder("TestResourceContainingData");
  QVERIFY(resource->exists());
  QVERIFY(resource->name() == "TestResourceContainingData");
  QVERIFY(resource->format() == "");
  QVERIFY(resource->content() == "");
  QVERIFY(resource->tags() == "");

  QString tempDirPath = QDir::tempPath() + QUuid::createUuid().toString().mid(1, 8);
  QString uploadFileName = tempDirPath + "/ctk_xnat_upload_" + QUuid::createUuid().toString().mid(1, 8) + ".txt";
  QString downloadFileName = tempDirPath + "/ctk_xnat_download_" + QUuid::createUuid().toString().mid(1, 8) + ".txt";

  QDir tempDir;
  if (tempDir.mkdir(tempDirPath))
  {
    QFile uploadedFile(uploadFileName);

    if (uploadedFile.open(QFile::ReadWrite))
    {
      QTextStream stream( &uploadedFile );
      stream << "Hi, I am a CTK test file! ;-)" << endl;

      QFileInfo fileInfo;
      fileInfo.setFile(uploadFileName);
      // Create xnatFile object
      ctkXnatFile* xnatFile = new ctkXnatFile(resource);
      xnatFile->setLocalFilePath(fileInfo.filePath());
      xnatFile->setName(fileInfo.fileName());
      xnatFile->setFileFormat("some format");
      xnatFile->setFileContent("some content");
      xnatFile->setFileTags("some, tags");
      resource->add(xnatFile);

      // Actual file upload
      xnatFile->save();

      QVERIFY(xnatFile->exists());

      xnatFile->download(downloadFileName);

      QFile downloadedFile(downloadFileName);

      QVERIFY(downloadedFile.exists());

      uploadedFile.close();
      if (downloadedFile.open(QFile::ReadOnly) && uploadedFile.open(QFile::ReadOnly))
      {
        QCryptographicHash hashUploaded(QCryptographicHash::Md5);
        QCryptographicHash hashDownloaded(QCryptographicHash::Md5);

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        hashUploaded.addData(&uploadedFile);
        hashDownloaded.addData(&downloadedFile);
#else
        hashUploaded.addData(uploadedFile.readAll());
        hashDownloaded.addData(downloadedFile.readAll());
#endif

        QString md5ChecksumUploaded(hashUploaded.result().toHex());
        QString md5ChecksumDownloaded(hashDownloaded.result().toHex());

        QVERIFY (md5ChecksumDownloaded == md5ChecksumUploaded);

        // Remove the data from XNAT
        project->erase();
        QVERIFY(!project->exists());
        QVERIFY(!resource->exists());
        QVERIFY(!xnatFile->exists());

        // Remove the local data
        uploadedFile.close();
        downloadedFile.close();
        uploadedFile.remove();
        downloadedFile.remove();
        tempDir.cdUp();
        tempDir.rmdir(tempDirPath);
      }
      else
      {
        qWarning()<<"Could not open files for validation! Could not finish test!";
      }
    }
    else
    {
      qWarning()<<"Could not create temporary file for upload! Could not finish test!";
      return;
    }
  }
  else
  {
    qWarning()<<"Could not create temporary directory! Could not finish test!";
  }
}

// --------------------------------------------------------------------------
int ctkXnatSessionTest(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  ctkXnatSessionTestCase test;
  return QTest::qExec(&test, argc, argv);
}
