/*=============================================================================

  Library: XNAT/Core

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

#include "ctkXnatAssessor.h"
#include "ctkXnatDataModel.h"
#include "ctkXnatDefaultSchemaTypes.h"
#include "ctkXnatException.h"
#include "ctkXnatExperiment.h"
#include "ctkXnatFile.h"
#include "ctkXnatLoginProfile.h"
#include "ctkXnatObject.h"
#include "ctkXnatProject.h"
#include "ctkXnatReconstruction.h"
#include "ctkXnatResource.h"
#include "ctkXnatScan.h"
#include "ctkXnatSubject.h"

#include <QDateTime>
#include <QDebug>
#include <QScopedPointer>
#include <QStringBuilder>
#include <QNetworkCookie>

#include <ctkXnatAPI_p.h>
#include <qRestResult.h>

//----------------------------------------------------------------------------
static const char* HEADER_AUTHORIZATION = "Authorization";
static const char* HEADER_USER_AGENT = "User-Agent";
static const char* HEADER_COOKIE = "Cookie";

static QString SERVER_VERSION = "version";
static QString SESSION_EXPIRATION_DATE = "expires";

//----------------------------------------------------------------------------
class ctkXnatSessionPrivate
{
public:
  const ctkXnatLoginProfile loginProfile;

  QScopedPointer<ctkXnatAPI> xnat;
  QScopedPointer<ctkXnatDataModel> dataModel;
  QString sessionId;

  QMap<QString, QString> sessionProperties;

  ctkXnatSession* q;

  ctkXnatSessionPrivate(const ctkXnatLoginProfile& loginProfile, ctkXnatSession* q);
  ~ctkXnatSessionPrivate();

  void throwXnatException(const QString& msg);

  void createConnections();
  void setDefaultHttpHeaders();
  void checkSession() const;
  void setSessionProperties();
  QDateTime updateExpirationDate(qRestResult* restResult);

  void close();

  static QList<ctkXnatObject*> results(qRestResult* restResult, QString schemaType);
};

//----------------------------------------------------------------------------
ctkXnatSessionPrivate::ctkXnatSessionPrivate(const ctkXnatLoginProfile& loginProfile,
                                             ctkXnatSession* q)
  : loginProfile(loginProfile)
  , xnat(new ctkXnatAPI())
  , q(q)
{
  // TODO This is a workaround for connecting to sites with self-signed
  // certificate. Should be replaced with something more clever.
  xnat->setSuppressSslErrors(true);

  createConnections();
}

//----------------------------------------------------------------------------
ctkXnatSessionPrivate::~ctkXnatSessionPrivate()
{
}

//----------------------------------------------------------------------------
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
    throw ctkXnatProtocolFailureException(errorMsg);
  case qRestAPI::UnknownUuidError:
    throw ctkInvalidArgumentException(errorMsg);
  case qRestAPI::AuthenticationError:
    // This signals either an initial authentication error
    // or a session timeout.
    this->close();
    throw ctkXnatAuthenticationException(errorMsg);
  default:
    throw ctkRuntimeException(errorMsg);
  }
}

//----------------------------------------------------------------------------
void ctkXnatSessionPrivate::createConnections()
{
//  Q_D(ctkXnatSession);
//  connect(d->xnat, SIGNAL(resultReceived(QUuid,QList<QVariantMap>)),
//           this, SLOT(processResult(QUuid,QList<QVariantMap>)));
//  connect(d->xnat, SIGNAL(progress(QUuid,double)),
  //           this, SLOT(progress(QUuid,double)));
}

//----------------------------------------------------------------------------
void ctkXnatSessionPrivate::setDefaultHttpHeaders()
{
  ctkXnatAPI::RawHeaders rawHeaders;
  rawHeaders[HEADER_USER_AGENT] = "Qt";
  /*
  rawHeaders["Authorization"] = "Basic " +
      QByteArray(QString("%1:%2").arg(d->loginProfile.userName())
                 .arg(d->loginProfile.password()).toAscii()).toBase64();
  */
  if (!sessionId.isEmpty())
  {
    rawHeaders[HEADER_COOKIE] = QString("JSESSIONID=%1").arg(sessionId).toLatin1();
  }
  xnat->setDefaultRawHeaders(rawHeaders);
}

//----------------------------------------------------------------------------
void ctkXnatSessionPrivate::checkSession() const
{
  if (sessionId.isEmpty())
  {
    throw ctkXnatInvalidSessionException("Session closed.");
  }
}

//----------------------------------------------------------------------------
void ctkXnatSessionPrivate::setSessionProperties()
{
  sessionProperties.clear();
  QUuid uuid = xnat->get("/data/version");
  QScopedPointer<qRestResult> restResult(xnat->takeResult(uuid));
  if (restResult)
  {
    QString version = restResult->result()["content"].toString();
    if (version.isEmpty())
    {
      throw ctkXnatProtocolFailureException("No version information available.");
    }
    sessionProperties[SERVER_VERSION] = version;
  }
  else
  {
    this->throwXnatException("Retrieving session properties failed.");
  }
}

//----------------------------------------------------------------------------
QDateTime ctkXnatSessionPrivate::updateExpirationDate(qRestResult* restResult)
{
  QByteArray cookieHeader = restResult->rawHeader("Set-Cookie");
  QDateTime expirationDate = QDateTime::currentDateTime();
  if (!cookieHeader.isEmpty())
  {
    QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(cookieHeader);
    foreach(const QNetworkCookie& cookie, cookies)
    {
      if (cookie.name() == "SESSION_EXPIRATION_TIME")
      {
        QList<QByteArray> expirationCookie = cookie.value().split(',');
        if (expirationCookie.size() == 2)
        {
          unsigned long long startTime = expirationCookie[0].mid(1).toULongLong();
          if (startTime > 0)
          {
            expirationDate = QDateTime::fromTime_t(startTime / 1000);
          }
          QByteArray timeSpan = expirationCookie[1];
          timeSpan.chop(1);
          expirationDate = expirationDate.addMSecs(timeSpan.toLong());
          sessionProperties[SESSION_EXPIRATION_DATE] = expirationDate.toString(Qt::ISODate);
          emit q->sessionRenewed(expirationDate);
        }
      }
    }
  }
  return expirationDate;
}

//----------------------------------------------------------------------------
void ctkXnatSessionPrivate::close()
{
  sessionProperties.clear();
  sessionId.clear();
  this->setDefaultHttpHeaders();

  dataModel.reset();
}

//----------------------------------------------------------------------------
QList<ctkXnatObject*> ctkXnatSessionPrivate::results(qRestResult* restResult, QString schemaType)
{
  QList<ctkXnatObject*> results;
  foreach (const QVariantMap& propertyMap, restResult->results())
  {
    QString customSchemaType;
    if (propertyMap.contains("xsiType"))
    {
      customSchemaType = propertyMap["xsiType"].toString();
    }

    int typeId = 0;
    // try to create an object based on the custom schema type first
    if (!customSchemaType.isEmpty())
    {
      typeId = QMetaType::type(qPrintable(customSchemaType));
    }

    // Fall back. Create the default class according to the default schema type
    if (!typeId)
    {
      if (!customSchemaType.isEmpty())
      {
        qWarning() << QString("No ctkXnatObject sub-class registered for the schema %1. Falling back to the default class %2.").arg(customSchemaType).arg(schemaType);
      }
      typeId = QMetaType::type(qPrintable(schemaType));
    }

    if (!typeId)
    {
      qWarning() << QString("No ctkXnatObject sub-class registered as a meta-type for the schema %1. Skipping result.").arg(schemaType);
      continue;
    }

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    ctkXnatObject* object = reinterpret_cast<ctkXnatObject*>(QMetaType::construct(typeId));
#else
    ctkXnatObject* object = reinterpret_cast<ctkXnatObject*>(QMetaType(typeId).create());
#endif
    if (!customSchemaType.isEmpty())
    {
      // We might have created the default ctkXnatObject sub-class, but can still set
      // the custom schema type.
      object->setSchemaType(customSchemaType);
    }

    // Fill in the properties
    QMapIterator<QString, QVariant> it(propertyMap);
    QString description;
    
    while (it.hasNext())
    {
      it.next();

      QString  str = it.key().toLatin1().data();
      QVariant var = it.value();

      object->setProperty(str, var);
      description.append (str + QString ("\t::\t") + var.toString() + "\n");
    }

    QVariant lastModifiedHeader = restResult->rawHeader("Last-Modified");
    QDateTime lastModifiedTime;
    if (lastModifiedHeader.isValid())
    {
      lastModifiedTime = lastModifiedHeader.toDateTime();
    }
    if (lastModifiedTime.isValid())
    {
      object->setLastModifiedTime(lastModifiedTime);
    }

    object->setDescription(description);
    results.push_back(object);
  }
  return results;
}


//----------------------------------------------------------------------------
// ctkXnatSession class

//----------------------------------------------------------------------------
ctkXnatSession::ctkXnatSession(const ctkXnatLoginProfile& loginProfile)
: d_ptr(new ctkXnatSessionPrivate(loginProfile, this))
{
  Q_D(ctkXnatSession);

  qRegisterMetaType<ctkXnatProject>(qPrintable(ctkXnatDefaultSchemaTypes::XSI_PROJECT));
  qRegisterMetaType<ctkXnatSubject>(qPrintable(ctkXnatDefaultSchemaTypes::XSI_SUBJECT));
  qRegisterMetaType<ctkXnatExperiment>(qPrintable(ctkXnatDefaultSchemaTypes::XSI_EXPERIMENT));
  qRegisterMetaType<ctkXnatScan>(qPrintable(ctkXnatDefaultSchemaTypes::XSI_SCAN));
  qRegisterMetaType<ctkXnatReconstruction>(qPrintable(ctkXnatDefaultSchemaTypes::XSI_RECONSTRUCTION));
  qRegisterMetaType<ctkXnatResource>(qPrintable(ctkXnatDefaultSchemaTypes::XSI_RESOURCE));
  qRegisterMetaType<ctkXnatAssessor>(qPrintable(ctkXnatDefaultSchemaTypes::XSI_ASSESSOR));
  qRegisterMetaType<ctkXnatFile>(qPrintable(ctkXnatDefaultSchemaTypes::XSI_FILE));
  
  QString url = d->loginProfile.serverUrl().toString();
  d->xnat->setServerUrl(url);

  d->setDefaultHttpHeaders();
}

//----------------------------------------------------------------------------
ctkXnatSession::~ctkXnatSession()
{
  this->close();
}

//----------------------------------------------------------------------------
void ctkXnatSession::open()
{
  Q_D(ctkXnatSession);

  if (this->isOpen()) return;

  qRestAPI::RawHeaders headers;
  headers[HEADER_AUTHORIZATION] = "Basic " +
                                  QByteArray(QString("%1:%2").arg(this->userName())
                                             .arg(this->password()).toLatin1()).toBase64();
  QUuid uuid = d->xnat->get("/data/JSESSION", qRestAPI::Parameters(), headers);
  QScopedPointer<qRestResult> restResult(d->xnat->takeResult(uuid));
  if (restResult)
  {
    QString sessionId = restResult->result()["content"].toString();
    d->sessionId = sessionId;
    d->setDefaultHttpHeaders();
    d->setSessionProperties();
    d->updateExpirationDate(restResult.data());
  }
  else
  {
    d->throwXnatException("Could not get a session id.");
  }

  d->dataModel.reset(new ctkXnatDataModel(this));
  d->dataModel->setProperty(ctkXnatObject::LABEL, this->url().toString());
  emit sessionOpened();
}

//----------------------------------------------------------------------------
void ctkXnatSession::close()
{
  Q_D(ctkXnatSession);

  if (!this->isOpen()) return;

  emit sessionAboutToBeClosed();
  d->close();
}

//----------------------------------------------------------------------------
bool ctkXnatSession::isOpen() const
{
  Q_D(const ctkXnatSession);
  return !d->sessionId.isEmpty();
}

//----------------------------------------------------------------------------
QString ctkXnatSession::version() const
{
  Q_D(const ctkXnatSession);
  if (d->sessionProperties.contains(SERVER_VERSION))
  {
    return d->sessionProperties[SERVER_VERSION];
  }
  else
  {
    return QString::null;
  }
}

//----------------------------------------------------------------------------
QDateTime ctkXnatSession::expirationDate() const
{
  Q_D(const ctkXnatSession);
  d->checkSession();
  return QDateTime::fromString(d->sessionProperties[SESSION_EXPIRATION_DATE], Qt::ISODate);
}

//----------------------------------------------------------------------------
QDateTime ctkXnatSession::renew()
{
  Q_D(ctkXnatSession);
  d->checkSession();

  QUuid uuid = d->xnat->get("/data/auth");
  QScopedPointer<qRestResult> restResult(d->xnat->takeResult(uuid));
  if (!restResult)
  {
    d->throwXnatException("Session renewal failed.");
  }
  return d->updateExpirationDate(restResult.data());
}

//----------------------------------------------------------------------------
ctkXnatLoginProfile ctkXnatSession::loginProfile() const
{
  Q_D(const ctkXnatSession);
  return d->loginProfile;
}

//----------------------------------------------------------------------------
void ctkXnatSession::progress(QUuid /*queryId*/, double /*progress*/)
{
//  qDebug() << "ctkXnatSession::progress(QUuid queryId, double progress)";
//  qDebug() << "query id:" << queryId;
//  qDebug() << "progress:" << (progress * 100.0) << "%";
}

//----------------------------------------------------------------------------
QUrl ctkXnatSession::url() const
{
  Q_D(const ctkXnatSession);
  return d->loginProfile.serverUrl();
}

//----------------------------------------------------------------------------
QString ctkXnatSession::userName() const
{
  Q_D(const ctkXnatSession);
  return d->loginProfile.userName();
}

//----------------------------------------------------------------------------
QString ctkXnatSession::password() const
{
  Q_D(const ctkXnatSession);
  return d->loginProfile.password();
}

//----------------------------------------------------------------------------
QString ctkXnatSession::sessionId() const
{
  Q_D(const ctkXnatSession);
  return d->sessionId;
}

//----------------------------------------------------------------------------
ctkXnatDataModel* ctkXnatSession::dataModel() const
{
  Q_D(const ctkXnatSession);
  d->checkSession();
  return d->dataModel.data();
}

//----------------------------------------------------------------------------
QUuid ctkXnatSession::httpGet(const QString& resource, const ctkXnatSession::UrlParameters& parameters, const ctkXnatSession::HttpRawHeaders& rawHeaders)
{
  Q_D(ctkXnatSession);
  d->checkSession();
  return d->xnat->get(resource, parameters, rawHeaders);
}

//----------------------------------------------------------------------------
QList<ctkXnatObject*> ctkXnatSession::httpResults(const QUuid& uuid, const QString& schemaType)
{
  Q_D(ctkXnatSession);
  d->checkSession();

  QScopedPointer<qRestResult> restResult(d->xnat->takeResult(uuid));
  if (restResult == NULL)
  {
    d->throwXnatException("Http request failed.");
  }
  return d->results(restResult.data(), schemaType);
}

//----------------------------------------------------------------------------
QList<QVariantMap> ctkXnatSession::httpSync(const QUuid& uuid)
{
  Q_D(ctkXnatSession);
  d->checkSession();

  QList<QVariantMap> result;
  qRestResult* restResult = d->xnat->takeResult(uuid);
  if (restResult == NULL)
  {
    d->throwXnatException("Syncing with http request failed.");
  }
  else
  {
    d->updateExpirationDate(restResult);
    result = restResult->results();
  }
  return result;
}

//----------------------------------------------------------------------------
bool ctkXnatSession::exists(const ctkXnatObject* object)
{
  Q_D(ctkXnatSession);

  QString query = object->resourceUri();
  bool success = d->xnat->sync(d->xnat->get(query));

  return success;
}

//----------------------------------------------------------------------------
const QMap<QByteArray, QByteArray> ctkXnatSession::httpHeadSync(const QUuid &uuid)
{
  Q_D(ctkXnatSession);
  QScopedPointer<qRestResult> result (d->xnat->takeResult(uuid));
  if (result == NULL)
  {
    d->throwXnatException("Sending HEAD request failed.");
  }
  return result->rawHeaders();
}

//----------------------------------------------------------------------------
QUuid ctkXnatSession::httpHead(const QString& resourceUri)
{
  Q_D(ctkXnatSession);
  QUuid queryId = d->xnat->head(resourceUri);
  return queryId;
}

//----------------------------------------------------------------------------
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
    QVariant id = maps[0][ctkXnatObject::ID];
    if (!id.isNull())
    {
      object->setId(id.toString());
    }
  }
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
void ctkXnatSession::download(const QString& fileName,
    const QString& resource,
    const UrlParameters& parameters,
    const HttpRawHeaders& rawHeaders)
{
  Q_D(ctkXnatSession);

  QUuid queryId = d->xnat->download(fileName, resource, parameters, rawHeaders);
  d->xnat->sync(queryId);
}

//----------------------------------------------------------------------------
void ctkXnatSession::processResult(QUuid queryId, QList<QVariantMap> parameters)
{
  Q_UNUSED(queryId)
  Q_UNUSED(parameters)
}
