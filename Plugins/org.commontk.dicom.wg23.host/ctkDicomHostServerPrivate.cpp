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

#include "ctkDicomHostServerPrivate.h"

#include <ctkDicomHostInterface.h>

#include <QHostAddress>

#include <stdexcept>
#include <ctkDicomWG23TypesHelper.h>

ctkDicomHostServerPrivate::ctkDicomHostServerPrivate(ctkDicomHostInterface* hostInterface, int port) :
    hostInterface(hostInterface), port(port)
{

  connect(&server, SIGNAL(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)),
          this, SLOT(incomingSoapMessage(QtSoapMessage,QtSoapMessage*)));

  if (!server.listen(QHostAddress::LocalHost, this->port))
  {
    qCritical() << "Listening to 127.0.0.1:" << port << " failed.";
  }
}

void ctkDicomHostServerPrivate::incomingSoapMessage(
  const QtSoapMessage& message, QtSoapMessage* reply)
{
  const QtSoapType& method = message.method();
  QString methodName = method.name().name();

  qDebug() << "Received soap method request: " << methodName;

  if (methodName == "getAvailableScreen")
  {
    processGetAvailableScreen(message, reply);
  }
  else if (methodName == "notifyStateChanged")
  {
    processNotifyStateChanged(message, reply);
  }
  else if (methodName == "notifyStatus")
  {
    processNotifyStatus(message, reply);
  }
  else if (methodName == "generateUID")
  {
    processGenerateUID(message, reply);
  }
  else if (methodName == "getOutputLocation")
  {
    processGetOutputLocation(message, reply);
  }
  else
  {
    // error
    reply->setFaultCode( QtSoapMessage::Server );
    reply->setFaultString( "Unknown method." );
  }
}

void ctkDicomHostServerPrivate::processGetAvailableScreen(
    const QtSoapMessage &message, QtSoapMessage *reply) const
{
  const QtSoapType& preferredScreenType = message.method()["preferredScreen"];
  const QRect preferredScreen = ctkDicomSoapRectangle::getQRect(preferredScreenType);

  const QRect result = hostInterface->getAvailableScreen(preferredScreen);

  reply->setMethod("getAvailableScreenResponse");
  QtSoapStruct* availableScreenType = new ctkDicomSoapRectangle("availableScreen",result);
  reply->addMethodArgument(availableScreenType);
}

void ctkDicomHostServerPrivate::processNotifyStateChanged(
    const QtSoapMessage &message, QtSoapMessage * /* reply */) const
{
    const QtSoapType& stateType = message.method()["state"];
    hostInterface->notifyStateChanged(ctkDicomSoapState::getState(stateType));
}

void ctkDicomHostServerPrivate::processNotifyStatus(
    const QtSoapMessage &message, QtSoapMessage * /* reply */) const
{
    const QtSoapType& status = message.method()["status"];
    hostInterface->notifyStatus(ctkDicomSoapStatus::getStatus(status));
}

void ctkDicomHostServerPrivate::processGenerateUID(
  const QtSoapMessage& message, QtSoapMessage* reply) const
{
  Q_UNUSED(message)
  Q_UNUSED(reply)
}

void ctkDicomHostServerPrivate::processGetOutputLocation(
  const QtSoapMessage& message, QtSoapMessage* reply) const
{
  Q_UNUSED(message)
  Q_UNUSED(reply)
}
