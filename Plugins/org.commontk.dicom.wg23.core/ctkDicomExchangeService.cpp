/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include "ctkDicomWG23TypesHelper.h"

ctkDicomExchangeService::ctkDicomExchangeService(ushort port)
 : d(new ctkDicomServicePrivate(port))
{

}
ctkDicomExchangeService::~ctkDicomExchangeService()
{
    delete d;
    d = NULL;
}

bool ctkDicomExchangeService::notifyDataAvailable(
        ctkDicomWG23::AvailableData data, bool lastData){
    //Q_D(ctkDicomService);
    QList<QtSoapType*> list;
    list << new ctkDicomSoapAvailableData("data", data);
    list << new ctkDicomSoapBool("lastData", lastData);
    const QtSoapType & result = d->askHost("notifyDataAvailable",list);
    return ctkDicomSoapBool::getBool(result);
}

QList<ctkDicomWG23::ObjectLocator> ctkDicomExchangeService::getData(
        QList<QUuid> objectUUIDs,
        QList<QString> acceptableTransferSyntaxUIDs, bool includeBulkData){
    QList<ctkDicomWG23::ObjectLocator> ol;
    return ol;
}

void ctkDicomExchangeService::releaseData(QList<QUuid> objectUUIDs){

}


