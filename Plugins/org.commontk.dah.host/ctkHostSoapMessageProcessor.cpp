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

#include "ctkHostSoapMessageProcessor_p.h"

#include <ctkDicomAppHostingTypesHelper.h>


ctkHostSoapMessageProcessor::ctkHostSoapMessageProcessor( ctkDicomHostInterface* inter )
: hostInterface(inter)
{}

bool ctkHostSoapMessageProcessor::process(
	const QtSoapMessage& message,
	QtSoapMessage* reply ) const
{
  // TODO check for NULL hostInterface?
  
  const QtSoapType& method = message.method();
  QString methodName = method.name().name();

  qDebug() << "HostMessageProcessor: Received soap method request: " << methodName;

  bool foundMethod = false;
  
  if (methodName == "getAvailableScreen")
  {
    processGetAvailableScreen(message, reply);
    foundMethod = true;
  }
  else if (methodName == "notifyStateChanged")
  {
    processNotifyStateChanged(message, reply);
    foundMethod = true;
  }
  else if (methodName == "notifyStatus")
  {
    processNotifyStatus(message, reply);
    foundMethod = true;
  }
  else if (methodName == "generateUID")
  {
    processGenerateUID(message, reply);
    foundMethod = true;
  }
  else if (methodName == "getOutputLocation")
  {
    processGetOutputLocation(message, reply);
    foundMethod = true;
  }
  
  return foundMethod;
}
		
void ctkHostSoapMessageProcessor::processGetAvailableScreen(
    const QtSoapMessage &message, QtSoapMessage *reply) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()["preferredScreen"];
  const QRect preferredScreen = ctkDicomSoapRectangle::getQRect(inputType);
  // query interface
  const QRect result = hostInterface->getAvailableScreen(preferredScreen);
  // set reply message
  reply->setMethod("getAvailableScreenResponse");
  QtSoapStruct* returnType = new ctkDicomSoapRectangle("availableScreen",result);
  reply->addMethodArgument(returnType);
}

void ctkHostSoapMessageProcessor::processNotifyStateChanged(
    const QtSoapMessage &message, QtSoapMessage * /* reply */) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()[0];//["state"]; java sends ["newState"]; FIX JAVA/STANDARD
  // query interface
  hostInterface->notifyStateChanged(ctkDicomSoapState::getState(inputType));
  // set reply message: nothing to be done
}

void ctkHostSoapMessageProcessor::processNotifyStatus(
    const QtSoapMessage &message, QtSoapMessage * /* reply */) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()["status"];
  // query interface
  hostInterface->notifyStatus(ctkDicomSoapStatus::getStatus(inputType));
  // set reply message: nothing to be done
}

void ctkHostSoapMessageProcessor::processGenerateUID(
  const QtSoapMessage& message, QtSoapMessage* reply) const
{
  Q_UNUSED(message)
  // extract arguments from input message: nothing to be done
  // query interface
  const QString uid = hostInterface->generateUID();
  // set reply message
  reply->setMethod("generateUID");
  QtSoapType* resultType = new ctkDicomSoapUID("uid",uid);
  reply->addMethodArgument(resultType);
}

void ctkHostSoapMessageProcessor::processGetOutputLocation(
  const QtSoapMessage& message, QtSoapMessage* reply) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()["preferredProtocols"];
  const QStringList preferredProtocols = ctkDicomSoapArrayOfStringType::getArray(
    dynamic_cast<const QtSoapArray&>(inputType));
  // query interface
  const QString result = hostInterface->getOutputLocation(preferredProtocols);
  // set reply message
  reply->setMethod("getOutputLocation");
  QtSoapType* resultType = new QtSoapSimpleType( QtSoapQName("preferredProtocols"), result );
  reply->addMethodArgument(resultType);
}
