/*=============================================================================

  Plugin: org.commontk.xnat

  Copyright (c) University College London,
    Centre for Medical Image Computing

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "ctkXnatConnection.h"

#include "ctkXnatException.h"
#include "ctkXnatExperiment.h"
#include "ctkXnatObject.h"
#include "ctkXnatObjectPrivate.h"
#include "ctkXnatProject.h"
#include "ctkXnatReconstruction.h"
#include "ctkXnatReconstructionFolder.h"
#include "ctkXnatReconstructionResource.h"
#include "ctkXnatReconstructionResourceFile.h"
#include "ctkXnatScan.h"
#include "ctkXnatScanFolder.h"
#include "ctkXnatScanResource.h"
#include "ctkXnatScanResourceFile.h"
#include "ctkXnatServer.h"
#include "ctkXnatSubject.h"

#include <QDebug>
#include <QScopedPointer>
#include <QStringBuilder>

#include <qXnatAPI.h>
#include <qRestResult.h>

class ctkXnatConnectionPrivate
{
public:
  QString url;
  QString userName;
  QString password;

  qXnatAPI* xnat;
  qXnatAPI::RawHeaders rawHeaders;

  QSharedPointer<ctkXnatServer> server;
};

// ctkXnatConnection class

ctkXnatConnection::ctkXnatConnection()
: d_ptr(new ctkXnatConnectionPrivate())
{
  Q_D(ctkXnatConnection);
  d->xnat = new qXnatAPI();
//  d->xnat->setSuppressSslErrors(true);
  d->rawHeaders["User-Agent"] = "Qt";
  d->xnat->setDefaultRawHeaders(d->rawHeaders);

  d->server = ctkXnatServer::Create(this);
  createConnections();
}

ctkXnatConnection::~ctkXnatConnection()
{
  Q_D(ctkXnatConnection);
  delete d->xnat;
}

void ctkXnatConnection::createConnections()
{
//  Q_D(ctkXnatConnection);
//  connect(d->xnat, SIGNAL(resultReceived(QUuid,QList<QVariantMap>)),
//           this, SLOT(processResult(QUuid,QList<QVariantMap>)));
//  connect(d->xnat, SIGNAL(progress(QUuid,double)),
//           this, SLOT(progress(QUuid,double)));
}

void ctkXnatConnection::progress(QUuid queryId, double progress)
{
  qDebug() << "ctkXnatConnection::progress(QUuid queryId, double progress)";
  qDebug() << "query id:" << queryId;
  qDebug() << "progress:" << (progress * 100.0) << "%";
}

QString ctkXnatConnection::url() const
{
  Q_D(const ctkXnatConnection);
  return d->url;
}

void ctkXnatConnection::setUrl(const QString& url)
{
  Q_D(ctkXnatConnection);
  d->url = url;
  d->xnat->setServerUrl(d->url);
}

QString ctkXnatConnection::userName() const
{
  Q_D(const ctkXnatConnection);
  return d->userName;
}

void ctkXnatConnection::setUserName(const QString& userName)
{
  Q_D(ctkXnatConnection);
  d->userName = userName;
  d->rawHeaders["Authorization"] = "Basic " +
      QByteArray(QString("%1:%2").arg(d->userName).arg(d->password).toAscii()).toBase64();
  d->xnat->setDefaultRawHeaders(d->rawHeaders);
}

QString ctkXnatConnection::password() const
{
  Q_D(const ctkXnatConnection);
  return d->password;
}

void ctkXnatConnection::setPassword(const QString& password)
{
  Q_D(ctkXnatConnection);
  d->password = password;
  d->rawHeaders["Authorization"] = "Basic " +
      QByteArray(QString("%1:%2").arg(d->userName).arg(d->password).toAscii()).toBase64();
  d->xnat->setDefaultRawHeaders(d->rawHeaders);
}

ctkXnatServer::Pointer ctkXnatConnection::server() const
{
  Q_D(const ctkXnatConnection);
  return d->server;
}

void ctkXnatConnection::fetch(const ctkXnatServer::Pointer& server)
{
  Q_D(ctkXnatConnection);
  //qDebug() << "ctkXnatConnection::fetch(ctkXnatServer* server)";

  QString query("/data/archive/projects");

  QUuid queryId = d->xnat->get(query);
  qRestResult* restResult = d->xnat->takeResult(queryId);
  QList<ctkXnatProject*> projects = restResult->results<ctkXnatProject>();

  foreach (ctkXnatProject* project, projects)
  {
    qDebug() << "Found project:" << project->getName();
    ctkXnatObject::Pointer ptr(project);
    ptr->d_func()->selfPtr = ptr;
    server->addChild(ptr);
  }

  delete restResult;
}

void ctkXnatConnection::fetch(const ctkXnatProject::Pointer& project)
{
  const QString& projectTitle = project->getId();

  Q_D(ctkXnatConnection);

  QString query = QString("/data/projects/%1/subjects").arg(projectTitle);
  QUuid queryId = d->xnat->get(query);
  qRestResult* restResult = d->xnat->takeResult(queryId);
  QList<ctkXnatSubject*> subjects = restResult->results<ctkXnatSubject>();

  foreach (ctkXnatSubject* subject, subjects)
  {
    ctkXnatObject::Pointer ptr(subject);
    ptr->d_func()->selfPtr = ptr;
    project->addChild(ptr);
  }

  delete restResult;
}

void ctkXnatConnection::fetch(const QSharedPointer<ctkXnatSubject>& subject)
{
  const QString& subjectName = subject->getId();
  const QString& projectName = subject->getParent()->getId();

  Q_D(ctkXnatConnection);

  QString query = QString("/REST/projects/%1/subjects/%2/experiments").arg(projectName, subjectName);
  QUuid queryId = d->xnat->get(query);
  qRestResult* restResult = d->xnat->takeResult(queryId);
  //QList<ctkXnatExperiment*> experiments = restResult->results<ctkXnatExperiment>();

  //foreach (ctkXnatExperiment* experiment, experiments)
  //{
  //  subject->addChild(experiment->id(), experiment);
  //}

  delete restResult;
}

//void ctkXnatConnection::fetch(ctkXnatExperiment* experiment)
//{
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/scans").arg(projectName, subjectName, experimentName);
//  QList<QVariantMap> result;
//  bool success = d->xnat->sync(d->xnat->get(query), result);
//  if (!success)
//  {
//    throw ctkXnatException("Error occurred while retrieving scan list from XNAT.");
//  }

//  int scanNumber = result.size();

//  query = QString("/REST/projects/%1/subjects/%2/experiments/%3/reconstructions").arg(projectName, subjectName, experimentName);
//  success = d->xnat->sync(d->xnat->get(query), result);
//  if (!success)
//  {
//    throw ctkXnatException("Error occurred while retrieving reconstruction list from XNAT.");
//  }

//  int reconstructionNumber = result.size();

//  if (scanNumber > 0)
//  {
//    ctkXnatScanFolder* scanFolder = new ctkXnatScanFolder(experiment);
//    experiment->addChild("Scan", scanFolder);
//  }

//  if (reconstructionNumber > 0)
//  {
//    ctkXnatReconstructionFolder* reconstructionFolder = new ctkXnatReconstructionFolder(experiment);
//    experiment->addChild("Reconstruction", reconstructionFolder);
//  }
//}

//void ctkXnatConnection::fetch(ctkXnatScanFolder* scanFolder)
//{
//  ctkXnatObject* experiment = scanFolder->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/scans").arg(projectName, subjectName, experimentName);
//  QList<QVariantMap> result;
//  bool success = d->xnat->sync(d->xnat->get(query), result);
//  if (!success)
//  {
//    throw ctkXnatException("Error occurred while retrieving scan list from XNAT.");
//  }

//  foreach (QVariantMap map, result)
//  {
//    ctkXnatScan* scan = new ctkXnatScan(scanFolder);
//    QString scanID = map["ID"].toString();
//    scanFolder->addChild(scanID, scan);
//  }
////  QUuid queryId = d->xnat->get(query);
////  qRestResult* restResult = d->xnat->takeResult(queryId);
////  QList<ctkXnatScan*> scans = restResult->results<ctkXnatScan>();
////
////  foreach (ctkXnatScan* scan, scans)
////  {
////    experiment->addChild(scan->id(), scan);
////  }
////  delete restResult;
//}

//void ctkXnatConnection::fetch(ctkXnatScan* scan)
//{
//  const QString& scanName = scan->getName();
//  ctkXnatObject* experiment = scan->getParent()->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/scans/%4/resources").arg(projectName, subjectName, experimentName, scanName);
//  QList<QVariantMap> result;
//  bool success = d->xnat->sync(d->xnat->get(query), result);
//  if (!success)
//  {
//    throw ctkXnatException("Error occurred while retrieving scan resource list from XNAT.");
//  }

//  foreach (QVariantMap map, result)
//  {
//    QString scanResourceLabel = map["label"].toString();
//    ctkXnatScanResource* scanResource = new ctkXnatScanResource(scan);
//    scan->addChild(scanResourceLabel, scanResource);
//  }
//}

//void ctkXnatConnection::fetch(ctkXnatScanResource* scanResource)
//{
//  const QString& resourceName = scanResource->getName();
//  ctkXnatObject* scan = scanResource->getParent();
//  const QString& scanName = scan->getName();
//  ctkXnatObject* experiment = scan->getParent()->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/scans/%4/resources/%5/files").arg(projectName, subjectName, experimentName, scanName, resourceName);
//  QList<QVariantMap> result;
//  bool success = d->xnat->sync(d->xnat->get(query), result);
//  if (!success)
//  {
//    throw ctkXnatException("Error occurred while retrieving scan resource file list from XNAT.");
//  }

//  foreach (QVariantMap map, result)
//  {
//    QString scanResourceFileName = map["Name"].toString();
//    ctkXnatScanResourceFile* scanResourceFile = new ctkXnatScanResourceFile(scanResource);
//    scanResource->addChild(scanResourceFileName, scanResourceFile);
//  }
//}

//void ctkXnatConnection::fetch(ctkXnatReconstructionFolder* reconstructionFolder)
//{
//  ctkXnatObject* experiment = reconstructionFolder->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/reconstructions").arg(projectName, subjectName, experimentName);
//  QList<QVariantMap> result;
//  bool success = d->xnat->sync(d->xnat->get(query), result);
//  if (!success)
//  {
//    throw ctkXnatException("Error occurred while retrieving reconstruction list from XNAT.");
//  }

//  foreach (QVariantMap map, result)
//  {
//    QString reconstructionID = map["ID"].toString();
//    ctkXnatReconstruction* reconstruction = new ctkXnatReconstruction(reconstructionFolder);
//    reconstructionFolder->addChild(reconstructionID, reconstruction);
//  }
//}

//void ctkXnatConnection::fetch(ctkXnatReconstruction* reconstruction)
//{
//  const QString& reconstructionName = reconstruction->getName();
//  ctkXnatObject* experiment = reconstruction->getParent()->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/reconstructions/%4/ALL/resources").arg(projectName, subjectName, experimentName, reconstructionName);
//  QList<QVariantMap> result;
//  bool success = d->xnat->sync(d->xnat->get(query), result);
//  if (!success)
//  {
//    throw ctkXnatException("Error occurred while retrieving reconstruction resource list from XNAT.");
//  }

//  foreach (QVariantMap map, result)
//  {
//    QString reconstructionResourceLabel = map["label"].toString();
//    ctkXnatReconstructionResource* reconstructionResource = new ctkXnatReconstructionResource(reconstruction);
//    reconstruction->addChild(reconstructionResourceLabel, reconstructionResource);
//  }
//}

//void ctkXnatConnection::fetch(ctkXnatReconstructionResource* reconstructionResource)
//{
//  const QString& resourceName = reconstructionResource->getName();
//  ctkXnatObject* reconstruction = reconstructionResource->getParent();
//  const QString& reconstructionName = reconstruction->getName();
//  ctkXnatObject* experiment = reconstruction->getParent()->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/reconstructions/%4/ALL/resources/%5/files").arg(projectName, subjectName, experimentName, reconstructionName, resourceName);
//  QList<QVariantMap> result;
//  bool success = d->xnat->sync(d->xnat->get(query), result);
//  if (!success)
//  {
//    throw ctkXnatException("Error occurred while retrieving reconstruction resource file list from XNAT.");
//  }

//  foreach (QVariantMap map, result)
//  {
//    QString reconstructionResourceFileName = map["Name"].toString();
//    ctkXnatReconstructionResourceFile* reconstructionResourceFile = new ctkXnatReconstructionResourceFile(reconstructionResource);
//    reconstructionResource->addChild(reconstructionResourceFileName, reconstructionResourceFile);
//  }
//}

//void ctkXnatConnection::create(ctkXnatProject* project)
//{
//  const QString& projectName = project->name();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1").arg(projectName);
//  bool success = d->xnat->sync(d->xnat->put(query));

//  if (!success)
//  {
//    throw ctkXnatException("Error occurred while creating the project.");
//  }
//}

//void ctkXnatConnection::remove(ctkXnatProject* project)
//{
//  const QString& projectName = project->name();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1").arg(projectName);
//  bool success = d->xnat->sync(d->xnat->del(query));

//  if (!success)
//  {
//    throw ctkXnatException("Error occurred while removing the project.");
//  }
//}

//void ctkXnatConnection::create(ctkXnatSubject* subject)
//{
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2").arg(projectName, subjectName);
//  bool success = d->xnat->sync(d->xnat->put(query));

//  if (!success)
//  {
//    throw ctkXnatException("Error occurred while creating the subject.");
//  }
//}

//void ctkXnatConnection::downloadScanFiles(ctkXnatExperiment* experiment, const QString& fileName)
//{
////  qDebug() << "ctkXnatConnection::downloadScanFiles(ctkXnatExperiment* experiment, const QString& zipFilename)";
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/scans/ALL/files").arg(projectName, subjectName, experimentName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

//void ctkXnatConnection::downloadReconstructionFiles(ctkXnatExperiment* experiment, const QString& fileName)
//{
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/reconstructions/ALL/files").arg(projectName, subjectName, experimentName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

//void ctkXnatConnection::addReconstruction(ctkXnatExperiment* /*experiment*/, const QString& /*reconstruction*/)
//{
////  const QString& experimentName = experiment->getName();
////  ctkXnatObject* subject = experiment->getParent();
////  const QString& subjectName = subject->getName();
////  ctkXnatObject* project = subject->getParent();
////  const QString& projectName = project->getName();

//  // TODO
//}

//void ctkXnatConnection::downloadReconstruction(ctkXnatReconstruction* reconstruction, const QString& fileName)
//{
//  const QString& reconstructionName = reconstruction->getName();
//  ctkXnatObject* experiment = reconstruction->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/reconstructions/%4/ALL/files").arg(projectName, subjectName, experimentName, reconstructionName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

//void ctkXnatConnection::addReconstructionResource(ctkXnatReconstruction* /*reconstruction*/, const QString& /*resourceName*/)
//{
////  const QString& reconstructionName = reconstruction->getName();
////  ctkXnatObject* experiment = reconstruction->getParent()->getParent();
////  const QString& experimentName = experiment->getName();
////  ctkXnatObject* subject = experiment->getParent();
////  const QString& subjectName = subject->getName();
////  ctkXnatObject* project = subject->getParent();
////  const QString& projectName = project->getName();

//  // TODO
//}

//void ctkXnatConnection::removeReconstruction(ctkXnatReconstruction* /*reconstruction*/)
//{
////  const QString& reconstructionName = reconstruction->getName();
////  ctkXnatObject* experiment = reconstruction->getParent()->getParent();
////  const QString& experimentName = experiment->getName();
////  ctkXnatObject* subject = experiment->getParent();
////  const QString& subjectName = subject->getName();
////  ctkXnatObject* project = subject->getParent();
////  const QString& projectName = project->getName();

//  // TODO
//}

//void ctkXnatConnection::downloadReconstructionResourceFiles(ctkXnatReconstructionResource* reconstructionResource, const QString& fileName)
//{
//  const QString& reconstructionResourceName = reconstructionResource->getName();
//  ctkXnatObject* reconstruction = reconstructionResource->getParent();
//  const QString& reconstructionName = reconstruction->getName();
//  ctkXnatObject* experiment = reconstruction->getParent()->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/reconstructions/%4/ALL/resources/%5/files").arg(projectName, subjectName, experimentName, reconstructionName, reconstructionResourceName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

//void ctkXnatConnection::uploadReconstructionResourceFiles(ctkXnatReconstructionResource* /*reconstructionResource*/, const QString& /*zipFilename*/)
//{
////  const QString& resourceName = reconstructionResource->getName();
////  ctkXnatObject* reconstruction = reconstructionResource->getParent();
////  const QString& reconstructionName = reconstruction->getName();
////  ctkXnatObject* experiment = reconstruction->getParent()->getParent();
////  const QString& experimentName = experiment->getName();
////  ctkXnatObject* subject = experiment->getParent();
////  const QString& subjectName = subject->getName();
////  ctkXnatObject* project = subject->getParent();
////  const QString& projectName = project->getName();

//  // TODO
//}

//void ctkXnatConnection::removeReconstructionResource(ctkXnatReconstructionResource* /*reconstructionResource*/)
//{
////  const QString& resourceName = reconstructionResource->getName();
////  ctkXnatObject* reconstruction = reconstructionResource->getParent();
////  const QString& reconstructionName = reconstruction->getName();
////  ctkXnatObject* experiment = reconstruction->getParent()->getParent();
////  const QString& experimentName = experiment->getName();
////  ctkXnatObject* subject = experiment->getParent();
////  const QString& subjectName = subject->getName();
////  ctkXnatObject* project = subject->getParent();
////  const QString& projectName = project->getName();

//  // TODO
//}

//void ctkXnatConnection::download(ctkXnatReconstructionResourceFile* reconstructionResourceFile, const QString& fileName)
//{
////  qDebug() <<  "ctkXnatConnection::download(ctkXnatReconstructionResourceFile* reconstructionResourceFile, const QString& zipFilename)";
//  const QString& reconstructionResourceFileName = reconstructionResourceFile->getName();
//  ctkXnatObject* reconstructionResource = reconstructionResourceFile->getParent();
//  const QString& reconstructionResourceName = reconstructionResource->getName();
//  ctkXnatObject* reconstruction = reconstructionResource->getParent();
//  const QString& reconstructionName = reconstruction->getName();
//  ctkXnatObject* experiment = reconstruction->getParent()->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/reconstructions/%4/resources/%5/files/%6").arg(projectName, subjectName, experimentName, reconstructionName, reconstructionResourceName, reconstructionResourceFileName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

//void ctkXnatConnection::remove(ctkXnatReconstructionResourceFile* /*reconstructionResourceFile*/)
//{
////  const QString& filename = reconstructionResourceFile->getName();
////  ctkXnatObject* reconstructionResource = reconstructionResourceFile->getParent();
////  const QString& resourceName = reconstructionResource->getName();
////  ctkXnatObject* reconstruction = reconstructionResource->getParent();
////  const QString& reconstructionName = reconstruction->getName();
////  ctkXnatObject* experiment = reconstruction->getParent()->getParent();
////  const QString& experimentName = experiment->getName();
////  ctkXnatObject* subject = experiment->getParent();
////  const QString& subjectName = subject->getName();
////  ctkXnatObject* project = subject->getParent();
////  const QString& projectName = project->getName();

//  // TODO
//}

//void ctkXnatConnection::download(ctkXnatScan* scan, const QString& fileName)
//{
////  qDebug() << "ctkXnatConnection::download(ctkXnatScan* scan, const QString& zipFilename)";
//  const QString& scanName = scan->getName();
//  ctkXnatObject* experiment = scan->getParent()->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/scans/%4/files").arg(projectName, subjectName, experimentName, scanName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

//void ctkXnatConnection::download(ctkXnatScanResource* scanResource, const QString& fileName)
//{
////  qDebug() << "ctkXnatConnection::download(ctkXnatScanResource* scanResource, const QString& zipFilename)";
//  const QString& scanResourceName = scanResource->getName();
//  ctkXnatObject* scan = scanResource->getParent();
//  const QString& scanName = scan->getName();
//  ctkXnatObject* experiment = scan->getParent()->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/scans/%4/resources/%5/files").arg(projectName, subjectName, experimentName, scanName, scanResourceName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

//void ctkXnatConnection::download(ctkXnatScanResourceFile* scanResourceFile, const QString& fileName)
//{
////  qDebug() << "ctkXnatConnection::download(ctkXnatScanResourceFile* scanResourceFile)";
//  const QString& scanResourceFileName = scanResourceFile->getName();
//  ctkXnatObject* scanResource = scanResourceFile->getParent();
//  const QString& scanResourceName = scanResource->getName();
//  ctkXnatObject* scan = scanResource->getParent();
//  const QString& scanName = scan->getName();
//  ctkXnatObject* experiment = scan->getParent()->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatConnection);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/scans/%4/resources/%5/files/%6").arg(projectName, subjectName, experimentName, scanName, scanResourceName, scanResourceFileName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

void ctkXnatConnection::processResult(QUuid /*queryId*/, QList<QVariantMap> /*parameters*/)
{
}
