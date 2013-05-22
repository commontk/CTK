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

#ifndef ctkXnatConnection_h
#define ctkXnatConnection_h

#include "ctkXNATExport.h"

#include <QScopedPointer>
#include <QString>
#include <QScriptValue>

#include <qXnatAPI.h>

class ctkXnatConnectionPrivate;

class QAuthenticator;
class QNetworkReply;

class ctkXnatExperiment;
class ctkXnatObject;
class ctkXnatProject;
class ctkXnatReconstruction;
class ctkXnatReconstructionFolder;
class ctkXnatReconstructionResource;
class ctkXnatReconstructionResourceFile;
class ctkXnatRoot;
class ctkXnatScan;
class ctkXnatScanFolder;
class ctkXnatScanResource;
class ctkXnatScanResourceFile;
class ctkXnatServer;
class ctkXnatSubject;

class CTK_XNAT_EXPORT ctkXnatConnection : public QObject
{

  Q_OBJECT

public:

  ctkXnatConnection();
  ~ctkXnatConnection();

  void createConnections();

  QString url() const;
  void setUrl(const QString& url);

  QString userName() const;
  void setUserName(const QString& userName);

  QString password() const;
  void setPassword(const QString& password);

  ctkXnatServer* server();

  void fetch(ctkXnatServer* server);
  void fetch(ctkXnatProject* project);
  void fetch(ctkXnatSubject* subject);
  void fetch(ctkXnatExperiment* experiment);
  void fetch(ctkXnatScanFolder* scanFolder);
  void fetch(ctkXnatScan* scan);
  void fetch(ctkXnatScanResource* scanResource);
  void fetch(ctkXnatReconstructionFolder* reconstructionFolder);
  void fetch(ctkXnatReconstruction* reconstruction);
  void fetch(ctkXnatReconstructionResource* reconstructionResource);

  void create(ctkXnatProject* project);
  void create(ctkXnatSubject* subject);

  void remove(ctkXnatProject* project);

  void downloadScanFiles(ctkXnatExperiment* experiment, const QString& zipFileName);
  void downloadReconstructionFiles(ctkXnatExperiment* experiment, const QString& zipFileName);
  void addReconstruction(ctkXnatExperiment* experiment, const QString& categoryEntry);

  void download(ctkXnatScan* scan, const QString& zipFileName);
  void download(ctkXnatScanResource* scanResource, const QString& zipFileName);
  void download(ctkXnatScanResourceFile* scanResourceFile, const QString& fileName);

  void downloadReconstruction(ctkXnatReconstruction* reconstruction, const QString& zipFilename);
  void addReconstructionResource(ctkXnatReconstruction* reconstruction, const QString& resource);
  void removeReconstruction(ctkXnatReconstruction* reconstruction);

  void downloadReconstructionResourceFiles(ctkXnatReconstructionResource* reconstructionResource, const QString& zipFilename);
  void uploadReconstructionResourceFiles(ctkXnatReconstructionResource* reconstructionResource, const QString& zipFilename);
  void removeReconstructionResource(ctkXnatReconstructionResource* reconstructionResource);

  void download(ctkXnatReconstructionResourceFile* reconstructionResourceFile, const QString& zipFileName);
  void remove(ctkXnatReconstructionResourceFile* reconstructionResourceFile);

public slots:
  void processResult(QUuid queryId, QList<QVariantMap> parameters);
  void progress(QUuid queryId, double progress);

protected:
  QScopedPointer<ctkXnatConnectionPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkXnatConnection);
  Q_DISABLE_COPY(ctkXnatConnection);
};

#endif
