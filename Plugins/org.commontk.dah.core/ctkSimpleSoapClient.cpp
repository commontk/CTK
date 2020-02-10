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

#include "ctkSimpleSoapClient.h"
#include "ctkDicomAppHostingTypes.h"
#include "ctkSoapLog.h"

#include <QApplication>
#include <QCursor>
#include <QNetworkReply>
#include <QtSoapHttpTransport>

//----------------------------------------------------------------------------
class ctkSimpleSoapClientPrivate
{
public:

  QEventLoop BlockingLoop;
  QtSoapHttpTransport Http;

  int Port;
  QString Path;
};

//----------------------------------------------------------------------------
ctkSimpleSoapClient::ctkSimpleSoapClient(int port, QString path)
  : d_ptr(new ctkSimpleSoapClientPrivate())
{
  Q_D(ctkSimpleSoapClient);

  d->Port = port;
  d->Path = path;

  connect(&d->Http, SIGNAL(responseReady()), this, SLOT(responseReady()));

  d->Http.setHost("127.0.0.1", false, port);
}

//----------------------------------------------------------------------------
ctkSimpleSoapClient::~ctkSimpleSoapClient()
{

}

//----------------------------------------------------------------------------
void ctkSimpleSoapClient::responseReady()
{
  Q_D(ctkSimpleSoapClient);
  d->BlockingLoop.exit();
}

//----------------------------------------------------------------------------
const QtSoapType & ctkSimpleSoapClient::submitSoapRequest(const QString& methodName,
                                                   QtSoapType* soapType )
{
  QList<QtSoapType*> list;
  if(soapType != NULL)
    {
    list.append(soapType);
    }
    return submitSoapRequest(methodName,list);
}

//----------------------------------------------------------------------------
const QtSoapType & ctkSimpleSoapClient::submitSoapRequest(const QString& methodName,
                                                   const QList<QtSoapType*>& soapTypes )
{
  Q_D(ctkSimpleSoapClient);

  /*QString action="\"";
  //action.append(methodName);
  action.append("\"");
  d->Http.setAction(action);*/
  QString action = "http://dicom.nema.org/PS3.19/IHostService/" + methodName;

  d->Http.setAction(action);

  CTK_SOAP_LOG( << "Submitting action " << action
                << " method " << methodName
                << " to path " << d->Path );

  QtSoapMessage request;
  //request.setMethod(QtSoapQName(methodName,"http://wg23.dicom.nema.org/"));
  request.setMethod(QtSoapQName(methodName,"http://dicom.nema.org/PS3.19" + d->Path ));
  if(!soapTypes.isEmpty())
    {
    for (QList<QtSoapType*>::ConstIterator it = soapTypes.begin();
         it < soapTypes.constEnd(); it++)
      {
      request.addMethodArgument(*it);
      CTK_SOAP_LOG( << "  Argument type added " << (*it)->typeName() << ". "
                    << " Argument name is " << (*it)->name().name() );
      }
    }
  CTK_SOAP_LOG_LOWLEVEL( << "Submitting request " << methodName);
  CTK_SOAP_LOG_LOWLEVEL( << request.toXmlString());

  d->Http.submitRequest(request, d->Path);;

  CTK_SOAP_LOG_LOWLEVEL( << "Submitted request " << methodName);

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  d->BlockingLoop.exec(QEventLoop::ExcludeUserInputEvents | QEventLoop::WaitForMoreEvents);

  QApplication::restoreOverrideCursor();

  //qDebug() << "Reply error: " << reply->errorString();
  //qDebug() << reply->readAll();
  const QtSoapMessage& response = d->Http.getResponse();

  CTK_SOAP_LOG( << "Got Response." );

  if (response.isFault())
    {
    qCritical() << "ctkSimpleSoapClient: server error (response.IsFault())";
    CTK_SOAP_LOG_LOWLEVEL( << qPrintable(response.faultString().toString()) << endl );
    CTK_SOAP_LOG_LOWLEVEL( << response.toXmlString() );
    return response.returnValue();
    //    throw ctkRuntimeException("ctkSimpleSoapClient: server error (response.IsFault())");
    }

  CTK_SOAP_LOG_LOWLEVEL( << "Response: " << response.toXmlString() );

  const QtSoapType &returnValue = response.returnValue();

  CTK_SOAP_LOG( << "  ReturnValue valid:" << returnValue.isValid() << "     "
                << "Name: " << returnValue.name().name() << "     "
                << "Value:" << returnValue.value().toString() );

  return returnValue;
}
