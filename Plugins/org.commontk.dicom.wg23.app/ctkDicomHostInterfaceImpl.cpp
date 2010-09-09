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


#include "ctkDicomHostInterfaceImpl_p.h"

#include <QApplication>
#include <QCursor>
#include <QNetworkReply>

#include <stdexcept>

ctkDicomHostInterfaceImpl::ctkDicomHostInterfaceImpl()
{
  connect(&http, SIGNAL(responseReady()), this, SLOT(responseReady()));

  http.setHost("127.0.0.1", false, 8080);
}

void ctkDicomHostInterfaceImpl::responseReady()
{
  blockingLoop.exit();
}

QString ctkDicomHostInterfaceImpl::generateUID()
{
  return QString();
}

QRect ctkDicomHostInterfaceImpl::getAvailableScreen(const QRect& preferredScreen)
{
  http.setAction("GetAvailableScreen");

  QtSoapMessage request;
  request.setMethod("GetAvailableScreen");

  QtSoapStruct* preferredScreenType = new QtSoapStruct(QtSoapQName("preferredScreen"));
  preferredScreenType->insert(new QtSoapSimpleType(QtSoapQName("Height"), preferredScreen.height()));
  preferredScreenType->insert(new QtSoapSimpleType(QtSoapQName("Width"), preferredScreen.width()));
  preferredScreenType->insert(new QtSoapSimpleType(QtSoapQName("RefPointX"), preferredScreen.x()));
  preferredScreenType->insert(new QtSoapSimpleType(QtSoapQName("RefPointY"), preferredScreen.y()));

  request.addMethodArgument(preferredScreenType);

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
    return QRect();
  }

  qDebug() << screenResult.count() << screenResult["Height"].typeName();
  QRect resultRect;
  resultRect.setHeight(screenResult["Height"].toInt());
  resultRect.setWidth(screenResult["Width"].toInt());
  resultRect.setX(screenResult["RefPointX"].toInt());
  resultRect.setY(screenResult["RefPointY"].toInt());

  qDebug() << "x:" << resultRect.x() << " y:" << resultRect.y();

  return resultRect;
}

QString ctkDicomHostInterfaceImpl::getOutputLocation(const QStringList& preferredProtocols)
{
  Q_UNUSED(preferredProtocols)
  return QString();
}

void ctkDicomHostInterfaceImpl::notifyStateChanged(State state)
{
  Q_UNUSED(state)
}

void ctkDicomHostInterfaceImpl::notifyStatus(const Status& status)
{
  Q_UNUSED(status)
}
