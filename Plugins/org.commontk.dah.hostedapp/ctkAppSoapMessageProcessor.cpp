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

//----------------------------------------------------------------------------
ctkAppSoapMessageProcessor::ctkAppSoapMessageProcessor(ctkDicomAppInterface* inter)
  : AppInterface(inter)
{}

//----------------------------------------------------------------------------
bool ctkAppSoapMessageProcessor::process(
  const QtSoapMessage& message, QtSoapMessage* reply ) const
{
  // TODO check for NULL appInterface?
  
  const QtSoapType& method = message.method();
  QString methodName = method.name().name();

  qDebug() << "AppMessageProcessor: Received soap method request: " << methodName;

  bool foundMethod = false;
  
  if (methodName == "GetState")
    {
    processGetState(message, reply);
    foundMethod = true;
    }
  else if (methodName == "SetState")
    {
    processSetState(message, reply);
    foundMethod = true;
    }
  else if (methodName == "BringToFront")
    {
    processBringToFront(message, reply);
    foundMethod = true;
    }
  
  return foundMethod;
}

//----------------------------------------------------------------------------
void ctkAppSoapMessageProcessor::processGetState(
  const QtSoapMessage &message, QtSoapMessage *reply) const
{
  Q_UNUSED(message)

  // extract arguments from input message: nothing to be done
  // query interface
  const ctkDicomAppHosting::State result = this->AppInterface->getState();
  // set reply message
  reply->setMethod("GetStateResponse");
  QtSoapSimpleType* resultType = new ctkDicomSoapState("GetStateResult",result);
  reply->addMethodArgument(resultType);
}

//----------------------------------------------------------------------------
void ctkAppSoapMessageProcessor::processSetState(
  const QtSoapMessage &message, QtSoapMessage *reply) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()["state"];
  // query interface
  bool result = this->AppInterface->setState(ctkDicomSoapState::getState(inputType));
  // set reply message
  reply->setMethod("SetStateResponse");
  QtSoapType* resultType = new ctkDicomSoapBool("SetStateResult",result);
  reply->addMethodArgument(resultType);
}

//----------------------------------------------------------------------------
void ctkAppSoapMessageProcessor::processBringToFront(
  const QtSoapMessage &message, QtSoapMessage *reply) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()["RequestedScreenArea"];
  const QRect requestedScreenArea = ctkDicomSoapRectangle::getQRect(inputType);
  // query interface
  bool result = this->AppInterface->bringToFront(requestedScreenArea);
  // set reply message
  reply->setMethod("BringToFrontResponse");
  QtSoapType* resultType = new ctkDicomSoapBool("BringToFrontResult",result);
  reply->addMethodArgument(resultType);
}
