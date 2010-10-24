/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

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

#ifndef CTKEXAMPLEDICOMHOST_H
#define CTKEXAMPLEDICOMHOST_H

#include <ctkDicomAbstractHost.h>
#include <ctkHostedAppPlaceholderWidget.h>

#include <org_commontk_dah_examplehost_Export.h>

#include <QUrl>
#include <QProcess>

class org_commontk_dah_examplehost_EXPORT ctkExampleDicomHost : public ctkDicomAbstractHost
{
  Q_OBJECT

public:

  ctkExampleDicomHost(ctkHostedAppPlaceholderWidget* placeholderWidget, int hostPort = 8080, int appPort = 8081);

  virtual void StartApplication(QString AppPath);
  virtual QString generateUID() { return ""; }
  virtual QRect getAvailableScreen(const QRect& preferredScreen);
  virtual QString getOutputLocation(const QStringList& /*preferredProtocols*/) { return ""; }
  virtual void notifyStateChanged(ctkDicomAppHosting::State state);
  virtual void notifyStatus(const ctkDicomAppHosting::Status& status);
  // exchange methods
  virtual bool notifyDataAvailable(ctkDicomAppHosting::AvailableData data, bool lastData);
  virtual QList<ctkDicomAppHosting::ObjectLocator> getData(
      QList<QUuid> objectUUIDs, 
      QList<QString> acceptableTransferSyntaxUIDs, 
      bool includeBulkData);
  virtual void releaseData(QList<QUuid> objectUUIDs);

  const QProcess& getAppProcess() const { return appProcess; }
  ~ctkExampleDicomHost();

signals:

  void stateChangedReceived(ctkDicomAppHosting::State state);
  void statusReceived(const ctkDicomAppHosting::Status& status);
  void giveAvailableScreen(QRect rect);

protected:

  QProcess appProcess;
  ctkHostedAppPlaceholderWidget* placeholderWidget;
  ctkDicomAppHosting::State applicationState;

protected slots:

  void forwardConsoleOutput();
};

#endif // CTKEXAMPLEDICOMHOST_H
