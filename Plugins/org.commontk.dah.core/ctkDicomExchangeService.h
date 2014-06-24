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


#ifndef CTKDICOMEXCHANGESERVICE_H
#define CTKDICOMEXCHANGESERVICE_H

#include "ctkSimpleSoapClient.h"
#include "ctkDicomExchangeInterface.h"

#include <org_commontk_dah_core_Export.h>

class org_commontk_dah_core_EXPORT ctkDicomExchangeService :
    public ctkSimpleSoapClient, public virtual ctkDicomExchangeInterface
{

public:

  ctkDicomExchangeService(ushort port, QString path);
  virtual ~ctkDicomExchangeService();

  bool notifyDataAvailable(const ctkDicomAppHosting::AvailableData& data, bool lastData);

  QList<ctkDicomAppHosting::ObjectLocator> getData(
    const QList<QUuid>& objectUUIDs,
    const QList<QString>& acceptableTransferSyntaxUIDs,
    bool includeBulkData);

  void releaseData(const QList<QUuid>& objectUUIDs);

};

#endif // CTKDICOMEXCHANGESERVICE_H
