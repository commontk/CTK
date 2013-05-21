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
#include "ctkSoapLog.h"

#include <ctkDicomAppHostingTypesHelper.h>

#include <QFile>
#include <QTextStream>

//----------------------------------------------------------------------------
ctkExchangeSoapMessageProcessor::ctkExchangeSoapMessageProcessor(ctkDicomExchangeInterface* inter)
: exchangeInterface(inter)
{}

//----------------------------------------------------------------------------
bool ctkExchangeSoapMessageProcessor::process(
  const QtSoapMessage& message, QtSoapMessage* reply ) const
{
  // TODO check for NULL exchangeInterface?

  const QtSoapType& method = message.method();
  QString methodName = method.name().name();

  qDebug() << "ExchangeMessageProcessor: Received soap method request: " << methodName;

  bool foundMethod = false;

  if (methodName == "NotifyDataAvailable")
    {
    processNotifyDataAvailable(message, reply);
    foundMethod = true;
    }
  else if (methodName == "GetData")
    {
    processGetData(message, reply);
    foundMethod = true;
    }
  else if (methodName == "ReleaseData")
    {
    processReleaseData(message, reply);
    foundMethod = true;
    }

  return foundMethod;
}

//----------------------------------------------------------------------------
void ctkExchangeSoapMessageProcessor::processNotifyDataAvailable(
  const QtSoapMessage &message, QtSoapMessage *reply) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()[0];//"availableData"];
  if(inputType.isValid()==false)
    {
    qCritical() << "  NotifyDataAvailable: availableData not valid. " << inputType.errorString();
    }
  CTK_SOAP_LOG( << inputType.toString());
  const ctkDicomAppHosting::AvailableData data = ctkDicomSoapAvailableData::getAvailableData(inputType);
  const QtSoapType& inputType2 = message.method()["lastData"];
  const bool lastData = ctkDicomSoapBool::getBool(inputType2);

  CTK_SOAP_LOG_HIGHLEVEL( << "  NotifyDataAvailable: patients.count: " << data.patients.count());
  // query interface
  bool result = exchangeInterface->notifyDataAvailable(data, lastData);
  // set reply message
  reply->setMethod("NotifyDataAvailableResponse");
  QtSoapType* resultType = new ctkDicomSoapBool("NotifyDataAvailableResult",result);
  reply->addMethodArgument(resultType);
}

//----------------------------------------------------------------------------
void ctkExchangeSoapMessageProcessor::processGetData(
    const QtSoapMessage &message, QtSoapMessage *reply) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()["objects"];
  const QList<QUuid> objectUUIDs = ctkDicomSoapArrayOfUUIDS::getArray(inputType);
  const QtSoapType& inputType2 = message.method()["acceptableTransferSyntaxes"];
  const QList<QString> acceptableTransferSyntaxUIDs = ctkDicomSoapArrayOfUIDS::getArray(inputType2);
  const QtSoapType& inputType3 = message.method()["includeBulkData"];
  const bool includeBulkData = ctkDicomSoapBool::getBool(inputType3);
  // query interface
  const QList<ctkDicomAppHosting::ObjectLocator> result = exchangeInterface->getData(
    objectUUIDs, acceptableTransferSyntaxUIDs, includeBulkData);
  // set reply message
  reply->setMethod(QtSoapQName("GetDataResponse","http://dicom.nema.org/PS3.19/ApplicationService-20100825"));
  //reply->setMethod(QtSoapQName("GetDataResponse","http://dicom.nema.org/PS3.19/HostService-20100825"));
  QtSoapType* resultType = new ctkDicomSoapArrayOfObjectLocators("GetDataResult", result);
  reply->addMethodArgument(resultType);
}

//----------------------------------------------------------------------------
void ctkExchangeSoapMessageProcessor::processReleaseData(
    const QtSoapMessage &message, QtSoapMessage * /*reply*/) const
{
  // extract arguments from input message
  const QtSoapType& inputType = message.method()["objects"];
  const QList<QUuid> objectUUIDs = ctkDicomSoapArrayOfUUIDS::getArray(
    dynamic_cast<const QtSoapArray&>(inputType));
  // query interface
  exchangeInterface->releaseData(objectUUIDs);
  // set reply message: nothing to be done
}
