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
#include <org_commontk_dah_host_Export.h>

class org_commontk_dah_host_EXPORT ctkDicomAppService : public ctkDicomAppInterface
{

public:
  ctkDicomAppService(ushort port, QString path);
  ~ctkDicomAppService();

  ctkDicomAppHosting::State getState();
  bool setState(ctkDicomAppHosting::State newState);
  bool bringToFront(const QRect& requestedScreenArea);

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

#endif // CTKDICOMAPPSERVICE_H
