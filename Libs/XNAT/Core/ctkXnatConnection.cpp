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

#include <QScopedPointer>
#include <QStringBuilder>

#include <qXnatAPI.h>
#include <qRestResult.h>

class ctkXnatConnectionPrivate
{
public:
  QString profileName;
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

  // TODO This is a workaround for connecting to sites with self-signed
  // certificate. Should be replaced with something more clever.  
  d->xnat->setSuppressSslErrors(true);
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
//  qDebug() << "ctkXnatConnection::progress(QUuid queryId, double progress)";
//  qDebug() << "query id:" << queryId;
//  qDebug() << "progress:" << (progress * 100.0) << "%";
}

QString ctkXnatConnection::profileName() const
{
  Q_D(const ctkXnatConnection);
  return d->profileName;
}

void ctkXnatConnection::setProfileName(const QString& profileName)
{
  Q_D(ctkXnatConnection);
  d->profileName = profileName;
  d->server->setProperty("name", profileName);
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
  d->server->setProperty("ID", url);
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

  QString projectsUri("/data/archive/projects");

  QUuid queryId = d->xnat->get(projectsUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);
  QList<ctkXnatProject*> projects = restResult->results<ctkXnatProject>();

  foreach (ctkXnatProject* project, projects)
  {
    project->setUri(projectsUri + "/" + project->id());
    ctkXnatObject::Pointer ptr(project);
    ptr->d_func()->selfPtr = ptr;
    server->addChild(ptr);
  }

  delete restResult;
}

void ctkXnatConnection::fetch(const ctkXnatProject::Pointer& project)
{
  Q_D(ctkXnatConnection);

  QString subjectsUri = project->uri() + "/subjects";
  QUuid queryId = d->xnat->get(subjectsUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);
  QList<ctkXnatSubject*> subjects = restResult->results<ctkXnatSubject>();

  foreach (ctkXnatSubject* subject, subjects)
  {
    QString label = subject->property("label");
    if (label.size())
    {
      subject->setProperty("ID", label);
    }
    subject->setUri(subjectsUri + "/" + subject->id());

    ctkXnatObject::Pointer ptr(subject);
    ptr->d_func()->selfPtr = ptr;
    project->addChild(ptr);
  }

  delete restResult;
}

void ctkXnatConnection::fetch(const QSharedPointer<ctkXnatSubject>& subject)
{
  Q_D(ctkXnatConnection);

  QString experimentsUri = subject->uri() + "/experiments";
  QUuid queryId = d->xnat->get(experimentsUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);

  QList<ctkXnatExperiment*> experiments = restResult->results<ctkXnatExperiment>();

  foreach (ctkXnatExperiment* experiment, experiments)
  {
    QString label = experiment->property ("label");
    if (label.size())
    {
      experiment->setProperty ("ID", label);
    }
    experiment->setUri(experimentsUri + "/" + experiment->id());

    ctkXnatObject::Pointer ptr(experiment);
    ptr->d_func()->selfPtr = ptr;
    subject->addChild(ptr);
  }

  delete restResult;
}

void ctkXnatConnection::fetch(const QSharedPointer<ctkXnatExperiment>& experiment)
{
  Q_D(ctkXnatConnection);
  
  QString scansUri = experiment->uri() + "/scans";
  QUuid scansQueryId = d->xnat->get(scansUri);
  qRestResult* restResult = d->xnat->takeResult(scansQueryId);
  
  QList<ctkXnatScan*> scans = restResult->results<ctkXnatScan>();
  
  if (scans.size() > 0)
  {
    ctkXnatObject::Pointer ptr = ctkXnatScanFolder::Create();

    ptr->setUri(scansUri);

    ptr->d_func()->selfPtr = ptr;
    experiment->addChild(ptr);
  }
  
  delete restResult;

  QString reconstructionsUri = experiment->uri() + "/reconstructions";
  QUuid reconstructionsQueryId = d->xnat->get(reconstructionsUri);
  restResult = d->xnat->takeResult(reconstructionsQueryId);

  QList<ctkXnatReconstruction*> reconstructions = restResult->results<ctkXnatReconstruction>();

  if (reconstructions.size() > 0)
  {
    ctkXnatObject::Pointer ptr = ctkXnatReconstructionFolder::Create();

    ptr->setUri(reconstructionsUri);

    ptr->d_func()->selfPtr = ptr;
    experiment->addChild(ptr);
  }

  delete restResult;
}

void ctkXnatConnection::fetch(const QSharedPointer<ctkXnatScanFolder>& scanFolder)
{
  Q_D(ctkXnatConnection);
  
  QString scansUri = scanFolder->uri();
  QUuid queryId = d->xnat->get(scansUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);

  QList<ctkXnatScan*> scans = restResult->results<ctkXnatScan>();
  
  foreach (ctkXnatScan* scan, scans)
  {
    scan->setUri(scansUri + "/" + scan->id());

    ctkXnatObject::Pointer ptr(scan);
    ptr->d_func()->selfPtr = ptr;
    scanFolder->addChild(ptr);
  }
  
  delete restResult;
  
}

void ctkXnatConnection::fetch(const QSharedPointer<ctkXnatScan>& scan)
{
  Q_D(ctkXnatConnection);
  
  QString scanResourcesUri = scan->uri() + "/resources";
  QUuid queryId = d->xnat->get(scanResourcesUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);
  
  QList<ctkXnatScanResource*> scanResources = restResult->results<ctkXnatScanResource>();
  
  foreach (ctkXnatScanResource* scanResource, scanResources)
  {
    QString label = scanResource->property("label");
    if (label.size())
    {
      scanResource->setProperty("ID", label);
    }
    
    scanResource->setUri(scanResourcesUri + "/" + label);

    ctkXnatObject::Pointer ptr(scanResource);
    ptr->d_func()->selfPtr = ptr;
    scan->addChild(ptr);
  }
}

void ctkXnatConnection::fetch(const QSharedPointer<ctkXnatScanResource>& scanResource)
{
  Q_D(ctkXnatConnection);
  
  QString scanResourceFilesUri = scanResource->uri() + "/files";
  QUuid queryId = d->xnat->get(scanResourceFilesUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);
  
  QList<ctkXnatScanResourceFile*> scanResourceFiles = restResult->results<ctkXnatScanResourceFile>();

  foreach (ctkXnatScanResourceFile* scanResourceFile, scanResourceFiles)
  {
    QString uri = scanResourceFile->property("URI");
    if (uri.size())
    {
      scanResourceFile->setUri(uri);
    }

    QString label = scanResourceFile->property("Name");
    if (label.size())
    {
      scanResourceFile->setProperty("ID", label);
    }

    scanResourceFile->setUri(scanResourceFilesUri + "/" + label);

    ctkXnatObject::Pointer ptr(scanResourceFile);
    ptr->d_func()->selfPtr = ptr;
    scanResource->addChild(ptr);
  }
}

void ctkXnatConnection::fetch(const QSharedPointer<ctkXnatReconstructionFolder>& reconstructionFolder)
{
  Q_D(ctkXnatConnection);

  QString reconstructionsUri = reconstructionFolder->uri();
  QUuid queryId = d->xnat->get(reconstructionsUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);

  QList<ctkXnatReconstruction*> reconstructions = restResult->results<ctkXnatReconstruction>();

  foreach (ctkXnatReconstruction* reconstruction, reconstructions)
  {
    reconstruction->setUri(reconstructionsUri + "/" + reconstruction->id());

    ctkXnatObject::Pointer ptr(reconstruction);
    ptr->d_func()->selfPtr = ptr;
    reconstructionFolder->addChild(ptr);
  }

  delete restResult;

}

void ctkXnatConnection::fetch(const QSharedPointer<ctkXnatReconstruction>& reconstruction)
{
  Q_D(ctkXnatConnection);

  QString reconstructionResourcesUri = reconstruction->uri() + "/resources";
  QUuid queryId = d->xnat->get(reconstructionResourcesUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);

  QList<ctkXnatReconstructionResource*> reconstructionResources = restResult->results<ctkXnatReconstructionResource>();

  foreach (ctkXnatReconstructionResource* reconstructionResource, reconstructionResources)
  {
    QString label = reconstructionResource->property("label");
    if (label.size())
    {
      reconstructionResource->setProperty("ID", label);
    }

    reconstructionResource->setUri(reconstructionResourcesUri + "/" + label);

    ctkXnatObject::Pointer ptr(reconstructionResource);
    ptr->d_func()->selfPtr = ptr;
    reconstruction->addChild(ptr);
  }
}

void ctkXnatConnection::fetch(const QSharedPointer<ctkXnatReconstructionResource>& reconstructionResource)
{
  Q_D(ctkXnatConnection);

  QString reconstructionResourceFilesUri = reconstructionResource->uri() + "/files";
  QUuid queryId = d->xnat->get(reconstructionResourceFilesUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);

  QList<ctkXnatReconstructionResourceFile*> reconstructionResourceFiles = restResult->results<ctkXnatReconstructionResourceFile>();

  foreach (ctkXnatReconstructionResourceFile* reconstructionResourceFile, reconstructionResourceFiles)
  {
    QString uri = reconstructionResourceFile->property("URI");
    if (uri.size())
    {
      reconstructionResourceFile->setUri(uri);
    }

    QString label = reconstructionResourceFile->property("Name");
    if (label.size())
    {
      reconstructionResourceFile->setProperty("ID", label);
    }

    reconstructionResourceFile->setUri(reconstructionResourceFilesUri + "/" + label);

    ctkXnatObject::Pointer ptr(reconstructionResourceFile);
    ptr->d_func()->selfPtr = ptr;
    reconstructionResource->addChild(ptr);
  }
}

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

void ctkXnatConnection::download(ctkXnatScanResource* scanResource, const QString& fileName)
{
  Q_D(ctkXnatConnection);

  QString query = scanResource->uri() + "/files";
  qRestAPI::Parameters parameters;
  parameters["format"] = "zip";
  QUuid queryId = d->xnat->download(fileName, query, parameters);
  d->xnat->sync(queryId);
}

void ctkXnatConnection::download(ctkXnatScanResourceFile* scanResourceFile, const QString& fileName)
{
  Q_D(ctkXnatConnection);
  QString query = scanResourceFile->uri();

  QUuid queryId = d->xnat->download(fileName, query);
  d->xnat->sync(queryId);
}

void ctkXnatConnection::download(ctkXnatReconstructionResource* reconstructionResource, const QString& fileName)
{
  Q_D(ctkXnatConnection);

  QString query = reconstructionResource->uri() + "/files";
  qRestAPI::Parameters parameters;
  parameters["format"] = "zip";
  QUuid queryId = d->xnat->download(fileName, query, parameters);
  d->xnat->sync(queryId);
}

void ctkXnatConnection::download(ctkXnatReconstructionResourceFile* reconstructionResourceFile, const QString& fileName)
{
  Q_D(ctkXnatConnection);
  QString query = reconstructionResourceFile->uri();

  QUuid queryId = d->xnat->download(fileName, query);
  d->xnat->sync(queryId);
}

void ctkXnatConnection::processResult(QUuid queryId, QList<QVariantMap> parameters)
{
  Q_UNUSED(queryId)
  Q_UNUSED(parameters)
}
