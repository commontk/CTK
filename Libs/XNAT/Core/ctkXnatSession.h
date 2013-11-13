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
class ctkXnatScanResource;
class ctkXnatReconstructionResource;

class CTK_XNAT_CORE_EXPORT ctkXnatSession : public QObject
{
  Q_OBJECT

public:

  typedef QMap<QString, QString> UrlParameters;
  typedef QMap<QByteArray, QByteArray> HttpRawHeaders;

  ctkXnatSession(const ctkXnatLoginProfile& loginProfile);
  ~ctkXnatSession();

  void open();
  void close();

  bool isOpen() const;

  QString version() const;

  QDateTime expirationDate() const;
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

  QUuid httpGet(const QString& resource,
                const UrlParameters& parameters = UrlParameters(),
                const HttpRawHeaders& rawHeaders = HttpRawHeaders());

  QList<ctkXnatObject*> httpResults(const QUuid& uuid, const QString& schemaType);

  QList<QVariantMap> httpSync(const QUuid& uuid);

  bool exists(const ctkXnatObject* object);

  void save(ctkXnatObject* object);
  void remove(ctkXnatObject* object);

  void download(ctkXnatFile* file, const QString& fileName);

//  void downloadScanFiles(ctkXnatExperiment* experiment, const QString& zipFileName);
//  void downloadReconstructionFiles(ctkXnatExperiment* experiment, const QString& zipFileName);

//  void download(ctkXnatScan* scan, const QString& zipFileName);

  void download(ctkXnatScanResource* scanResource, const QString& zipFileName);
  void download(ctkXnatReconstructionResource* reconstructionResource, const QString& zipFileName);

//  void downloadReconstruction(ctkXnatReconstruction* reconstruction, const QString& zipFilename);

//  void downloadReconstructionResourceFiles(ctkXnatReconstructionResource* reconstructionResource, const QString& zipFilename);

//  void download(ctkXnatReconstructionResourceFile* reconstructionResourceFile, const QString& zipFileName);

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
