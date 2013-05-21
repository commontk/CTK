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

#include "ctkDicomExchangeService.h"

#include "ctkSimpleSoapClient.h"

#include "ctkDicomAppHostingTypesHelper.h"

//----------------------------------------------------------------------------
ctkDicomExchangeService::ctkDicomExchangeService(ushort port, QString path)
  : ctkSimpleSoapClient(port, path)
{

}

//----------------------------------------------------------------------------
ctkDicomExchangeService::~ctkDicomExchangeService()
{

}

//----------------------------------------------------------------------------
bool ctkDicomExchangeService::notifyDataAvailable(
    const ctkDicomAppHosting::AvailableData& data, bool lastData)
{
  QList<QtSoapType*> list;
  list << new ctkDicomSoapAvailableData("data", data);
  list << new ctkDicomSoapBool("lastData", lastData);
  const QtSoapType & result = submitSoapRequest("NotifyDataAvailable",list);
  return ctkDicomSoapBool::getBool(result);
}

//----------------------------------------------------------------------------
QList<ctkDicomAppHosting::ObjectLocator> ctkDicomExchangeService::getData(
    const QList<QUuid>& objectUUIDs,
    const QList<QString>& acceptableTransferSyntaxUIDs, bool includeBulkData)
{
  //Q_D(ctkDicomService);
  QList<QtSoapType*> list;

  list << new ctkDicomSoapArrayOfUUIDS("objects",objectUUIDs);
  list << new ctkDicomSoapArrayOfUIDS("acceptableTransferSyntaxes", acceptableTransferSyntaxUIDs);
  list << new ctkDicomSoapBool("includeBulkData", includeBulkData);
  const QtSoapType & result = submitSoapRequest("GetData",list);
//QtSoapType *tt;
//  list << (tt=new ctkDicomSoapArrayOfUUIDS("uuids",objectUUIDs));
extern void DumpAll(const QtSoapType& type, int indent=0);
//DumpAll(*tt);
//  list << (tt=new ctkDicomSoapBool("includeBulkData", includeBulkData));
//DumpAll(*tt);
//  const QtSoapType & result = submitSoapRequest("getDataAsFile",list);
DumpAll(result); //xxx

  return ctkDicomSoapArrayOfObjectLocators::getArray(result);
}

//----------------------------------------------------------------------------
void ctkDicomExchangeService::releaseData(const QList<QUuid>& objectUUIDs)
{
  QList<QtSoapType*> list;

  list << new ctkDicomSoapArrayOfUUIDS("objects",objectUUIDs);
   submitSoapRequest("ReleaseData",list);
  return;
}
