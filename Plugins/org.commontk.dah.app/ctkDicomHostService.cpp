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

#include "ctkDicomHostService_p.h"

#include "ctkDicomServicePrivate.h"

#include "ctkDicomAppHostingTypesHelper.h"

ctkDicomHostService::ctkDicomHostService(ushort port, QString path):
    d(new ctkDicomServicePrivate(port, path)), service(port, path)
{
}

ctkDicomHostService::~ctkDicomHostService()
{
}

QString ctkDicomHostService::generateUID()
{
  //Q_D(ctkDicomService);

  const QtSoapType& result = d->askHost("generateUID", NULL);
  QString resultUID = ctkDicomSoapUID::getUID(result);
  return resultUID;
}

QString ctkDicomHostService::getOutputLocation(const QStringList& preferredProtocols)
{
  //Q_D(ctkDicomHostService);

  QtSoapStruct* input = dynamic_cast<QtSoapStruct*>(
    new ctkDicomSoapArrayOfStringType("string","preferredProtocols", preferredProtocols));
  const QtSoapType& result = d->askHost("getOutputLocation", input);
  QString resultString = result.value().toString();
  return resultString;
}

QRect ctkDicomHostService::getAvailableScreen(const QRect& preferredScreen)
{
  //Q_D(ctkDicomService);

  QtSoapStruct* input = new ctkDicomSoapRectangle("preferredScreen", preferredScreen);
  const QtSoapType& result = d->askHost("getAvailableScreen", input);
  QRect resultRect = ctkDicomSoapRectangle::getQRect(result);
  qDebug() << "x:" << resultRect.x() << " y:" << resultRect.y();
  return resultRect;
}

void ctkDicomHostService::notifyStateChanged(ctkDicomAppHosting::State state)
{
  //Q_D(ctkDicomService);

  QtSoapType* input = new ctkDicomSoapState("newState", state); // spec would be "state", java has "newState" FIX JAVA/STANDARD
  d->askHost("notifyStateChanged", input);
}

void ctkDicomHostService::notifyStatus(const ctkDicomAppHosting::Status& status)
{
  //Q_D(ctkDicomService);
  QtSoapStruct* input = new ctkDicomSoapStatus("status", status);
  d->askHost("notifyStatus", input);
}

// Exchange methods

bool ctkDicomHostService::notifyDataAvailable(ctkDicomAppHosting::AvailableData data, bool lastData)
{
  return service.notifyDataAvailable(data, lastData);
}

QList<ctkDicomAppHosting::ObjectLocator> ctkDicomHostService::getData(
  QList<QUuid> objectUUIDs, 
  QList<QString> acceptableTransferSyntaxUIDs, 
  bool includeBulkData)
{
  return service.getData(objectUUIDs, acceptableTransferSyntaxUIDs, includeBulkData);
}

void ctkDicomHostService::releaseData(QList<QUuid> objectUUIDs)
{
  service.releaseData(objectUUIDs);
}
