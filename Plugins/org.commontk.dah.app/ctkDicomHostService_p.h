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


#ifndef CTKDICOMHOSTSERVICE_P_H
#define CTKDICOMHOSTSERVICE_P_H

#include <ctkDicomHostInterface.h>
#include <ctkDicomExchangeService.h>

class ctkDicomHostService : public ctkDicomHostInterface
{

public:
  ctkDicomHostService(ushort port, QString path);
  ~ctkDicomHostService();

  QString generateUID();
  QRect getAvailableScreen(const QRect& preferredScreen);
  QString getOutputLocation(const QStringList& preferredProtocols);
  void notifyStateChanged(ctkDicomAppHosting::State state);
  void notifyStatus(const ctkDicomAppHosting::Status& status);

  // Exchange methods
  bool notifyDataAvailable(ctkDicomAppHosting::AvailableData data, bool lastData);
  QList<ctkDicomAppHosting::ObjectLocator> getData(
    QList<QUuid> objectUUIDs, 
    QList<QString> acceptableTransferSyntaxUIDs, 
    bool includeBulkData);
  void releaseData(QList<QUuid> objectUUIDs);

private:
  ctkDicomServicePrivate * d;

  ctkDicomExchangeService service;

};

#endif // CTKDICOMHOSTSERVICE_P_H
