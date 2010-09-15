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

#include "ctkDicomHostServicePrivate.h"
#include "ctkDicomWG23TypesHelper.h"

#include <QApplication>
#include <QCursor>
#include <QNetworkReply>

#include <stdexcept>

ctkDicomHostServicePrivate::ctkDicomHostServicePrivate()
{
  connect(&http, SIGNAL(responseReady()), this, SLOT(responseReady()));

  http.setHost("127.0.0.1", false, 8080);
}

void ctkDicomHostServicePrivate::responseReady()
{
  blockingLoop.exit();
}

QtSoapType ctkDicomHostServicePrivate::askHost(const QString& methodName, QtSoapType* soapType )
{
  http.setAction(methodName);

  QtSoapMessage request;
  request.setMethod(methodName);
  if( soapType != NULL )
  {
    request.addMethodArgument(soapType);
  }

  http.submitRequest(request, "/IHostService");

  qDebug() << "Submitted request GetAvailableScreen";

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  blockingLoop.exec(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);

  QApplication::restoreOverrideCursor();

  //qDebug() << "Reply error: " << reply->errorString();
  //qDebug() << reply->readAll();
  const QtSoapMessage& response = http.getResponse();

  if (response.isFault())
  {
    throw std::runtime_error("server error");
  }

  qDebug() << "Response: " << response.toXmlString();

  const QtSoapType &meth = response.method();

  if (!meth.isValid() || meth.type() != QtSoapType::Struct)
    qDebug() << "SOAP returning NIL: invalid or type != Struct";

  const QtSoapStruct &m = dynamic_cast<const QtSoapStruct &>(meth);
  if (m.count() == 0)
    qDebug() << "SOAP returning NIL: count == 0";


  const QtSoapType& screenResult = response.returnValue();
  if (!screenResult.isValid())
  {
    //throw std::runtime_error("invalid return value");
    qDebug() << response.errorString() << response.faultString().toString();
    qDebug() << response.toXmlString();
  }
  else
  {
    qDebug() << screenResult.count();
  }  

  return screenResult;
}

QString ctkDicomHostServicePrivate::generateUID()
{
  const QtSoapType& result = askHost("generateUID", NULL);
  QString resultUID = ctkDicomSoapUID::getUID(result);
  return resultUID;
}

QString ctkDicomHostServicePrivate::getOutputLocation(const QStringList& preferredProtocols)
{
  Q_UNUSED(preferredProtocols)
  return QString();
}

QRect ctkDicomHostServicePrivate::getAvailableScreen(const QRect& preferredScreen)
{
  QtSoapStruct* input = new ctkDicomSoapRectangle("preferredScreen", preferredScreen);
  const QtSoapType& result = askHost("getAvailableScreen", input);
  QRect resultRect = ctkDicomSoapRectangle::getQRect(result);
  qDebug() << "x:" << resultRect.x() << " y:" << resultRect.y();
  return resultRect;
}

void ctkDicomHostServicePrivate::notifyStateChanged(ctkDicomWG23::State state)
{
  QtSoapType* input = new ctkDicomSoapState("stateChanged", state);
  askHost("notifyStateChanged", input);
}

void ctkDicomHostServicePrivate::notifyStatus(const ctkDicomWG23::Status& status)
{
  QtSoapStruct* input = new ctkDicomSoapStatus("status", status);
  askHost("notifyStatus", input);
}
