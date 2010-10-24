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

#include "ctkExchangeSoapMessageProcessor.h"

#include <ctkDicomAppHostingTypesHelper.h>

ctkExchangeSoapMessageProcessor::ctkExchangeSoapMessageProcessor(ctkDicomExchangeInterface* inter)
: exchangeInterface(inter)
{}

bool ctkExchangeSoapMessageProcessor::process(
	const QtSoapMessage& message,
	QtSoapMessage* reply ) const
{
  // TODO check for NULL exchangeInterface?
  
  const QtSoapType& method = message.method();
  QString methodName = method.name().name();

  qDebug() << "ExchangeMessageProcessor: Received soap method request: " << methodName;

  bool foundMethod = false;
  
  if (methodName == "notifyDataAvailable")
  {
    processNotifyDataAvailable(message, reply);
    foundMethod = true;
  }
  else if (methodName == "getData")
  {
    processGetData(message, reply);
    foundMethod = true;
  }
  else if (methodName == "releaseData")
  {
    processReleaseData(message, reply);
    foundMethod = true;
  }
  
  return foundMethod;
}
		
void ctkExchangeSoapMessageProcessor::processNotifyDataAvailable(
    const QtSoapMessage &message, QtSoapMessage *reply) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()["data"];
  const ctkDicomAppHosting::AvailableData data = ctkDicomSoapAvailableData::getAvailableData(inputType);
  const QtSoapType& inputType2 = message.method()["lastData"];
  const bool lastData = ctkDicomSoapBool::getBool(inputType2);
  // query interface
  bool result = exchangeInterface->notifyDataAvailable(data, lastData);
  // set reply message
  reply->setMethod("notifyDataAvailable");
  QtSoapType* resultType = new ctkDicomSoapBool("dataAvailable",result);
  reply->addMethodArgument(resultType);
}

void ctkExchangeSoapMessageProcessor::processGetData(
    const QtSoapMessage &message, QtSoapMessage *reply) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()["objectUUIDs"];
  const QList<QUuid> objectUUIDs = ctkDicomSoapArrayOfUUIDS::getArray(
    dynamic_cast<const QtSoapArray&>(inputType));
  const QtSoapType& inputType2 = message.method()["acceptableTransferSyntaxUIDs"];
  const QStringList acceptableTransferSyntaxUIDs = ctkDicomSoapArrayOfStringType::getArray(
    dynamic_cast<const QtSoapArray&>(inputType2));
  const QtSoapType& inputType3 = message.method()["includeBulkData"];
  const bool includeBulkData = ctkDicomSoapBool::getBool(inputType3);
  // query interface
  const QList<ctkDicomAppHosting::ObjectLocator> result = exchangeInterface->getData(
    objectUUIDs, acceptableTransferSyntaxUIDs, includeBulkData);
  // set reply message
  reply->setMethod("getData");
  QtSoapType* resultType = new ctkDicomSoapArrayOfObjectLocators("arrayOfObjectLocator", result);
  reply->addMethodArgument(resultType);
}

void ctkExchangeSoapMessageProcessor::processReleaseData(
    const QtSoapMessage &message, QtSoapMessage *reply) const
{
  Q_UNUSED(message)
  Q_UNUSED(reply)
  // extract arguments from input message
  //const QtSoapType& inputType = message.method()["objectUUIDs"];
  const QList<QUuid> objectUUIDs;
  // query interface
  exchangeInterface->releaseData(objectUUIDs);
  // set reply message: nothing to be done
}
