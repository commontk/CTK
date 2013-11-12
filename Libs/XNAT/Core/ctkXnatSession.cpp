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

#include "ctkXnatSession.h"

#include "ctkXnatDataModel.h"
#include "ctkXnatException.h"
#include "ctkXnatExperiment.h"
#include "ctkXnatFile.h"
#include "ctkXnatLoginProfile.h"
#include "ctkXnatObject.h"
#include "ctkXnatProject.h"
#include "ctkXnatReconstruction.h"
#include "ctkXnatReconstructionFolder.h"
#include "ctkXnatReconstructionResource.h"
#include "ctkXnatScan.h"
#include "ctkXnatScanFolder.h"
#include "ctkXnatScanResource.h"
#include "ctkXnatSubject.h"

#include <QDebug>
#include <QScopedPointer>
#include <QStringBuilder>

#include <ctkXnatAPI_p.h>
#include <qRestResult.h>

class ctkXnatSessionPrivate
{
public:
  const ctkXnatLoginProfile loginProfile;

  ctkXnatAPI* xnat;
  ctkXnatDataModel* dataModel;

  ctkXnatSessionPrivate(const ctkXnatLoginProfile& loginProfile, ctkXnatSession* q);
  ~ctkXnatSessionPrivate();

  void throwXnatException(const QString& msg);
};

ctkXnatSessionPrivate::ctkXnatSessionPrivate(const ctkXnatLoginProfile& loginProfile, ctkXnatSession* q)
  : loginProfile(loginProfile)
  , xnat(new ctkXnatAPI())
  , dataModel(new ctkXnatDataModel(q))
{
  // TODO This is a workaround for connecting to sites with self-signed
  // certificate. Should be replaced with something more clever.
  xnat->setSuppressSslErrors(true);
}

ctkXnatSessionPrivate::~ctkXnatSessionPrivate()
{
  delete dataModel;
  delete xnat;
}

void ctkXnatSessionPrivate::throwXnatException(const QString& msg)
{
  QString errorMsg = msg.trimmed();
  if (!errorMsg.isEmpty())
  {
    errorMsg.append(' ');
  }
  errorMsg.append(xnat->errorString());

  switch (xnat->error())
  {
  case qRestAPI::TimeoutError:
    throw ctkXnatTimeoutException(errorMsg);
  case qRestAPI::ResponseParseError:
    throw ctkXnatResponseParseError(errorMsg);
  default:
    throw ctkRuntimeException(errorMsg);
  }
}

// ctkXnatSession class

ctkXnatSession::ctkXnatSession(const ctkXnatLoginProfile& loginProfile)
: d_ptr(new ctkXnatSessionPrivate(loginProfile, this))
{
  Q_D(ctkXnatSession);

  ctkXnatAPI::RawHeaders rawHeaders;
  rawHeaders["User-Agent"] = "Qt";
  rawHeaders["Authorization"] = "Basic " +
      QByteArray(QString("%1:%2").arg(d->loginProfile.userName())
                 .arg(d->loginProfile.password()).toAscii()).toBase64();
  d->xnat->setDefaultRawHeaders(rawHeaders);

  QString url = d->loginProfile.serverUrl().toString();
  d->xnat->setServerUrl(url);

  d->dataModel->setProperty("ID", url);

  createConnections();
}

ctkXnatSession::~ctkXnatSession()
{
}

void ctkXnatSession::createConnections()
{
//  Q_D(ctkXnatSession);
//  connect(d->xnat, SIGNAL(resultReceived(QUuid,QList<QVariantMap>)),
//           this, SLOT(processResult(QUuid,QList<QVariantMap>)));
//  connect(d->xnat, SIGNAL(progress(QUuid,double)),
  //           this, SLOT(progress(QUuid,double)));
}

ctkXnatLoginProfile ctkXnatSession::loginProfile() const
{
  Q_D(const ctkXnatSession);
  return d->loginProfile;
}

void ctkXnatSession::progress(QUuid /*queryId*/, double /*progress*/)
{
//  qDebug() << "ctkXnatSession::progress(QUuid queryId, double progress)";
//  qDebug() << "query id:" << queryId;
//  qDebug() << "progress:" << (progress * 100.0) << "%";
}

QUrl ctkXnatSession::url() const
{
  Q_D(const ctkXnatSession);
  return d->loginProfile.serverUrl();
}

QString ctkXnatSession::userName() const
{
  Q_D(const ctkXnatSession);
  return d->loginProfile.userName();
}

QString ctkXnatSession::password() const
{
  Q_D(const ctkXnatSession);
  return d->loginProfile.password();
}

ctkXnatDataModel* ctkXnatSession::dataModel() const
{
  Q_D(const ctkXnatSession);
  return d->dataModel;
}

void ctkXnatSession::fetch(ctkXnatDataModel* dataModel)
{
  Q_D(ctkXnatSession);

  QString projectsUri("/data/archive/projects");

  QUuid queryId = d->xnat->get(projectsUri);
  qRestResult* restResult = d->xnat->takeResult(queryId);
  QList<ctkXnatProject*> projects = restResult->results<ctkXnatProject>();

  qDebug() << "ctkXnatSession::fetch(ctkXnatDataModel* server): project number:" << projects.size();

  foreach (ctkXnatProject* project, projects)
  {
    dataModel->add(project);
  }

  delete restResult;
}

void ctkXnatSession::fetch(ctkXnatProject* project)
{
  Q_D(ctkXnatSession);

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

void ctkXnatSession::fetch(ctkXnatSubject* subject)
{
  Q_D(ctkXnatSession);

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

void ctkXnatSession::fetch(ctkXnatExperiment* experiment)
{
  Q_D(ctkXnatSession);

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

void ctkXnatSession::fetch(ctkXnatScanFolder* scanFolder)
{
  Q_D(ctkXnatSession);

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

void ctkXnatSession::fetch(ctkXnatScan* scan)
{
  Q_D(ctkXnatSession);

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

void ctkXnatSession::fetch(ctkXnatScanResource* scanResource)
{
  Q_D(ctkXnatSession);

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

void ctkXnatSession::fetch(ctkXnatReconstructionFolder* reconstructionFolder)
{
  Q_D(ctkXnatSession);

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

void ctkXnatSession::fetch(ctkXnatReconstruction* reconstruction)
{
  Q_D(ctkXnatSession);

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

void ctkXnatSession::fetch(ctkXnatReconstructionResource* reconstructionResource)
{
  Q_D(ctkXnatSession);

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

bool ctkXnatSession::exists(const ctkXnatObject* object)
{
  Q_D(ctkXnatSession);

  QString query = object->resourceUri();
  bool success = d->xnat->sync(d->xnat->get(query));

  return success;
}

void ctkXnatSession::save(ctkXnatObject* object)
{
  Q_D(ctkXnatSession);

  QString query = object->resourceUri();
  query.append(QString("?%1=%2").arg("xsi:type", object->schemaType()));
  const QMap<QString, QString>& properties = object->properties();
  QMapIterator<QString, QString> itProperties(properties);
  while (itProperties.hasNext())
  {
    itProperties.next();
    query.append(QString("&%1=%2").arg(itProperties.key(), itProperties.value()));
  }

  qDebug() << "ctkXnatSession::save() query:" << query;
  QUuid queryId = d->xnat->put(query);
  qRestResult* result = d->xnat->takeResult(queryId);

  if (!result || !result->error().isNull())
  {
    d->throwXnatException("Error occurred while creating the data.");
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

void ctkXnatSession::remove(ctkXnatObject* object)
{
  Q_D(ctkXnatSession);

  QString query = object->resourceUri();
  bool success = d->xnat->sync(d->xnat->del(query));

  if (!success)
  {
    d->throwXnatException("Error occurred while removing the data.");
  }
}

//void ctkXnatSession::create(ctkXnatSubject* subject)
//{
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatSession);

//  QString query = QString("/REST/projects/%1/subjects/%2").arg(projectName, subjectName);
//  bool success = d->xnat->sync(d->xnat->put(query));

//  if (!success)
//  {
//    throw ctkXnatException("Error occurred while creating the subject.");
//  }
//}

//void ctkXnatSession::downloadScanFiles(ctkXnatExperiment* experiment, const QString& fileName)
//{
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatSession);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/scans/ALL/files").arg(projectName, subjectName, experimentName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

//void ctkXnatSession::downloadReconstructionFiles(ctkXnatExperiment* experiment, const QString& fileName)
//{
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatSession);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/reconstructions/ALL/files").arg(projectName, subjectName, experimentName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

//void ctkXnatSession::downloadReconstruction(ctkXnatReconstruction* reconstruction, const QString& fileName)
//{
//  const QString& reconstructionName = reconstruction->getName();
//  ctkXnatObject* experiment = reconstruction->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatSession);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/reconstructions/%4/ALL/files").arg(projectName, subjectName, experimentName, reconstructionName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

//void ctkXnatSession::downloadReconstructionResourceFiles(ctkXnatReconstructionResource* reconstructionResource, const QString& fileName)
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

//  Q_D(ctkXnatSession);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/reconstructions/%4/ALL/resources/%5/files").arg(projectName, subjectName, experimentName, reconstructionName, reconstructionResourceName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

//void ctkXnatSession::download(ctkXnatReconstructionResourceFile* reconstructionResourceFile, const QString& fileName)
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

//  Q_D(ctkXnatSession);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/reconstructions/%4/resources/%5/files/%6").arg(projectName, subjectName, experimentName, reconstructionName, reconstructionResourceName, reconstructionResourceFileName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

//void ctkXnatSession::download(ctkXnatScan* scan, const QString& fileName)
//{
//  const QString& scanName = scan->getName();
//  ctkXnatObject* experiment = scan->getParent()->getParent();
//  const QString& experimentName = experiment->getName();
//  ctkXnatObject* subject = experiment->getParent();
//  const QString& subjectName = subject->getName();
//  ctkXnatObject* project = subject->getParent();
//  const QString& projectName = project->getName();

//  Q_D(ctkXnatSession);

//  QString query = QString("/REST/projects/%1/subjects/%2/experiments/%3/scans/%4/files").arg(projectName, subjectName, experimentName, scanName);
//  qRestAPI::Parameters parameters;
//  parameters["format"] = "zip";
//  QUuid queryId = d->xnat->download(fileName, query, parameters);
//  d->xnat->sync(queryId);
//}

void ctkXnatSession::download(ctkXnatFile* file, const QString& fileName)
{
  Q_D(ctkXnatSession);
  QString query = file->resourceUri();

  QUuid queryId = d->xnat->download(fileName, query);
  d->xnat->sync(queryId);
}

void ctkXnatSession::download(ctkXnatScanResource* scanResource, const QString& fileName)
{
  Q_D(ctkXnatSession);

  QString query = scanResource->resourceUri() + "/files";
  qRestAPI::Parameters parameters;
  parameters["format"] = "zip";
  QUuid queryId = d->xnat->download(fileName, query, parameters);
  d->xnat->sync(queryId);
}

void ctkXnatSession::download(ctkXnatReconstructionResource* reconstructionResource, const QString& fileName)
{
  Q_D(ctkXnatSession);

  QString query = reconstructionResource->resourceUri() + "/files";
  qRestAPI::Parameters parameters;
  parameters["format"] = "zip";
  QUuid queryId = d->xnat->download(fileName, query, parameters);
  d->xnat->sync(queryId);
}

void ctkXnatSession::processResult(QUuid queryId, QList<QVariantMap> parameters)
{
  Q_UNUSED(queryId)
  Q_UNUSED(parameters)
}
