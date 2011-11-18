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

#ifndef CTKSIMPLESOAPCLIENT_H
#define CTKSIMPLESOAPCLIENT_H

#include <QObject>
#include <QScopedPointer>

#include <QtSoapType>

#include <org_commontk_dah_core_Export.h>

class ctkSimpleSoapClientPrivate;

class org_commontk_dah_core_EXPORT ctkSimpleSoapClient : public QObject
{
  Q_OBJECT

public:
  ctkSimpleSoapClient(int port, QString path);
  virtual ~ctkSimpleSoapClient();

  const QtSoapType & submitSoapRequest(const QString& methodName, const QList<QtSoapType*>& soapTypes);
  const QtSoapType & submitSoapRequest(const QString& methodName, QtSoapType* soapType);

private Q_SLOTS:

  void responseReady();

private:

  const QScopedPointer<ctkSimpleSoapClientPrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkSimpleSoapClient);
  Q_DISABLE_COPY(ctkSimpleSoapClient);
};

#endif // CTKSIMPLESOAPCLIENT_H
