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

#include "ctkDicomHostService.h"

#include "ctkDicomServicePrivate.h"

#include "ctkDicomWG23TypesHelper.h"

ctkDicomHostService::ctkDicomHostService(ushort port):
    ctkDicomExchangeService(port)
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
    new ctkDicomSoapArrayOfString("preferredProtocols", preferredProtocols));
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

void ctkDicomHostService::notifyStateChanged(ctkDicomWG23::State state)
{
  //Q_D(ctkDicomService);

  QtSoapType* input = new ctkDicomSoapState("state", state);
  d->askHost("notifyStateChanged", input);
}

void ctkDicomHostService::notifyStatus(const ctkDicomWG23::Status& status)
{
  //Q_D(ctkDicomService);
  QtSoapStruct* input = new ctkDicomSoapStatus("status", status);
  d->askHost("notifyStatus", input);
}
