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

  void createConnections();
};

ctkXnatSessionPrivate::ctkXnatSessionPrivate(const ctkXnatLoginProfile& loginProfile, ctkXnatSession* q)
  : loginProfile(loginProfile)
  , xnat(new ctkXnatAPI())
  , dataModel(new ctkXnatDataModel(q))
{
  // TODO This is a workaround for connecting to sites with self-signed
  // certificate. Should be replaced with something more clever.
  xnat->setSuppressSslErrors(true);

  createConnections();
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

void ctkXnatSessionPrivate::createConnections()
{
//  Q_D(ctkXnatSession);
//  connect(d->xnat, SIGNAL(resultReceived(QUuid,QList<QVariantMap>)),
//           this, SLOT(processResult(QUuid,QList<QVariantMap>)));
//  connect(d->xnat, SIGNAL(progress(QUuid,double)),
  //           this, SLOT(progress(QUuid,double)));
}

// ctkXnatSession class

ctkXnatSession::ctkXnatSession(const ctkXnatLoginProfile& loginProfile)
: d_ptr(new ctkXnatSessionPrivate(loginProfile, this))
{
  Q_D(ctkXnatSession);

  qRegisterMetaType<ctkXnatProject>(ctkXnatProject::staticSchemaType());
  qRegisterMetaType<ctkXnatSubject>(ctkXnatSubject::staticSchemaType());
  qRegisterMetaType<ctkXnatExperiment>(ctkXnatExperiment::staticSchemaType());
  qRegisterMetaType<ctkXnatScan>(ctkXnatScan::staticSchemaType());
  qRegisterMetaType<ctkXnatScanFolder>(ctkXnatScanFolder::staticSchemaType());
  qRegisterMetaType<ctkXnatReconstruction>(ctkXnatReconstruction::staticSchemaType());
  qRegisterMetaType<ctkXnatScanResource>(ctkXnatScanResource::staticSchemaType());
  qRegisterMetaType<ctkXnatFile>(ctkXnatFile::staticSchemaType());
  qRegisterMetaType<ctkXnatReconstructionResource>(ctkXnatReconstructionResource::staticSchemaType());

  ctkXnatAPI::RawHeaders rawHeaders;
  rawHeaders["User-Agent"] = "Qt";
  rawHeaders["Authorization"] = "Basic " +
      QByteArray(QString("%1:%2").arg(d->loginProfile.userName())
                 .arg(d->loginProfile.password()).toAscii()).toBase64();
  d->xnat->setDefaultRawHeaders(rawHeaders);

  QString url = d->loginProfile.serverUrl().toString();
  d->xnat->setServerUrl(url);

  d->dataModel->setProperty("ID", url);
}

ctkXnatSession::~ctkXnatSession()
{
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

QUuid ctkXnatSession::httpGet(const QString& resource, const ctkXnatSession::UrlParameters& parameters, const ctkXnatSession::HttpRawHeaders& rawHeaders)
{
  Q_D(ctkXnatSession);
  return d->xnat->get(resource, parameters, rawHeaders);
}

QList<ctkXnatObject*> ctkXnatSession::httpResults(const QUuid& uuid, const QString& schemaType)
{
  Q_D(ctkXnatSession);

  QScopedPointer<qRestResult> restResult(d->xnat->takeResult(uuid));
  if (restResult == NULL)
  {
    d->throwXnatException("REST result is NULL");
  }
  return restResult->results<ctkXnatObject>(schemaType);
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
