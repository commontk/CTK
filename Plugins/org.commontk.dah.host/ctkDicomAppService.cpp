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

#include "ctkDicomAppService.h"

#include "ctkDicomServicePrivate.h"

#include "ctkDicomAppHostingTypesHelper.h"

ctkDicomAppService::ctkDicomAppService(ushort port, QString path):
  d(new ctkDicomServicePrivate(port, path)), service(port, path)
{
}

ctkDicomAppService::~ctkDicomAppService()
{
}

ctkDicomAppHosting::State ctkDicomAppService::getState()
{
  //Q_D(ctkDicomService);
  const QtSoapType & result = d->askHost("getState", NULL);
  return ctkDicomSoapState::getState(result);
}

bool ctkDicomAppService::setState(ctkDicomAppHosting::State newState)
{
  //Q_D(ctkDicomService);
  QtSoapType* input = new ctkDicomSoapState("newState", newState);
  const QtSoapType & result = d->askHost("setState", input);
  return ctkDicomSoapBool::getBool(result);
}

bool ctkDicomAppService::bringToFront(const QRect& requestedScreenArea)
{
  //Q_D(ctkDicomService);
  QtSoapType* input = new ctkDicomSoapRectangle("requestedScreenArea", requestedScreenArea);
  const QtSoapType & result = d->askHost("bringToFront", input);
  return ctkDicomSoapBool::getBool(result);
}

// Exchange methods

bool ctkDicomAppService::notifyDataAvailable(ctkDicomAppHosting::AvailableData data, bool lastData)
{
  return service.notifyDataAvailable(data, lastData);
}

QList<ctkDicomAppHosting::ObjectLocator> ctkDicomAppService::getData(
  QList<QUuid> objectUUIDs, 
  QList<QString> acceptableTransferSyntaxUIDs, 
  bool includeBulkData)
{
  return service.getData(objectUUIDs, acceptableTransferSyntaxUIDs, includeBulkData);
}

void ctkDicomAppService::releaseData(QList<QUuid> objectUUIDs)
{
  service.releaseData(objectUUIDs);
}
