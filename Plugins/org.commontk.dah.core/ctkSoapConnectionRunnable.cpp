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

// Qt includes
#include <QTcpSocket>

// CTK includes
#include "ctkSoapConnectionRunnable_p.h"
#include "ctkSoapLog.h"

//----------------------------------------------------------------------------
ctkSoapConnectionRunnable::ctkSoapConnectionRunnable(int socketDescriptor)
  : socketDescriptor(socketDescriptor), isAboutToQuit(0)
{
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));
}

//----------------------------------------------------------------------------
ctkSoapConnectionRunnable::~ctkSoapConnectionRunnable()
{

}

//----------------------------------------------------------------------------
void ctkSoapConnectionRunnable::run()
{
  QTcpSocket tcpSocket;
  if (!tcpSocket.setSocketDescriptor(socketDescriptor))
    {
    // error handling
    return;
    }

  const int timeout = 1 * 1000;
  while (tcpSocket.state() == QTcpSocket::ConnectedState &&
         isAboutToQuit.fetchAndAddOrdered(0) == 0)
    {

    tcpSocket.waitForReadyRead(timeout);

    readClient(tcpSocket);
    }

}

//----------------------------------------------------------------------------
void ctkSoapConnectionRunnable::readClient(QTcpSocket& socket)
{
  QString requestType;
  int contentLength = -1;
  //qDebug() << socket->readAll();
  while (socket.canReadLine()) {
    QString line = socket.readLine();
    CTK_SOAP_LOG_LOWLEVEL( << line );
    if(line.contains("?wsdl HTTP"))
      {
      requestType = "?wsdl";
      }
    if(line.contains("?xsd=1"))
      {
      requestType = "?xsd=1";
      }
    if(line.contains("SoapAction"))
      {
      requestType = line;
      }
    if(line.contains("Content-Length: "))
      {
      contentLength = line.section(':',1).trimmed().toInt();
      }
    if (line.trimmed().isEmpty())
      {
      QString content;
      if(requestType.startsWith("?"))
        {
        QByteArray body = socket.readAll();
        emit incomingWSDLMessage(requestType, &content);
        }
      else
        {
        // Read the http body, which contains the soap message
        int bytesRead = 0;
        QByteArray body;
        while(body.size() < contentLength)
          {
          QByteArray bodyPart = socket.read(contentLength);
          CTK_SOAP_LOG_LOWLEVEL( << bodyPart );
          bytesRead += bodyPart.size();
          body.append(bodyPart);
          CTK_SOAP_LOG_LOWLEVEL( << " Expected content-length: " << contentLength << ". Bytes read so far: " << body.size() );
          if (body.size()<contentLength)
            {
            qCritical() << " Message body too small. Trying to read more.";
            socket.waitForReadyRead(-1);
            }
          }
        if(body.trimmed().isEmpty()==false)
          {
          QtSoapMessage msg;
          if (!msg.setContent(body))
            {
            qCritical() << "QtSoap import failed:" << msg.errorString();
            return;
            }

          QtSoapMessage reply;
          CTK_SOAP_LOG(<< "###################" << msg.toXmlString());
          emit incomingSoapMessage(msg, &reply);

          if (reply.isFault())
            {
            qCritical() << "QtSoap reply faulty";
            return;
            }

          CTK_SOAP_LOG_LOWLEVEL( << "SOAP reply:" );

          content = reply.toXmlString();
          }
        }

      QByteArray block;
      block.append("HTTP/1.1 200 OK\n");
      block.append("Content-Type: text/xml;charset=utf-8\n");
      block.append("Content-Length: ").append(QString::number(content.size())).append("\n");
      block.append("\n");

      block.append(content);

      CTK_SOAP_LOG_LOWLEVEL( << block );

      socket.write(block);

      requestType = "";
      contentLength = -1;
      }
    }
}

void ctkSoapConnectionRunnable::aboutToQuit()
{
  isAboutToQuit.testAndSetOrdered(0, 1);
}
