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


#ifndef CTKSIMPLESOAPSERVER_H
#define CTKSIMPLESOAPSERVER_H

// Qt includes
#include <QTcpServer>

// QtSoap includes
#include <qtsoap.h>

// CTK includes
#include <org_commontk_dah_core_Export.h>
#include <ctkDicomAppHostingTypes.h>

class org_commontk_dah_core_EXPORT ctkSimpleSoapServer : public QTcpServer
{
  Q_OBJECT

public:

  ctkSimpleSoapServer(QObject *parent = 0);

Q_SIGNALS:

  void incomingSoapMessage(const QtSoapMessage& message, QtSoapMessage* reply);
  void incomingWSDLMessage(const QString& message, QString* reply);

public Q_SLOTS:

protected:

#if (QT_VERSION < 0x50000)
  virtual void incomingConnection(int socketDescriptor);
#else
  virtual void incomingConnection(qintptr socketDescriptor);
#endif

};

#endif // CTKSIMPLESOAPSERVER_H
