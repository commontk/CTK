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

#include "ctkAppSoapMessageProcessor.h"

#include <ctkDicomAppHostingTypesHelper.h>


ctkAppSoapMessageProcessor::ctkAppSoapMessageProcessor(ctkDicomAppInterface* inter)
: appInterface(inter)
{}

bool ctkAppSoapMessageProcessor::process(
	const QtSoapMessage& message,
	QtSoapMessage* reply ) const
{
  // TODO check for NULL appInterface?
  
  const QtSoapType& method = message.method();
  QString methodName = method.name().name();

  qDebug() << "AppMessageProcessor: Received soap method request: " << methodName;

  bool foundMethod = false;
  
  if (methodName == "getState")
  {
    processGetState(message, reply);
    foundMethod = true;
  }
  else if (methodName == "setState")
  {
    processSetState(message, reply);
    foundMethod = true;
  }
  else if (methodName == "bringToFront")
  {
    processBringToFront(message, reply);
    foundMethod = true;
  }
  
  return foundMethod;
}
		
void ctkAppSoapMessageProcessor::processGetState(
    const QtSoapMessage &message, QtSoapMessage *reply) const
{
  Q_UNUSED(message)

  // extract arguments from input message: nothing to be done
  // query interface
  const ctkDicomAppHosting::State result = appInterface->getState();
  // set reply message
  reply->setMethod("getState");
  QtSoapSimpleType* resultType = new ctkDicomSoapState("getStateResponse",result);
  reply->addMethodArgument(resultType);
}

void ctkAppSoapMessageProcessor::processSetState(
    const QtSoapMessage &message, QtSoapMessage *reply) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()["newState"];
  // query interface
  bool result = appInterface->setState(ctkDicomSoapState::getState(inputType));
  // set reply message
  reply->setMethod("setState");
  QtSoapType* resultType = new ctkDicomSoapBool("setStateResponse",result);
  reply->addMethodArgument(resultType);
}

void ctkAppSoapMessageProcessor::processBringToFront(
    const QtSoapMessage &message, QtSoapMessage *reply) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()["requestedScreenArea"];
  const QRect requestedScreenArea = ctkDicomSoapRectangle::getQRect(inputType);
  // query interface
  bool result = appInterface->bringToFront(requestedScreenArea);
  // set reply message
  reply->setMethod("bringToFront");
  QtSoapType* resultType = new ctkDicomSoapBool("bringToFrontResponse",result);
  reply->addMethodArgument(resultType);
}
