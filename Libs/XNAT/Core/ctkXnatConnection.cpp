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
#include "ctkXnatFile.h"
#include "ctkXnatObject.h"
#include "ctkXnatProject.h"
#include "ctkXnatReconstruction.h"
#include "ctkXnatReconstructionFolder.h"
#include "ctkXnatReconstructionResource.h"
#include "ctkXnatScan.h"
#include "ctkXnatScanFolder.h"
#include "ctkXnatScanResource.h"
#include "ctkXnatServer.h"
#include "ctkXnatSubject.h"

#include <QDebug>
#include <QScopedPointer>
#include <QStringBuilder>

#include <ctkXnatAPI_p.h>
#include <qRestResult.h>

class ctkXnatConnectionPrivate
{
public:
  QString profileName;
  QString url;
  QString userName;
  QString password;

  ctkXnatAPI* xnat;
  ctkXnatAPI::RawHeaders rawHeaders;

  ctkXnatServer* server;
};

// ctkXnatConnection class

ctkXnatConnection::ctkXnatConnection()
: d_ptr(new ctkXnatConnectionPrivate())
{
  Q_D(ctkXnatConnection);
  d->xnat = new ctkXnatAPI();

  // TODO This is a workaround for connecting to sites with self-signed
  // certificate. Should be replaced with something more clever.  
  d->xnat->setSuppressSslErrors(true);
  d->rawHeaders["User-Agent"] = "Qt";
  d->xnat->setDefaultRawHeaders(d->rawHeaders);

  d->server = new ctkXnatServer(this);
  createConnections();
}

ctkXnatConnection::~ctkXnatConnection()
{
  Q_D(ctkXnatConnection);
  delete d->server;
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

ctkXnatServer* ctkXnatConnection::server() const
{
  Q_D(const ctkXnatConnection);
  return d->server;
}

void ctkXnatConnection::fetch(ctkXnatServer* server)
{
  Q_D(ctkXnatConnection);

  QString projectsUri("/data/archive/projects");

  QUuid queryId = d->xnat->get(projectsUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);
  QList<ctkXnatProject*> projects = restResult->results<ctkXnatProject>();

  qDebug() << "ctkXnatConnection::fetch(ctkXnatServer* server): project number:" << projects.size();

  foreach (ctkXnatProject* project, projects)
  {
    server->add(project);
  }

  delete restResult;
}

void ctkXnatConnection::fetch(ctkXnatProject* project)
{
  Q_D(ctkXnatConnection);

  QString subjectsUri = project->resourceUri() + "/subjects";
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

    project->add(subject);
  }

  delete restResult;
}

void ctkXnatConnection::fetch(ctkXnatSubject* subject)
{
  Q_D(ctkXnatConnection);

  QString experimentsUri = subject->resourceUri() + "/experiments";
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

    subject->add(experiment);
  }

  delete restResult;
}

void ctkXnatConnection::fetch(ctkXnatExperiment* experiment)
{
  Q_D(ctkXnatConnection);
  
  QString scansUri = experiment->resourceUri() + "/scans";
  QUuid scansQueryId = d->xnat->get(scansUri);
  qRestResult* restResult = d->xnat->takeResult(scansQueryId);
  
  QList<ctkXnatScan*> scans = restResult->results<ctkXnatScan>();
  
  if (scans.size() > 0)
  {
    ctkXnatScanFolder* scanFolder = new ctkXnatScanFolder();
    experiment->add(scanFolder);
  }
  
  delete restResult;

  QString reconstructionsUri = experiment->resourceUri() + "/reconstructions";
  QUuid reconstructionsQueryId = d->xnat->get(reconstructionsUri);
  restResult = d->xnat->takeResult(reconstructionsQueryId);

  QList<ctkXnatReconstruction*> reconstructions = restResult->results<ctkXnatReconstruction>();

  if (reconstructions.size() > 0)
  {
    ctkXnatReconstructionFolder* reconstructionFolder = new ctkXnatReconstructionFolder();
    experiment->add(reconstructionFolder);
  }

  delete restResult;
}

void ctkXnatConnection::fetch(ctkXnatScanFolder* scanFolder)
{
  Q_D(ctkXnatConnection);
  
  QString scansUri = scanFolder->resourceUri();
  QUuid queryId = d->xnat->get(scansUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);

  QList<ctkXnatScan*> scans = restResult->results<ctkXnatScan>();
  
  foreach (ctkXnatScan* scan, scans)
  {
    scanFolder->add(scan);
  }
  
  delete restResult;
  
}

void ctkXnatConnection::fetch(ctkXnatScan* scan)
{
  Q_D(ctkXnatConnection);
  
  QString scanResourcesUri = scan->resourceUri() + "/resources";
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
    scan->add(scanResource);
  }
}

void ctkXnatConnection::fetch(ctkXnatScanResource* scanResource)
{
  Q_D(ctkXnatConnection);
  
  QString scanResourceFilesUri = scanResource->resourceUri() + "/files";
  QUuid queryId = d->xnat->get(scanResourceFilesUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);
  
  QList<ctkXnatFile*> files = restResult->results<ctkXnatFile>();

  foreach (ctkXnatFile* file, files)
  {
    QString label = file->property("Name");
    if (label.size())
    {
      file->setProperty("ID", label);
    }
    scanResource->add(file);
  }
}

void ctkXnatConnection::fetch(ctkXnatReconstructionFolder* reconstructionFolder)
{
  Q_D(ctkXnatConnection);

  QString reconstructionsUri = reconstructionFolder->resourceUri();
  QUuid queryId = d->xnat->get(reconstructionsUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);

  QList<ctkXnatReconstruction*> reconstructions = restResult->results<ctkXnatReconstruction>();

  foreach (ctkXnatReconstruction* reconstruction, reconstructions)
  {
    reconstructionFolder->add(reconstruction);
  }

  delete restResult;

}

void ctkXnatConnection::fetch(ctkXnatReconstruction* reconstruction)
{
  Q_D(ctkXnatConnection);

  QString reconstructionResourcesUri = reconstruction->resourceUri() + "/resources";
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

    reconstruction->add(reconstructionResource);
  }
}

void ctkXnatConnection::fetch(ctkXnatReconstructionResource* reconstructionResource)
{
  Q_D(ctkXnatConnection);

  QString reconstructionResourceFilesUri = reconstructionResource->resourceUri() + "/files";
  QUuid queryId = d->xnat->get(reconstructionResourceFilesUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);

  QList<ctkXnatFile*> files = restResult->results<ctkXnatFile>();

  foreach (ctkXnatFile* file, files)
  {
    QString label = file->property("Name");
    if (label.size())
    {
      file->setProperty("ID", label);
    }

    reconstructionResource->add(file);
  }
}

bool ctkXnatConnection::exists(const ctkXnatObject* object)
{
  Q_D(ctkXnatConnection);

  QString query = object->resourceUri();
  bool success = d->xnat->sync(d->xnat->get(query));

  return success;
}

void ctkXnatConnection::save(ctkXnatObject* object)
{
  Q_D(ctkXnatConnection);

  QString query = object->resourceUri();

  query.append(QString("?%1=%2").arg("xsi:type", object->schemaType()));
  const QMap<QString, QString>& properties = object->properties();
  QMapIterator<QString, QString> itProperties(properties);
  while (itProperties.hasNext())
  {
    itProperties.next();
    query.append(QString("&%1=%2").arg(itProperties.key(), itProperties.value()));
  }

  qDebug() << "ctkXnatConnection::create() query:" << query;
  QUuid queryId = d->xnat->put(query);
  qRestResult* result = d->xnat->takeResult(queryId);

  if (!result || !result->error().isNull())
  {
    throw ctkXnatException("Error occurred while creating the data.");
  }

  const QList<QVariantMap>& maps = result->results();
  if (maps.size() == 1 && maps[0].size() == 1)
  {
    QVariant id = maps[0]["ID"];
    if (!id.isNull())
    {
      object->setId(id.toString());
    }
  }
}

void ctkXnatConnection::remove(ctkXnatObject* object)
{
  Q_D(ctkXnatConnection);

  QString query = object->resourceUri();
  bool success = d->xnat->sync(d->xnat->del(query));

  if (!success)
  {
    throw ctkXnatException("Error occurred while removing the data.");
  }
}

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

void ctkXnatConnection::download(ctkXnatFile* file, const QString& fileName)
{
  Q_D(ctkXnatConnection);
  QString query = file->resourceUri();

  QUuid queryId = d->xnat->download(fileName, query);
  d->xnat->sync(queryId);
}

void ctkXnatConnection::download(ctkXnatScanResource* scanResource, const QString& fileName)
{
  Q_D(ctkXnatConnection);

  QString query = scanResource->resourceUri() + "/files";
  qRestAPI::Parameters parameters;
  parameters["format"] = "zip";
  QUuid queryId = d->xnat->download(fileName, query, parameters);
  d->xnat->sync(queryId);
}

void ctkXnatConnection::download(ctkXnatReconstructionResource* reconstructionResource, const QString& fileName)
{
  Q_D(ctkXnatConnection);

  QString query = reconstructionResource->resourceUri() + "/files";
  qRestAPI::Parameters parameters;
  parameters["format"] = "zip";
  QUuid queryId = d->xnat->download(fileName, query, parameters);
  d->xnat->sync(queryId);
}

void ctkXnatConnection::processResult(QUuid queryId, QList<QVariantMap> parameters)
{
  Q_UNUSED(queryId)
  Q_UNUSED(parameters)
}
