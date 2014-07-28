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

#ifndef CTKXNATSESSION_H
#define CTKXNATSESSION_H

#include "ctkXNATCoreExport.h"

#include <QScopedPointer>
#include <QString>

#include <QObject>
#include <QVariantMap>
#include <QUuid>

class QDateTime;

class ctkXnatSessionPrivate;

class ctkXnatFile;
class ctkXnatLoginProfile;
class ctkXnatDataModel;
class ctkXnatObject;
class ctkXnatResource;

/**
 * @ingroup XNAT_Core
 *
 * @brief The ctkXnatSession class reprents a session object associated
 * with a specific XNAT connection.
 */
class CTK_XNAT_CORE_EXPORT ctkXnatSession : public QObject
{
  Q_OBJECT

public:

  typedef QMap<QString, QString> UrlParameters;
  typedef QMap<QByteArray, QByteArray> HttpRawHeaders;

  ctkXnatSession(const ctkXnatLoginProfile& loginProfile);
  ~ctkXnatSession();

  /**
   * @brief Open a new XNAT session.
   *
   * This method must be called on all ctkXnatSession objects before
   * any of the methods which communicate with an XNAT server are called.
   *
   * If the session has already been opened, this method does nothing.
   *
   * @throws ctkXnatAuthenticationException if the user credentials are invalid.
   * @throws ctkXnatException (or one of its subclasses) if a network error occurred.
   */
  void open();

  /**
   * @brief Closes this XNAT session.
   */
  void close();

  /**
   * @brief Returns the open state of this XNAT session.
   * @return \c true if the session is open, \c false otherwise.
   */
  bool isOpen() const;

  /**
   * @brief Get the XNAT server version.
   * @return The XNAT version running on the remote server. Returns a null string
   *         if the session is not open.
   */
  QString version() const;

  /**
   * @brief Get the expiration date for this XNAT session.
   *
   * @sa renew()
   * @throws ctkXnatInvalidSessionException if the session is closed.
   * @return The session expiration date.
   */
  QDateTime expirationDate() const;

  /**
   * @brief Re-new the XNAT session.
   * @throws ctkXnatInvalidSessionException if the session is closed.
   * @return The new session expiration data.
   */
  QDateTime renew();

  /**
   * @brief Get the current login profile for this session object.
   * @return A copy of the currently used login profile.
   */
  ctkXnatLoginProfile loginProfile() const;

  /**
   * @brief Get XNAT server url.
   *
   * The url is the one specified by the login profile.
   *
   * @return The XNAT server url.
   */
  QUrl url() const;

  /**
   * @brief Get the user name for this XNAT session.
   *
   * The user name is the one specified by the login profile.
   *
   * @return The XNAT session user name.
   */
  QString userName() const;

  /**
   * @brief Get the password for this XNAT session.
   *
   * The password is the one specified by the login profile.
   *
   * @return The XNAT session password.
   */
  QString password() const;

  ctkXnatDataModel* dataModel() const;

  /**
   * @brief TODO
   * @param resource
   * @param parameters
   * @param rawHeaders
   *
   * @throws ctkXnatInvalidSessionException if the session is closed.
   * @return
   */
  QUuid httpGet(const QString& resource,
                const UrlParameters& parameters = UrlParameters(),
                const HttpRawHeaders& rawHeaders = HttpRawHeaders());

  /**
   * @brief TODO
   * @param uuid
   * @param schemaType
   *
   * @throws ctkXnatInvalidSessionException if the session is closed.
   * @return
   */
  QList<ctkXnatObject*> httpResults(const QUuid& uuid, const QString& schemaType);

  /**
   * @brief TODO
   * @param uuid
   *
   * @throws ctkXnatInvalidSessionException if the session is closed.
   * @return
   */
  QList<QVariantMap> httpSync(const QUuid& uuid);

  /**
   * @brief Reads the result of a head request
   * @param uuid the uid of the related query
   *
   * @throws ctkXnatInvalidSessionException if the session is closed.
   * @return a QMap containing the retrieved header information
   */
  const QMap<QByteArray, QByteArray> httpHeadSync(const QUuid& uuid);

  bool exists(const ctkXnatObject* object);

  void save(ctkXnatObject* object);
  void remove(ctkXnatObject* object);

  void download(ctkXnatFile* file, const QString& fileName);

//  void downloadScanFiles(ctkXnatExperiment* experiment, const QString& zipFileName);
//  void downloadReconstructionFiles(ctkXnatExperiment* experiment, const QString& zipFileName);

//  void download(ctkXnatScan* scan, const QString& zipFileName);

  void download(ctkXnatResource* resource, const QString& zipFileName);

  /**
   * @brief Sends a http HEAD request to the xnat instance
   * @param resourceUri the URL to the server
   * @return the query uid
   */
  QUuid httpHead(const QString& resourceUri);

//  void downloadReconstruction(ctkXnatReconstruction* reconstruction, const QString& zipFilename);

//  void downloadReconstructionResourceFiles(ctkXnatReconstructionResource* reconstructionResource, const QString& zipFilename);

//  void download(ctkXnatReconstructionResourceFile* reconstructionResourceFile, const QString& zipFileName);

  /**
   * @brief Signals that the session was re-newed.
   * @param expirationDate The new session expiration date.
   */
  Q_SIGNAL void sessionRenewed(const QDateTime& expirationDate);

public slots:
  void processResult(QUuid queryId, QList<QVariantMap> parameters);
  void progress(QUuid queryId, double progress);

protected:
  QScopedPointer<ctkXnatSessionPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkXnatSession)
  Q_DISABLE_COPY(ctkXnatSession)
};

#endif
