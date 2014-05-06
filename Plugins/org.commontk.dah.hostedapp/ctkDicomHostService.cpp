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

#include <ctkDicomAppHostingTypesHelper.h>

//----------------------------------------------------------------------------
ctkDicomHostService::ctkDicomHostService(ushort port, QString path)
  : ctkDicomExchangeService(port, path)
{
}

//----------------------------------------------------------------------------
ctkDicomHostService::~ctkDicomHostService()
{
}

//----------------------------------------------------------------------------
QString ctkDicomHostService::generateUID()
{
  const QtSoapType& result = submitSoapRequest("GenerateUID", NULL);
  QString resultUID = ctkDicomSoapUID::getUID(result);
  return resultUID;
}

//----------------------------------------------------------------------------
QString ctkDicomHostService::getOutputLocation(const QStringList& preferredProtocols)
{
  QtSoapStruct* input = dynamic_cast<QtSoapStruct*>(
   new ctkDicomSoapArrayOfStringType("string","preferredProtocols", preferredProtocols));
  const QtSoapType& result = submitSoapRequest("GetOutputLocation", input);
  QString resultString = result.value().toString();
  return resultString;
}

//----------------------------------------------------------------------------
QRect ctkDicomHostService::getAvailableScreen(const QRect& preferredScreen)
{
  QtSoapStruct* input = new ctkDicomSoapRectangle("preferredScreen", preferredScreen);
  const QtSoapType& result = submitSoapRequest("GetAvailableScreen", input);
  QRect resultRect = ctkDicomSoapRectangle::getQRect(result);
  qDebug() << "x:" << resultRect.x() << " y:" << resultRect.y();
  return resultRect;
}

//----------------------------------------------------------------------------
void ctkDicomHostService::notifyStateChanged(ctkDicomAppHosting::State state)
{
  QtSoapType* input = new ctkDicomSoapState("state", state); // spec would be "state", java has "newState" FIX JAVA/STANDARD
  submitSoapRequest("NotifyStateChanged", input);
}

//----------------------------------------------------------------------------
void ctkDicomHostService::notifyStatus(const ctkDicomAppHosting::Status& status)
{
  //Q_D(ctkDicomService);
  QtSoapStruct* input = new ctkDicomSoapStatus("status", status);
  submitSoapRequest("NotifyStatus", input);
}

//----------------------------------------------------------------------------
// Exchange methods

//----------------------------------------------------------------------------
bool ctkDicomHostService::notifyDataAvailable(const ctkDicomAppHosting::AvailableData& data, bool lastData)
{
  return ctkDicomExchangeService::notifyDataAvailable(data, lastData);
}

//----------------------------------------------------------------------------
QList<ctkDicomAppHosting::ObjectLocator> ctkDicomHostService::getData(
  const QList<QUuid>& objectUUIDs,
  const QList<QString>& acceptableTransferSyntaxUIDs,
  bool includeBulkData)
{
  return ctkDicomExchangeService::getData(objectUUIDs, acceptableTransferSyntaxUIDs, includeBulkData);
}

//----------------------------------------------------------------------------
void ctkDicomHostService::releaseData(const QList<QUuid>& objectUUIDs)
{
  ctkDicomExchangeService::releaseData(objectUUIDs);
}
