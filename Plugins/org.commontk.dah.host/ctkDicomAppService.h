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


#ifndef CTKDICOMAPPSERVICE_H
#define CTKDICOMAPPSERVICE_H

#include <ctkDicomAppInterface.h>
#include <ctkDicomExchangeService.h>

class ctkDicomAppService : public ctkDicomExchangeService, public ctkDicomAppInterface
{

public:
  ctkDicomAppService(ushort port, QString path);
  virtual ~ctkDicomAppService();

  virtual ctkDicomAppHosting::State getState();
  virtual bool setState(ctkDicomAppHosting::State newState);
  virtual bool bringToFront(const QRect& requestedScreenArea);

  // Exchange methods implemented in ctkDicomExchangeService
  virtual bool notifyDataAvailable(const ctkDicomAppHosting::AvailableData& data, bool lastData);

  virtual QList<ctkDicomAppHosting::ObjectLocator> getData(
    const QList<QUuid>& objectUUIDs,
    const QList<QString>& acceptableTransferSyntaxUIDs,
    bool includeBulkData);

  virtual void releaseData(const QList<QUuid>& objectUUIDs);

};

#endif // CTKDICOMAPPSERVICE_H
