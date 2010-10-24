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

#include "ctkDicomExchangeService.h"

#include "ctkDicomServicePrivate.h"

#include "ctkDicomAppHostingTypesHelper.h"

ctkDicomExchangeService::ctkDicomExchangeService(ushort port, QString path)
  : d(new ctkDicomServicePrivate(port, path))
{

}

ctkDicomExchangeService::~ctkDicomExchangeService()
{
  delete d;
  d = NULL;
}

bool ctkDicomExchangeService::notifyDataAvailable(
    ctkDicomAppHosting::AvailableData data, bool lastData)
{
  //Q_D(ctkDicomService);
  QList<QtSoapType*> list;
  list << new ctkDicomSoapAvailableData("data", data);
  list << new ctkDicomSoapBool("lastData", lastData);
  const QtSoapType & result = d->askHost("notifyDataAvailable",list);
  return ctkDicomSoapBool::getBool(result);
}

QList<ctkDicomAppHosting::ObjectLocator> ctkDicomExchangeService::getData(
    QList<QUuid> objectUUIDs,
    QList<QString> acceptableTransferSyntaxUIDs, bool includeBulkData)
{
  //Q_D(ctkDicomService);
  QList<QtSoapType*> list;

  list << new ctkDicomSoapArrayOfUUIDS("objectUUIDS",objectUUIDs);
  list << new ctkDicomSoapArrayOfStringType("UID","acceptableTransferSyntaxUIDs", acceptableTransferSyntaxUIDs);
  list << new ctkDicomSoapBool("includeBulkData", includeBulkData);
  const QtSoapType & result = d->askHost("getData",list);
  return ctkDicomSoapArrayOfObjectLocators::getArray(static_cast<const QtSoapArray &>(result));
}

void ctkDicomExchangeService::releaseData(QList<QUuid> objectUUIDs)
{
  Q_UNUSED(objectUUIDs)
}


