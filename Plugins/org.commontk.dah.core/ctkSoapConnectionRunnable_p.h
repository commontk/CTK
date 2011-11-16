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


#ifndef CTKSOAPCONNECTIONRUNNABLE_P_H
#define CTKSOAPCONNECTIONRUNNABLE_P_H

#include <QObject>
#include <QRunnable>
#include <QTcpSocket>

#include <qtsoap.h>

class ctkSoapConnectionRunnable : public QObject, public QRunnable
{
  Q_OBJECT

public:

  ctkSoapConnectionRunnable(int socketDescriptor);
  virtual ~ctkSoapConnectionRunnable();

  void run();

Q_SIGNALS:

  void incomingSoapMessage(const QtSoapMessage& message, QtSoapMessage* reply);
  void incomingWSDLMessage(const QString& message, QString* reply);

protected Q_SLOTS:

  void aboutToQuit();

private:

  void readClient(QTcpSocket& socket);

  int socketDescriptor;

  QAtomicInt isAboutToQuit;

};

#endif // CTKSOAPCONNECTIONRUNNABLE_P_H
