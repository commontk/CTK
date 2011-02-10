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

// CTK includes
#include <ctkDicomHostInterface.h>
#include <ctkDicomExchangeService.h>

class ctkDicomHostService : public ctkDicomExchangeService, public ctkDicomHostInterface
{
  Q_OBJECT
  Q_INTERFACES(ctkDicomHostInterface)

public:
  ctkDicomHostService(ushort port, QString path);
  virtual ~ctkDicomHostService();

  virtual QString generateUID();
  virtual QRect getAvailableScreen(const QRect& preferredScreen);
  virtual QString getOutputLocation(const QStringList& preferredProtocols);
  virtual void notifyStateChanged(ctkDicomAppHosting::State state);
  virtual void notifyStatus(const ctkDicomAppHosting::Status& status);

  // Exchange methods implemented in ctkDicomExchangeService
  virtual bool notifyDataAvailable(const ctkDicomAppHosting::AvailableData& data, bool lastData);

  virtual QList<ctkDicomAppHosting::ObjectLocator> getData(
    const QList<QUuid>& objectUUIDs,
    const QList<QString>& acceptableTransferSyntaxUIDs,
    bool includeBulkData);

  virtual void releaseData(const QList<QUuid>& objectUUIDs);

};

#endif // CTKDICOMHOSTSERVICE_P_H
