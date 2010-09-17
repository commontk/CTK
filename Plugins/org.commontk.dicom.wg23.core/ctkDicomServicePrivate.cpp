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

#include "ctkDicomServicePrivate.h"
#include "ctkDicomWG23TypesHelper.h"

#include <QApplication>
#include <QCursor>
#include <QNetworkReply>

#include <stdexcept>

ctkDicomServicePrivate::ctkDicomServicePrivate(int port)
{
  connect(&http, SIGNAL(responseReady()), this, SLOT(responseReady()));

  http.setHost("127.0.0.1", false, port);
}

void ctkDicomServicePrivate::responseReady()
{
  blockingLoop.exit();
}

const QtSoapType & ctkDicomServicePrivate::askHost(const QString& methodName, QtSoapType* soapType )
{
  qDebug() << "Submitting request " << methodName << " to path " << "/HostInterface";

  http.setAction(methodName);

  QtSoapMessage request;
  request.setMethod(methodName);
  if( soapType != NULL )
  {
    request.addMethodArgument(soapType);
    qDebug() << "  Argument type is " << soapType->typeName();
  }

  http.submitRequest(request, "/HostInterface");//"/IHostService");

  qDebug() << "Submitted request " << methodName ;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  blockingLoop.exec(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);

  QApplication::restoreOverrideCursor();

  //qDebug() << "Reply error: " << reply->errorString();
  //qDebug() << reply->readAll();
  const QtSoapMessage& response = http.getResponse();

  if (response.isFault())
  {
    throw std::runtime_error("ctkDicomServicePrivate: server error (response.IsFault())");
  }

  qDebug() << "Response: " << response.toXmlString();

  const QtSoapType &returnValue = response.returnValue();

  qDebug() << " Is returnValue valid:" << returnValue.isValid();
  qDebug() << " Name of returnValue:" << returnValue.name().name();
  qDebug() << " Value of returnValue:" << returnValue.value().toString();

  return returnValue;
}
