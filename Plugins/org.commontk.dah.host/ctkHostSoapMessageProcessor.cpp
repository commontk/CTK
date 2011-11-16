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

//----------------------------------------------------------------------------
ctkHostSoapMessageProcessor::ctkHostSoapMessageProcessor( ctkDicomHostInterface* inter )
: HostInterface(inter)
{}

//----------------------------------------------------------------------------
bool ctkHostSoapMessageProcessor::process(
  const QtSoapMessage& message, QtSoapMessage* reply ) const
{
  // TODO check for NULL hostInterface?
  
  const QtSoapType& method = message.method();
  QString methodName = method.name().name();

  qDebug() << "HostMessageProcessor: Received soap method request: " << methodName;

  bool foundMethod = false;
  
  if (methodName == "GetAvailableScreen")
    {
    processGetAvailableScreen(message, reply);
    foundMethod = true;
    }
  else if (methodName == "NotifyStateChanged")
    {
    processNotifyStateChanged(message, reply);
    foundMethod = true;
    }
  else if (methodName == "NotifyStatus")
    {
    processNotifyStatus(message, reply);
    foundMethod = true;
    }
  else if (methodName == "GenerateUID")
    {
    processGenerateUID(message, reply);
    foundMethod = true;
    }
  else if (methodName == "GetOutputLocation")
    {
    processGetOutputLocation(message, reply);
    foundMethod = true;
    }
  
  return foundMethod;
}

//----------------------------------------------------------------------------
void ctkHostSoapMessageProcessor::processGetAvailableScreen(
    const QtSoapMessage &message, QtSoapMessage *reply) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()["PreferredScreen"];
  const QRect preferredScreen = ctkDicomSoapRectangle::getQRect(inputType);
  // query interface
  const QRect result = this->HostInterface->getAvailableScreen(preferredScreen);
  // set reply message
  reply->setMethod("GetAvailableScreenResponse");
  QtSoapStruct* returnType = new ctkDicomSoapRectangle("AvailableScreen",result);
  reply->addMethodArgument(returnType);
}

//----------------------------------------------------------------------------
void ctkHostSoapMessageProcessor::processNotifyStateChanged(
    const QtSoapMessage &message, QtSoapMessage *reply) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()[0];//["state"]; java sends ["newState"]; FIX JAVA/STANDARD
  // query interface
  this->HostInterface->notifyStateChanged(ctkDicomSoapState::getState(inputType));
  // set reply message: nothing to be done

  /*bool result = true;
  reply->setMethod("NotifyStateChanged");
  QtSoapType* resultType = new ctkDicomSoapBool("NotifyStateChangedResponse",result);
  reply->addMethodArgument(resultType);*/

}

//----------------------------------------------------------------------------
void ctkHostSoapMessageProcessor::processNotifyStatus(
    const QtSoapMessage &message, QtSoapMessage *reply) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()["status"];
  // query interface
  this->HostInterface->notifyStatus(ctkDicomSoapStatus::getStatus(inputType));
  // set reply message: nothing to be done
	
  /*bool result = true;
  reply->setMethod("NotifyStatus");
  QtSoapType* resultType = new ctkDicomSoapBool("NotifyStatusResponse",result);
  reply->addMethodArgument(resultType);*/

}

//----------------------------------------------------------------------------
void ctkHostSoapMessageProcessor::processGenerateUID(
  const QtSoapMessage& message, QtSoapMessage* reply) const
{
  Q_UNUSED(message)
  // extract arguments from input message: nothing to be done
  // query interface
  const QString uid = this->HostInterface->generateUID();
  // set reply message
  reply->setMethod("GenerateUID");
  QtSoapType* resultType = new ctkDicomSoapUID("uid",uid);
  reply->addMethodArgument(resultType);
}

//----------------------------------------------------------------------------
void ctkHostSoapMessageProcessor::processGetOutputLocation(
  const QtSoapMessage& message, QtSoapMessage* reply) const
{
  // extract arguments from input message
  //const QtSoapType& inputType = message.method()["preferredProtocols"];
  //const QStringList preferredProtocols = ctkDicomSoapArrayOfStringType::getArray(
    //dynamic_cast<const QtSoapArray&>(inputType));

  const QtSoapType& inputType = message.method()["preferredProtocols"];
  const QStringList preferredProtocols = ctkDicomSoapArrayOfStringType::getArray(inputType);


  // query interface
  const QString result = this->HostInterface->getOutputLocation(preferredProtocols);
  // set reply message
  reply->setMethod("GetOutputLocation");
  QtSoapType* resultType = new QtSoapSimpleType( QtSoapQName("preferredProtocols"), result );
  reply->addMethodArgument(resultType);
}
